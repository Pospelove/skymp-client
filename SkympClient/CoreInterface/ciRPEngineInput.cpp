#include "../stdafx.h"
#include "CoreInterface.h"
#include "ciRPEngineInput.h"

#include "../Sync/SyncEssential.h"
#include "Skyrim\Camera\PlayerCamera.h"

#include "ciRPEngineBase.h"
#include "ciRPGhostAxe.h"

extern clock_t localPlCrosshairRefUpdateMoment;

ci::RPEngineInput::RPEngineInput(RemotePlayer *argParent) : IRemotePlayerEngine(argParent)
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	pimpl->posTask = nullptr;
	pimpl->angleTask = nullptr;
	pimpl->pathToTarget.reset();
}

ci::RPEngineInput::~RPEngineInput()
{
}

void ci::RPEngineInput::SetPos(NiPoint3 pos)
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	pimpl->movementData.pos = pos;
}

void ci::RPEngineInput::SetAngleZ(float angle)
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	pimpl->movementData.angleZ = (UInt16)angle;
}

void ci::RPEngineInput::ApplyMovementData(const MovementData &movementData)
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	pimpl->movementData = movementData;
}

void ci::RPEngineInput::EquipSpell(const Spell *spell, bool leftHand)
{
	if (!spell || spell->IsEmpty())
		return;
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	pimpl->handsMagicProxy[leftHand] = spell;
	pimpl->eq.hands[leftHand] = nullptr;
}

void ci::RPEngineInput::UnequipSpell(const Spell *spell, bool leftHand)
{
	if (!spell || spell->IsEmpty())
		return;
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	if (pimpl->handsMagicProxy[leftHand] == spell)
		pimpl->handsMagicProxy[leftHand] = nullptr;
}

NiPoint3 ci::RPEngineInput::GetPos() const
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	return pimpl->movementData.pos;
}

float ci::RPEngineInput::GetAngleZ() const
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	return pimpl->movementData.angleZ;
}

ci::MovementData ci::RPEngineInput::GetMovementData() const
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	return pimpl->movementData;
}

const ci::Spell *ci::RPEngineInput::GetEquippedSpell(bool isLeftHand) const
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	return pimpl->handsMagicProxy[isLeftHand];
}

void ci::RPEngineInput::ApplyMovementDataImpl()
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	auto actor = (Actor *)LookupFormByID(pimpl->formID);
	if (!actor)
		return;

	SAFE_CALL("RemotePlayer", [&] {
		if (clock() - localPlCrosshairRefUpdateMoment > SyncOptions::GetSingleton()->GetInt("MAX_CD_LAG") || clock() - pimpl->lastOutOfPos < 1000)
			pimpl->unsafeSyncTimer = clock() + SyncOptions::GetSingleton()->GetInt("CD_LAG_RECOVER_TIME");
		pimpl->syncState.fullyUnsafeSync = RemotePlayer::GetNumInstances() > SyncOptions::GetSingleton()->GetInt("MAX_PLAYERS_SYNCED_SAFE") || pimpl->unsafeSyncTimer > clock();
	});

	const auto ghostAxeID = ci::GhostAxeManager::GetSingleton().GetGhostAxeFormID();

	bool success = false;
	SAFE_CALL("RemotePlayer", [&] {
		if (pimpl->rating < SyncOptions::GetSingleton()->GetInt("MAX_HARDSYNCED_PLAYERS")
			&& pimpl->syncState.syncMode == MovementData_::SyncMode::Normal)
			pimpl->syncState.syncMode = MovementData_::SyncMode::Hard;
		{
			auto md = pimpl->movementData;
			//if (sd::IsInCombat(g_thePlayer) && sd::GetCombatTarget(g_thePlayer) != actor)
			//	md.isWeapDrawn = true; // предотвращение эпилепсии
			MovementData_::Apply(md, actor, &pimpl->syncState, ghostAxeID);

			if (pimpl->afk)
				sd::EnableAI(actor, false);
			else
				sd::EnableAI(actor, true);
		}
		success = true;
	});

	SAFE_CALL("RemotePlayer", [&] {
		if (!success)
			MovementData_::Apply(pimpl->movementData, actor, &pimpl->syncState, ghostAxeID);
	});
}


const char *ci::RPEngineInput::GetEngineName() const
{
	return "RPEngineInput";
}

void ci::RPEngineInput::EngineUpdateSpawned(Actor *actor)
{
	// Already locked
	auto &pimpl = this->GetImpl();

	//sd::SetActorValue(actor, "Agression", pimpl->movementData.isWeapDrawn ? 1.0 : 0.0);

	const float confidence = pimpl->movementData.isWeapDrawn ? 4.0 : 0.0;
	if (abs(sd::GetBaseActorValue(actor, "Confidence") - confidence) > 0.25)
	{
		///sd::SetActorValue(actor, "Confidence", confidence);
		///sd::ForceActorValue(actor, "Confidence", confidence);
	}

	if (sd::IsInCombat(actor))
	{
		auto target = sd::GetCombatTarget(actor);
		if (target != nullptr)
		{
			const auto ghostAxeID = ci::GhostAxeManager::GetSingleton().GetGhostAxeFormID();
			const auto ghostAxeRef = (Actor *)LookupFormByID(ghostAxeID);
			if (ghostAxeRef != nullptr && target != ghostAxeRef)
			{
				sd::StopCombat(actor);
				sd::StartCombat(actor, ghostAxeRef);
				this->GetParent()->ApplyMovementDataImpl();
			}
		}
	}
}

void ci::RPEngineInput::EngineApplyFactions(Actor *actor)
{
	if (this->factionsState == IRemotePlayerEngine::FactionsState::Unknown)
	{
		sd::RemoveFromAllFactions(actor);
		this->factionsState = IRemotePlayerEngine::FactionsState::NoFactions;
	}

	if (this->factionsState == IRemotePlayerEngine::FactionsState::NoFactions)
	{
		enum {
			CWPlayerAlly = 0x0008F36D,
		};
		cd::AddToFaction(
			cd::Value<Actor>(actor->formID),
			cd::Value<TESFaction>(CWPlayerAlly));
		this->factionsState = IRemotePlayerEngine::FactionsState::PlayerAllyFaction;
	}
}