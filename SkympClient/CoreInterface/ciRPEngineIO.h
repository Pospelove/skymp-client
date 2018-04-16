// Requires include CoreInterface.h first
#pragma once

#include "ciRPEngineBase.h"

namespace ci
{
	class RPEngineIO : public IRemotePlayerEngine
	{
	public:
		RPEngineIO(RemotePlayer *argParent);
		~RPEngineIO();

		void SetPos(NiPoint3 pos) override;
		void SetAngleZ(float angle) override;
		void ApplyMovementData(const MovementData &movementData) override;
		void EquipSpell(const Spell *spell, bool leftHand) override;
		void UnequipSpell(const Spell *spell, bool leftHand) override;
		NiPoint3 GetPos() const override;
		float GetAngleZ() const override;
		MovementData GetMovementData() const override;
		const Spell *GetEquippedSpell(bool isLeftHand = false) const override;
		void ApplyMovementDataImpl();
		const char *GetEngineName() const override;
		void EngineUpdateSpawned(Actor *actor) override;
		void EngineApplyFactions(Actor *actor) override;

		clock_t lastCombatUpdate = 0;
	};
}