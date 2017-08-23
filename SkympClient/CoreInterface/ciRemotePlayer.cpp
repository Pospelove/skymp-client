#include "../stdafx.h"
#include "../Sync/LookData.h"
#include "../Sync/MovementData.h"
#include "Skyrim\Camera\PlayerCamera.h"
#include "CoreInterface.h"
#include <algorithm>

#define NUM_GHOST_AXES 1
#define MAX_HARDSYNCED_PLAYERS 5
#define MAX_PLAYERS_SYNCED_SAFE 10
#define GHOST_AXE_OFFSET_Z (2048.f * 1)

namespace ci
{
	class GhostAxe;
	GhostAxe *CreateGhostAxe();

	std::set<RemotePlayer *> allRemotePlayers;
	std::vector<RemotePlayer *> ghostAxes;
	RemotePlayer *currentSpawning = nullptr;
	FormID currentSpawningBaseID = 0;
	clock_t lastForceSpawn = 0;
	RemotePlayer *currentFixingGreyFace = nullptr;
	std::recursive_mutex gMutex;

	float GetRespawnRadius(bool isInterior)
	{
		return isInterior ? 4096.0 : 8192.0;
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

		static std::recursive_mutex mutex;

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
				std::lock_guard<std::recursive_mutex> l(mutex);
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
					std::lock_guard<std::recursive_mutex> l(mutex);
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
		std::recursive_mutex mutex;
		FormID formID = 0;
		std::wstring name;
		LookData lookData;
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
	};

	RemotePlayer::RemotePlayer(const std::wstring &name, const LookData &lookData, NiPoint3 spawnPoint, uint32_t cellID, uint32_t worldSpaceID) :
		pimpl(new Impl)
	{
		std::lock_guard<std::recursive_mutex> l(gMutex);
		std::lock_guard<std::recursive_mutex> l1(pimpl->mutex);

		allRemotePlayers.insert(this);

		pimpl->name = name;
		pimpl->lookData = lookData;
		pimpl->movementData.pos = spawnPoint;
		pimpl->currentNonExteriorCell = LookupNonExteriorCellByID(cellID);
		pimpl->worldSpaceID = worldSpaceID;
	}

	RemotePlayer::~RemotePlayer()
	{
		std::lock_guard<std::recursive_mutex> l(gMutex);
		std::lock_guard<std::recursive_mutex> l1(pimpl->mutex);

		allRemotePlayers.erase(this);

		if (pimpl->formID != NULL)
			WorldCleaner::GetSingleton()->SetFormProtected(pimpl->formID, false);
		if (currentSpawning == this)
			currentSpawning = nullptr;
		if (currentFixingGreyFace == this)
			currentFixingGreyFace = nullptr;
	}

	void RemotePlayer::Update()
	{
		std::lock_guard<std::recursive_mutex> l1(pimpl->mutex);

		const NiPoint3 localPlPos = cd::GetPosition(g_thePlayer);
		TESObjectCELL *const localPlCell = sd::GetParentCell(g_thePlayer);
		const bool isInterior = localPlCell ? sd::IsInterior(localPlCell) : false;

		switch (pimpl->spawnStage)
		{
		case SpawnStage::NonSpawned:
		{
			const FormID worldSpaceID = sd::GetWorldSpace(g_thePlayer) ? sd::GetWorldSpace(g_thePlayer)->formID : NULL;

			if (markerFormID
				&& NiPoint3{ pimpl->movementData.pos - localPlPos }.Length() < GetRespawnRadius(isInterior)
				&& (pimpl->currentNonExteriorCell == GetParentNonExteriorCell(g_thePlayer) || pimpl->currentNonExteriorCell == nullptr)
				&& pimpl->worldSpaceID == worldSpaceID
				)
			{
				if (currentSpawning == nullptr && currentSpawningBaseID == NULL)
				{
					currentSpawning = this;
					lastForceSpawn = clock();
					auto npc = this->GetNPC();
					currentSpawningBaseID = npc->GetFormID();
					WorldCleaner::GetSingleton()->SetFormProtected(currentSpawningBaseID, true);
					return this->ForceSpawn(currentSpawningBaseID);
				}
			}
		}
		break;

		case SpawnStage::Spawning:
		{
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
			auto actor = (Actor *)LookupFormByID(pimpl->formID);
			if (!actor)
				return this->ForceDespawn(L"Despawned: Unloaded by the game");

			if (pimpl->spawnMoment + 1000 < clock() 
				&& (sd::GetRace(actor)->formID != this->pimpl->lookData.raceID || memcmp(((TESNPC *)actor->baseForm)->faceMorph->option, &this->pimpl->lookData.options[0], sizeof LookData().options))
				&& this->pimpl->lookData.isEmpty() == false)
			{
				auto lookData = this->pimpl->lookData;
				this->pimpl->lookData = {};
				this->ApplyLookData(lookData);
				return;
			}

			if (pimpl->greyFaceFixed)
			{
				this->ApplyMovementDataImpl();
			}

			if (pimpl->timer250ms + 250 < clock())
			{
				pimpl->timer250ms = clock();

				if (pimpl->currentNonExteriorCell != GetParentNonExteriorCell(g_thePlayer))
					return this->ForceDespawn(L"Despawned: Cell changed");

				const FormID worldSpaceID = sd::GetWorldSpace(g_thePlayer) ? sd::GetWorldSpace(g_thePlayer)->formID : NULL;
				if (pimpl->worldSpaceID != worldSpaceID && worldSpaceID != NULL && pimpl->worldSpaceID != NULL)
					return this->ForceDespawn(std::wstring(L"Despawned: WorldSpace changed " + std::to_wstring(worldSpaceID) + L' ' + std::to_wstring(pimpl->worldSpaceID)).data());

				if (NiPoint3{ pimpl->movementData.pos - localPlPos }.Length() >= GetRespawnRadius(isInterior))
					return this->ForceDespawn(L"Despawned: Player is too far");
			}

			if (pimpl->timer1000ms + 1000 < clock())
			{
				pimpl->timer1000ms = clock();
			}

			if (!pimpl->greyFaceFixed && currentFixingGreyFace == nullptr)
			{
				currentFixingGreyFace = this;
				actor->baseForm->formID = g_thePlayer->baseForm->formID; // AAAAAA
				LookData_::ApplyTintMasks((TESNPC *)actor->baseForm, pimpl->lookData.tintmasks);
				pimpl->greyFaceFixed = true;
				sd::Disable(actor, false);

				const auto formID = pimpl->formID;
				const bool isInterior = pimpl->currentNonExteriorCell != nullptr;
				Timer::Set(0, [=] {
					auto actor = (Actor *)LookupFormByID(formID);
					sd::Enable(actor, false);
					cd::KeepOffsetFromActor(cd::Value<Actor>(formID), cd::Value<Actor>(formID), 0, 0, 0, 0, 0, 0, 1, 1);
					Timer::Set(100, [=] {
						if (this == currentFixingGreyFace)
							currentFixingGreyFace = nullptr;

						auto actor = (Actor *)LookupFormByID(formID);
						if (!actor)
							return;

						//if ((cd::GetPosition(actor) - pimpl->movementData.pos).Length() > 0.5 * GetRespawnRadius(isInterior) * 0.85)
						//	this->SetPos(pimpl->movementData.pos);

						LookData_::PreventCrash((TESNPC *)actor->baseForm);
					});
				});
			}
		}
		break;
		}
	}

	void RemotePlayer::Update_OT()
	{
		if (MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
			return this->ForceDespawn(L"Despawned: Game exit");
	}

	void RemotePlayer::ForceSpawn(uint32_t npcID)
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

		if (pimpl->spawnStage != SpawnStage::NonSpawned)
			return;

		pimpl->spawnStage = SpawnStage::Spawning;

		auto refToPlaceAt = (TESObjectREFR *)LookupFormByID(markerFormID);
		if (refToPlaceAt && refToPlaceAt->formType != FormType::Reference)
			refToPlaceAt = nullptr;
		if (!refToPlaceAt)
			refToPlaceAt = g_thePlayer;

		cd::PlaceAtMe(refToPlaceAt, (TESNPC *)LookupFormByID(npcID), 1, true, false, [=](cd::Value<TESObjectREFR> ac) {
			Timer::Set(0, [=] {
				if (LookupFormByID(ac.GetFormID()) == nullptr)
					return;

				std::lock_guard<std::recursive_mutex> l(gMutex);
				if (allRemotePlayers.find(this) == allRemotePlayers.end())
					return;

				std::lock_guard<std::recursive_mutex> l1(pimpl->mutex);

				pimpl->formID = ac.GetFormID();
				WorldCleaner::GetSingleton()->SetFormProtected(pimpl->formID, true);
				pimpl->spawnStage = SpawnStage::Spawned;
				pimpl->spawnMoment = clock();
				pimpl->greyFaceFixed = false;
				pimpl->syncState = {};

				auto actor = (Actor *)LookupFormByID(ac.GetFormID());
				if (actor)
				{
					sd::SetActorValue(actor, "Confidence", 4.0);
					sd::SetActorValue(actor, "Agression", 0.0);

					cd::AddToFaction(cd::Value<Actor>(actor->formID), cd::Value<TESFaction>(0x0008F36D));

					BSFixedString name = WstringToString(pimpl->name).c_str();
					actor->SetDisplayName(name, true);

					cd::SetPosition(ac, pimpl->movementData.pos);

					//sd::AllowPCDialogue(actor, false);
					sd::RemoveFromAllFactions(actor);
					sd::BlockActivation(actor, true);
				}

				cd::KeepOffsetFromActor(cd::Value<Actor>(ac.GetFormID()), cd::Value<Actor>(ac.GetFormID()), 0, 0, 0, 0, 0, 0, 1, 1);

				if (currentSpawning == this)
					currentSpawning = nullptr;
			});
		});
	}

	void RemotePlayer::ForceDespawn(const wchar_t *reason)
	{

		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

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
		}
	}

	void RemotePlayer::UpdateAll()
	{
		UpdatePlaceAtMeMarker();

		std::lock_guard<std::recursive_mutex> l(gMutex);

		if (currentSpawning != nullptr && allRemotePlayers.find(currentSpawning) == allRemotePlayers.end())
			currentSpawning = nullptr;
		if (currentFixingGreyFace != nullptr && allRemotePlayers.find(currentFixingGreyFace) == allRemotePlayers.end())
			currentFixingGreyFace = nullptr;

		if (currentSpawning == nullptr && currentSpawningBaseID != NULL)
		{
			WorldCleaner::GetSingleton()->SetFormProtected(currentSpawningBaseID, false);
			currentSpawningBaseID = NULL;
		}

		auto pos = cd::GetPosition(g_thePlayer);
		std::vector<RemotePlayer *> allRemotePlayers_(allRemotePlayers.begin(), allRemotePlayers.end());
		std::sort(allRemotePlayers_.begin(), allRemotePlayers_.end(), [=](RemotePlayer *p1, RemotePlayer *p2) {
			float p1Distance = 1000 * 1000 * 1000,
				p2Distance = 1000 * 1000 * 1000;
			if (p1)
				p1Distance = (pos - p1->GetPos()).Length();
			if (p2)
				p2Distance = (pos - p2->GetPos()).Length();
			return p1Distance < p2Distance;
		});

		std::vector<RemotePlayer *> notUpdatedYet = {};
		notUpdatedYet.reserve(allRemotePlayers_.size() + 1);
		UInt32 rating = 0;
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

		for (auto it = notUpdatedYet.begin(); it != notUpdatedYet.end(); ++it)
		{
			if (*it)
				(*it)->Update();
		}

		if (ghostAxes.empty())
		{
			for (SInt32 i = 0; i != NUM_GHOST_AXES; ++i)
				ghostAxes.push_back(reinterpret_cast<RemotePlayer *>(CreateGhostAxe()));
		}
		for (auto it = ghostAxes.begin(); it != ghostAxes.end(); ++it)
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
	}

	void RemotePlayer::UpdateAll_OT()
	{
		std::lock_guard<std::recursive_mutex> l(gMutex);
		for (auto it = allRemotePlayers.begin(); it != allRemotePlayers.end(); ++it)
		{
			if (*it)
				(*it)->Update_OT();
		}
	}

	void RemotePlayer::SetName(const std::wstring &name)
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

		if (pimpl->name != name)
		{
			pimpl->name = name;
			if (pimpl->spawnStage == SpawnStage::Spawned)
			{
				const auto formID = pimpl->formID;
				Timer::Set(0, [=] {
					cd::SetDisplayName(cd::Value<TESObjectREFR>(formID), WstringToString(name), true);
				});
			}
		}
	}

	void RemotePlayer::SetPos(NiPoint3 pos)
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		pimpl->movementData.pos = pos;
	}

	void RemotePlayer::SetAngleZ(float angleZ)
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		pimpl->movementData.angleZ = (UInt16)angleZ;
	}

	void RemotePlayer::SetCell(uint32_t cellID)
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

		auto cell = LookupNonExteriorCellByID(cellID);
		pimpl->currentNonExteriorCell = cell;
	}

	void RemotePlayer::SetWorldSpace(uint32_t worldSpaceID)
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		pimpl->worldSpaceID = worldSpaceID;
	}

	void RemotePlayer::ApplyLookData(const LookData &lookData)
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

		//if (pimpl->lookData != lookData)
		{
			pimpl->lookData = lookData;
			if (pimpl->spawnStage == SpawnStage::Spawned)
				this->ForceDespawn(L"Despawned: LookData updated");
		}
	}

	void RemotePlayer::ApplyMovementData(const MovementData &movementData)
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		pimpl->movementData = movementData;
	}

	std::wstring RemotePlayer::GetName() const
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		return pimpl->name;
	}

	NiPoint3 RemotePlayer::GetPos() const
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		return pimpl->movementData.pos;
	}

	float RemotePlayer::GetAngleZ() const
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		return pimpl->movementData.angleZ;
	}

	uint32_t RemotePlayer::GetCell() const
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		return pimpl->currentNonExteriorCell ? pimpl->currentNonExteriorCell->formID : 0;
	}

	uint32_t RemotePlayer::GetWorldSpace() const
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		return pimpl->worldSpaceID;
	}

	LookData RemotePlayer::GetLookData() const
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		return pimpl->lookData;
	}

	MovementData RemotePlayer::GetMovementData() const
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		return pimpl->movementData;
	}

	void RemotePlayer::SetInAFK(bool val) 
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		pimpl->afk = val;
	}

	TESNPC *RemotePlayer::GetNPC() const
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
		auto result = LookData_::Apply(pimpl->lookData);
		ApplyPackage(result);
		return result;
	}

	void RemotePlayer::ApplyMovementDataImpl()
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
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
		
		~GhostAxe()
		{
		}

		void ApplyMovementDataImpl() override
		{
			auto actor = (Actor *)LookupFormByID(pimpl->formID);
			if (!actor)
				return;
			auto movData = pimpl->movementData;
			if (this->timer < clock())
			{
				timer = clock() + 333;
				cd::TranslateTo(actor, movData.pos.x, movData.pos.y, movData.pos.z, 0, 0, 0, 100000.0, 4.0);
				if (sd::GetBaseActorValue(actor, "Invisibility") == 0)
					sd::SetActorValue(actor, "Invisibility", 100);
			}
			if (this->aiEnabled)
			{
				this->aiEnabled = false;
				sd::EnableAI(actor, false);
			}
		}

		TESNPC *GetNPC() const override
		{
			return (TESNPC *)LookupFormByID(0x00071E6B);
		}

		clock_t timer = 0;
		bool aiEnabled = true;
	};

	GhostAxe *CreateGhostAxe()
	{
		return new GhostAxe;
	}
}