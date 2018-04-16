#include "../stdafx.h"
#include "../Sync/SyncEssential.h"
#include "CoreInterface.h"
#include "ciRPEngineIO.h"

#include "ciRPEngineBase.h"
#include "ciRPGhostAxe.h"

namespace ci
{
	class ObjectAccess
	{
	public:
		static uint32_t GetObjectRefID(const Object *obj) {
			return obj->GetRefID();
		}
	};

	class IActorAccess
	{
	public:
		static uint32_t GetActorRefID(const IActor *ac) {
			return ac->GetRefID();
		}
	};
}

ci::RPEngineIO::RPEngineIO(RemotePlayer *argParent) : IRemotePlayerEngine(argParent)
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	pimpl->pathToTarget.reset();
}

ci::RPEngineIO::~RPEngineIO()
{
}

void ci::RPEngineIO::SetPos(NiPoint3 pos)
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	pimpl->posTask = [pos](Actor *actor) {
		sd::SetPosition(actor, pos.x, pos.y, pos.z);
	};
	pimpl->movementData.pos = pos;
}

void ci::RPEngineIO::SetAngleZ(float angle)
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	pimpl->angleTask = [angle](Actor *actor) {
		sd::SetAngle(actor, 0, 0, angle);
	};
	pimpl->movementData.angleZ = angle;
}

void ci::RPEngineIO::ApplyMovementData(const MovementData &movementData)
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	this->GetParent()->SetPos(movementData.pos);
	this->GetParent()->SetAngleZ(movementData.angleZ);
	pimpl->movementData = movementData;
}

void ci::RPEngineIO::EquipSpell(const Spell *spell, bool leftHand)
{
}

void ci::RPEngineIO::UnequipSpell(const Spell *spell, bool leftHand)
{
}

NiPoint3 ci::RPEngineIO::GetPos() const
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	return pimpl->movementDataOut.pos;
}

float ci::RPEngineIO::GetAngleZ() const
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	return pimpl->movementDataOut.angleZ;
}

ci::MovementData ci::RPEngineIO::GetMovementData() const
{
	auto &pimpl = this->GetImpl();
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	if (pimpl->fullySpawned == false)
		return pimpl->movementData;
	return pimpl->movementDataOut;
}

const ci::Spell *ci::RPEngineIO::GetEquippedSpell(bool isLeftHand) const
{
	return nullptr;
}

void ci::RPEngineIO::ApplyMovementDataImpl()
{
}

const char *ci::RPEngineIO::GetEngineName() const
{
	return "RPEngineIO";
}

void ci::RPEngineIO::EngineUpdateSpawned(Actor *actor)
{
	//sd::SetActorValue(actor, "Agression", 1.0);

	// Already locked
	auto &pimpl = this->GetImpl();

	sd::ClearKeepOffsetFromActor(actor);
	sd::SetHeadTracking(actor, false);

	auto animPtr = AnimData_::UpdateActor(pimpl->formID);
	if (animPtr != nullptr)
	{
		pimpl->hitAnimsOut.push(*animPtr);
	}

	if (clock() - this->lastCombatUpdate > (pimpl->combatTarget ? 500 : 10))
	{
		this->lastCombatUpdate = clock();

		const float newVal = pimpl->combatTarget != 0 ? 4.0f : 0.0f;
		sd::SetActorValue(actor, "Confidence", newVal);
		sd::ForceActorValue(actor, "Confidence", newVal);

		//ci::Chat::AddMessage(std::to_wstring(pimpl->combatTarget));
		if (pimpl->combatTarget != 0)
		{
			auto combatTarget = (Actor *)LookupFormByID(pimpl->combatTarget);
			if (combatTarget != nullptr)
			{
				sd::ClearKeepOffsetFromActor(actor);
				if (sd::GetCombatTarget(actor) != combatTarget && sd::HasLOS(actor, combatTarget))
				{
					sd::StopCombat(actor);
					sd::StartCombat(actor, combatTarget);
				}
				sd::SetActorValue(actor, "Agression", 4.0);
				sd::ForceActorValue(actor, "Agression", 4.0);
			}
			else
			{
				sd::SetActorValue(actor, "Agression", 0.0);
				sd::ForceActorValue(actor, "Agression", 0.0);
			}
		}
		else
		{
			sd::SetActorValue(actor, "Agression", 0.0);
			sd::ForceActorValue(actor, "Agression", 0.0);
			sd::PathToReference(actor, actor, 0.5);
			if (sd::IsInCombat(actor))
				sd::StopCombat(actor);
		}
	}

	TESObjectREFR *const target = pimpl->pathToTarget ? (TESObjectREFR *)LookupFormByID(ObjectAccess::GetObjectRefID(pimpl->pathToTarget.get())) : nullptr;
	if (target != nullptr)
	{
		sd::PathToReference(actor, target, 0.5);
		sd::ForceActorValue(actor, "speedmult", pimpl->pathingSpeedmult);
	}
	else
		sd::ForceActorValue(actor, "speedmult", 100);
}

void ci::RPEngineIO::EngineApplyFactions(Actor *actor)
{
	if (this->factionsState != IRemotePlayerEngine::FactionsState::NoFactions)
	{
		sd::RemoveFromAllFactions(actor);
		this->factionsState = IRemotePlayerEngine::FactionsState::NoFactions;
	}
}

void ci::RemotePlayer::SetCombatTarget(const IActor *target)
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