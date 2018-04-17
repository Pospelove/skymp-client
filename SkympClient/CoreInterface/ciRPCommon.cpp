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

#include "ciRPInvisibleFox.h" // Used as SetLookAt() target for aiming npc

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
	RPGMUTEX gMutex{"ci_remoteplayer_cont"};
	ILookSynchronizer *lookSync = ILookSynchronizer::GetV17();

	std::function<void()> traceTask = nullptr;

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
		pimpl->gnomeHost.SetName(name);
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

		//this->UndoSpawn();
	}

	void RemotePlayer::UpdateNonSpawned()
	{
		SAFE_CALL("RemotePlayer", [&] {
			pimpl->nicknameLabel = nullptr;
			pimpl->visualMagicEffect = nullptr;
			pimpl->eqLast = {};
			for (int32_t i = 0; i <= 1; ++i)
				pimpl->isMagicAttackStarted[i] = false;
		});

		//this->SpawnIfNeed();
	}

	void RemotePlayer::UpdateSpawning()
	{

		//this->DetectSpawningTimeout();
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

	void RemotePlayer::UpdateDispenser()
	{
		pimpl->dispenser.SetActive(this->IsSpellEquipped() || this->IsBowEquipped());
		pimpl->dispenser.UpdateAndMoveTo(this->GetMovementData(), this->IsSpellEquipped());
	}

	void RemotePlayer::UpdateInvisibleFox()
	{
		SAFE_CALL("RemotePlayer", [&] {
			const bool needsFox = this->IsSpellEquipped() || this->IsBowEquipped();
			pimpl->invisibleFox.SetActive(needsFox);
			if (needsFox)
			{
				pimpl->invisibleFox.UpdateAndMoveTo(this->GetMovementData());
				pimpl->invisibleFox.SetOffsetZ(SyncOptions::GetSingleton()->GetFloat("INVISIBLE_FOX_OFFSET_Z_MAGIC"));
				pimpl->syncState.myFoxID = pimpl->invisibleFox.GetFormID();
			}
		});
	}

	void RemotePlayer::UpdateSpawned()
	{
		if (pimpl->stopProcessing)
			return;
		auto actor = pimpl->model.GetActor();
		if (actor && actor->formType != FormType::Character)
			actor = nullptr;
		if (!actor)
			return;

		for (auto task : pimpl->tasksSpawned)
			task();
		pimpl->tasksSpawned.clear();

		if (clock() - pimpl->lastLosCheck > 300)
		{
			pimpl->hasLOS = sd::HasLOS(g_thePlayer, actor);
			pimpl->lastLosCheck = clock();
		}

		SAFE_CALL("RemotePlayer", [&] {
			pimpl->movementDataOut = MovementData_::Get(actor);
			if ((pimpl->movementData.pos - pimpl->movementDataOut.pos).Length() < 128)
			{
				pimpl->fullySpawnedIO = true;
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

		if (this->NeedsGnome(0) || this->NeedsGnome(1))
		{
			pimpl->gnomeHost.SetActive(true);
			pimpl->gnomeHost.SetName(this->GetName());
		}

		pimpl->gnomeHost.UpdateAndMoveTo(actor, this->GetMovementData(), { this->NeedsGnome(0), this->NeedsGnome(1) });
		this->DeleteProjectile(actor);
		this->UpdateMovement(actor);
		this->ApplyHitAnimations(actor);
		this->ApplyActorValues(actor);

		this->UpdateInvisibleFox();
		this->UpdateDispenser();

		if (pimpl->timer250ms + 250 < clock())
		{
			pimpl->timer250ms = clock();

			// 'lastOutOfPos'
			SAFE_CALL("RemotePlayer", [&] {
				if ((this->GetPos() - cd::GetPosition(actor)).Length() > SyncOptions::GetSingleton()->GetFloat("MAX_OUT_OF_POS"))
					pimpl->lastOutOfPos = clock();
			});

			this->ApplyEquipmentHands(actor);
			this->ApplyEquipmentOther(actor);

			this->ManageMagicEquipment(actor);

			this->DespawnIfNeed();

			//if (pimpl->spawnStage == SpawnStage::NonSpawned)
			//	return;
		}

		//this->FixGreyFaceBug(actor);
		//this->FixGreyFaceBugUnsafe(actor);
	}

	void RemotePlayer::Update()
	{
		std::lock_guard<dlf_mutex> l1(pimpl->mutex);

		const auto isSpawned = pimpl->model.IsSpawned();

		pimpl->model.SetSpawnPoint(this->GetPos());
		pimpl->model.SetNPCSource([this] {
			auto npc = this->AllocateNPC();
			if (pimpl->baseNpc != nullptr)
				npc->race = pimpl->baseNpc->GetRace();
			return npc;
		});
		pimpl->model.SetTints(this->GetLookData().tintmasks);
		pimpl->model.SetName(this->GetName());

		pimpl->model.Update();

		if (pimpl->isSpawned != pimpl->model.IsSpawned())
		{
			pimpl->isSpawned = pimpl->model.IsSpawned();
			if (pimpl->isSpawned) // OnSpawn
			{
				pimpl->syncState = {};
			}
		}
		
		pimpl->formID = pimpl->model.GetActor() ? pimpl->model.GetActor()->formID : 0;

		try {
			isSpawned ? this->UpdateSpawned() : this->UpdateNonSpawned();
		}
		catch (...) {
			ErrorHandling::SendError("ERROR:MT:RemotePlayer '%s' '%d' ", WstringToString(this->GetName()).data(), (int32_t)isSpawned);
		}
	}

	int32_t RemotePlayer::GetSyncMode() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return (int32_t)pimpl->syncState.syncMode;
	}

	uint32_t RemotePlayer::GetRefID() const {
		return pimpl->engine->GetRefID();
	}

	bool RemotePlayer::IsBowEquipped() const
	{
		return this->IsSpawned() && ((pimpl->eq.hands[0] && pimpl->eq.hands[0]->GetSubclass() == ci::ItemType::Subclass::WEAP_Bow)
			|| (pimpl->eq.hands[1] && pimpl->eq.hands[1]->GetSubclass() == ci::ItemType::Subclass::WEAP_Bow));
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

		//RemotePlayer::FinishSpawning();

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

		SAFE_CALL("RemotePlayer", ci::WorldSpell::Tick);

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
				static dlf_mutex npcMutex{"ci_remoteplayer_tesnpc"};
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

	std::shared_ptr<uint32_t> RemotePlayer::GetNextHitAnim()
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (this->IsSpawned() == false)
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
		if(this->IsSpawned())
		{
			const auto weap = this->GetEquippedWeapon();
			const auto ammo = this->GetEquippedAmmo();
			pimpl->dispenser.Fire(weap, ammo, power);
		}
	}

	void RemotePlayer::FireMagic(const ci::Spell *spell, int32_t handID)
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if(this->IsSpawned())
		{
			const auto foxID = pimpl->syncState.myFoxID;
			if (spell != nullptr)
			{
				pimpl->tasksSpawned.push_back([=] {
					pimpl->gnomeHost.DoCombatSpellApply(spell->GetFormID(), handID, foxID);
				});
			}
		}
	}

	void RemotePlayer::MagicAttackBegin(int32_t handID)
	{
		if (handID < 0 || handID > 1)
			return;
		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		if (!pimpl->isMagicAttackStarted[handID]
			&& pimpl->handsMagicProxy[handID] != nullptr
			&& pimpl->handsMagicProxy[handID]->GetCastingType() == Spell::CastingType::Concentration
			&& pimpl->handsMagicProxy[handID]->GetDelivery() == Spell::Delivery::Aimed)
		{
			pimpl->isMagicAttackStarted[handID] = true;

			const auto spell = (SpellItem *)LookupFormByID(pimpl->handsMagicProxy[handID]->GetFormID());
			const auto myFoxID = pimpl->syncState.myFoxID;

			pimpl->tasksSpawned.push_back([=] {
				pimpl->gnomeHost.DoCombatSpellApply(spell->GetFormID(), handID, myFoxID);
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
			pimpl->tasksSpawned.push_back([=] {
				pimpl->gnomeHost.DoCombatSpellApply(ID_SpellItem::Telekinesis, handID, 0x00000000);
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
		return pimpl->isSpawned;
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
		pimpl->model.Despawn(L"ClientLogic");
	}

	uint32_t RemotePlayer::GetFormID() const
	{
		// TODO: Remove this function!
		return this->GetRefID();
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