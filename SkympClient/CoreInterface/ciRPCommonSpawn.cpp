#include "../stdafx.h"
#include "../Sync/SyncEssential.h"
#include "CoreInterface.h"

#include "ciRPEngineBase.h"

#include "ciRPPlaceMarker.h"
#include "ciRPGhostAxe.h"

namespace ci
{
	RemotePlayer *currentSpawning = nullptr;
	uint32_t currentSpawningBaseID = 0;
	clock_t lastForceSpawn = 0;
	RemotePlayer *currentFixingGreyFace = nullptr;
	uint64_t errorsInSpawn = false;
	clock_t lastGreyFaceFix = 0;
	clock_t lastTintMaskUse = 0;
}

void ci::RemotePlayer::SpawnIfNeed()
{
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

void ci::RemotePlayer::UndoSpawn()
{
	if (currentSpawning == this)
		currentSpawning = nullptr;
	if (currentFixingGreyFace == this)
		currentFixingGreyFace = nullptr;
}

void ci::RemotePlayer::DetectSpawningTimeout()
{
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

void ci::RemotePlayer::DespawnIfNeed(Actor *actor)
{
	if (!actor)
		return this->ForceDespawn(L"Despawned: Unloaded by the game");

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

void ci::RemotePlayer::FixGreyFaceBugUnsafe(Actor *actor) // 1.0.1 shitfix
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

void ci::RemotePlayer::FixGreyFaceBug(Actor *actor)
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

void ci::RemotePlayer::ForceSpawn(TESNPC *npc)
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

	pimpl->gnomeHost.SetActive(false);
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

void ci::RemotePlayer::ForceDespawn(const wchar_t *reason)
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

void ci::RemotePlayer::FinishSpawning()
{
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
}