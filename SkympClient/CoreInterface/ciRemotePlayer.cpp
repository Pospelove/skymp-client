#include "../stdafx.h"
#include "../Sync/LookData.h"
#include "../Sync/MovementData.h"
#include "../Sync/Equipment.h"
#include "../Sync/HitData.h"
#include "../Sync/SyncOptions.h"
#include "Skyrim\Camera\PlayerCamera.h"
#include "CoreInterface.h"
#include <algorithm>
#include <queue>

enum class InvisibleFoxEngine {
	None = 0x0,
	Object = 0x2,
};

extern std::map<TESForm *, const ci::ItemType *> knownItems;
extern clock_t localPlCrosshairRefUpdateMoment;

class CIAccess
{
public:
	static ci::Mutex &GetMutex() {
		return ci::IClientLogic::callbacksMutex;
	}
};

namespace ci
{
	RemotePlayer *CreateGhostAxe();
	std::set<RemotePlayer *> allRemotePlayers;
	RemotePlayer *ghostAxe = nullptr;
	RemotePlayer *currentSpawning = nullptr;
	uint32_t currentSpawningBaseID = 0;
	clock_t lastForceSpawn = 0;
	RemotePlayer *currentFixingGreyFace = nullptr;
	dlf_mutex gMutex;
	uint32_t numInvisibleFoxes = 0;
	RemotePlayer *useMagicPackageHolder = nullptr;

	struct WorldSpellData
	{
		const ci::Spell *spell = nullptr;
		uint32_t casters = 0;
		SpellItem *casterRealEquippedSpell = nullptr;
		uint32_t differentSpells = 0;
	};

	WorldSpellData worldSpell;

	inline float GetRespawnRadius(bool isInterior) {
		return SyncOptions::GetSingleton()->GetFloat("DRAW_DISTANCE");
	}

	std::vector<std::pair<uint32_t, uint32_t>> packages = {
		{ ID_TESPackage::WCollegePracticeCastWard, ID_SpellItem::CollegePracticeWard },
	};

	void SetPackageSpell(size_t packageN, uint32_t spellID)
	{
		const auto collegePracticeWard = (SpellItem *)LookupFormByID(packages[packageN].second);
		const auto formID = collegePracticeWard->formID;
		const auto s = (SpellItem *)LookupFormByID(spellID);
		memcpy(collegePracticeWard, s, sizeof SpellItem);
		collegePracticeWard->data.calculations.cost = 0;
		collegePracticeWard->formID = formID;
	}

	void SetPackageCondition_ZeroVariable01(TESPackage *package)
	{
		static auto ambushSitPackage = (TESPackage *)LookupFormByID(ID_TESPackage::ambushSitPackage); // Condition: Variable01 == 0
		package->unk3C = ambushSitPackage->unk3C;
	}

	void ApplyPackage(TESNPC *npc)
	{
		// forceav variable10 1.0/0.0 to start/stop combat
		// forceav variable01 0.0/1.0 to start/stop casting

		auto nonDrawn = (TESPackage *)LookupFormByID(0x000654E2);
		auto drawn = (TESPackage *)LookupFormByID(0x0004E4BB);

		static auto useMagic = (TESPackage *) nullptr;
		if (!useMagic)
		{
			const size_t packageN = 0;
			const auto defaultSpell = ID_SpellItem::Flames;

			useMagic = (TESPackage *)LookupFormByID(packages[packageN].first);
			SetPackageCondition_ZeroVariable01(useMagic);
			useMagic->packageFlags = drawn->packageFlags;

			SetPackageSpell(packageN, defaultSpell);
		}

		if (!npc)
			return;
		auto &data = npc->TESAIForm::unk10;
		data.unk0 = (UInt32)useMagic;
		data.next = FormHeap_Allocate<TESAIForm::Data>();
		data.next->unk0 = (UInt32)drawn;
		data.next->next = FormHeap_Allocate<TESAIForm::Data>();
		data.next->next->unk0 = (UInt32)nonDrawn;
		data.next->next->next = nullptr;
	}

	TESObjectCELL *GetParentNonExteriorCell(TESObjectREFR *ref)
	{
		auto cell = sd::GetParentCell(ref);
		if (cell && sd::IsInterior(cell) == false)
			cell = nullptr;
		return cell;
	}

	TESObjectCELL *LookupNonExteriorCellByID(FormID cellID)
	{
		auto cell = (TESObjectCELL *)LookupFormByID(cellID);
		if (!cell)
			return nullptr;
		if (cell->formType != FormType::Cell)
			return nullptr;
		//if (!sd::IsInterior(cell)) // Та самая ошибка с вылетом в интерьере. несколько часов. боль
		//	return nullptr;
		if (!cell->IsInterior())
			return nullptr;
		return cell;
	}

	TESObjectSTAT *GetPlaceAtMeMarkerBase()
	{
		return (TESObjectSTAT *)LookupFormByID(0x15);
	}

	FormID markerFormID = 0;
	void UpdatePlaceAtMeMarker()
	{
		auto GetRelXy = [](float rzrot, float dist, float *outX, float *outY) {
			rzrot = (float)(rzrot * 3.14 / 180);
			*outX += dist * sin(rzrot);
			*outY += dist * cos(rzrot);
		};

		static dlf_mutex mutex;

		static TESObjectCELL *cellWas = nullptr;
		static void *wpWas = nullptr;
		auto cellNow = sd::GetParentCell(g_thePlayer);
		auto wpNow = sd::GetWorldSpace(g_thePlayer);
		bool isInteriorNow = false;
		if (cellWas != cellNow)
		{
			const bool isInterior[2] = {
				cellWas && cellWas->IsInterior(),
				cellNow && cellNow->IsInterior()
			};
			isInteriorNow = isInterior[1];
			if (isInterior[0] || isInterior[1] || wpWas != wpNow)
			{
				std::lock_guard<dlf_mutex> l(mutex);
				markerFormID = 0;
			}

			cellWas = cellNow;
			wpWas = wpNow;
		}

		auto ref = (TESObjectREFR *)LookupFormByID(markerFormID);
		if (!ref || ref->formType != FormType::Reference || sd::GetParentCell(ref) == nullptr)
		{
			static bool isTaskRunning = false;
			if (!isTaskRunning)
			{
				isTaskRunning = true;
				cd::PlaceAtMe(g_thePlayer, GetPlaceAtMeMarkerBase(), 1, true, false, [](cd::Value<TESObjectREFR> result) {
					std::lock_guard<dlf_mutex> l(mutex);
					markerFormID = result.GetFormID();
					isTaskRunning = false;
				});
			}
			return;
		}

		auto pos = cd::GetPosition(g_thePlayer);
		GetRelXy(sd::GetAngleZ(g_thePlayer) + 128 + 90, GetRespawnRadius(isInteriorNow) * 0.85, &pos.x, &pos.y);
		static clock_t lastTranslateTo = 0;
		if (clock() - lastTranslateTo > 1000)
		{
			lastTranslateTo = clock();
			if ((pos - cd::GetPosition(ref)).Length() > 2000)
				cd::SetPosition(ref, { pos.x,pos.y, pos.z });
		}
	}

	enum class SpawnStage
	{
		NonSpawned,
		Spawning,
		Spawned,
	};

	struct RemotePlayer::Impl
	{
		Impl() {
			handGnome[0] = handGnome[1] = nullptr;
			isMagicAttackStarted[0] = isMagicAttackStarted[1] = false;
			lastGnomeHide[0] = lastGnomeHide[1] = NULL;
			createGnomeCalled[0] = createGnomeCalled[1] = false;
		}

		dlf_mutex mutex;
		FormID formID = 0;
		std::wstring name;
		std::unique_ptr<ci::Text3D> nicknameLabel;
		bool nicknameEnabled = true;
		LookData lookData;
		ILookSynchronizer *lookSync = ILookSynchronizer::GetV17();
		TESObjectCELL *currentNonExteriorCell = nullptr;
		FormID worldSpaceID = 0;
		SpawnStage spawnStage = SpawnStage::NonSpawned;
		clock_t spawnMoment = 0;
		clock_t timer250ms = 0;
		clock_t unsafeSyncTimer = 0;
		clock_t lastOutOfPos = 0;
		bool greyFaceFixed = false;
		MovementData movementData;
		MovementData_::SyncState syncState;
		UInt32 rating = 0;
		bool afk = false;
		bool stopProcessing = false;
		std::map<const ci::ItemType *, uint32_t> inventory;
		std::set<const ci::Spell *> spellList;
		std::queue<uint8_t> hitAnimsToApply;
		OnHit onHit = nullptr;
		std::map<std::string, ci::AVData> avData, avDataLast;
		Object *myPseudoFox = nullptr;
		Object *dispenser = nullptr;
		float height = 1;
		RemotePlayer *handGnome[2];
		bool isMagicAttackStarted[2];
		clock_t lastGnomeHide[2];
		bool createGnomeCalled[2];

		struct Equipment
		{
			Equipment() {
				hands[0] = hands[1] = nullptr;
				handsMagic[0] = handsMagic[1] = nullptr;
			}

			std::array<const ci::ItemType *, 2> hands;
			std::array<const ci::Spell *, 2> handsMagic;
			std::set<const ci::ItemType *> armor;
			const ci::ItemType *ammo = nullptr;

			friend bool operator==(const Equipment &l, const Equipment &r) {
				return l.hands[0] == r.hands[0] && l.hands[1] == r.hands[1] && l.armor == r.armor && l.ammo == r.ammo;
			}
			friend bool operator!=(const Equipment &l, const Equipment &r) {
				return !(l == r);
			}
		} eq, eqLast;

		std::map<int32_t, const ci::Spell *> handsMagicProxy;
		clock_t lastMagicEquipmentChange = 0;

		clock_t lastWeaponsUpdate = 0;

		static decltype(knownItems) *RemotePlayerKnownItems() { // lock gMutex to use this
			static decltype(knownItems) remotePlayerKnownItems;
			return &remotePlayerKnownItems;
		}

		class HitEventSinkGlobal : public BSTEventSink<TESHitEvent>
		{
		public:
			HitEventSinkGlobal() {
				g_hitEventSource.AddEventSink(this);
			}

			virtual ~HitEventSinkGlobal() override {
				g_hitEventSource.RemoveEventSink(this);
			}

			virtual	EventResult	ReceiveEvent(TESHitEvent *evn, BSTEventSource<TESHitEvent> * source) override
			{
				if (LookupFormByID(evn->sourceFormID) != nullptr && LookupFormByID(evn->sourceFormID)->formType == FormType::Enchantment)
					return EventResult::kEvent_Continue;

				if (evn->caster != g_thePlayer || evn->target == nullptr || evn->projectileFormID != NULL)
					return EventResult::kEvent_Continue;

				const uint32_t targetID = evn->target->formID;
				HitEventData hitEventData_;
				hitEventData_.powerAttack = evn->flags.powerAttack;

				std::thread([=] {
					auto hitEventData = hitEventData_;

					std::lock_guard<dlf_mutex> l(gMutex);

					const ci::ItemType *weapon = nullptr;
					auto sourceForm = LookupFormByID(evn->sourceFormID);
					if (sourceForm != nullptr)
					{
						try {
							std::lock_guard<dlf_mutex> l(gMutex);
							weapon = RemotePlayerKnownItems()->at(sourceForm);
						}
						catch (...) {
						}
					}
					hitEventData.weapon = weapon;

					for (auto it = allRemotePlayers.begin(); it != allRemotePlayers.end(); ++it)
					{
						auto pl = *it;
						if (pl == nullptr)
							continue;
						std::lock_guard<dlf_mutex> l1(pl->pimpl->mutex);
						auto ref = (TESObjectREFR *)LookupFormByID(pl->pimpl->formID);
						if (!ref/* || ref->formType != FormType::Reference*/)
							continue;
						if (ref->formID != targetID)
							continue;
						auto onHit = pl->pimpl->onHit;
						if (onHit)
						{
							std::thread([=] {
								std::lock_guard<ci::Mutex> l(CIAccess::GetMutex());
								onHit(hitEventData);
							}).detach();
						}
						break;
					}
				}).detach();

				return EventResult::kEvent_Continue;
			}
		};
	};

	void RemotePlayer::ApplyWorldSpell()
	{
		std::lock_guard<dlf_mutex> l(gMutex);

		auto spell = worldSpell.spell ? LookupFormByID(worldSpell.spell->GetFormID()) : nullptr;
		if (worldSpell.differentSpells == 1 || worldSpell.casters == 1)
			spell = worldSpell.casterRealEquippedSpell;

		static TESForm *appliedSpell = nullptr;
		if (appliedSpell != spell)
		{
			if (spell != nullptr)
			{
				auto dest = (SpellItem *)LookupFormByID(packages[0].second);
				const auto formID = dest->formID;
				memcpy(dest, spell, sizeof SpellItem);
				dest->formID = formID;

				if (dest->effectItemList.size() > 0)
				{
					const bool isTelekinesis = dest->effectItemList.front()->mgef->properties.archetype == EffectSetting::Properties::Archetype::kArchetype_Telekinesis;
					if (!isTelekinesis) // Clear effectItemList of Telekinesis would remove unique cast anim
						dest->effectItemList.clear();
				}
			}
			appliedSpell = spell;
		}
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
				if (!sd::HasLOS(g_thePlayer, actor))
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
								worldSpell.casterRealEquippedSpell = sd::GetEquippedSpell(actor, !i);
						}
						break;
					}
				}
			}

		});

		if (bestSpell != nullptr)
		{
			worldSpell.spell = bestSpell;
			worldSpell.casters = max;
			worldSpell.differentSpells = spells.size();
		}
	}

	RemotePlayer::RemotePlayer(const std::wstring &name, const LookData &lookData, NiPoint3 spawnPoint, uint32_t cellID, uint32_t worldSpaceID, OnHit onHit) :
		pimpl(new Impl)
	{
		static auto hitEventSink = new Impl::HitEventSinkGlobal;

		std::lock_guard<dlf_mutex> l(gMutex);
		std::lock_guard<dlf_mutex> l1(pimpl->mutex);

		allRemotePlayers.insert(this);

		pimpl->name = name;
		pimpl->lookData = lookData;
		pimpl->movementData.pos = spawnPoint;
		pimpl->currentNonExteriorCell = LookupNonExteriorCellByID(cellID);
		pimpl->worldSpaceID = worldSpaceID;
		pimpl->onHit = onHit;

		AVData avData;
		avData.base = 0;
		avData.modifier = 0;
		avData.percentage = 1;
		this->UpdateAVData("invisibility", avData);
	}

	RemotePlayer::RemotePlayer(const IActor &src) :
		RemotePlayer(src.GetName(), src.GetLookData(), src.GetPos(), src.GetCell(), src.GetWorldSpace())
	{
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

		for (int32_t i = 0; i <= 1; ++i)
		{
			auto gnome = pimpl->handGnome[i];
			if (gnome != nullptr)
			{
				std::thread([=] {
					delete gnome;
				}).detach();
			}
		}
	}

	void RemotePlayer::AsyncGnomeCreate(int32_t i)
	{
		std::thread([=] {
			std::lock_guard<dlf_mutex> l1(gMutex);
			if (allRemotePlayers.find(this) != allRemotePlayers.end())
			{
				std::lock_guard<dlf_mutex> l1(pimpl->mutex);
				if (pimpl->handGnome[i] == nullptr)
					pimpl->handGnome[i] = new ci::RemotePlayer(this->GetName(), this->GetLookData(), { 0,0,0 }, -1, -1);
			}
		}).detach();
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

	void RemotePlayer::UpdateNonSpawned()
	{
		pimpl->nicknameLabel = nullptr;


		if (Utility::IsForegroundProcess())
		{
			const NiPoint3 localPlPos = cd::GetPosition(g_thePlayer);
			TESObjectCELL *const localPlCell = sd::GetParentCell(g_thePlayer);
			const bool isInterior = localPlCell ? sd::IsInterior(localPlCell) : false;
			const FormID worldSpaceID = sd::GetWorldSpace(g_thePlayer) ? sd::GetWorldSpace(g_thePlayer)->formID : NULL;

			static clock_t *firstSpawnAttempt = nullptr;
			if (firstSpawnAttempt == nullptr)
				firstSpawnAttempt = new clock_t{ clock() };
			if (*firstSpawnAttempt + 2333 < clock()
				&& markerFormID
				&& NiPoint3{ pimpl->movementData.pos - localPlPos }.Length() < GetRespawnRadius(isInterior)
				&& (pimpl->currentNonExteriorCell == GetParentNonExteriorCell(g_thePlayer) || pimpl->currentNonExteriorCell == nullptr)
				&& pimpl->worldSpaceID == worldSpaceID
				)
			{
				if (currentSpawning == nullptr && currentSpawningBaseID == NULL)
				{
					currentSpawning = this;
					lastForceSpawn = clock();
					auto npc = this->AllocateNPC();
					npc->combatStyle = (TESCombatStyle *)LookupFormByID(0x000F960C);
					npc->combatStyle->general.magicMult = 5;
					npc->combatStyle->general.meleeMult = 15;
					npc->height = pimpl->height;
					currentSpawningBaseID = npc->GetFormID();
					WorldCleaner::GetSingleton()->SetFormProtected(currentSpawningBaseID, true);
					return this->ForceSpawn(currentSpawningBaseID);
				}
			}
		}
	}

	void RemotePlayer::UpdateSpawning()
	{
		pimpl->avDataLast.clear();
		pimpl->eqLast = {};
		for (int32_t i = 0; i <= 1; ++i)
			pimpl->isMagicAttackStarted[i] = false;


		if (currentSpawning != this)
		{
			pimpl->spawnStage = SpawnStage::NonSpawned;
			return;
		}
		if (lastForceSpawn + 3000 < clock())
		{
			currentSpawning = nullptr;
			pimpl->spawnStage = SpawnStage::NonSpawned;

			allRemotePlayers.erase(this);
			new (this) RemotePlayer(this->GetName(), this->GetLookData(), this->GetPos(), this->GetCell(), this->GetWorldSpace());
		}
	}

	void RemotePlayer::UpdateNickname(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {
			if (this->IsDerived() == false)
			{
				const NiPoint3 offset = { 0, 0, 128 + 16 };
				const auto nicknamePos = cd::GetPosition(actor) + offset;

				if (pimpl->nicknameLabel == nullptr && pimpl->nicknameEnabled)
					pimpl->nicknameLabel.reset(new ci::Text3D(this->GetName(), nicknamePos));

				if (pimpl->nicknameLabel != nullptr && !pimpl->nicknameEnabled)
					pimpl->nicknameLabel.reset();


				if (pimpl->nicknameLabel != nullptr)
				{
					pimpl->nicknameLabel->SetPos(nicknamePos);
					if (pimpl->nicknameLabel->GetText() != this->GetName())
						pimpl->nicknameLabel->SetText(this->GetName());
					pimpl->nicknameLabel->SetDrawDistance(SyncOptions::GetSingleton()->GetFloat("NICKNAME_DISTANCE"));
				}
			}
		});
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
				pimpl->syncState.isWorldSpell[i] = (pimpl->eq.handsMagic[i] == worldSpell.spell);
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
				HitData_::Apply(actor, hitAnimID, pimpl->syncState.fullyUnsafeSync);
				if (HitData_::IsPowerAttack(hitAnimID))
					pimpl->syncState.forceFixAfterHitAnim = true;
			}
		});
	}

	void RemotePlayer::ApplyActorValues(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {
			auto applyAV = [&](char *avNameLowerCase) {
				if (pimpl->avDataLast != pimpl->avData)
				{
					pimpl->avDataLast = pimpl->avData;
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
				}
			};
			applyAV("health");
			applyAV("stamina");
			applyAV("magicka");

			sd::SetActorValue(actor, "invisibility",
				pimpl->avData["invisibility"].base + pimpl->avData["invisibility"].modifier);
		});
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

	void RemotePlayer::ManageMyGnomes(Actor *actor)
	{
		SAFE_CALL("RemotePlayer", [&] {
			for (int32_t i = 0; i <= 1; ++i)
			{
				auto gnome = pimpl->handGnome[i];
				if (gnome)
				{
					gnome->SetHeight(0.1);

					gnome->SetInAFK(true);
					gnome->SetNicknameVisible(false);

					AVData avData;
					avData.base = 100;
					avData.modifier = 0;
					avData.percentage = 1;
					gnome->UpdateAVData("invisibility", avData);



					if (!this->NeedsGnome(i) && (clock() - pimpl->lastGnomeHide[i] > 5000 || gnome->GetSpawnStage() != (int32_t)SpawnStage::Spawned))
					{
						gnome->SetCell(-1);
						gnome->SetWorldSpace(-1);
						gnome->SetPos({ 0,0,0 });
						pimpl->lastGnomeHide[i] = clock();
					}
					else
					{
						gnome->SetWorldSpace(this->GetWorldSpace());
						gnome->SetCell(this->GetCell());

						const float angleRad = this->GetAngleZ() / 180 * acos(-1);
						auto md = this->GetMovementData();

						auto node = i ? "NPC L Hand [LHnd]" : "NPC R MagicNode [RMag]";
						md.pos = Utility::GetNodeWorldPosition(actor, node, false);
						const bool nodeFound = md.pos != NiPoint3{ 0,0,0 };

						if (!nodeFound)
						{
							md.pos = cd::GetPosition(actor);
							const float distance = SyncOptions::GetSingleton()->GetFloat("HANDGNOME_OFFSET_FORWARD");
							md.pos += {distance * sin(angleRad), distance * cos(angleRad), 0};
							md.pos += {0, 0, SyncOptions::GetSingleton()->GetFloat("HANDGNOME_OFFSET_Z")};
						}
						else
						{
							float distance = SyncOptions::GetSingleton()->GetFloat("HANDGNOME_OFFSET_FORWARD_FROM_HAND");
							if (i == 1)
								distance += 48;
							md.pos += {distance * sin(angleRad), distance * cos(angleRad), 0};
							md.pos += {0, 0, SyncOptions::GetSingleton()->GetFloat("HANDGNOME_OFFSET_Z_FROM_HAND")};
						}
						if (pimpl->eq.handsMagic[i] == nullptr)
							md.pos += {0, 0, 150};
						gnome->ApplyMovementData(md);

						auto gnomeRef = (Actor *)LookupFormByID(gnome->GetRefID());
						if (gnomeRef != nullptr)
							sd::TranslateTo(gnomeRef, md.pos.x, md.pos.y, md.pos.z, 0, 0, md.angleZ, 10000, 10000);
					}
				}
				else
				{
					if (this->NeedsGnome(i) && pimpl->createGnomeCalled[i] == false)
					{
						this->AsyncGnomeCreate(i);
						pimpl->createGnomeCalled[i] = true;
					}
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
			if (this->IsBowEquipped())
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
			if (sd::HasLOS(g_thePlayer, actor))
			{
				Equipment_::Equipment eq;
				if (pimpl->eq.ammo != nullptr)
					eq.other.insert(LookupFormByID(pimpl->eq.ammo->GetFormID()));
				for (auto &item : pimpl->eq.armor)
					eq.other.insert(LookupFormByID(item->GetFormID()));
				Equipment_::ApplyOther(actor, eq);
				pimpl->eqLast.armor = pimpl->eq.armor;
				pimpl->eqLast.ammo = pimpl->eq.ammo;
			}
		});
	}

	void RemotePlayer::DespawnIfNeed(Actor *actor)
	{
		/*SAFE_CALL("RemotePlayer", [&] {
			for (int32_t i = 0; i <= 1; ++i)
			{
				if (pimpl->eq.handsMagic[i] == nullptr && sd::GetEquippedSpell(actor, !i) != nullptr)
				{
					this->ForceDespawn(L"Despawned: Equipment error");
					break;
				}
			}
		});*/

		SAFE_CALL("RemotePlayer", [&] {
			if (pimpl->height != ((TESNPC *)actor->baseForm)->height)
				this->ForceDespawn(L"Despawned: Height changed");
		});

		SAFE_CALL("RemotePlayer", [&] {
			if (pimpl->syncState.fatalErrors != 0)
			{
				this->ForceDespawn(L"Despawned: Fatal Error in Sync");
				pimpl->syncState.fatalErrors = 0;
			}
		});

		SAFE_CALL("RemotePlayer", [&] {
			if (pimpl->currentNonExteriorCell != GetParentNonExteriorCell(g_thePlayer))
				this->ForceDespawn(L"Despawned: Cell changed");
		});

		SAFE_CALL("RemotePlayer", [&] {
			const FormID worldSpaceID = sd::GetWorldSpace(g_thePlayer) ? sd::GetWorldSpace(g_thePlayer)->formID : NULL;
			if (pimpl->worldSpaceID != worldSpaceID && worldSpaceID != NULL && pimpl->worldSpaceID != NULL)
				this->ForceDespawn(std::wstring(L"Despawned: WorldSpace changed " + std::to_wstring(worldSpaceID) + L' ' + std::to_wstring(pimpl->worldSpaceID)).data());
		});

		SAFE_CALL("RemotePlayer", [&] {
			const NiPoint3 localPlPos = cd::GetPosition(g_thePlayer);
			const bool isInterior = sd::GetParentCell(g_thePlayer) ? sd::IsInterior(sd::GetParentCell(g_thePlayer)) : false;
			if (NiPoint3{ pimpl->movementData.pos - localPlPos }.Length() >= GetRespawnRadius(isInterior))
				this->ForceDespawn(L"Despawned: Player is too far");
		});

		SAFE_CALL("RemotePlayer", [&] {
			if (sd::IsDead(actor) && this->GetAVData("Health").percentage > 0.0)
				this->ForceDespawn(L"Despawned: Fake death");
		});
	}

	void RemotePlayer::FixGreyFaceBug(Actor *actor)
	{
		if (!pimpl->greyFaceFixed && currentFixingGreyFace == nullptr)
		{
			bool success = false;
			SAFE_CALL_RETURN("RemotePlayer", [&] {
				currentFixingGreyFace = this;
				actor->baseForm->formID = g_thePlayer->baseForm->formID; // AAAAAA
				pimpl->lookSync->ApplyTintMasks((TESNPC *)actor->baseForm, pimpl->lookData.tintmasks);
				pimpl->greyFaceFixed = true;
				sd::Disable(actor, false);
				return true;
			}, success);
			if (!success)
				currentFixingGreyFace = nullptr;

			const auto formID = pimpl->formID;
			const bool isInterior = pimpl->currentNonExteriorCell != nullptr; //-V561
			SET_TIMER(0, [=] {
				auto actor = (Actor *)LookupFormByID(formID);
				sd::Enable(actor, false);
				cd::KeepOffsetFromActor(cd::Value<Actor>(formID), cd::Value<Actor>(formID), 0, 0, 0, 0, 0, 0, 1, 1);
				SET_TIMER(100, [=] {
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

		if (pimpl->spawnMoment + 1000 < clock()
			&& (sd::GetRace(actor)->formID != this->pimpl->lookData.raceID || 0 != memcmp(((TESNPC *)actor->baseForm)->faceMorph->option, &this->pimpl->lookData.options[0], sizeof LookData().options))
			&& this->pimpl->lookData.isEmpty() == false)
		{
			auto lookData = this->pimpl->lookData;
			this->pimpl->lookData = {};
			this->ApplyLookData(lookData);
			return;
		}

		this->UpdateNickname(actor);

		if (pimpl->greyFaceFixed)
		{
			this->DeleteProjectile(actor);
			this->UpdateMovement(actor);
			this->ApplyHitAnimations(actor);
			this->ApplyActorValues(actor);
		}

		this->ManageMyFox(actor);
		this->ManageMyGnomes(actor);

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
	}

	void RemotePlayer::Update()
	{
		try {
			std::lock_guard<dlf_mutex> l1(pimpl->mutex);

			switch (pimpl->spawnStage)
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
			ErrorHandling::SendError("ERROR:MT:RemotePlayer '%s'", WstringToString(this->GetName()).data());
		}
	}

	void RemotePlayer::Update_OT()
	{
		if (MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
			return this->ForceDespawn(L"Despawned: Game exit");
		if (!Utility::IsForegroundProcess() && GameSettings::IsFullScreen() && !this->IsDerived())
			return this->ForceDespawn(L"Despawned: Window isn't active");
	}

	void RemotePlayer::ForceSpawn(uint32_t npcID)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		if (pimpl->spawnStage != SpawnStage::NonSpawned)
			return;

		pimpl->spawnStage = SpawnStage::Spawning;

		auto refToPlaceAt = (TESObjectREFR *)LookupFormByID(markerFormID);
		if (refToPlaceAt && refToPlaceAt->formType != FormType::Reference)
			refToPlaceAt = nullptr;
		if (!refToPlaceAt)
			refToPlaceAt = g_thePlayer;

		cd::PlaceAtMe(refToPlaceAt, (TESNPC *)LookupFormByID(npcID), 1, true, false, [=](cd::Value<TESObjectREFR> ac) {
			SET_TIMER(0, [=] {
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
				pimpl->syncState = {};

				auto actor = (Actor *)LookupFormByID(id);
				if (actor != nullptr)
				{
					sd::SetActorValue(actor, "Confidence", 4.0);
					sd::SetActorValue(actor, "Agression", 0.0);
					sd::SetActorValue(actor, "attackdamagemult", 0.0);
					sd::SetActorValue(actor, "Variable01", rand());
					sd::SetActorValue(actor, "MagicResist", 100);

					BSFixedString name = WstringToString(pimpl->name).c_str();
					actor->SetDisplayName(name, true);

					cd::SetPosition(ac, pimpl->movementData.pos);
					sd::RemoveFromAllFactions(actor);
					sd::BlockActivation(actor, true);
					sd::AllowPCDialogue(actor, false);

					enum {
						CWPlayerAlly = 0x0008F36D,
					};
					cd::AddToFaction(
						cd::Value<Actor>(actor->formID),
						cd::Value<TESFaction>(CWPlayerAlly));

					if (!this->IsDerived())
					{
					}
				}

				cd::KeepOffsetFromActor(cd::Value<Actor>(ac.GetFormID()), cd::Value<Actor>(ac.GetFormID()), 0, 0, 0, 0, 0, 0, 1, 1);

				if (currentSpawning == this)
					currentSpawning = nullptr;
			});
		});
	}

	void RemotePlayer::ForceDespawn(const wchar_t *reason)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		if (pimpl->spawnStage == SpawnStage::Spawned ||
			(pimpl->spawnStage == SpawnStage::Spawning && MenuManager::GetSingleton()->IsMenuOpen("Main Menu")))
		{
			WorldCleaner::GetSingleton()->SetFormProtected(pimpl->formID, false);
			pimpl->formID = 0;
			pimpl->spawnStage = SpawnStage::NonSpawned;
			if (currentSpawning == this)
				currentSpawning = nullptr;
			if (currentFixingGreyFace == this)
				currentFixingGreyFace = nullptr;

			for (int32_t i = 0; i <= 1; ++i)
			{
				auto gnome = pimpl->handGnome[i];
				if (gnome != nullptr)
				{
					std::thread([=] {
						delete gnome;
					}).detach();
				}
				pimpl->handGnome[i] = nullptr;
				pimpl->createGnomeCalled[i] = false;
			}
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

	uint32_t RemotePlayer::GetRefID() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->formID;
	}

	bool RemotePlayer::IsBowEquipped() const
	{
		if (this->pimpl->spawnStage != SpawnStage::Spawned)
			return false;
		auto actor = (Actor *)LookupFormByID(pimpl->formID);
		if (!actor || actor->baseForm->formType != FormType::NPC)
			return false;
		const bool bowEquipped = sd::GetEquippedItemType(actor, 0) == 7
			|| sd::GetEquippedItemType(actor, 1) == 7;
		return bowEquipped;
	}

	bool RemotePlayer::IsSpellEquipped() const
	{
		return this->GetEquippedSpell(0) != nullptr
			|| this->GetEquippedSpell(1) != nullptr;
	}

	uint32_t RemotePlayer::GetLocationID() const
	{
		const uint32_t locationID =
			pimpl->currentNonExteriorCell ? pimpl->currentNonExteriorCell->formID : pimpl->worldSpaceID;
		return locationID;
	}

	bool RemotePlayer::NeedsGnome(int32_t i) const
	{
		return pimpl->eq.handsMagic[i] != nullptr;
	}

	void RemotePlayer::UpdateAll()
	{
		SAFE_CALL("RemotePlayer", [&] {
			*Impl::RemotePlayerKnownItems() = knownItems;
		});

		SAFE_CALL("RemotePlayer", [&] {
			UpdatePlaceAtMeMarker();
		});

		std::lock_guard<dlf_mutex> l(gMutex);

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

		UInt32 rating = 0;
		SAFE_CALL("RemotePlayer", [&] {
			std::for_each(allRemotePlayers.begin(), allRemotePlayers.end(), [&](RemotePlayer *p) {
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
			ApplyWorldSpell();
		});

		SAFE_CALL("RemotePlayer", [&] {
			if (ghostAxe == nullptr)
				ghostAxe = CreateGhostAxe();
			auto movData = MovementData_::GetFromPlayer();
			movData.pos.z += SyncOptions::GetSingleton()->GetFloat("GHOST_AXE_OFFSET_Z");
			movData.pos.x += GetRespawnRadius(false) * 0.85;
			ghostAxe->ApplyMovementData(movData);
			static auto localPl = ci::LocalPlayer::GetSingleton();
			ghostAxe->SetCell(localPl->GetCell());
			ghostAxe->SetWorldSpace(localPl->GetWorldSpace());
			ghostAxe->Update();
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

	void RemotePlayer::SetName(const std::wstring &name)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		if (pimpl->name != name)
		{
			pimpl->name = name;
			if (pimpl->spawnStage == SpawnStage::Spawned)
			{
				const auto formID = pimpl->formID;
				SET_TIMER(0, [=] {
					cd::SetDisplayName(cd::Value<TESObjectREFR>(formID), WstringToString(name), true);
				});
			}
		}
	}

	void RemotePlayer::SetPos(NiPoint3 pos)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->movementData.pos = pos;
	}

	void RemotePlayer::SetAngleZ(float angleZ)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->movementData.angleZ = (UInt16)angleZ;
	}

	void RemotePlayer::SetCell(uint32_t cellID)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		auto cell = LookupNonExteriorCellByID(cellID);
		pimpl->currentNonExteriorCell = cell;
	}

	void RemotePlayer::SetWorldSpace(uint32_t worldSpaceID)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->worldSpaceID = worldSpaceID;
	}

	void RemotePlayer::ApplyLookData(const LookData &lookData)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		//if (pimpl->lookData != lookData)
		{
			pimpl->lookData = lookData;
			if (pimpl->spawnStage == SpawnStage::Spawned)
				this->ForceDespawn(L"Despawned: LookData updated");
		}
	}

	void RemotePlayer::ApplyMovementData(const MovementData &movementData)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->movementData = movementData;
	}

	void RemotePlayer::UseFurniture(const Object *target, bool anim)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		// Not implemented
	}

	void RemotePlayer::AddItem(const ItemType *item, uint32_t count, bool silent)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (item && count)
			pimpl->inventory[item] += count;
	}

	void RemotePlayer::RemoveItem(const ItemType *item, uint32_t count, bool silent)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (item && count)
		{
			if (pimpl->inventory[item] > count)
				pimpl->inventory[item] -= count;
			else
			{
				pimpl->inventory.erase(item);
				this->UnequipItem(item, silent, false, false);
				this->UnequipItem(item, silent, false, true);
			}
		}
	}

	void RemotePlayer::EquipItem(const ItemType *item, bool silent, bool preventRemoval, bool leftHand)
	{
		if (item != nullptr)
		{
			std::lock_guard<dlf_mutex> l(pimpl->mutex);
			uint32_t count;
			try {
				count = pimpl->inventory.at(item);
			}
			catch (...) {
				count = 0;
			}
			if (count > 0)
			{
				if (item->GetClass() == ci::ItemType::Class::Armor)
				{
					pimpl->eq.armor.insert(item);
					return;
				}
				if (item->GetClass() == ci::ItemType::Class::Weapon)
				{
					pimpl->eq.hands[leftHand] = item;
					pimpl->handsMagicProxy[leftHand] = nullptr;
					return;
				}
				if (item->GetClass() == ci::ItemType::Class::Ammo)
				{
					pimpl->eq.ammo = item;
					return;
				}
			}
		}
	}

	void RemotePlayer::UnequipItem(const ItemType *item, bool silent, bool preventEquip, bool isLeftHand)
	{
		if (item == nullptr)
			return;

		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (item->GetClass() == ci::ItemType::Class::Armor)
		{
			pimpl->eq.armor.erase(item);
			return;
		}
		if (item->GetClass() == ci::ItemType::Class::Weapon)
		{
			if (pimpl->eq.hands[isLeftHand] == item)
				pimpl->eq.hands[isLeftHand] = nullptr;
			return;
		}
		if (item->GetClass() == ci::ItemType::Class::Ammo)
		{
			if (pimpl->eq.ammo == item)
				pimpl->eq.ammo = nullptr;
			return;
		}
	}

	void RemotePlayer::AddSpell(const Spell *spell, bool silent)
	{
		if (!spell || spell->GetNumEffects() == 0)
			return;
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->spellList.insert(spell);
	}

	void RemotePlayer::RemoveSpell(const Spell *spell, bool silent)
	{
		if (!spell || spell->GetNumEffects() == 0)
			return;
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->spellList.erase(spell);

		for (int32_t i = 0; i <= 1; ++i)
			this->UnequipSpell(spell, i);
	}

	void RemotePlayer::EquipSpell(const Spell *spell, bool leftHand)
	{
		if (!spell || spell->GetNumEffects() == 0)
			return;
		if (pimpl->spellList.find(spell) == pimpl->spellList.end())
			return;
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->handsMagicProxy[leftHand] = spell;
		pimpl->eq.hands[leftHand] = nullptr;
	}

	void RemotePlayer::UnequipSpell(const Spell *spell, bool leftHand)
	{
		if (!spell || spell->GetNumEffects() == 0)
			return;
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (pimpl->handsMagicProxy[leftHand] == spell)
			pimpl->handsMagicProxy[leftHand] = nullptr;
	}

	void RemotePlayer::PlayHitAnimation(uint8_t hitAnimID)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->hitAnimsToApply.push(hitAnimID);
	}

	void RemotePlayer::UpdateAVData(const std::string &avName_, const AVData &data)
	{
		auto avName = avName_;
		std::transform(avName.begin(), avName.end(), avName.begin(), ::tolower);
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->avData[avName] = data;
	}

	std::wstring RemotePlayer::GetName() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->name;
	}

	NiPoint3 RemotePlayer::GetPos() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->movementData.pos;
	}

	float RemotePlayer::GetAngleZ() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->movementData.angleZ;
	}

	uint32_t RemotePlayer::GetCell() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->currentNonExteriorCell ? pimpl->currentNonExteriorCell->formID : 0;
	}

	uint32_t RemotePlayer::GetWorldSpace() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->worldSpaceID;
	}

	LookData RemotePlayer::GetLookData() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->lookData;
	}

	MovementData RemotePlayer::GetMovementData() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->movementData;
	}

	std::vector<ci::ItemType *> RemotePlayer::GetEquippedArmor() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		std::vector<ci::ItemType *> result;
		for (auto item : pimpl->eq.armor)
			result.push_back(const_cast<ci::ItemType *>(item));
		return result;
	}

	ci::ItemType *RemotePlayer::GetEquippedWeapon(bool isLeftHand) const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return const_cast<ci::ItemType *>(pimpl->eq.hands[isLeftHand]);
	}

	ci::ItemType *RemotePlayer::GetEquippedAmmo() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return const_cast<ci::ItemType *>(pimpl->eq.ammo);
	}

	const ci::Spell *RemotePlayer::GetEquippedSpell(bool isLeftHand) const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->handsMagicProxy[isLeftHand];
	}

	const ci::Spell *RemotePlayer::GetEquippedShout() const
	{
		return nullptr;
		// Not implemented
	}

	ci::AVData RemotePlayer::GetAVData(const std::string &avName_) const
	{
		try {
			auto avName = avName_;
			std::transform(avName.begin(), avName.end(), avName.begin(), ::tolower);
			return pimpl->avData.at(avName);
		}
		catch (...) {
			return {};
		}
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

	void RemotePlayer::MagicAttackBegin(int32_t handID)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (handID >= 0 && handID <= 1 && pimpl->spawnStage == SpawnStage::Spawned && pimpl->isMagicAttackStarted[handID] == false)
			if (pimpl->dispenser != nullptr && pimpl->handsMagicProxy[handID] != nullptr)
			{
				const auto dispenserRef = (TESObjectREFR *)LookupFormByID(pimpl->dispenser->GetRefID());
				const auto spell = (SpellItem *)LookupFormByID(pimpl->handsMagicProxy[handID]->GetFormID());
				if (spell != nullptr)
				{
					auto gnome = pimpl->handGnome[handID];
					if (gnome != nullptr)
					{
						const auto formID = gnome->GetRefID();
						SET_TIMER_LIGHT(0, [=] {
							auto gnomeRef = (Actor *)LookupFormByID(formID);
							if (gnomeRef != nullptr)
							{
								sd::DoCombatSpellApply(gnomeRef, spell, nullptr);
							}
						});
						pimpl->isMagicAttackStarted[handID] = true;
					}
				}
				else
					ErrorHandling::SendError("ERROR:RemotePlayer MagicAttackBegin() nullptr spell");
			}
	}

	void RemotePlayer::MagicAttackEnd(int32_t handID)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		auto gnome = pimpl->handGnome[handID];
		if (gnome != nullptr && pimpl->isMagicAttackStarted[handID] == true)
		{
			pimpl->isMagicAttackStarted[handID] = false;

			const auto formID = gnome->GetRefID();
			SET_TIMER_LIGHT(0, [=] {
				auto gnomeRef = (Actor *)LookupFormByID(formID);
				if (gnomeRef != nullptr)
				{
					static SpellItem *emptySpell = nullptr;
					if (emptySpell == nullptr)
					{
						emptySpell = FormHeap_Allocate<SpellItem>();
						enum {
							Telekinesis = 0x0001A4CC,
						};
						memcpy(emptySpell, LookupFormByID(Telekinesis), sizeof SpellItem);
						emptySpell->formID = 0;
						emptySpell->SetFormID(Utility::NewFormID(), 1);
					}
					sd::DoCombatSpellApply(gnomeRef, emptySpell, nullptr);
				}
			});
		}
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
		auto result = pimpl->lookSync->Apply(pimpl->lookData);
		ApplyPackage(result);
		result->TESActorBaseData::flags.bleedoutOverride = true;
		return result;
	}

	void RemotePlayer::ApplyMovementDataImpl()
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		auto actor = (Actor *)LookupFormByID(pimpl->formID);
		if (!actor)
			return;

		SAFE_CALL("RemotePlayer", [&] {
			if (clock() - localPlCrosshairRefUpdateMoment > SyncOptions::GetSingleton()->GetInt("MAX_CD_LAG") || clock() - pimpl->lastOutOfPos < 1000)
				pimpl->unsafeSyncTimer = clock() + SyncOptions::GetSingleton()->GetInt("CD_LAG_RECOVER_TIME");
			pimpl->syncState.fullyUnsafeSync = allRemotePlayers.size() > SyncOptions::GetSingleton()->GetInt("MAX_PLAYERS_SYNCED_SAFE") || pimpl->unsafeSyncTimer > clock();
		});

		bool success = false;
		SAFE_CALL("RemotePlayer", [&] {
			if (pimpl->rating < SyncOptions::GetSingleton()->GetInt("MAX_HARDSYNCED_PLAYERS")
				&& pimpl->syncState.syncMode == MovementData_::SyncMode::Normal)
				pimpl->syncState.syncMode = MovementData_::SyncMode::Hard;
			{
				MovementData_::Apply(pimpl->movementData, actor, &pimpl->syncState, ghostAxe ? ghostAxe->pimpl->formID : 0);

				if (pimpl->afk)
					sd::EnableAI(actor, false);
				else
					sd::EnableAI(actor, true);
			}
			success = true;
		});

		SAFE_CALL("RemotePlayer", [&] {
			if (!success)
				MovementData_::Apply(pimpl->movementData, actor, &pimpl->syncState, ghostAxe ? ghostAxe->pimpl->formID : 0);
		});
	}

	class GhostAxe : public RemotePlayer
	{
	public:
		GhostAxe() : RemotePlayer(L"Ghost Axe", {}, {}, NULL, NULL)
		{
		}

		virtual ~GhostAxe() override
		{
		}

	private:
		void ApplyMovementDataImpl() override {
			auto actor = (Actor *)LookupFormByID(pimpl->formID);
			if (!actor)
				return;
			auto movData = pimpl->movementData;
			if (this->timer < clock())
			{
				timer = clock() + SyncOptions::GetSingleton()->GetInt("GHOST_AXE_UPDATE_RATE");
				cd::TranslateTo(actor, movData.pos.x, movData.pos.y, movData.pos.z, 0, 0, 0, 100000.0, 4.0);
				if (sd::GetBaseActorValue(actor, "Invisibility") == 0)
				{
					this->aiEnabled = true;
					sd::SetActorValue(actor, "Invisibility", 100);
				}
			}
			if (this->aiEnabled)
			{
				this->aiEnabled = false;
				sd::EnableAI(actor, false);
			}
		}

		TESNPC *AllocateNPC() const override {
			enum {
				InvisibleNPC = 0x00071E6B,
			};
			return (TESNPC *)LookupFormByID(InvisibleNPC);
		}

		clock_t timer = 0;
		bool aiEnabled = true;
	};

	RemotePlayer *CreateGhostAxe() {
		return new GhostAxe;
	}
}