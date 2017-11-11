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

	class SyncState
	{
	public:

		SyncState() {
			isWorldSpell[0] = isWorldSpell[1] = false;
		}

		// in
		bool fullyUnsafeSync = false;
		uint32_t myFoxID = 0;
		bool isWorldSpell[2];

		// in-out
		SyncMode syncMode = SyncMode::Light;
		bool forceFixAfterHitAnim = false;

		// out
		ci::MovementData last;
		bool appliedOnce = false;
		clock_t firstApplyStartMoment = 0;
		struct {
			NiPoint3 offset = { 0,0,0 };
			NiPoint3 offsetAngle = { 0,0,0 };
			float catchUpR = -1.f;
			float followR = -1.f;
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
		clock_t strictTranslateToTimer = 0;
		clock_t disableSyncTimer = 0;
		bool jumpedStanding = false;
		bool isAiming = false;
		bool aiDrivenBowSync = false;
		uint32_t numShots = 0;
		clock_t shotsRecordStart = 0;
		clock_t lastShot = 0;
		clock_t lastCastRelease = 0;
		size_t fatalErrors = 0;
	};


	ci::MovementData GetFromPlayer();
	void Apply(ci::MovementData movementData, Actor *actor, SyncState *syncState, uint32_t ghostAxeID);
}