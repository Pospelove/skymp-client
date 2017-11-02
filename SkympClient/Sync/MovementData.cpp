#include "../stdafx.h"
#include "Skyrim\BSDevices\PlayerControls.h"
#include "Skyrim\Camera\PlayerCamera.h"
#include "..\ScriptDragon\obscript.h"
#include "MovementData.h"
#include "Skyrim\NetImmerse\NiCamera.h"

void HitData_OnAnimationEvent(TESObjectREFR *source, std::string animEventName);

namespace MovementData_
{
	using RunMode = ci::MovementData::RunMode;
	using JumpStage = ci::MovementData::JumpStage;

	JumpStage currentPCJumpStage;
	bool isPCBlocking = false;
	clock_t weapDrawStart = 0;
	clock_t lastAnySwing = 0;

	enum {
		FirstApplyDelay = 300
	};

	bool HasEquippedBow(Actor *actor) {
		enum {
			Bow = 7,
		};
		return sd::GetEquippedItemType(actor, 0) == Bow 
			|| sd::GetEquippedItemType(actor, 1) == Bow;
	}

	void SendAnimationEvent(Actor *actor, const char *aeName, bool unsafe) {
		if (unsafe)
		{
			std::lock_guard<BSSpinLock>l(actor->processManager->middleProcess->animGraphManager.Get()->lock);
			actor->SendAnimationEvent(aeName);
		}
		else
			cd::SendAnimationEvent(actor, aeName);
	}

	void SendAnimationEvent(cd::Value<Actor> actor, const char *aeName, bool unsafe) {
		if (unsafe)
		{
			const auto actorPtr = actor.operator Actor *();
			if (actorPtr == nullptr)
				return;
			std::lock_guard<BSSpinLock>l(actorPtr->processManager->middleProcess->animGraphManager.Get()->lock);
			actorPtr->SendAnimationEvent(aeName);
		}
		else
			cd::SendAnimationEvent(actor, aeName);
	}

	NiPoint3 RotationMatrixToEulerAngles(const NiMatrix3 &R)
	{
		const float sy = sqrt(R.GetEntry(0, 0) * R.GetEntry(0, 0) + R.GetEntry(1, 0) * R.GetEntry(1, 0));
		const bool singular = sy < 1e-6; // If
		float x, y, z;
		if (!singular)
		{
			x = atan2(R.GetEntry(2, 1), R.GetEntry(2, 2));
			y = atan2(-R.GetEntry(2, 0), sy);
			z = atan2(R.GetEntry(1, 0), R.GetEntry(0, 0));
		}
		else
		{
			x = atan2(-R.GetEntry(1, 2), R.GetEntry(1, 1));
			y = atan2(-R.GetEntry(2, 0), sy);
			z = 0;
		}
		return { x, y, z };
	}

	void OnAnimationEvent(TESObjectREFR *src, std::string animEventName)
	{
		static std::string lastAE;
		std::transform(animEventName.begin(), animEventName.end(), animEventName.begin(), ::tolower);

		if (animEventName == "jumpup")
			currentPCJumpStage = JumpStage::Jumping;
		else if (animEventName == "jumpfall")
			currentPCJumpStage = JumpStage::Falling;
		else if (animEventName == "jumpdown")
		{
			if (lastAE != animEventName)
			{
				SET_TIMER(200, [] {
					if (currentPCJumpStage != JumpStage::Jumping)
						currentPCJumpStage = JumpStage::Landed;
				});
			}
		}
		else if (animEventName == "tailcombatidle")
			weapDrawStart = clock();
		else if (animEventName == "blockstartout")
			isPCBlocking = true;
		else  if (animEventName == "blockstop")
			isPCBlocking = false;
		else  if (animEventName == "weaponswing")
			lastAnySwing = clock();
		else if (animEventName == "weaponleftswing")
			lastAnySwing = clock();

		HitData_OnAnimationEvent(src, animEventName);

		lastAE = animEventName;
	}

	ci::MovementData Get(Actor *actor)
	{
		ci::MovementData result;

		result.pos = cd::GetPosition(actor);
		result.angleZ = (UInt32)sd::GetAngleZ(actor);

		static BSFixedString fsDirection("Direction");
		float animDirection = 0.0;
		actor->GetAnimationVariableFloat(fsDirection, animDirection);
		result.direction = UInt16(animDirection * 360);

		static BSFixedString fsSpeedSampled("SpeedSampled");
		float animSpeedSampled = 0.0;
		actor->GetAnimationVariableFloat(fsSpeedSampled, animSpeedSampled);
		result.speedSampled = (UInt16)animSpeedSampled;

		static BSFixedString fsBInJumpState("bInJumpState");
		bool animBInJumpState = false;
		actor->GetAnimationVariableBool(fsBInJumpState, animBInJumpState);
		result.isInJumpState = animBInJumpState;
		result.isWeapDrawn = actor->IsWeaponDrawn() || (actor == g_thePlayer && weapDrawStart + 250 > clock());

		result.isBlocking = /*actor == g_thePlayer ? isPCBlocking :*/ sd::Obscript::IsBlocking(actor) != 0.0;
		result.isSneaking = cd::IsSneaking(actor);
		result.isSprinting = cd::IsSprinting(actor);
		result.isSwimming = cd::IsSwimming(actor);

		if (result.speedSampled != 0)
		{
			const bool isRunning =
				actor == g_thePlayer ? !!PlayerControls::GetSingleton()->unk14.runMode : sd::IsRunning(actor);
			if (isRunning)
				result.runMode = RunMode::Running;
			else
				result.runMode = RunMode::Walking;
		}
		else
			result.runMode = RunMode::Standing;

		if (actor == g_thePlayer
			&& currentPCJumpStage != JumpStage::Landed && animBInJumpState != true)
			currentPCJumpStage = JumpStage::Landed;

		result.jumpStage = actor == g_thePlayer ? currentPCJumpStage : JumpStage::Landed;

		if (actor == g_thePlayer)
			result.isFirstPerson = PlayerCamera::GetSingleton()->IsFirstPerson();
		else
			result.isFirstPerson = true;

		static bool addedToSignal = false;
		if (!addedToSignal)
		{
			Costile2::OnAnimationEvent.Add(MovementData_::OnAnimationEvent);
			addedToSignal = true;
		}

		result.attackState = (uint16_t)sd::Obscript::GetAttackState(actor);

		auto mat = PlayerCamera::GetSingleton()->GetNiCamera()->GetWorldRotate();
		auto angles = RotationMatrixToEulerAngles(mat);
		result.aimingAngle = angles.y * 180 / std::acos(-1);

		return result;
	}

	ci::MovementData GetFromPlayer() {
		return Get(g_thePlayer);
	}

	void KeepOffsetFromSelf(Actor *self, MovementData_::SyncState &syncStatus, NiPoint3 offset, NiPoint3 offsetAngle, float afCatchUpRadius, float afFollowRadius)
	{
		auto &dat = syncStatus.lastKeepOffsetCall;
		bool argsChanged = false;
		if (dat.calledOnce)
		{
			if (dat.catchUpR != afCatchUpRadius
				|| dat.followR != afFollowRadius
				|| dat.offset.x != offset.x || dat.offset.y != offset.y || dat.offset.z != offset.z
				|| dat.offsetAngle.x != offsetAngle.x || dat.offsetAngle.y != offsetAngle.y || dat.offsetAngle.z != offsetAngle.z)
			{
				argsChanged = true;
			}
		}
		else
			argsChanged = true;
		dat.catchUpR = afCatchUpRadius;
		dat.followR = afFollowRadius;
		dat.offset = offset;
		dat.offsetAngle = offsetAngle;
		dat.calledOnce = true;
		if (argsChanged || syncStatus.forceCallKeepOffset)
		{
			if (syncStatus.fullyUnsafeSync)
				sd::KeepOffsetFromActor(self, self, offset.x, offset.y, offset.z, offsetAngle.x, offsetAngle.y, offsetAngle.z, afCatchUpRadius, afFollowRadius);
			else
				cd::KeepOffsetFromActor(self, self, offset.x, offset.y, offset.z, offsetAngle.x, offsetAngle.y, offsetAngle.z, afCatchUpRadius, afFollowRadius);
		}
	}

	void OnFirstApply(cd::Value<Actor> actor)
	{
		SET_TIMER(FirstApplyDelay, [=] {
			if (!cd::IsWeaponDrawn(actor))
				cd::SendAnimationEvent(actor, "IdleForceDefaultState");
		});
	}

	void ApplyImpl(ci::MovementData md, Actor *ac, SyncState &syncStatus, uint32_t ghostAxeID)
	{
		if (!syncStatus.appliedOnce)
		{
			OnFirstApply(ac);
			syncStatus.firstApplyStartMoment = clock();
			syncStatus.appliedOnce = true;
		}

		const bool firstApplyFinished = clock() - syncStatus.firstApplyStartMoment > FirstApplyDelay * 1.25;
		if (!firstApplyFinished)
			return;
		if (syncStatus.firstNormalApplyMoment == 0)
		{
			syncStatus.firstNormalApplyMoment = clock();
			syncStatus.isFirstNormalApply = true;
		}

		const float angleOffset = (float)md.angleZ - sd::GetAngleZ(ac);
		auto pos1 = md.pos, pos2 = cd::GetPosition(ac);
		pos1.z = 0.0;
		pos2.z = 0.0;
		const float distance = (pos1 - pos2).Length();
		pos1.z = md.pos.z;
		pos2.z = cd::GetPosition(ac).z;

		if (sd::IsDead(ac))
			return;
		const auto oldSyncMode = syncStatus.syncMode;

		const bool isInScreenDest = PlayerCamera::GetSingleton()->IsInScreen(pos1)
			|| PlayerCamera::GetSingleton()->IsInScreen(pos1 + NiPoint3{ 0,0,64 })
			|| PlayerCamera::GetSingleton()->IsInScreen(pos1 + NiPoint3{ 0,0,128 });
		const bool isInScreenObject = PlayerCamera::GetSingleton()->IsInScreen(pos2)
			|| PlayerCamera::GetSingleton()->IsInScreen(pos2 + NiPoint3{ 0,0,64 })
			|| PlayerCamera::GetSingleton()->IsInScreen(pos2 + NiPoint3{ 0,0,128 });
		const float distanceToThePlayerDest = (pos1 - cd::GetPosition(g_thePlayer)).Length(),
			distanceToThePlayerObject = (pos2 - cd::GetPosition(g_thePlayer)).Length();
		const float distanceToThePlayer = min(distanceToThePlayerDest, distanceToThePlayerObject);

		if (distanceToThePlayer < 128.0 * 0.99 || isInScreenDest || isInScreenObject)
		{
			if (syncStatus.syncMode != SyncMode::Hard)
				syncStatus.syncMode = SyncMode::Normal;
		}
		else
			syncStatus.syncMode = SyncMode::Light;

		struct
		{
			bool smartAngle;
			bool jumpingSync;
			bool unsafeSDFuncsEnabled;
			bool headtrackingDisabled;
			UInt32 weapDrawnUpdateRate;
			UInt32 positionUpdateRate;
			UInt32 sneakingUpdateRate;
			UInt32 blockingUpdateRate;
			UInt32 unsafeSDFuncsUpdateRate;
			float maxLagDistance;
			float maxLagDistanceOnRotate;
			float negligibleTime;
		} config;
		switch (syncStatus.syncMode)
		{
		case SyncMode::Hard:
			if (!md.isBlocking && !md.isWeapDrawn && !md.isInJumpState)
				config.smartAngle = true;
			else
				config.smartAngle = false;
			config.weapDrawnUpdateRate = 1000;
			config.positionUpdateRate = 0;
			config.sneakingUpdateRate = 1333;
			config.blockingUpdateRate = 500;
			config.unsafeSDFuncsUpdateRate = 0;
			config.jumpingSync = true;
			config.unsafeSDFuncsEnabled = true;
			config.maxLagDistance = 64.0;
			config.maxLagDistanceOnRotate = 48.0;
			config.negligibleTime = 0.100;
			config.headtrackingDisabled = true;
			break;
		case SyncMode::Normal:
			config.smartAngle = true;
			config.weapDrawnUpdateRate = 4500;
			config.positionUpdateRate = 50;
			config.sneakingUpdateRate = 2000;
			config.blockingUpdateRate = 10000;
			config.unsafeSDFuncsUpdateRate = 0;
			config.jumpingSync = true;
			config.unsafeSDFuncsEnabled = false;
			config.maxLagDistance = 128.0;
			config.maxLagDistanceOnRotate = 128.0;
			config.negligibleTime = 0.100;
			config.headtrackingDisabled = true;
			break;
		case SyncMode::Light:
			config.smartAngle = true;
			config.weapDrawnUpdateRate = 11000;
			config.positionUpdateRate = 400;
			config.sneakingUpdateRate = 11000;
			config.blockingUpdateRate = 11000;
			config.unsafeSDFuncsUpdateRate = 0;
			config.jumpingSync = false;
			config.unsafeSDFuncsEnabled = false;
			config.maxLagDistance = 128.0;
			config.maxLagDistanceOnRotate = 128.0;
			config.negligibleTime = 0.200;
			config.headtrackingDisabled = false;
			break;
		};

		enum AttackState {
			BowClick = 8,
			BowDrawing = 9,
			BowHoldingAShot = 10,
			BowRelease = 11,
			BowReleased = 12,
		};

		auto getIsAiming = [](const ci::MovementData &md) {
			return md.attackState == BowClick
				|| md.attackState == BowDrawing
				|| md.attackState == BowHoldingAShot
				|| md.attackState == BowRelease;
		};
		const bool isAiming = getIsAiming(md);

		auto myFox = (TESObjectREFR *)LookupFormByID(syncStatus.myFoxID);

		const bool bowEquipped = sd::GetEquippedItemType(ac, 0) == 7
			|| sd::GetEquippedItemType(ac, 1) == 7;
		if (bowEquipped && ac->IsWeaponDrawn())
		{
			const cd::Value<Actor> cdActor(ac);
			if (isAiming)
			{
				if (!syncStatus.aiDrivenBowSync || !myFox || myFox->baseForm->formType != FormType::NPC)
					SendAnimationEvent(cdActor, "bowAttackStart", 1);
			}
			else
			{
				if (isAiming != getIsAiming(syncStatus.last))
				{
					if (syncStatus.shotsRecordStart == 0)
						syncStatus.shotsRecordStart = clock();
					syncStatus.numShots++;
					syncStatus.lastShot = clock();
				}
				SendAnimationEvent(cdActor, "attackRelease", 1);
			}
		}
		if (isAiming && bowEquipped && myFox != nullptr)
		{
			sd::SetHeadTracking(ac, true);
			sd::SetLookAt(ac, myFox, false);
		}
		else
		{
			if (config.headtrackingDisabled)
			{
				sd::ClearLookAt(ac);
				sd::SetHeadTracking(ac, false);
			}
		}
		if (clock() - syncStatus.lastShot > 3300 && syncStatus.lastShot != 0)
		{
			syncStatus.lastShot = 0;
			syncStatus.shotsRecordStart = 0;
			syncStatus.numShots = 0;
		}
		const float fireRate = syncStatus.numShots * 1000.0f / (clock() - syncStatus.shotsRecordStart + 1);
		syncStatus.aiDrivenBowSync = fireRate > 1.1;

		if (syncStatus.disableSyncTimer < clock())
		{
			if (syncStatus.strictTranslateToTimer >= clock())
				syncStatus.forceCallKeepOffset = true;

			if (md.runMode == RunMode::Standing)
			{
				if (abs((SInt64)md.angleZ - (SInt64)sd::GetAngleZ(ac)) > 10)
				{
					if (!syncStatus.timer1)
					{
						syncStatus.timer1 = clock() + 500;
						KeepOffsetFromSelf(ac, syncStatus, { 0,0,0 }, { 0,0, config.smartAngle ? angleOffset : 0 }, 1, 1);
					}
				}
				else
					KeepOffsetFromSelf(ac, syncStatus, { 0,0,0 }, { 0,0,0 }, 1, 1);
			}
			else
			{
				const float pi = std::acos(-1);
				const float unitsOffset = 512.0;
				const NiPoint3 offset{
					unitsOffset * std::sin(float(md.direction) / 180 * pi),
					unitsOffset * std::cos(float(md.direction) / 180 * pi),
					0.0f
				};
				const float catchUpRadius = md.runMode == RunMode::Running ? 1.0 : 2048.0;
				const float followRadius = 1.0;
				KeepOffsetFromSelf(ac, syncStatus, offset, { 0,0,0 }, catchUpRadius, followRadius);
			}

			float maxDistance = config.maxLagDistance;
			if (syncStatus.lastDirectionChangeMoment + syncStatus.translateToMSOnDirChange > clock())
				maxDistance = config.maxLagDistanceOnRotate;

			if (distance > maxDistance
				|| (md.angleZ != syncStatus.last.angleZ && (md.runMode != RunMode::Standing || abs(angleOffset) >= (config.smartAngle ? 74.99 : 0.0)))
				|| syncStatus.lastRunModeChangeMoment + 300 > clock()
				|| syncStatus.lastDirectionChangeMoment + syncStatus.translateToMSOnDirChange > clock()
				|| syncStatus.lastBlockingChangeMoment + 800 > clock()
				|| md.isInJumpState
				|| (syncStatus.syncMode == SyncMode::Hard && md.isSprinting)
				|| syncStatus.forceFixAfterHitAnim
				|| syncStatus.strictTranslateToTimer >= clock()
				)
			{
				float speed = distance / config.negligibleTime;
				if (md.runMode == RunMode::Standing)
					speed = 128.0;
				if (md.runMode == RunMode::Standing && (distance > maxDistance || syncStatus.strictTranslateToTimer >= clock()))
					speed = 8192.0;
				if (syncStatus.lastTranslateToMoment + config.positionUpdateRate <= clock())
				{
					syncStatus.lastTranslateToMoment = clock();
					if (md.runMode == RunMode::Standing)
					{
						if (syncStatus.fullyUnsafeSync)
							sd::TranslateTo(ac, md.pos.x, md.pos.y, md.pos.z, 0, 0, md.angleZ, speed, 10 * abs(angleOffset));
						else
							cd::TranslateTo(ac, md.pos.x, md.pos.y, md.pos.z, 0, 0, md.angleZ, speed, 10 * abs(angleOffset));
					}
					else
					{
						auto pos = md.pos;
						if (distance <= 128.0 && !md.isInJumpState)
						{
							speed = md.speedSampled;
							const float unitsOffset = 12.0;
							const float pi = std::acos(-1);
							const NiPoint3 offset{
								unitsOffset * std::sin(float(md.direction + md.angleZ) / 180 * pi),
								unitsOffset * std::cos(float(md.direction + md.angleZ) / 180 * pi),
								0
							};
							pos += offset;
						}
						if (syncStatus.fullyUnsafeSync)
							sd::TranslateTo(ac, pos.x, pos.y, pos.z, 0, 0, md.angleZ, speed, 700);
						else
							cd::TranslateTo(ac, pos.x, pos.y, pos.z, 0, 0, md.angleZ, speed, 700);
					}
				}
			}

			if (md.runMode != syncStatus.last.runMode)
			{
				syncStatus.lastRunModeChangeMoment = clock();
				if (md.runMode != RunMode::Standing)
				{
					if (!md.isWeapDrawn && !ac->IsWeaponDrawn() && !md.isJumping)
						SendAnimationEvent(ac, "IdleForceDefaultState", syncStatus.fullyUnsafeSync);
				}
			}
			if (md.direction != syncStatus.last.direction)
			{
				syncStatus.translateToMSOnDirChange = 300;
				if (syncStatus.last.direction == 90 || syncStatus.last.direction == 270)
					syncStatus.translateToMSOnDirChange = 700;
				syncStatus.lastDirectionChangeMoment = clock();
			}

			if (syncStatus.timer1 != NULL && syncStatus.timer1 < clock())
			{
				syncStatus.timer1 = 0;
				KeepOffsetFromSelf(ac, syncStatus, { 0,0,0 }, { 0,0,0 }, 1, 1);
			}

			bool needUpdateSneaking = false;
			if (syncStatus.updateSneakingTimer && syncStatus.updateSneakingTimer < clock())
			{
				needUpdateSneaking = true;
				syncStatus.updateSneakingTimer = 0;
			}
			if (md.isSneaking != syncStatus.last.isSneaking || syncStatus.isFirstNormalApply
				|| needUpdateSneaking)
			{
				if (!needUpdateSneaking)
					syncStatus.updateSneakingTimer = clock() + config.sneakingUpdateRate;
				SendAnimationEvent(ac, md.isSneaking ? "SneakStart" : "SneakStop", syncStatus.fullyUnsafeSync);
			}

			if (md.isSprinting != syncStatus.last.isSprinting || syncStatus.isFirstNormalApply)
			{
				if (md.isSprinting)
				{
					SendAnimationEvent(ac, "SprintStart", syncStatus.fullyUnsafeSync);
					SendAnimationEvent(ac, "MoveStart", syncStatus.fullyUnsafeSync);
				}
				else
					SendAnimationEvent(ac, "SprintStop", syncStatus.fullyUnsafeSync);
			}

			if (md.isWeapDrawn != syncStatus.last.isWeapDrawn || syncStatus.isFirstNormalApply || syncStatus.updateWeapDrawnTimer < clock())
			{
				if (md.isWeapDrawn != syncStatus.last.isWeapDrawn)
				{
					syncStatus.shotsRecordStart = 0;
					syncStatus.numShots = 0;
				}

				syncStatus.updateWeapDrawnTimer = clock() + config.weapDrawnUpdateRate;
				cd::SendAnimationEvent(ac, md.isWeapDrawn ? "Skymp_StartCombat" : "Skymp_StopCombat");

				auto val = md.isWeapDrawn == false ? 0.0f : 4.0f;
				if (sd::GetActorValue(ac, "Confidence") != val)
					sd::SetActorValue(ac, "Confidence", val);
				if (md.isWeapDrawn)
				{
					if (myFox != nullptr && syncStatus.aiDrivenBowSync)
					{
						if (myFox->baseForm->formType == FormType::NPC
							&& sd::GetEquippedSpell(ac, 1) == nullptr && sd::GetEquippedSpell(ac, 0) == nullptr)
							sd::StartCombat(ac, (Actor *)myFox);
					}
					else
					{
						if (sd::GetCombatTarget(ac) == myFox)
							sd::StopCombat(ac);
						auto ghostAxe = (Actor *)LookupFormByID(ghostAxeID);
						if (ghostAxe != nullptr)
							if (sd::GetEquippedSpell(ac, 1) == nullptr && sd::GetEquippedSpell(ac, 0) == nullptr)
								sd::StartCombat(ac, ghostAxe);
					}
					ac->DrawSheatheWeapon(true);
				}
				else
				{
					sd::StopCombat(ac);
					ac->DrawSheatheWeapon(false);
				}
			}

			if (md.isBlocking != syncStatus.last.isBlocking || syncStatus.isFirstNormalApply)
			{
				syncStatus.updateBlockingTimer = clock() + config.blockingUpdateRate;
				syncStatus.lastBlockingChangeMoment = clock();
				if (0.0 != sd::Obscript::IsBlocking(ac) != md.isBlocking)
					SendAnimationEvent(ac, md.isBlocking ? "BlockStart" : "BlockStop", syncStatus.fullyUnsafeSync);
			}

			bool isj;
			ac->GetAnimationVariableBool("bInJumpState", isj);
			if (md.jumpStage != JumpStage::Landed)
			{
				if (!isj || syncStatus.lastJump + 1139 < clock())
				{
					if (ac->processManager && ac->processManager->middleProcess && ac->processManager->middleProcess->animGraphManager)
					{
						std::lock_guard<BSSpinLock>l(ac->processManager->middleProcess->animGraphManager.Get()->lock);
						ac->SendAnimationEvent(md.runMode == RunMode::Standing ? "JumpStandingStart" : "JumpDirectionalStart");
						syncStatus.lastJump = clock();
					}
				}
			}
			if (md.jumpStage != syncStatus.last.jumpStage && md.jumpStage == JumpStage::Landed)
			{
				if (isj)
					if (ac->processManager && ac->processManager->middleProcess && ac->processManager->middleProcess->animGraphManager)
					{
						std::lock_guard<BSSpinLock>l(ac->processManager->middleProcess->animGraphManager.Get()->lock);
						ac->SendAnimationEvent(md.runMode == RunMode::Standing ? "JumpLand" : "JumpLandDirectional");
					}
			}

			if (syncStatus.forceFixAfterHitAnim
				&& syncStatus.syncMode == SyncMode::Hard)
			{
				syncStatus.updateUnsafeSDFuncsTimer = 0;
				syncStatus.lastRunModeChangeMoment = 0;
				syncStatus.lastDirectionChangeMoment = 0;
				syncStatus.lastTranslateToMoment = 0;
				syncStatus.strictTranslateToTimer = clock() + 5000;
			}
		}

		static bool tdt = true;
		if (tdt)
		{
			sd::ExecuteConsoleCommand("toggledetection", nullptr);
			tdt = false;
		}

		syncStatus.last = md;
		syncStatus.forceCallKeepOffset = false;
		syncStatus.isFirstNormalApply = false;
		syncStatus.forceFixAfterHitAnim = false;
	}

	void ApplyToThePlayer(const ci::MovementData &md)
	{
		cd::SetPosition(g_thePlayer, md.pos);
		cd::SetAngle(g_thePlayer, 0, 0, md.angleZ);
		if (md.isSneaking != g_thePlayer->IsSneaking())
			cd::StartSneaking(g_thePlayer, []() {});
		if (g_thePlayer->IsWeaponDrawn() != md.isWeapDrawn)
			g_thePlayer->DrawSheatheWeapon(md.isWeapDrawn);
		if (md.isInJumpState)
			SendAnimationEvent(g_thePlayer, md.runMode == RunMode::Standing ? "JumpStandingStart" : "JumpDirectionalStart", false);
		if (md.isSprinting != g_thePlayer->IsSprinting())
		{
			if (md.isSprinting)
			{
				SendAnimationEvent(g_thePlayer, "SprintStart", false);
				SendAnimationEvent(g_thePlayer, "MoveStart", false);
			}
			else
				SendAnimationEvent(g_thePlayer, "SprintStop", false);
		}
		if ((0.0 != sd::Obscript::IsBlocking(g_thePlayer)) != md.isBlocking)
			SendAnimationEvent(g_thePlayer, md.isBlocking ? "BlockStart" : "BlockStop", false);
	}

	void Apply(ci::MovementData md, Actor *actor, SyncState *syncState, uint32_t ghostAxeID)
	{
		if (actor == nullptr)
			return;

		if (actor == g_thePlayer)
			return ApplyToThePlayer(md);

		if (syncState != nullptr)
			ApplyImpl(md, actor, *syncState, ghostAxeID);
	}
}