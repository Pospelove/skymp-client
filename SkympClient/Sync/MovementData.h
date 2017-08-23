#pragma once
#include "../CoreInterface/CoreInterface.h"
#include <vector>

namespace MovementData_
{
	enum class SyncMode
	{
		Hard,
		Normal,
		Light
	};

	struct SyncState
	{
		// in
		bool fullyUnsafeSync = false;

		// in-out
		SyncMode syncMode = SyncMode::Light;

		// out
		ci::MovementData last;
		bool appliedOnce = false;
		clock_t firstApplyStartMoment = 0;
		struct {
			NiPoint3 offset;
			NiPoint3 offsetAngle;
			float catchUpR;
			float followR;
			bool calledOnce = false;
		} lastKeepOffsetCall;
		bool forceCallKeepOffset = true;
		bool isFirstNormalApply = false;
		clock_t updateSneakingTimer = 0;
		clock_t updateWeapDrawnTimer = 0;
		clock_t updateBlockingTimer = 0;
		clock_t updateUnsafeSDFuncsTimer = 0;
		clock_t firstNormalApplyMoment = 0;
		clock_t lastRunModeChangeMoment = 0;
		clock_t lastDirectionChangeMoment = 0;
		clock_t lastBlockingChangeMoment = 0;
		clock_t lastJumpingChangeMoment = 0;
		clock_t lastTranslateToMoment = 0;
		UInt32 translateToMSOnDirChange = 0;
		clock_t lastJump = 0;
		clock_t timer1 = 0;
		UInt64 ghostAxeSeed = 0;
		bool jumpedStanding = false;
	};

	using GhostAxeVector = std::vector<RefHandle>;

	ci::MovementData GetFromPlayer();
	void Apply(ci::MovementData movementData, Actor *actor, SyncState *syncState, GhostAxeVector ghostAxes);
}