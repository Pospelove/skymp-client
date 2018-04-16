#include "../stdafx.h"
#include "../Sync/SyncEssential.h"
#include "Skyrim\Camera\PlayerCamera.h"
#include "CoreInterface.h"

#include "ciRPEngineBase.h"
#include "ciRPPackageApplier.h"
#include "ciRPPlaceMarker.h"
#include "ciRPGhostAxe.h"
#include "ciRPCommonEvents.h"

#include "ciRPEngineInput.h" // To run other players
#include "ciRPEngineIO.h" // To run NPCs

enum class InvisibleFoxEngine {
	None = 0x0,
	Object = 0x2,
};

extern std::map<TESForm *, const ci::ItemType *> knownItems;
extern std::map<TESForm *, const ci::Spell *> knownSpells;

namespace ci
{
	class IActorAccess
	{
	public:
		static uint32_t GetActorRefID(const IActor *ac) {
			return ac->GetRefID();
		}
	};
}

namespace ci
{
	// Used from other files with 'extern' keyword
	std::set<RemotePlayer *> allRemotePlayers;
	RPGMUTEX gMutex;
	ILookSynchronizer *lookSync = ILookSynchronizer::GetV17();

	size_t numSpawned = 0;
	std::function<void()> traceTask = nullptr;
	RemotePlayer *currentSpawning = nullptr;
	uint32_t currentSpawningBaseID = 0;
	clock_t lastForceSpawn = 0;
	RemotePlayer *currentFixingGreyFace = nullptr;
	uint32_t numInvisibleFoxes = 0;
	uint64_t errorsInSpawn = false;

	void RunGnomeBehavior(TESObjectREFR *ref, const std::wstring &name)
	{
		sd::EnableAI((Actor *)ref, false);
		sd::SetActorValue((Actor *)ref, "Invisibility", 100.0f);
		sd::AllowPCDialogue((Actor *)ref, false);
		cd::SetDisplayName(ref, WstringToString(name), true);
		sd::SetActorValue((Actor *)ref, "DamageResist", 100.0f);
		sd::SetActorValue((Actor *)ref, "MagicResist", 100.0f);
		sd::SetActorValue((Actor *)ref, "Health", 32000.f);
		sd::StopCombat((Actor *)ref);
	}

	void RemotePlayer::UpdateWorldSpell()
	{
		std::lock_guard<dlf_mutex> l(gMutex);

		std::map<const ci::Spell *, int32_t> worldSpellElection;
		int32_t max = -1;
		const ci::Spell *bestSpell = nullptr;

		std::set<const ci::Spell *> spells;

		std::for_each(allRemotePlayers.begin(), allRemotePlayers.end(), [&](RemotePlayer *p) {

			if (p->GetSyncMode() != (int32_t)MovementData_::SyncMode::Hard)
				return;

			{
				std::lock_guard<dlf_mutex> l(p->pimpl->mutex);
				if (p->pimpl->spawnStage != SpawnStage::Spawned)
					return;
				auto actor = (Actor *)LookupFormByID(p->pimpl->formID);
				if (actor == nullptr)
					return;
				const auto pos = p->GetPos();
				if (PlayerCamera::GetSingleton()->IsInScreen(pos)
					|| PlayerCamera::GetSingleton()->IsInScreen(pos + NiPoint3{ 0,0,64 })
					|| PlayerCamera::GetSingleton()->IsInScreen(pos + NiPoint3{ 0,0,128 }))
				{
				}
				else
					return;
			}

			for (int32_t i = 0; i <= 1; ++i)
				spells.insert(p->GetEquippedSpell(i));
			spells.erase(nullptr);

			const auto md = p->GetMovementData();
			for (int32_t i = 0; i <= 1; ++i)
			{
				if (md.castStage[i] != ci::MovementData::CastStage::None)
				{
					auto activeSpell = p->GetEquippedSpell(i);
					if (activeSpell != nullptr)
					{
						++worldSpellElection[activeSpell];
						if (worldSpellElection[activeSpell] > max)
						{
							max = worldSpellElection[activeSpell];
							bestSpell = activeSpell;

							auto actor = (Actor *)LookupFormByID(p->pimpl->formID);
							if (actor != nullptr)
								ci::WorldSpell::SetCasterRealEquippedSpell(
									sd::GetEquippedSpell(actor, !i)
								);
						}
						break;
					}
				}
			}

		});

		if (bestSpell != nullptr)
		{
			ci::WorldSpell::Set(bestSpell, max, (uint32_t)spells.size());
		}
	}

	RemotePlayer::RemotePlayer(const std::wstring &name,
		const LookData &lookData,
		NiPoint3 spawnPoint,
		uint32_t cellID,
		uint32_t worldSpaceID,
		OnHit onHit,
		const std::string &engineName,
		OnActivate onActivate) : pimpl(new Impl)
	{
		ci::CommonEvents::Hook();

		std::lock_guard<dlf_mutex> l(gMutex);
		std::lock_guard<dlf_mutex> l1(pimpl->mutex);

		allRemotePlayers.insert(this);

		pimpl->name = name;
		pimpl->lookData = lookData;
		pimpl->movementData.pos = spawnPoint;
		pimpl->currentNonExteriorCell = CellUtil::LookupNonExteriorCellByID(cellID);
		pimpl->worldSpaceID = worldSpaceID;
		pimpl->onHit = onHit;
		pimpl->onActivate = onActivate;
		pimpl->engine.reset(new ci::RPEngineInput(this));
		this->SetEngine(engineName);

		AVData avData;
		avData.base = 0;
		avData.modifier = 0;
		avData.percentage = 1;
		this->UpdateAVData("invisibility", avData);
	}

	RemotePlayer::~RemotePlayer()
	{
		std::lock_guard<dlf_mutex> l(gMutex);
		std::lock_guard<dlf_mutex> l1(pimpl->mutex);

		allRemotePlayers.erase(this);

		if (pimpl->formID != NULL)
			WorldCleaner::GetSingleton()->SetFormProtected(pimpl->formID, false);
		if (currentSpawning == this)
			currentSpawning = nullptr;
		if (currentFixingGreyFace == this)
			currentFixingGreyFace = nullptr;

		auto myPseudoFox = pimpl->myPseudoFox;
		if (myPseudoFox != nullptr)
		{
			--numInvisibleFoxes;
			std::thread([=] {
				delete myPseudoFox;
			}).detach();
		}

		auto dispenser = pimpl->dispenser;
		if (dispenser != nullptr)
		{
			std::thread([=] {
				delete dispenser;
			}).detach();
		}
	}

	void RemotePlayer::AsyncFoxDestroy()
	{
		std::thread([=] {
			std::lock_guard<dlf_mutex> l1(gMutex);
			if (allRemotePlayers.find(this) != allRemotePlayers.end())
			{
				std::lock_guard<dlf_mutex> l1(pimpl->mutex);
				if (pimpl->myPseudoFox != nullptr)
				{
					delete pimpl->myPseudoFox;
					pimpl->myPseudoFox = nullptr;
					--numInvisibleFoxes;
				}
			}
		}).detach();
	}

	void RemotePlayer::AsyncFoxCreate()
	{
		std::thread([=] {
			std::lock_guard<dlf_mutex> l1(gMutex);
			if (allRemotePlayers.find(this) != allRemotePlayers.end())
			{
				std::lock_guard<dlf_mutex> l1(pimpl->mutex);
				if (pimpl->myPseudoFox == nullptr)
				{
					pimpl->myPseudoFox = new ci::Object(0, ID_TESObjectSTAT::XMarkerHeading, this->GetLocationID(), { 0,0,0 }, { 0,0,0 });
					pimpl->myPseudoFox->SetMotionType(Object::Motion_Keyframed);
					++numInvisibleFoxes;
				}
			}
		}).detach();
	}

	void RemotePlayer::CreateGnomes()
	{
		for (int32_t i = 0; i <= 1; ++i)
		{
			if (pimpl->gnomes[i] == nullptr)
			{
				static TESNPC *gnomeNpc = nullptr;
				if (gnomeNpc == nullptr)
				{
					static auto exampleNpc = AllocateNPC();
					gnomeNpc = (TESNPC *)LookupFormByID(ID_TESNPC::WEAdventurerBattlemageBretonMFire);
					gnomeNpc->TESSpellList::unk04->numSpells = 0;
					gnomeNpc->TESSpellList::unk04->spells = nullptr;
					gnomeNpc->combatStyle = exampleNpc->GetCombatStyle();
					gnomeNpc->voiceType = exampleNpc->GetVoiceType();
					gnomeNpc->TESAIForm::unk10 = exampleNpc->TESAIForm::unk10;
				}

				gnomeNpc->defaultOutfit = gnomeNpc->sleepOutfit = nullptr;
				gnomeNpc->TESContainer::numEntries = 0;
				gnomeNpc->TESContainer::entries = nullptr;
				gnomeNpc->race = (TESRace *)LookupFormByID(0x00071E6A);
				gnomeNpc->height = 0.1f;
				gnomeNpc->TESSpellList::unk04->numSpells = 0;
				gnomeNpc->TESSpellList::unk04->spells = 0;
				gnomeNpc->TESSpellList::unk04->numShouts = 0;
				gnomeNpc->TESSpellList::unk04->shouts = 0;
				gnomeNpc->numHeadParts = 0;
				gnomeNpc->headparts = 0;

				pimpl->gnomes[i] = std::unique_ptr<SimpleRef>(new SimpleRef(gnomeNpc, { 0,0,0 }, SyncOptions::GetRespawnRadius(0)));
				const auto name = this->GetName();
				pimpl->gnomes[i]->TaskPersist([=](TESObjectREFR *ref) {
					RunGnomeBehavior(ref, name);
				});
			}
		}
	}

	void RemotePlayer::DestroyGnomes()
	{
		for (int32_t i = 0; i <= 1; ++i)
			pimpl->gnomes[i] = nullptr;
	}

	void RemotePlayer::UpdateNonSpawned()
	{
		SAFE_CALL("RemotePlayer", [&] {
			pimpl->nicknameLabel = nullptr;
			pimpl->visualMagicEffect = nullptr;
		});

		if (Utility::IsForegroundProcess())
		{
			const NiPoint3 localPlPos = cd::GetPosition(g_thePlayer);
			TESObjectCELL *const localPlCell = sd::GetParentCell(g_thePlayer);
			const bool isInterior = localPlCell ? sd::IsInterior(localPlCell) : false;
			const FormID worldSpaceID = sd::GetWorldSpace(g_thePlayer) ? sd::GetWorldSpace(g_thePlayer)->formID : NULL;

			static clock_t *firstSpawnAttempt = nullptr;
			if (firstSpawnAttempt == nullptr)
				firstSpawnAttempt = new clock_t{ clock() };

			pimpl->usl1 = *firstSpawnAttempt + 2333 < clock();
			pimpl->usl2 = NiPoint3{ pimpl->movementData.pos - localPlPos }.Length() < SyncOptions::GetRespawnRadius(isInterior);
			pimpl->usl3 = (pimpl->currentNonExteriorCell == CellUtil::GetParentNonExteriorCell(g_thePlayer) || pimpl->currentNonExteriorCell == nullptr);
			pimpl->usl4 = pimpl->worldSpaceID == worldSpaceID;
			pimpl->usl5 = ci::PlaceMarker::GetFormID() != 0;


			//ci::Chat::AddMessage(pimpl->name + L"1");

			if (pimpl->usl1
				&& pimpl->usl2
				&& pimpl->usl3
				&& pimpl->usl4
				&& pimpl->usl5)
			{
				//ci::Chat::AddMessage(pimpl->name + L"2");

				if (clock() - lastForceSpawn > 2333 && lastForceSpawn != 0)
				{
					currentSpawning = nullptr;
					currentSpawningBaseID = 0;
					ci::Log("WARN:RemotePlayer bad currentSpawning");
				}

				if (currentSpawning == nullptr && currentSpawningBaseID == NULL)
				{
					//ci::Chat::AddMessage(pimpl->name + L"3");

					auto npc = this->AllocateNPC();
					if (pimpl->baseNpc != nullptr)
					{
						npc->race = pimpl->baseNpc->GetRace();
					}

					bool successfullForceSpawn;

					try {
						this->ForceSpawn(npc);
						successfullForceSpawn = (pimpl->spawnStage != SpawnStage::NonSpawned);
					}
					catch (...) {
						successfullForceSpawn = false;
					}
					if (successfullForceSpawn)
					{
						currentSpawning = this;
						lastForceSpawn = clock();
						currentSpawningBaseID = npc->GetFormID();
						WorldCleaner::GetSingleton()->SetFormProtected(currentSpawningBaseID, true);
					}
					return;
				}
			}
		}
	}

	void RemotePlayer::UpdateSpawning()
	{
		pimpl->eqLast = {};
		for (int32_t i = 0; i <= 1; ++i)
			pimpl->isMagicAttackStarted[i] = false;


		if (currentSpawning != this)
		{
			pimpl->spawnStage = SpawnStage::NonSpawned;
			return;
		}

		const auto timeoutMs = SyncOptions::GetSingleton()->GetInt("SPAWNING_TIMEOUT");
		if (timeoutMs != NULL && lastForceSpawn + timeoutMs < clock())
		{
			ErrorHandling::SendError("ERROR:RemotePlayer Spawning %s failed",
				WstringToString(this->GetName()).data());

			currentSpawning = nullptr;
			pimpl->spawnStage = SpawnStage::NonSpawned;

			allRemotePlayers.erase(this);
			new (this) RemotePlayer(this->GetName(), this->GetLookData(), this->GetPos(), this->GetCell(), this->GetWorldSpace(), pimpl->onHit, this->GetEngine(), pimpl->onActivate);
		}
	}

	void RemotePlayer::UpdateGnomes(Actor *actor)
	{
		for (int32_t i = 0; i <= 1; ++i)
		{
			auto &gnome = pimpl->gnomes[i];
			if (gnome != nullptr)
			{
				NiPoint3 newPos;

				if (this->NeedsGnome(i))
				{
					newPos = Utility::GetNodeWorldPosition(actor, i ? "NPC L Hand [LHnd]" : "NPC R MagicNode [RMag]", false);

					const float angleRad = this->GetAngleZ() / 180 * acos(-1);
					float distance = SyncOptions::GetSingleton()->GetFloat("HANDGNOME_OFFSET_FORWARD_FROM_HAND");

					auto isCasting = [&](int32_t i) {
						return this->GetMovementData().castStage[i] != ci::MovementData::CastStage::None;
					};
					if (isCasting(!i) && !isCasting(i))
						newPos.z -= 48;

					if (sd::GetEquippedSpell(actor, !i) == nullptr || !actor->IsWeaponDrawn())
						distance += 48;
					else if (i == 1)
						distance += 44;
					newPos += {distance * sin(angleRad), distance * cos(angleRad), 0};
					newPos.z += SyncOptions::GetSingleton()->GetFloat("HANDGNOME_OFFSET_Z_FROM_HAND");
				}
				else
				{
					newPos = { 0, 0, -1000.f * 1000.f * 1000.f };
				}

				gnome->SetPos(newPos);
				gnome->SetRot({ 0,0,this->GetAngleZ() });

				static std::map<uint32_t, clock_t> lastBehaviorUpdate;
				lastBehaviorUpdate.erase(NULL);
				lastBehaviorUpdate.erase(~0);
				if (newPos.z > -100000000 && clock() - lastBehaviorUpdate[gnome->GetFormID()] > 12000)
				{
					const auto name = this->GetName();
					gnome->TaskSingle([name](TESObjectREFR *ref) {
						RunGnomeBehavior(ref, name);
					});
					lastBehaviorUpdate[gnome->GetFormID()] = clock();
				}
			}
		}
	}

	void RemotePlayer::UpdateNickname(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {
			if (this->IsDerived() == false)
			{
				const NiPoint3 offset = { 0, 0, 128 + 16 };
				auto nicknamePos = cd::GetPosition(actor) + offset;

				const auto headPos = Utility::GetNodeWorldPosition(actor, "NPC Head [Head]", false);
				if (headPos.Length() > 0)
				{
					nicknamePos = headPos;
					nicknamePos += {0, 0, 22};
				}

				//ci::Chat::AddMessage(this->GetName() + L" " + std::to_wstring(nicknamePos.x) + L" " + std::to_wstring(this->GetPos().x));
				// ¬ыведет крайне различающиес€ числа, если человек исчез. 
				// Ќапример, в ¬айтране x позици€ ноды может быть 60, 228, а x добытый через интерфейс this->GetPos() нормальный
				// ≈сли человек не исчез, то числа мало различаютс€
				// Ќа основе этого делаетс€ следующий фикс

				if (std::abs(nicknamePos.x - this->GetPos().x) > 1000.0)
				{
					this->ForceDespawn(L"Bad Node pos 'NPC Head [Head]' (1.0.34d)");
					return;
				}

				if (pimpl->nicknameLabel == nullptr && pimpl->nicknameEnabled && pimpl->hasLOS)
					pimpl->nicknameLabel.reset(new ci::Text3D(this->GetName(), nicknamePos));

				if (pimpl->nicknameLabel != nullptr && (!pimpl->nicknameEnabled || !pimpl->hasLOS))
					pimpl->nicknameLabel.reset();


				if (pimpl->nicknameLabel != nullptr)
				{
					pimpl->nicknameLabel->SetPos(nicknamePos);
					if (pimpl->nicknameLabel->GetText() != this->GetName())
						pimpl->nicknameLabel->SetText(this->GetName());
					pimpl->nicknameLabel->SetDrawDistance(this->GetNicknameDrawDistance());
				}
			}
		});
	}

	void RemotePlayer::UpdatePerks(Actor *actor)
	{
		// ...
	}

	void RemotePlayer::DeleteProjectile(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {
			if (this->IsBowEquipped())
			{
				if (pimpl->syncState.last.attackState != this->GetMovementData().attackState)
				{
					auto ammoItemType = this->GetEquippedAmmo();
					if (ammoItemType != nullptr)
					{
						auto ammoForm = (TESAmmo *)LookupFormByID(ammoItemType->GetFormID());
						if (ammoForm != nullptr)
						{
							const auto pos = this->GetPos();
							SET_TIMER_LIGHT(30, [=] {
								auto ref = sd::FindClosestReferenceOfType(ammoForm->settings.projectile, pos.x, pos.y, pos.z, 256);
								if (ref != nullptr)
									sd::Delete(ref);
							});
						}
					}
				}
			}
		});
	}

	void RemotePlayer::UpdateMovement(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {
			for (int32_t i = 0; i <= 1; ++i)
				pimpl->syncState.isWorldSpell[i] = (pimpl->eq.handsMagic[i] == ci::WorldSpell::Get());
			this->ApplyMovementDataImpl();
		});
	}

	void RemotePlayer::ApplyHitAnimations(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {
			if (!pimpl->hitAnimsToApply.empty())
			{
				auto hitAnimID = pimpl->hitAnimsToApply.front();
				pimpl->hitAnimsToApply.pop();
				AnimData_::Apply(actor, hitAnimID, pimpl->syncState.fullyUnsafeSync);
				if (AnimData_::IsPowerAttack(hitAnimID))
					pimpl->syncState.forceFixAfterHitAnim = true;
			}
		});
	}

	void RemotePlayer::ApplyActorValues(Actor *actor)
	{
		auto applyAV = [&](char *avNameLowerCase) {
			enum {
				InternalMult = 10000,
			};
			const auto full = (pimpl->avData[avNameLowerCase].base + pimpl->avData[avNameLowerCase].modifier) * InternalMult;
			const auto dest = full * pimpl->avData[avNameLowerCase].percentage;
			sd::SetActorValue(actor, avNameLowerCase, full);
			auto current = sd::GetActorValue(actor, avNameLowerCase);
			auto change = dest - current;
			if (change > 0)
				sd::RestoreActorValue(actor, avNameLowerCase, change);
			else
				sd::DamageActorValue(actor, avNameLowerCase, -change);
		};
		applyAV("health");
		applyAV("stamina");
		applyAV("magicka");

		sd::SetActorValue(actor, "invisibility",
			pimpl->avData["invisibility"].base + pimpl->avData["invisibility"].modifier);
	}

	void RemotePlayer::ManageMyFox(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {

			auto pointAtSphere = [](float angleZ, float aimingAngle, float r)->NiPoint3 {
				auto toRad = [](float v) {
					return v / 180 * acos(-1);
				};
				return { float(r * cos(-toRad(-90.f + angleZ)) * sin(toRad(aimingAngle + 90))),
					float(r * sin(-toRad(-90.f + angleZ)) * sin(toRad(aimingAngle + 90))),
					float(r * cos(toRad(aimingAngle + 90)))
				};
			};

			pimpl->syncState.myFoxID = 0;
			if (pimpl->myPseudoFox != nullptr)
			{
				const float r = 600;
				const auto myMd = this->GetMovementData();
				auto pos = myMd.pos + pointAtSphere(myMd.angleZ, myMd.aimingAngle, r);

				if (this->IsSpellEquipped())
					pos.z += SyncOptions::GetSingleton()->GetFloat("INVISIBLE_FOX_OFFSET_Z_MAGIC");

				pimpl->myPseudoFox->SetPosition(pos);
				auto baseForm = LookupFormByID(pimpl->myPseudoFox->GetBase());
				if (baseForm != nullptr)
				{
					auto ref = sd::Game::FindClosestReferenceOfType(baseForm, pos.x, pos.y, pos.z, 128.0);
					if (ref != nullptr)
						pimpl->syncState.myFoxID = ref->formID;
				}
			}
		});
	}

	void RemotePlayer::UpdateDispenser(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {
			if (this->IsBowEquipped() || this->IsSpellEquipped())
			{
				if (pimpl->dispenser == nullptr)
				{
					std::thread([=] {
						std::lock_guard<dlf_mutex> l1(gMutex);
						if (allRemotePlayers.find(this) != allRemotePlayers.end())
						{
							std::lock_guard<dlf_mutex> l1(pimpl->mutex);
							if (pimpl->dispenser == nullptr)
							{
								pimpl->dispenser = new ci::Object(0, ID_TESObjectSTAT::XMarkerHeading, this->GetLocationID(), { 0,0,0 }, { 0,0,0 });
								pimpl->dispenser->SetMotionType(Object::Motion_Keyframed);
							}
						}
					}).detach();
				}
				if (pimpl->dispenser != nullptr)
				{
					enum Var {
						OffsetZSneaking,
						OffsetZ,
						OffsetDistance,
						NumVars,
					};

					enum DispenserMode {
						DispenserModeBow,
						DispenserModeMagic,
						NumModes,
					};

					static const char *vars[NumVars][NumModes] = {
						{ "DISPENSER_OFFSET_Z_SNEAKING",	"MAGIC_DISPENSER_OFFSET_Z_SNEAKING" },
						{ "DISPENSER_OFFSET_Z",				"MAGIC_DISPENSER_OFFSET_Z" },
						{ "DISPENSER_OFFSET_DISTANCE",		"MAGIC_DISPENSER_OFFSET_DISTANCE" },
					};

					const DispenserMode mode = this->IsSpellEquipped() ? DispenserModeMagic : DispenserModeBow;

					auto pos = this->GetPos();
					pos += {0, 0, this->GetMovementData().isSneaking ?
						SyncOptions::GetSingleton()->GetFloat(vars[OffsetZSneaking][mode]) :
						SyncOptions::GetSingleton()->GetFloat(vars[OffsetZ][mode])
					};
					const float distance = SyncOptions::GetSingleton()->GetFloat(vars[OffsetDistance][mode]);
					const float angleRad = this->GetAngleZ() / 180 * acos(-1);
					pos += {distance * sin(angleRad), distance * cos(angleRad), 0};

					const auto rot = NiPoint3{
						(float)this->GetMovementData().aimingAngle,
						0.0f,
						this->GetAngleZ()
					};

					pimpl->dispenser->SetLocation(this->GetLocationID());
					pimpl->dispenser->TranslateTo(pos, rot, 10000.f, 10000.f);
				}
			}
			else
			{
				if (pimpl->dispenser != nullptr)
					pimpl->dispenser->SetLocation(-1);
			}
		});
	}

	void RemotePlayer::CreateDestroyMyFox(Actor *actor)
	{
		if (SyncOptions::GetSingleton()->GetInt("INVISIBLE_FOX_ENGINE") == (int32_t)InvisibleFoxEngine::Object)
		{
			if (this->IsSpellEquipped() || this->IsBowEquipped())
			{
				if (pimpl->myPseudoFox == nullptr
					&& numInvisibleFoxes < SyncOptions::GetSingleton()->GetInt("MAX_INVISIBLE_FOXES")
					&& sd::HasLOS(g_thePlayer, actor))
				{
					this->AsyncFoxCreate();
				}
			}
			else
			{
				if (pimpl->myPseudoFox != nullptr)
					this->AsyncFoxDestroy();
			}
		}
	}

	void RemotePlayer::ManageMagicEquipment(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {

			auto handsMagicProxy = pimpl->handsMagicProxy;

			if (handsMagicProxy[0] != handsMagicProxy[1] && handsMagicProxy[0] && handsMagicProxy[1])
				handsMagicProxy[1] = handsMagicProxy[0];

			for (int32_t i = 0; i <= 1; ++i)
			{
				if (pimpl->eq.handsMagic[i] != handsMagicProxy[i])
				{
					if (clock() - pimpl->lastMagicEquipmentChange > 750)
					{
						pimpl->eq.handsMagic[i] = handsMagicProxy[i];
						pimpl->lastMagicEquipmentChange = clock();
					}
				}
			}
		});
	}

	void RemotePlayer::ApplyEquipmentHands(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {

			auto isAiming = [=] {
				std::lock_guard<dlf_mutex> l(gMutex);
				if (allRemotePlayers.find(this) == allRemotePlayers.end())
					return false;

				auto md = this->GetMovementData();

				auto getIsAiming = [](const ci::MovementData &md) {
					enum AttackState {
						BowClick = 8,
						BowDrawing = 9,
						BowHoldingAShot = 10,
						BowRelease = 11,
						BowReleased = 12,
					};
					return md.attackState == BowClick
						|| md.attackState == BowDrawing
						|| md.attackState == BowHoldingAShot
						|| md.attackState == BowRelease;
				};

				return getIsAiming(md);
			};

			if ((pimpl->eq.hands != pimpl->eqLast.hands && clock() - pimpl->lastWeaponsUpdate > 300)
				|| (pimpl->eq.handsMagic != pimpl->eqLast.handsMagic))
			{
				Equipment_::Equipment eq;
				for (int32_t i = 0; i <= 1; ++i)
				{
					TESForm *form = nullptr;
					if (pimpl->eq.hands[i])
					{
						form = LookupFormByID(pimpl->eq.hands[i]->GetFormID());
					}
					else if (pimpl->eq.handsMagic[i])
					{
						form = LookupFormByID(pimpl->eq.handsMagic[i]->GetFormID());
					}
					eq.hands[i] = form;
				}

				Equipment_::ApplyHands(actor, eq, isAiming);
				pimpl->eqLast.hands = pimpl->eq.hands;
				pimpl->eqLast.handsMagic = pimpl->eq.handsMagic;
				pimpl->lastWeaponsUpdate = clock();
			}
		});
	}

	void RemotePlayer::ApplyEquipmentOther(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {
			//if (sd::HasLOS(g_thePlayer, actor))
			{
				Equipment_::Equipment eq;
				if (pimpl->eq.ammo != nullptr)
					eq.other.insert(LookupFormByID(pimpl->eq.ammo->GetFormID()));
				for (auto &item : pimpl->eq.armor)
				{
					const bool emptyLeftHand = pimpl->eq.hands[1] == nullptr &&
						(!pimpl->eq.hands[0]
							|| pimpl->eq.hands[0]->GetSubclass() < ci::ItemType::Subclass::WEAP_Greatsword
							|| pimpl->eq.hands[0]->GetSubclass() > ci::ItemType::Subclass::WEAP_Crossbow);

					const bool dontEquipPlease = item->GetSubclass() == ci::ItemType::Subclass::ARMO_Shield && !emptyLeftHand;

					if (!dontEquipPlease)
					{
						eq.other.insert(LookupFormByID(item->GetFormID()));
					}
					else
					{
						static bool reported = false;
						if (reported == false)
						{
							reported = true;
							ErrorHandling::SendError("WARN:RemotePlayer Incorrect equipment");
						}
					}
				}

				// Prevent breaking SetWerewolf 
				static auto wwSkin = LookupFormByID(0x000F6002);
				if (sd::IsEquipped(actor, wwSkin))
				{
					eq.other.insert(wwSkin);
				}

				Equipment_::ApplyOther(actor, eq);
				pimpl->eqLast.armor = pimpl->eq.armor;
				pimpl->eqLast.ammo = pimpl->eq.ammo;
			}
		});
	}

	void RemotePlayer::DespawnIfNeed(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {
			if (this->IsDerived() == false && pimpl->baseNpc == nullptr && pimpl->height != ((TESNPC *)actor->baseForm)->height)
				this->ForceDespawn(L"Despawned: Height changed");
		});

		SAFE_CALL("RemotePlayer", [&] {
			if (pimpl->syncState.fatalErrors != 0 && clock() - pimpl->lastDespawn > 50)
			{
				//this->ForceDespawn(L"Despawned: Fatal Error in Sync");
				ci::Log(L"Fatal error in sync.");
				pimpl->syncState.fatalErrors = 0;
				pimpl->lastDespawn = clock();
				pimpl->broken = true;
				errorsInSpawn++; // should i uncomment ?
				ci::PlaceMarker::SetFormID(0);
			}
		});
		
		// this reason to respawn is pretty buggy
		/*SAFE_CALL("RemotePlayer", [&] {
			if (pimpl->currentNonExteriorCell != CellUtil::GetParentNonExteriorCell(g_thePlayer))
				this->ForceDespawn(L"Despawned: Cell changed");
		});*/

		SAFE_CALL("RemotePlayer", [&] {
			const FormID worldSpaceID = sd::GetWorldSpace(g_thePlayer) ? sd::GetWorldSpace(g_thePlayer)->formID : NULL;
			if (pimpl->worldSpaceID != worldSpaceID && worldSpaceID != NULL && pimpl->worldSpaceID != NULL)
				this->ForceDespawn(std::wstring(L"Despawned: WorldSpace changed " + std::to_wstring(worldSpaceID) + L' ' + std::to_wstring(pimpl->worldSpaceID)).data());
		});

		SAFE_CALL("RemotePlayer", [&] {
			const NiPoint3 localPlPos = cd::GetPosition(g_thePlayer);
			const bool isInterior = sd::GetParentCell(g_thePlayer) ? sd::IsInterior(sd::GetParentCell(g_thePlayer)) : false;
			if (NiPoint3{ this->GetMovementData().pos - localPlPos }.Length() >= SyncOptions::GetRespawnRadius(isInterior))
				this->ForceDespawn(L"Despawned: Player is too far");
		});

		SAFE_CALL("RemotePlayer", [&] {
			if (sd::IsDead(actor) && this->GetAVData("Health").percentage > 0.0)
				this->ForceDespawn(L"Despawned: Fake death");
		});
	}

	clock_t lastGreyFaceFix = 0;
	clock_t lastTintMaskUse = 0;

	void RemotePlayer::FixGreyFaceBugUnsafe(Actor *actor) // 1.0.1 shitfix
	{
		if (!pimpl->greyFaceFixedUnsafe && currentFixingGreyFace == nullptr
			&& pimpl->baseNpc == nullptr
			&& clock() - lastGreyFaceFix > 2000
			&& clock() - pimpl->spawnMoment > /*3500*/0
			&& clock() - lastTintMaskUse > 5000)
		{
			lastTintMaskUse = clock();
			lastGreyFaceFix = clock();
			actor->baseForm->formID = g_thePlayer->baseForm->formID; // AAAAAA
			lookSync->ApplyTintMasks((TESNPC *)actor->baseForm, pimpl->lookData.tintmasks);
			actor->QueueNiNodeUpdate(true);
			pimpl->greyFaceFixedUnsafe = true;
			pimpl->greyFaceFixed = true;
			sd::StopCombat(actor);
			actor->DrawSheatheWeapon(false);
		}
	}

	void RemotePlayer::FixGreyFaceBug(Actor *actor)
	{
		if (!pimpl->greyFaceFixed && currentFixingGreyFace == nullptr
			&& pimpl->baseNpc == nullptr)
		{
			bool success = false;
			SAFE_CALL_RETURN("RemotePlayer", [&] {
				currentFixingGreyFace = this;
				actor->baseForm->formID = g_thePlayer->baseForm->formID; // AAAAAA
				lookSync->ApplyTintMasks((TESNPC *)actor->baseForm, pimpl->lookData.tintmasks);
				pimpl->greyFaceFixed = true;
				sd::Disable(actor, false);
				lastGreyFaceFix = clock();
				return true;
			}, success);
			if (!success)
				currentFixingGreyFace = nullptr;

			const auto formID = pimpl->formID;
			const bool isInterior = pimpl->currentNonExteriorCell != nullptr; //-V561
			SET_TIMER(0, [=] {
				auto actor = (Actor *)LookupFormByID(formID);
				if (actor != nullptr)
				{
					sd::Enable(actor, false);
					cd::KeepOffsetFromActor(cd::Value<Actor>(formID), cd::Value<Actor>(formID), 0, 0, 0, 0, 0, 0, 1, 1);
				}
				SET_TIMER(200, [=] {
					if (this == currentFixingGreyFace)
						currentFixingGreyFace = nullptr;

					auto actor = (Actor *)LookupFormByID(formID);
					if (!actor)
						return;

					//LookData_::PreventCrash((TESNPC *)actor->baseForm);
				});
			});
		}
	}

	void RemotePlayer::UpdateSpawned()
	{
		if (pimpl->stopProcessing)
			return;
		auto actor = (Actor *)LookupFormByID(pimpl->formID);
		if (!actor)
			return this->ForceDespawn(L"Despawned: Unloaded by the game");

		{
			if (clock() - pimpl->spawnMoment > 2500
				&& sd::Is3DLoaded(actor) == false)
			{
				return this->ForceDespawn(L"Despawned: 3D isn't loaded (1.0.30)");
			}

			auto myCell = sd::GetParentCell(g_thePlayer);
			auto actorCell = sd::GetParentCell(actor);
			if (myCell && myCell->IsInterior() && myCell != actorCell)
			{
				return this->ForceDespawn(L"Despawned: Cell changed (1.0.28e)");
			}
		}

		if (clock() - pimpl->lastLosCheck > 300)
		{
			pimpl->hasLOS = sd::HasLOS(g_thePlayer, actor);
			pimpl->lastLosCheck = clock();

			if (sd::GetWorldSpace(g_thePlayer) == LookupFormByID(0x3c)) // tamriel
			{
				const bool posInScreen = PlayerCamera::GetSingleton()->IsInScreen(this->GetPos())
					&& PlayerCamera::GetSingleton()->IsInScreen(this->GetPos() += {0, 0, 128});

				if (posInScreen && !pimpl->hasLOS)
				{
					//return this->ForceDespawn(L"Despawned: No LOS (1.0.33)");
				}
			}
		}

		static auto getIdentifier = [](TESForm *form) {
			if (form == nullptr)
				return std::string{};
			std::string iden = form->GetName();
			if (iden.empty())
			{
				auto tesFullName = DYNAMIC_CAST<TESFullName *, TESForm *>(form);
				if (tesFullName != nullptr)
					iden = tesFullName->GetFullName();
			}
			if (iden.empty())
			{
				auto refr = DYNAMIC_CAST<TESObjectREFR *, TESForm *>(form);
				if (refr != nullptr)
				{
					auto xTextData = refr->extraData.GetByType<ExtraTextDisplayData>();
					if (xTextData != nullptr)
						iden = xTextData->name.c_str();
				}
			}
			if (iden.empty())
			{
				std::stringstream idss;
				idss << std::hex;
				idss << form->GetFormID();
				iden = idss.str();
			}
			for (size_t i = 0; i != iden.size(); ++i)
			{
				if (iden[i] == *"\"")
					iden[i] = *"'";
			}
			return iden;
		};
		auto ct = sd::GetCombatTarget(actor);
		std::string target = "None";
		if (ct != nullptr)
		{
			target = getIdentifier(ct->baseForm);
		}
		//ci::Chat::AddMessage(this->GetName() + L"'s combat target is " + StringToWstring(target));

		bool brk = false;
		SAFE_CALL("RemotePlayer", [&] {
			if (pimpl->baseNpc == nullptr)
			{
				const auto currentRace = sd::GetRace(actor);
				enum {
					WerewolfRace = 0x000CDD84,
				};

				if (pimpl->spawnMoment + 1000 < clock()
					&& ((currentRace->formID != WerewolfRace && currentRace->formID != this->pimpl->lookData.raceID) || 0 != memcmp(((TESNPC *)actor->baseForm)->faceMorph->option, &this->pimpl->lookData.options[0], sizeof LookData().options))
					&& this->pimpl->lookData.isEmpty() == false)
				{
					/*auto lookData = this->pimpl->lookData;
					this->pimpl->lookData = {};
					this->ApplyLookData(lookData);
					brk = true;*/
					// «ачем вообще нужен был этот костыль???
				}
			}
		});
		if (brk)
			return;

		SAFE_CALL("RemotePlayer", [&] {
			pimpl->movementDataOut = MovementData_::Get(actor);
			if ((pimpl->movementData.pos - pimpl->movementDataOut.pos).Length() < 128)
			{
				pimpl->fullySpawned = true;
			}
			if (pimpl->angleTask)
			{
				pimpl->angleTask(actor);
				pimpl->angleTask = nullptr;
			}
			if (pimpl->posTask)
			{
				pimpl->posTask(actor);
				pimpl->posTask = nullptr;
			}
		});


		if (pimpl->engineTask && clock() - pimpl->spawnMoment > 2500)
		{
			pimpl->engineTask();
			pimpl->engineTask = nullptr;
		}

		if (sd::IsDead(actor))
		{
			static std::set<Actor *> dead;
			if (dead.count(actor) == 0)
			{
				dead.insert(actor);
				auto s = WstringToString(this->GetName()) + " ";
				auto fixedStr = new BSFixedString(s.data());
				actor->SetDisplayName(*fixedStr, true);
				sd::AllowPCDialogue(actor, true);

				// Prevent local weap drop
				for (int32_t i = 0; i <= 1; ++i)
				{
					auto weap = sd::GetEquippedWeapon(actor, i != 0);
					if (weap)
					{
						sd::RemoveItem(actor, weap, -1, true, nullptr);
					}
				}
			}
		}

		if (pimpl->engine != nullptr)
		{
			pimpl->engine->EngineUpdateSpawned(actor);
			pimpl->engine->EngineApplyFactions(actor);
		}
		else
			ErrorHandling::SendError("ERROR:RemotePlayer Nullptr engine");

		this->UpdateNickname(actor);
		this->UpdatePerks(actor);

		if (pimpl->greyFaceFixed || pimpl->baseNpc != nullptr)
		{
			if (this->NeedsGnome(0) || this->NeedsGnome(1))
				this->CreateGnomes();
			this->UpdateGnomes(actor);
			this->DeleteProjectile(actor);
			this->UpdateMovement(actor);
			this->ApplyHitAnimations(actor);
			this->ApplyActorValues(actor);
		}

		this->ManageMyFox(actor);

		if (pimpl->timer250ms + 250 < clock())
		{
			pimpl->timer250ms = clock();

			// 'lastOutOfPos'
			SAFE_CALL("RemotePlayer", [&] {
				if ((this->GetPos() - cd::GetPosition(actor)).Length() > SyncOptions::GetSingleton()->GetFloat("MAX_OUT_OF_POS"))
					pimpl->lastOutOfPos = clock();
			});

			this->UpdateDispenser(actor);
			this->CreateDestroyMyFox(actor);

			this->ManageMagicEquipment(actor);

			this->ApplyEquipmentHands(actor);
			this->ApplyEquipmentOther(actor);

			this->DespawnIfNeed(actor);
			if (pimpl->spawnStage == SpawnStage::NonSpawned)
				return;
		}

		this->FixGreyFaceBug(actor);
		this->FixGreyFaceBugUnsafe(actor);
	}

	void RemotePlayer::Update()
	{
		std::lock_guard<dlf_mutex> l1(pimpl->mutex);

		const auto spawnStage = pimpl->spawnStage;

		try {
			switch (spawnStage)
			{
			case SpawnStage::NonSpawned:
				this->UpdateNonSpawned();
				break;

			case SpawnStage::Spawning:
				this->UpdateSpawning();
				break;

			case SpawnStage::Spawned:
				this->UpdateSpawned();
				break;
			}
		}
		catch (...) {
			ErrorHandling::SendError("ERROR:MT:RemotePlayer '%s' %d ", WstringToString(this->GetName()).data(), (int32_t)spawnStage);
		}
	}

	void RemotePlayer::Update_OT()
	{
		if (MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
			return this->ForceDespawn(L"Despawned: Game exit");
		if (!Utility::IsForegroundProcess() && GameSettings::IsFullScreen() && !this->IsDerived())
			return this->ForceDespawn(L"Despawned: Window isn't active");
	}

	void RemotePlayer::ForceSpawn(TESNPC *npc)
	{
		//ci::Log(pimpl->name + L"5");
		const bool isDerived = this->IsDerived(); // Do not move IsDerived() call to SET_TIMER_LIGHT !

												  //SAFE_CALL("RemotePlayer", [&] {
		if (npc == nullptr)
		{
			ErrorHandling::SendError("ERROR:RemotePlayer Null NPC");
			return;
		}
		//ci::Log(pimpl->name + L"6");

		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		if (pimpl->spawnStage != SpawnStage::NonSpawned)
			return;

		//ci::Log(pimpl->name + L"7");

		if (this->GetAVData("Health").percentage <= 0.0)
			return;

		//ci::Log(pimpl->name + L"8");

		this->DestroyGnomes();
		pimpl->spawnStage = SpawnStage::Spawning;

		auto refToPlaceAt =
			WorldCleaner::GetSingleton()->FindFarObject();
		if (refToPlaceAt && refToPlaceAt->formType != FormType::Reference)
			refToPlaceAt = nullptr;
		if (errorsInSpawn > 28 || !refToPlaceAt)
		{
			if (this->IsDerived())
				refToPlaceAt = nullptr;
			else
				refToPlaceAt = (Actor *)LookupFormByID(GhostAxeManager::GetSingleton().GetGhostAxeFormID());
			if (!refToPlaceAt)
				refToPlaceAt = g_thePlayer;
		}

		//ci::Log(pimpl->name + L"9");

		auto onPlace = [=](cd::Value<TESObjectREFR> ac) {

			auto setAVsToDefault = [](Actor *actor) {
				sd::SetActorValue(actor, "Agression", 0.0);
				sd::SetActorValue(actor, "attackdamagemult", 0.0);
				sd::SetActorValue(actor, "Variable01", rand());
				sd::SetActorValue(actor, "MagicResist", 99);
			};

			auto actor = (Actor *)LookupFormByID(ac.GetFormID());
			if (actor != nullptr)
			{
				setAVsToDefault(actor);
				sd::AddItem(actor, LookupFormByID(ID_TESSoulGem::SoulGemBlack), 100, true);
				if (pimpl->baseNpc != nullptr)
				{
					sd::Enable(actor, true);
				}
			}

			SET_TIMER_LIGHT(isDerived ? 0 : 300, [=] {
				const auto id = ac.GetFormID();
				if (LookupFormByID(id) == nullptr)
					return;

				std::lock_guard<dlf_mutex> l(gMutex);
				if (allRemotePlayers.find(this) == allRemotePlayers.end())
					return;

				std::lock_guard<dlf_mutex> l1(pimpl->mutex);

				pimpl->formID = id;
				WorldCleaner::GetSingleton()->SetFormProtected(pimpl->formID, true);
				pimpl->spawnStage = SpawnStage::Spawned;
				pimpl->spawnMoment = clock();
				pimpl->greyFaceFixed = false;
				pimpl->greyFaceFixedUnsafe = false;
				pimpl->syncState = {};

				auto actor = (Actor *)LookupFormByID(id);
				if (actor != nullptr)
				{
					setAVsToDefault(actor);

					BSFixedString name = WstringToString(pimpl->name).c_str();
					actor->SetDisplayName(name, true);

					cd::SetPosition(ac, pimpl->movementData.pos);
					sd::BlockActivation(actor, true);
					sd::AllowPCDialogue(actor, false);
					//sd::AllowPCDialogue(actor, true);

					AnimData_::Register(actor);
				}

				cd::KeepOffsetFromActor(cd::Value<Actor>(ac.GetFormID()), cd::Value<Actor>(ac.GetFormID()), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0);

				if (currentSpawning == this)
					currentSpawning = nullptr;
			});
		};

		pimpl->fullySpawned = false;

		const bool disable = (pimpl->baseNpc != nullptr);

		if (sd::IsInterior(sd::GetParentCell(g_thePlayer)))
		{
			refToPlaceAt = g_thePlayer; // from 1.0.3
		}
		auto randomRefID = ci::Object::GetRandomRefID(false);
		const bool isInterior = sd::GetParentCell(g_thePlayer) && sd::IsInterior(sd::GetParentCell(g_thePlayer));
		if (isInterior)
		{
			randomRefID = ci::Object::GetFarObject();
		}
		const auto randomRef = (TESObjectREFR *)LookupFormByID(randomRefID);
		if (randomRef != nullptr)
		{
			refToPlaceAt = randomRef;
		}

		if (SyncOptions::GetSingleton()->GetInt("UNSAFE_PLACEATME") != 0)
		{
			//SAFE_CALL("RemotePlayer", [&] {
			if (refToPlaceAt == nullptr)
				return ErrorHandling::SendError("ERORR:RemotePlayer ForceSpawn() bad argument 1");
			if (npc == nullptr)
				return ErrorHandling::SendError("ERORR:RemotePlayer ForceSpawn() bad argument 2");
			auto refr = sd::PlaceAtMe(refToPlaceAt, npc, 1, true, disable);
			sd::SetActorValue((Actor *)refr, "Variable01", rand()); // Fix magic cast on spawn
			if (refr == nullptr)
				return ErrorHandling::SendError("ERORR:RemotePlayer ForceSpawn() bad argument 3");


			cd::Value<TESObjectREFR> cdRefr;
			try {
				cdRefr = refr;
			}
			catch (...) {
				ErrorHandling::SendError("ERORR:RemotePlayer ForceSpawn() cd::Value wtf");
			}
			SET_TIMER_LIGHT(200, [cdRefr, onPlace] {
				onPlace(cdRefr);
			});
			//});
		}
		else
		{
			cd::PlaceAtMe(refToPlaceAt, npc, 1, true, disable, onPlace);
		}
		//});
	}

	void RemotePlayer::ForceDespawn(const wchar_t *reason)
	{
		ci::Log(L"DBG:RemotePlayer ForceDespawn " + std::wstring(reason));

		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		if (pimpl->spawnStage == SpawnStage::Spawned ||
			(pimpl->spawnStage == SpawnStage::Spawning && MenuManager::GetSingleton()->IsMenuOpen("Main Menu")))
		{
			WorldCleaner::GetSingleton()->SetFormProtected(pimpl->formID, false);
			pimpl->formID = 0;
			pimpl->spawnStage = SpawnStage::NonSpawned;
			pimpl->angleTask = nullptr;
			pimpl->posTask = nullptr;
			if (currentSpawning == this)
				currentSpawning = nullptr;
			if (currentFixingGreyFace == this)
				currentFixingGreyFace = nullptr;
			pimpl->hitAnimsOut = {};
			pimpl->engine->factionsState = IRemotePlayerEngine::FactionsState::Unknown;
		}
	}

	int32_t RemotePlayer::GetSyncMode() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return (int32_t)pimpl->syncState.syncMode;
	}

	int32_t RemotePlayer::GetSpawnStage() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return (int32_t)pimpl->spawnStage;
	}

	uint32_t RemotePlayer::GetRefID() const {
		return pimpl->engine->GetRefID();
	}

	bool RemotePlayer::IsBowEquipped() const
	{
		if (this->pimpl->spawnStage != SpawnStage::Spawned)
			return false;
		return (pimpl->eq.hands[0] && pimpl->eq.hands[0]->GetSubclass() == ci::ItemType::Subclass::WEAP_Bow)
			|| (pimpl->eq.hands[1] && pimpl->eq.hands[1]->GetSubclass() == ci::ItemType::Subclass::WEAP_Bow);
	}

	bool RemotePlayer::IsSpellEquipped() const
	{
		return this->GetEquippedSpell(0) != nullptr
			|| this->GetEquippedSpell(1) != nullptr;
	}

	bool RemotePlayer::NeedsGnome(int32_t i) const
	{
		return this->GetMovementData().isWeapDrawn
			&& pimpl->handsMagicProxy[i] != nullptr
			&& (pimpl->handsMagicProxy[i]->GetDelivery() != Spell::Delivery::Self)
			&& (pimpl->handsMagicProxy[i]->GetDelivery() != Spell::Delivery::TargetActor);
	}

	uint32_t RemotePlayer::GetLocationID() const
	{
		const uint32_t locationID =
			pimpl->currentNonExteriorCell ? pimpl->currentNonExteriorCell->formID : pimpl->worldSpaceID;
		return locationID;
	}

	size_t RemotePlayer::GetNumInstances()
	{
		return allRemotePlayers.size();
	}

	void RemotePlayer::UpdateAll()
	{
		std::lock_guard<dlf_mutex> l(gMutex);

		if (traceTask)
			traceTask();

		SAFE_CALL("RemotePlayer", [&] {
			*Impl::RemotePlayerKnownItems() = knownItems;
			*Impl::RemotePlayerKnownSpells() = knownSpells;
		});

		SAFE_CALL("RemotePlayer", [&] {
			ci::PlaceMarker::Update();
		});

		SAFE_CALL("RemotePlayer", [&] {
			if (currentSpawning != nullptr && allRemotePlayers.find(currentSpawning) == allRemotePlayers.end())
				currentSpawning = nullptr;
			if (currentFixingGreyFace != nullptr && allRemotePlayers.find(currentFixingGreyFace) == allRemotePlayers.end())
				currentFixingGreyFace = nullptr;
		});

		SAFE_CALL("RemotePlayer", [&] {
			if (currentSpawning == nullptr && currentSpawningBaseID != NULL)
			{
				WorldCleaner::GetSingleton()->SetFormProtected(currentSpawningBaseID, false);
				currentSpawningBaseID = NULL;
			}
		});

		SAFE_CALL("RemotePlayer", [&] {
			numSpawned = 0;
			for (auto p : allRemotePlayers)
			{
				if (p->IsSpawned())
					++numSpawned;
			}
		});

		UInt32 rating = 0;
		SAFE_CALL("RemotePlayer", [&] {
			const auto allRemotePlayers_ = allRemotePlayers;
			std::for_each(allRemotePlayers_.begin(), allRemotePlayers_.end(), [&](RemotePlayer *p) {
				if (p == nullptr)
				{
					ErrorHandling::SendError("ERROR:RemotePlayer nullptr player");
					return;
				}
				auto pos = p->GetPos();
				if (PlayerCamera::GetSingleton()->IsInScreen(pos)
					|| PlayerCamera::GetSingleton()->IsInScreen(pos + NiPoint3{ 0,0,64 })
					|| PlayerCamera::GetSingleton()->IsInScreen(pos + NiPoint3{ 0,0,128 }))
				{
					p->pimpl->rating = rating++;
				}
				SAFE_CALL("RemotePlayer", [&] {
					p->Update();
				});
			});
		});

		SAFE_CALL("RemotePlayer", [&] {
			UpdateWorldSpell();
		});

		SAFE_CALL("RemotePlayer", [&] {
			ci::WorldSpell::Apply();
		});

		SAFE_CALL("RemotePlayer", [&] {
			if (allRemotePlayers.size() > 0)
				GhostAxeManager::GetSingleton().Update();
		});
	}

	void RemotePlayer::UpdateAll_OT()
	{
		std::lock_guard<dlf_mutex> l(gMutex);
		for (auto it = allRemotePlayers.begin(); it != allRemotePlayers.end(); ++it)
		{
			if (*it)
				(*it)->Update_OT();
		}
	}

	void RemotePlayer::SetName(const std::wstring &name) {
		return pimpl->engine->SetName(name);
	}

	void RemotePlayer::SetPos(NiPoint3 pos) {
		return pimpl->engine->SetPos(pos);
	}

	void RemotePlayer::SetAngleZ(float angleZ) {
		return pimpl->engine->SetAngleZ(angleZ);
	}

	void RemotePlayer::SetCell(uint32_t cellID) {
		return pimpl->engine->SetCell(cellID);
	}

	void RemotePlayer::SetWorldSpace(uint32_t worldSpaceID) {
		return pimpl->engine->SetWorldSpace(worldSpaceID);
	}

	void RemotePlayer::ApplyLookData(const LookData &lookData) {
		return pimpl->engine->ApplyLookData(lookData);
	}

	void RemotePlayer::ApplyMovementData(const MovementData &movementData) {
		return pimpl->engine->ApplyMovementData(movementData);
	}

	void RemotePlayer::UseFurniture(const Object *target, bool anim) {
		return pimpl->engine->UseFurniture(target, anim);
	}

	void RemotePlayer::AddItem(const ItemType *item, uint32_t count, bool silent) {
		return pimpl->engine->AddItem(item, count, silent);
	}

	void RemotePlayer::RemoveItem(const ItemType *item, uint32_t count, bool silent) {
		return pimpl->engine->RemoveItem(item, count, silent);
	}

	void RemotePlayer::EquipItem(const ItemType *item, bool silent, bool preventRemoval, bool leftHand) {
		return pimpl->engine->EquipItem(item, silent, preventRemoval, leftHand);
	}

	void RemotePlayer::UnequipItem(const ItemType *item, bool silent, bool preventEquip, bool isLeftHand) {
		return pimpl->engine->UnequipItem(item, silent, preventEquip, isLeftHand);
	}

	void RemotePlayer::AddSpell(const Spell *spell, bool silent) {
		return pimpl->engine->AddSpell(spell, silent);
	}

	void RemotePlayer::RemoveSpell(const Spell *spell, bool silent) {
		return pimpl->engine->RemoveSpell(spell, silent);
	}

	void RemotePlayer::EquipSpell(const Spell *spell, bool leftHand) {
		return pimpl->engine->EquipSpell(spell, leftHand);
	}

	void RemotePlayer::UnequipSpell(const Spell *spell, bool leftHand) {
		return pimpl->engine->UnequipSpell(spell, leftHand);
	}

	void RemotePlayer::PlayAnimation(uint32_t animID) {
		return pimpl->engine->PlayAnimation(animID);
	}

	void RemotePlayer::UpdateAVData(const std::string &avName_, const AVData &data) {
		return pimpl->engine->UpdateAVData(avName_, data);
	}

	void RemotePlayer::AddActiveEffect(const ci::MagicEffect *effect, float uiDisplayDuration, float uiDisplayMagnitude) {
		return pimpl->engine->AddActiveEffect(effect, uiDisplayDuration, uiDisplayMagnitude);
	}

	void RemotePlayer::RemoveActiveEffect(const ci::MagicEffect *effect) {
		return pimpl->engine->RemoveActiveEffect(effect);
	}

	void RemotePlayer::AddPerk(const ci::Perk *perk) {
		return pimpl->engine->AddPerk(perk);
	}

	void RemotePlayer::RemovePerk(const ci::Perk *perk) {
		return pimpl->engine->RemovePerk(perk);
	}

	std::wstring RemotePlayer::GetName() const {
		return pimpl->engine->GetName();
	}

	NiPoint3 RemotePlayer::GetPos() const {
		return pimpl->engine->GetPos();
	}

	float RemotePlayer::GetAngleZ() const {
		return pimpl->engine->GetAngleZ();
	}

	uint32_t RemotePlayer::GetCell() const {
		return pimpl->engine->GetCell();
	}

	uint32_t RemotePlayer::GetWorldSpace() const {
		return pimpl->engine->GetWorldSpace();
	}

	LookData RemotePlayer::GetLookData() const {
		return pimpl->engine->GetLookData();
	}

	MovementData RemotePlayer::GetMovementData() const {
		return pimpl->engine->GetMovementData();
	}

	std::vector<ci::ItemType *> RemotePlayer::GetEquippedArmor() const {
		return pimpl->engine->GetEquippedArmor();
	}

	ci::ItemType *RemotePlayer::GetEquippedWeapon(bool isLeftHand) const {
		return pimpl->engine->GetEquippedWeapon(isLeftHand);
	}

	ci::ItemType *RemotePlayer::GetEquippedAmmo() const {
		return pimpl->engine->GetEquippedAmmo();
	}

	const ci::Spell *RemotePlayer::GetEquippedSpell(bool isLeftHand) const {
		return pimpl->engine->GetEquippedSpell(isLeftHand);
	}

	const ci::Spell *RemotePlayer::GetEquippedShout() const {
		return pimpl->engine->GetEquippedShout();
	}

	ci::AVData RemotePlayer::GetAVData(const std::string &avName_) const {
		return pimpl->engine->GetAVData(avName_);
	}

	void RemotePlayer::SetEngine(const std::string &engineName)
	{
		if (engineName.empty())
			return;

		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		auto engineTask = [=] {
			if (engineName == "RPEngineInput")
				return pimpl->engine.reset(new RPEngineInput(this));
			if (engineName == "RPEngineIO")
				return pimpl->engine.reset(new RPEngineIO(this));

			return ErrorHandling::SendError("ERROR:RemotePlayer Unknown engine %s", engineName.data());
		};
		//pimpl->engineTask = engineTask; //crash time is < 1min
		engineTask(); // crash time with 100 chickens in whiterun is > 1min
	}

	std::string RemotePlayer::GetEngine() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->engine->GetEngineName();
	}

	void RemotePlayer::PathTo(const NiPoint3 &pos, bool walking)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (pimpl->pathToTarget == nullptr)
		{
			pimpl->pathToTarget.reset(new ci::Object(0, ID_TESObjectSTAT::XMarkerHeading, this->GetLocationID(), { 0,0,0 }, { 0,0,0 }));
		}
		if (pimpl->pathToTarget != nullptr)
		{
			pimpl->pathToTarget->SetPosition(pos);
			pimpl->pathToTarget->SetLocation(this->GetLocationID());
		}
		pimpl->pathingSpeedmult = walking ? 140 : 600;
	}

	void RemotePlayer::StopPathing()
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (pimpl->pathToTarget != nullptr)
		{
			pimpl->pathToTarget->SetLocation(~0);
		}
	}

	void RemotePlayer::SetBaseNPC(uint32_t npcID)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		auto lastNpc = pimpl->baseNpc;

		if (npcID == 0 || npcID == ~0)
		{
			pimpl->baseNpc = nullptr;
		}
		else
		{
			auto npc = (TESNPC *)LookupFormByID(npcID);
			if (!npc || npc->formType != FormType::NPC)
			{
				std::stringstream ss;
				ss << "ERROR:RemotePlayer SetBaseNPC() Bad NPC ID " << std::hex << npcID;
				return ErrorHandling::SendError("%s", ss.str().data());
			}

			{
				static dlf_mutex npcMutex;
				std::lock_guard<dlf_mutex> l1(npcMutex);

				auto newNpc = npc;

				newNpc->voiceType = (BGSVoiceType *)LookupFormByID(0x0002F7C3);
				newNpc->TESContainer::entries = nullptr;
				newNpc->TESContainer::numEntries = 0;
				newNpc->TESSpellList::unk04 = nullptr;
				newNpc->faction = nullptr;
				newNpc->nextTemplate = nullptr;
				newNpc->TESActorBaseData::flags.pcLevelMult = false;
				newNpc->TESActorBaseData::flags.unique = true;
				newNpc->TESActorBaseData::flags.protect = false;
				newNpc->TESActorBaseData::flags.essential = false;

				/*static std::map<TESNPC *, TESNPC *> backup;
				if (backup.count(newNpc) == 0)
				{
				backup[newNpc] = FormHeap_Allocate<TESNPC>();
				memcpy(backup[newNpc], newNpc, sizeof TESNPC);
				}
				else
				{
				memcpy(newNpc, backup[newNpc], sizeof TESNPC);
				}*/

				pimpl->baseNpc = newNpc;
			}
		}

		if (pimpl->spawnStage == SpawnStage::Spawned)
		{
			if (lastNpc != pimpl->baseNpc)
			{
				if (this->IsDerived() == false)
					this->ForceDespawn(L"Despawned: Base NPC updated");
			}
		}
	}

	void RemotePlayer::SetMark(const std::string &mark)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->mark = mark;
	}

	std::string RemotePlayer::GetMark() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->mark;
	}

	void RemotePlayer::SetCombatTarget(const IActor *target)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (target == this || target == nullptr)
		{
			pimpl->combatTarget = 0;
			return;
		}
		pimpl->combatTarget = IActorAccess::GetActorRefID(target);
		auto remotePl = dynamic_cast<const RemotePlayer *>(target);
		if (remotePl != nullptr && this->GetRefID())
			remotePl->pimpl->combatTarget = this->GetRefID();
	}

	std::shared_ptr<uint32_t> RemotePlayer::GetNextHitAnim()
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (pimpl->spawnStage != SpawnStage::Spawned)
			return nullptr;
		if (pimpl->hitAnimsOut.empty())
			return nullptr;
		auto r = pimpl->hitAnimsOut.front();
		pimpl->hitAnimsOut.pop();
		return std::shared_ptr<uint32_t>(new uint32_t(r));
	}

	void RemotePlayer::SetInAFK(bool val)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->afk = val;
	}

	void RemotePlayer::SetNicknameVisible(bool v)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->nicknameEnabled = v;
	}

	void RemotePlayer::Fire(float power)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (pimpl->spawnStage == SpawnStage::Spawned)
		{
			const auto refID = pimpl->formID;
			const auto dispenserID = pimpl->dispenser ? pimpl->dispenser->GetRefID() : 0;
			const auto weap = this->GetEquippedWeapon();
			const auto ammo = this->GetEquippedAmmo();
			if (ammo != nullptr
				&& weap != nullptr && weap->GetSubclass() == ci::ItemType::Subclass::WEAP_Bow)
			{
				SET_TIMER_LIGHT(1, [=] {
					auto actor = (Actor *)LookupFormByID(refID);
					if (actor != nullptr)
					{
						auto dispenserRef = (TESObjectREFR *)LookupFormByID(dispenserID);
						if (dispenserRef != nullptr)
						{
							auto ammoSrc = (TESAmmo *)LookupFormByID(ammo->GetFormID());

							auto ammoCopy = FormHeap_Allocate<TESAmmo>();
							memcpy(ammoCopy, ammoSrc, sizeof TESAmmo);
							ammoCopy->formID = 0;
							ammoCopy->SetFormID(Utility::NewFormID(), 1);

							auto projSrc = ammoSrc->settings.projectile;
							auto projCopy = FormHeap_Allocate<BGSProjectile>();
							memcpy(projCopy, projSrc, sizeof BGSProjectile);
							projCopy->formID = 0;
							projCopy->SetFormID(Utility::NewFormID(), 1);

							ammoCopy->settings.projectile = projCopy;
							ammoCopy->settings.damage = 0;

							auto &speed = projCopy->data.unk08;
							auto &range = projCopy->data.unk0C;
							auto &fadeDuration = projCopy->data.unk30;
							auto &collisionRadius = projCopy->data.unk48;

							const auto lastSpeed = speed;
							speed = pow(power, 2) * speed;

							auto bowSrc = (TESObjectWEAP *)LookupFormByID(weap->GetFormID());
							auto bowCopy = FormHeap_Allocate<TESObjectWEAP>();
							memcpy(bowCopy, bowSrc, sizeof TESObjectWEAP);
							bowCopy->formID = 0;
							bowCopy->SetFormID(Utility::NewFormID(), 1);
							bowCopy->attackDamage = 0;

							sd::Weapon::Fire(bowCopy, dispenserRef, ammoCopy);

							SET_TIMER(3000, [=] {
								sd::RemoveItem(g_thePlayer, ammoCopy, -1, true, nullptr);
							});
						}
					}
				});
			}
		}
	}

	void RemotePlayer::FireMagic(const ci::Spell *spell, int32_t handID)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (pimpl->spawnStage == SpawnStage::Spawned)
		{
			const auto foxID = pimpl->myPseudoFox ? pimpl->myPseudoFox->GetRefID() : 0;
			if (spell != nullptr)
			{
				if (pimpl->gnomes[handID])
				{
					pimpl->gnomes[handID]->TaskSingle([=](TESObjectREFR *ref) {
						const auto refID = ref->formID;
						auto actor = (Actor *)LookupFormByID(refID);
						if (actor != nullptr)
						{
							auto foxRef = (TESObjectREFR *)LookupFormByID(foxID);
							if (foxRef != nullptr)
							{
								auto spellForm = (SpellItem *)LookupFormByID(spell->GetFormID());
								if (spellForm != nullptr)
								{
									sd::DoCombatSpellApply(actor, spellForm, foxRef);
								}
							}
						}
					});
				}
			}
		}
	}

	void RemotePlayer::MagicAttackBegin(int32_t handID)
	{
		if (handID < 0 || handID > 1)
			return;
		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		if (!pimpl->isMagicAttackStarted[handID]
			&& pimpl->gnomes[handID] != nullptr
			&& pimpl->handsMagicProxy[handID] != nullptr
			&& pimpl->handsMagicProxy[handID]->GetCastingType() == Spell::CastingType::Concentration
			&& pimpl->handsMagicProxy[handID]->GetDelivery() == Spell::Delivery::Aimed)
		{
			pimpl->isMagicAttackStarted[handID] = true;

			const auto spell = (SpellItem *)LookupFormByID(pimpl->handsMagicProxy[handID]->GetFormID());
			const auto myFoxID = pimpl->myPseudoFox ? pimpl->myPseudoFox->GetRefID() : 0;

			pimpl->gnomes[handID]->TaskSingle([=](TESObjectREFR *gnomeRef) {
				auto target = (TESObjectREFR *)LookupFormByID(myFoxID);
				sd::DoCombatSpellApply((Actor *)gnomeRef, spell, target);
			});
		}
	}

	void RemotePlayer::MagicAttackEnd(int32_t handID)
	{
		if (handID < 0 || handID > 1)
			return;
		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		if (pimpl->isMagicAttackStarted[handID])
		{
			pimpl->gnomes[handID]->TaskSingle([=](TESObjectREFR *gnomeRef) {
				sd::DoCombatSpellApply((Actor *)gnomeRef, (SpellItem *)LookupFormByID(ID_SpellItem::Telekinesis), nullptr);
			});
			pimpl->isMagicAttackStarted[handID] = false;
		}
	}

	void RemotePlayer::SetVisualMagicEffect(const ci::Spell *spell)
	{
		SAFE_CALL("RemotePlayer", [&] {
			std::lock_guard<dlf_mutex> l(pimpl->mutex);

			if (!spell || spell->GetDelivery() == Spell::Delivery::Self)
			{
				if (spell != pimpl->visualMagicEffect)
				{
					pimpl->visualMagicEffect = spell;
					const auto formID = this->GetRefID();
					SET_TIMER_LIGHT(10, [=] {
						const auto actor = (Actor *)LookupFormByID(formID);
						if (actor != nullptr && actor->formType == FormType::Character)
						{
							const auto spellForm = spell ? (SpellItem *)LookupFormByID(spell->GetFormID()) : nullptr;
							enum {
								Telekinesis = 0x0001A4CC,
							};
							static auto telekinesis = (SpellItem *)LookupFormByID(Telekinesis);

							const auto toCast = spell ? spellForm : telekinesis;
							if (toCast != nullptr)
								sd::DoCombatSpellApply(actor, toCast, actor);
						}
					});
				}
			}
		});
	}

	bool RemotePlayer::IsBroken() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->broken;
	}

	void RemotePlayer::TestMakeBroken()
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->broken = true;
	}

	NiPoint3 RemotePlayer::GetNicknamePos() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->nicknameLabel ? pimpl->nicknameLabel->GetPos() : (this->GetPos() += {0, 0, 133});
	}

	float RemotePlayer::GetNicknameDrawDistance() const
	{
		if (pimpl->movementData.isSneaking)
			return SyncOptions::GetSingleton()->GetFloat("NICKNAME_DISTANCE_SNEAKING");
		return SyncOptions::GetSingleton()->GetFloat("NICKNAME_DISTANCE");
	}

	bool RemotePlayer::IsSpawned() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->spawnStage == SpawnStage::Spawned;
	}

	void RemotePlayer::SetAttachedHorse(ci::IActor *horse)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		const uint32_t id = horse != nullptr ? IActorAccess::GetActorRefID(horse) : 0;
		pimpl->syncState.myVehicleID = id;
	}

	bool RemotePlayer::HasAttachedHorse() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->syncState.myVehicleID != 0 && pimpl->syncState.myVehicleID != ~0;
	}

	void RemotePlayer::Respawn()
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		this->ForceDespawn(L"ClientLogic");
	}

	uint32_t RemotePlayer::GetFormID() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->formID;
	}

	RemotePlayer *RemotePlayer::LookupByFormID(uint32_t id)
	{
		std::lock_guard<dlf_mutex> l(gMutex);
		for (auto p : allRemotePlayers)
		{
			if (p->GetFormID() == id)
				return p;
		}
		return nullptr;
	}

	void RemotePlayer::SetTracing(bool trace)
	{
		std::lock_guard<dlf_mutex> l(gMutex);

		static std::function<void()> f = [] {
			std::lock_guard<dlf_mutex> l(gMutex);
			ci::Chat::AddMessage(L"allRemotePlayers size is " + std::to_wstring(allRemotePlayers.size()));
		};

		traceTask = trace ? f : nullptr;
	}

	void RemotePlayer::SetHeight(float h)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (h < 0.1)
			h = 0.1;
		if (h > 1.0)
			h = 1.0;
		pimpl->height = h;
	}

	TESNPC *RemotePlayer::AllocateNPC() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->engine->AllocateNPC();
	}

	void RemotePlayer::ApplyMovementDataImpl() {
		return pimpl->engine->ApplyMovementDataImpl();
	}
}