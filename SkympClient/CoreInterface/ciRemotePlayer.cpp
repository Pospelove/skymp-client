#include "../stdafx.h"
#include "../Sync/LookData.h"
#include "../Sync/MovementData.h"
#include "../Sync/Equipment.h"
#include "../Sync/HitData.h"
#include "Skyrim\Camera\PlayerCamera.h"
#include "CoreInterface.h"
#include <algorithm>
#include <queue>

enum {
	DRAW_DISTANCE = 						int32_t(70.0218818381 * 100), // 100 metres

	GHOST_AXE_COUNT = 						0,
	GHOST_AXE_OFFSET_Z = 					2048,
	GHOST_AXE_UPDATE_RATE =					750,

	//INVISIBLE_FOX_UPDATE_RATE =				0,

	MAX_HARDSYNCED_PLAYERS = 				5,
	MAX_PLAYERS_SYNCED_SAFE	=				10,
};

extern std::map<TESForm *, const ci::ItemType *> knownItems;

namespace ci
{
	RemotePlayer *CreateGhostAxe();
	std::set<RemotePlayer *> allRemotePlayers;
	std::vector<RemotePlayer *> ghostAxes;
	RemotePlayer *currentSpawning = nullptr;
	uint32_t currentSpawningBaseID = 0;
	clock_t lastForceSpawn = 0;
	RemotePlayer *currentFixingGreyFace = nullptr;
	dlf_mutex gMutex;

	inline float GetRespawnRadius(bool isInterior) {
		return DRAW_DISTANCE;
	}

	void ApplyPackage(TESNPC *npc)
	{
		// forceav variable10 1.0/0.0 to start/stop combat

		auto nonDrawn = (TESPackage *)LookupFormByID(0x000654E2);
		auto drawn = (TESPackage *)LookupFormByID(0x0004E4BB);

		if (!npc)
			return;
		auto &data = npc->TESAIForm::unk10;
		data.unk0 = (UInt32)drawn;
		data.next = FormHeap_Allocate<TESAIForm::Data>();
		data.next->unk0 = (UInt32)nonDrawn;
		data.next->next = nullptr;
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
		if(!cell->IsInterior())
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
		dlf_mutex mutex;
		FormID formID = 0;
		std::wstring name;
		LookData lookData;
		ILookSynchronizer *lookSync = ILookSynchronizer::GetV17();
		TESObjectCELL *currentNonExteriorCell = nullptr;
		FormID worldSpaceID = 0;
		SpawnStage spawnStage = SpawnStage::NonSpawned;
		clock_t spawnMoment = 0;
		clock_t timer250ms = 0;
		clock_t timer1000ms = 0;
		bool greyFaceFixed = false;
		MovementData movementData;
		MovementData_::SyncState syncState;
		UInt32 rating = 0;
		bool afk = false;
		bool stopProcessing = false;
		std::map<const ci::ItemType *, uint32_t> inventory;
		std::set<uint32_t> knownArmor;
		std::set<TESForm *> knownWeaps;
		std::queue<uint8_t> hitAnimsToApply;
		OnHit onHit = nullptr;
		std::map<std::string, ci::AVData> avData, avDataLast;

		struct Equipment
		{
			Equipment() {
				hands[0] = hands[1] = nullptr;
			}

			std::array<const ci::ItemType *, 2> hands;
			std::set<const ci::ItemType *> armor;
			const ci::ItemType *ammo = nullptr;

			friend bool operator==(const Equipment &l, const Equipment &r) {
				return l.hands[0] == r.hands[0] && l.hands[1] == r.hands[1] && l.armor == r.armor && l.ammo == r.ammo;
			}
			friend bool operator!=(const Equipment &l, const Equipment &r) {
				return !(l == r);
			}
		} eq, eqLast;

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
								onHit(hitEventData);
							}).detach();
						}
					}
				}).detach();

				return EventResult::kEvent_Continue;
			}
		};
	};

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
		
		/*if (pimpl->myFox != nullptr)
		{
			delete pimpl->myFox;
			pimpl->myFox = nullptr;
		}*/
	}

	void RemotePlayer::Update()
	{
		try {
			std::lock_guard<dlf_mutex> l1(pimpl->mutex);

			const NiPoint3 localPlPos = cd::GetPosition(g_thePlayer);
			TESObjectCELL *const localPlCell = sd::GetParentCell(g_thePlayer);
			const bool isInterior = localPlCell ? sd::IsInterior(localPlCell) : false;

			switch (pimpl->spawnStage)
			{
			case SpawnStage::NonSpawned:
			{
				if (Utility::IsForegroundProcess())
				{
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
							currentSpawningBaseID = npc->GetFormID();
							WorldCleaner::GetSingleton()->SetFormProtected(currentSpawningBaseID, true);
							return this->ForceSpawn(currentSpawningBaseID);
						}
					}
				}
			}
			break;

			case SpawnStage::Spawning:
			{
				pimpl->avDataLast.clear();

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
					return;
				}
			}
			break;

			case SpawnStage::Spawned:
			{
				if (pimpl->stopProcessing)
					break;
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

				if (pimpl->greyFaceFixed)
				{
					// Apply Movement
					SAFE_CALL("RemotePlayer", [&] {
						this->ApplyMovementDataImpl();
					});

					// Apply Hits
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

					// Apply Health
					SAFE_CALL("RemotePlayer", [&] {
						if (pimpl->avDataLast != pimpl->avData)
						{
							pimpl->avDataLast = pimpl->avData;
							enum {
								InternalMult = 10000,
							};
							const auto full = (pimpl->avData["health"].base + pimpl->avData["health"].modifier) * InternalMult;
							const auto dest = full * pimpl->avData["health"].percentage;
							sd::SetActorValue(actor, "health", full);
							auto current = sd::GetActorValue(actor, "health");
							auto change = dest - current;
							if (change > 0)
								sd::RestoreActorValue(actor, "health", change);
							else
								sd::DamageActorValue(actor, "health", -change);
						}
					});
				}

				if (pimpl->timer250ms + 250 < clock())
				{
					pimpl->timer250ms = clock();

					// Apply Equipment
					SAFE_CALL("RemotePlayer", [&] {
						if (pimpl->eq != pimpl->eqLast)
						{
							Equipment_::Equipment eq;
							for (int32_t i = 0; i <= 1; ++i)
								eq.hands[i] = pimpl->eq.hands[i] ? LookupFormByID(pimpl->eq.hands[i]->GetFormID()) : nullptr;
							if (pimpl->eq.ammo != nullptr)
								eq.other.insert(LookupFormByID(pimpl->eq.ammo->GetFormID()));
							for(auto &item : pimpl->eq.armor)
								eq.other.insert(LookupFormByID(item->GetFormID()));
							Equipment_::Apply(actor, eq);
							pimpl->eqLast = pimpl->eq;
						}
					});

					SAFE_CALL("RemotePlayer", [&] {
						if (pimpl->currentNonExteriorCell != GetParentNonExteriorCell(g_thePlayer))
							this->ForceDespawn(L"Despawned: Cell changed");
					});
					if (pimpl->spawnStage == SpawnStage::NonSpawned)
						return;

					SAFE_CALL("RemotePlayer", [&] {
						const FormID worldSpaceID = sd::GetWorldSpace(g_thePlayer) ? sd::GetWorldSpace(g_thePlayer)->formID : NULL;
						if (pimpl->worldSpaceID != worldSpaceID && worldSpaceID != NULL && pimpl->worldSpaceID != NULL)
							this->ForceDespawn(std::wstring(L"Despawned: WorldSpace changed " + std::to_wstring(worldSpaceID) + L' ' + std::to_wstring(pimpl->worldSpaceID)).data());
					});
					if (pimpl->spawnStage == SpawnStage::NonSpawned)
						return;

					SAFE_CALL("RemotePlayer", [&] {
						if (NiPoint3{ pimpl->movementData.pos - localPlPos }.Length() >= GetRespawnRadius(isInterior))
							this->ForceDespawn(L"Despawned: Player is too far");
					});
					if (pimpl->spawnStage == SpawnStage::NonSpawned)
						return;
				}

				if (pimpl->timer1000ms + 1000 < clock())
				{
					pimpl->timer1000ms = clock();
				}

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

					BSFixedString name = WstringToString(pimpl->name).c_str();
					actor->SetDisplayName(name, true);

					cd::SetPosition(ac, pimpl->movementData.pos);
					sd::RemoveFromAllFactions(actor);
					sd::BlockActivation(actor, true);

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

			/*if (pimpl->myFox != nullptr)
			{
				delete pimpl->myFox;
				pimpl->myFox = nullptr;
			}*/
		}
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


		std::vector<RemotePlayer *> allRemotePlayers_(allRemotePlayers.begin(), allRemotePlayers.end());
		SAFE_CALL("RemotePlayer", [&] {
			auto pos = cd::GetPosition(g_thePlayer);
			std::sort(allRemotePlayers_.begin(), allRemotePlayers_.end(), [=](RemotePlayer *p1, RemotePlayer *p2) {
				float p1Distance = 1000 * 1000 * 1000,
					p2Distance = 1000 * 1000 * 1000;
				if (p1)
					p1Distance = (pos - p1->GetPos()).Length();
				if (p2)
					p2Distance = (pos - p2->GetPos()).Length();
				return p1Distance < p2Distance;
			});
		});

		std::vector<RemotePlayer *> notUpdatedYet = {};
		notUpdatedYet.reserve(allRemotePlayers_.size() + 1);
		UInt32 rating = 0;
		SAFE_CALL("RemotePlayer", [&] {
			std::for_each(allRemotePlayers_.begin(), allRemotePlayers_.end(), [&](RemotePlayer *p){
				auto pos = p->GetPos();
				if (PlayerCamera::GetSingleton()->IsInScreen(pos)
					|| PlayerCamera::GetSingleton()->IsInScreen(pos + NiPoint3{ 0,0,64 })
					|| PlayerCamera::GetSingleton()->IsInScreen(pos + NiPoint3{ 0,0,128 }))
				{
					p->pimpl->rating = rating++;
					p->Update();
				}
				else
					notUpdatedYet.push_back(p);
			});
		});

		SAFE_CALL("RemotePlayer", [&] {
			for (auto it = notUpdatedYet.begin(); it != notUpdatedYet.end(); ++it)
			{
				if (*it)
					(*it)->Update();
			}
		});

		SAFE_CALL("RemotePlayer", [&] {
			if (ghostAxes.empty())
			{
				for (SInt32 i = 0; i != GHOST_AXE_COUNT; ++i)
					ghostAxes.push_back(CreateGhostAxe());
			}
		});
		SAFE_CALL("RemotePlayer", [&] {
			for	(auto it = ghostAxes.begin(); it != ghostAxes.end(); ++it)
			{
				auto ptr = (*it);
				auto movData = MovementData_::GetFromPlayer();
				movData.pos.z += GHOST_AXE_OFFSET_Z;
				movData.pos.x += GetRespawnRadius(false) * 0.85;
				ptr->ApplyMovementData(movData);
				static auto localPl = ci::LocalPlayer::GetSingleton();
				ptr->SetCell(localPl->GetCell());
				ptr->SetWorldSpace(localPl->GetWorldSpace());
				ptr->Update();
			}
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
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (item)
		{
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

	TESNPC *RemotePlayer::AllocateNPC() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		auto result = pimpl->lookSync->Apply(pimpl->lookData);
		ApplyPackage(result);
		result->TESActorBaseData::flags.bleedoutOverride = true;
		result->combatStyle =
			(TESCombatStyle *)LookupFormByID(0x000F960C);
		if (result->combatStyle == nullptr)
			ErrorHandling::SendError("ERROR:RemotePlayer CombatStyle");
		return result;
	}

	void RemotePlayer::ApplyMovementDataImpl()
	{
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		auto actor = (Actor *)LookupFormByID(pimpl->formID);
		if (!actor)
			return;

		std::vector<RefHandle> ghostAxeHandles;
		ghostAxeHandles.reserve(ghostAxes.size());
		for (auto it = ghostAxes.begin(); it != ghostAxes.end(); ++it)
			ghostAxeHandles.push_back((*it)->pimpl->formID);

		if (allRemotePlayers.size() <= MAX_PLAYERS_SYNCED_SAFE)
			pimpl->syncState.fullyUnsafeSync = false;
		else
			pimpl->syncState.fullyUnsafeSync = true;

		if (pimpl->rating < MAX_HARDSYNCED_PLAYERS
			&& pimpl->syncState.syncMode == MovementData_::SyncMode::Normal)
			pimpl->syncState.syncMode = MovementData_::SyncMode::Hard;
		{
			MovementData_::Apply(pimpl->movementData, actor, &pimpl->syncState, ghostAxeHandles);

			if (pimpl->afk)
				sd::EnableAI(actor, false);
			else
				sd::EnableAI(actor, true);
		}
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
				timer = clock() + GHOST_AXE_UPDATE_RATE;
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

	/*class InvisibleFox : public RemotePlayer
	{
	public:
		InvisibleFox() : RemotePlayer(InvisibleFox::GetName(), {}, {}, NULL, NULL)
		{
		}

		virtual ~InvisibleFox() override
		{
		}

		static wchar_t *GetName() {
			return L"Invisible Fox";
		}

	private:
		void ApplyMovementDataImpl() override {
			auto actor = (Actor *)LookupFormByID(pimpl->formID);
			if (!actor)
				return;
			auto movData = pimpl->movementData;
			if (this->timer < clock())
			{
				bool stop = L"Stopped Fox" == pimpl->name;
				if (!stop)
				{
					timer = clock() + INVISIBLE_FOX_UPDATE_RATE;
					const NiPoint3 pos = {
						sd::GetPositionX(actor), sd::GetPositionY(actor), sd::GetPositionZ(actor)
					};
					const float distance = (movData.pos - pos).Length();
					const float t = 0.150;
					const float speed = distance / t;
					cd::TranslateTo(actor, movData.pos.x, movData.pos.y, movData.pos.z, 0, 0, 0, speed, 4.0);
					if (sd::GetBaseActorValue(actor, "Invisibility") == 0)
					{
						this->aiEnabled = true;
					}
					cd::KeepOffsetFromActor(actor, actor, 0, 0, 0, 0, 0, 0, 1, 1);
				}
			}
			if (this->aiEnabled)
			{
				this->aiEnabled = false;
				//sd::EnableAI(actor, false);
			}
		}

		TESNPC *AllocateNPC() const override {
			enum {
				Pumpkin = 0x000B11A7,
				//0x2b362,
			};
			static auto npc = (TESNPC *)LookupFormByID(Pumpkin);
			npc->TESActorBaseData::flags.essential = true;
			return npc;
		}

		clock_t timer = 0;
		bool aiEnabled = true;
	};

	RemotePlayer *CreateInvisibleFox() {
		return new InvisibleFox;
	}*/
}