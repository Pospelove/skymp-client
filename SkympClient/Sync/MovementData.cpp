#include "../stdafx.h"
#include "Skyrim\BSDevices\PlayerControls.h"
#include "Skyrim\Camera\PlayerCamera.h"
#include "..\ScriptDragon\obscript.h"
#include "MovementData.h"
#include "Skyrim\NetImmerse\NiCamera.h"
#include "SyncOptions.h"

void AnimData_OnAnimationEvent(TESObjectREFR *source, std::string animEventName);

namespace MovementData_
{
	using RunMode = ci::MovementData::RunMode;
	using JumpStage = ci::MovementData::JumpStage;
	using CastStage = ci::MovementData::CastStage;
	using MountStage = ci::MovementData::MountStage;

	JumpStage currentPCJumpStage;
	CastStage currentPCCastStage[2] = { CastStage::None, CastStage::None };
	MountStage currentPCMountStage = MountStage::None;
	int32_t castSession[2] = { 0, 0 };
	bool isPCBlocking = false;
	clock_t weapDrawStart = 0;
	clock_t lastAnySwing = 0;
	std::set<uint32_t> actorsWithErrors;

	bool HasEquippedBow(Actor *actor) {
		enum {
			Bow = 7,
		};
		return sd::GetEquippedItemType(actor, 0) == Bow
			|| sd::GetEquippedItemType(actor, 1) == Bow;
	}

	void SendAnimationEvent(cd::Value<Actor> actor, const char *aeName, bool unsafe) {
		if (unsafe)
		{
			try {
				const auto actorPtr = actor.operator Actor *();
				if (actorPtr == nullptr)
					return;
				std::lock_guard<BSSpinLock>l(actorPtr->processManager->middleProcess->animGraphManager.Get()->lock);
				actorPtr->SendAnimationEvent(aeName);
			}
			catch (...) {
				ErrorHandling::SendError("ERROR:Movement SendAnimationEvent()");
				cd::SendAnimationEvent(actor, aeName);
				throw std::logic_error("ERROR:Movement SendAnimationEvent()");
			}
		}
		else
			cd::SendAnimationEvent(actor, aeName);
	}

	void EvaluatePackage(cd::Value<Actor> actor, bool unsafe) {
		if (unsafe)
		{
			if (actor.operator Actor *() != nullptr)
				sd::EvaluatePackage(actor);
		}
		else
		{
			cd::EvaluatePackage(actor);
		}
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

	void OnActorAction(int32_t actionID, int32_t slotID)
	{
	}

	void Update()
	{
		enum {
			Staff = 8,
			MagicSpell = 9,
		};

		const int32_t btnIDs[2] = { VK_LBUTTON, VK_RBUTTON };

		for (int32_t i = 0; i <= 1; ++i)
		{
			const auto pressed = sd::GetKeyPressed(btnIDs[i]);
			if (!pressed)
			{
				if (currentPCCastStage[i] == CastStage::Fire)
				{
					currentPCCastStage[i] = CastStage::Release;
				}
				else
					currentPCCastStage[i] = CastStage::None;
			}
			else if (pressed && sd::Obscript::IsCasting(g_thePlayer) && currentPCCastStage[i] == CastStage::None)
			{
				const auto typeID = sd::GetEquippedItemType(g_thePlayer, !i);
				if (typeID == Staff || typeID == MagicSpell)
				{
					currentPCCastStage[i] = CastStage::Casting;

					const int32_t session = rand();
					castSession[i] = session;

					const auto spell = sd::GetEquippedSpell(g_thePlayer, !i);
					const float duration = spell ? spell->data.duration * 1000 : 0;

					SET_TIMER_LIGHT(duration + 100, [=] {
						if (castSession[i] == session)
						{
							currentPCCastStage[i] = CastStage::Fire;
						}
					});
				}
			}
		}

		int32_t timerMS = 600;
		for (int32_t i = 0; i <= 1; ++i)
		{
			const auto typeID = sd::GetEquippedItemType(g_thePlayer, !i);
			if (typeID == Staff || typeID == MagicSpell)
			{
				timerMS = 100;
				break;
			}
		}
		SET_TIMER_LIGHT(timerMS, Update);
	}

	void OnAnimationEvent(TESObjectREFR *src, std::string animEventName)
	{
		{
			static bool startedUpdates = false;
			if (!startedUpdates)
			{
				SET_TIMER_LIGHT(0, Update);
				startedUpdates = true;
			}
		}

		static std::string lastAE;
		std::transform(animEventName.begin(), animEventName.end(), animEventName.begin(), ::tolower);

		if (src == g_thePlayer)
		{
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
		}

		AnimData_OnAnimationEvent(src, animEventName);

		if (lastAE != animEventName)
		{
			if (std::string{ animEventName.begin(), animEventName.begin() + 12 } == "skymp_action")
			{
				auto actionBegin = animEventName.begin() + 12,
					actionEnd = animEventName.begin() + 12;

				auto slotStr = "slot";

				while (*actionEnd != slotStr[0] && actionEnd - actionBegin <= 2)
					++actionEnd;
				const int32_t actionID = atoi(std::string{ actionBegin, actionEnd }.data());

				auto slotBegin = actionEnd + strlen(slotStr);
				auto slotEnd = animEventName.end();
				const int32_t slotID = atoi(std::string{ slotBegin, slotEnd }.data());

				OnActorAction(actionID, slotID);
			}
		}

		lastAE = animEventName;
	}

	bool IsHorse(Actor *actor)
	{
		enum {
			HorseRace = 0x000131FD
		};
		const bool isHorse = actor->GetRace()->formID == HorseRace;
		return isHorse;
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
		result.speedSampled = (UInt16)animSpeedSampled; // 0.0 if actor is werewolf

		static BSFixedString fsBInJumpState("bInJumpState");
		bool animBInJumpState = false;
		actor->GetAnimationVariableBool(fsBInJumpState, animBInJumpState);
		result.isInJumpState = animBInJumpState;
		result.isWeapDrawn = actor->IsWeaponDrawn() || (actor == g_thePlayer && weapDrawStart + 250 > clock());

		result.isFirstPerson = actor != g_thePlayer || PlayerCamera::GetSingleton()->IsFirstPerson();

		result.isBlocking = /*actor == g_thePlayer ? isPCBlocking :*/ sd::Obscript::IsBlocking(actor) != 0.0;
		result.isSneaking = cd::IsSneaking(actor);
		result.isSprinting = cd::IsSprinting(actor);
		result.isSwimming = cd::IsSwimming(actor);

		static bool rPressed = false;
		static clock_t rReleaseMoment = 0;
		if (sd::GetKeyPressed('R'))
		{
			rPressed = true;
			rReleaseMoment = clock();
		}
		else
		{
			if (clock() - rReleaseMoment > 200)
				rPressed = false;
		}
		result.isRPressed = (g_thePlayer && rPressed) ? 1 : 0;

		bool moving = false;
		static std::map<uint32_t, NiPoint3> lastPlPos;
		if (actor == g_thePlayer || IsHorse(actor))
		{
			static std::map<uint32_t, clock_t> lastMove;
			NiPoint3 plPos = result.pos;
			plPos.z = -1;
			if (abs(plPos.Length() - lastPlPos[actor->formID].Length()) > 0.1)
			{
				lastMove[actor->formID] = clock();
			}
			lastPlPos[actor->formID] = plPos;
			moving = (result.isFirstPerson == false || IsHorse(actor)) && clock() - lastMove[actor->formID] < 50;
		}

		if (moving || result.speedSampled != 0)
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
		result.mountStage = actor == g_thePlayer ? currentPCMountStage : MountStage::None;

		if (actor == g_thePlayer)
			result.castStage = { currentPCCastStage[0], currentPCCastStage[1] };
		else
			result.castStage = { sd::Obscript::IsCasting(actor) ? CastStage::Fire : CastStage::None, CastStage::None };

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
		SET_TIMER(SyncOptions::GetSingleton()->GetInt("FirstApplyDelay"), [=] {
			try {
				if (!cd::IsWeaponDrawn(actor))
					SendAnimationEvent(actor, "IdleForceDefaultState", true); // unsafe call to prevent delay
			}
			catch (...) {
				actorsWithErrors.insert(actor.GetFormID());
			}
		});
	}

	void SelectSyncMode(ci::MovementData md, Actor *ac, SyncState &syncStatus)
	{
		auto pos1 = md.pos, pos2 = cd::GetPosition(ac);
		pos1.z = 0.0;
		pos2.z = 0.0;
		const float distance = (pos1 - pos2).Length();
		pos1.z = md.pos.z;
		pos2.z = cd::GetPosition(ac).z;

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
	}

	struct Config
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
	};

	Config GetConfig(ci::MovementData md, Actor *ac, SyncState &syncStatus)
	{
		std::string prefix;
		switch (syncStatus.syncMode)
		{
		case SyncMode::Hard:
			prefix = "Hard";
			break;
		case SyncMode::Normal:
			prefix = "Normal";
			break;
		case SyncMode::Light:
			prefix = "Light";
			break;
		}
		prefix += '.';

		auto o = SyncOptions::GetSingleton();

		return Config{
			o->GetInt(prefix + "smartAngle"),
			o->GetInt(prefix + "weapDrawnUpdateRate"),
			o->GetInt(prefix + "positionUpdateRate"),
			o->GetInt(prefix + "sneakingUpdateRate"),
			o->GetInt(prefix + "blockingUpdateRate"),
			o->GetInt(prefix + "unsafeSDFuncsUpdateRate"),
			o->GetInt(prefix + "jumpingSync"),
			o->GetInt(prefix + "unsafeSDFuncsEnabled"),
			o->GetInt(prefix + "maxLagDistance"),
			o->GetInt(prefix + "maxLagDistanceOnRotate"),
			o->GetInt(prefix + "negligibleTime") / 1000.f,
			o->GetInt(prefix + "headtrackingDisabled"),
		};
	}

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

	void ApplyBowAttack(ci::MovementData md, Actor *ac, SyncState &syncStatus)
	{
		const bool isAiming = getIsAiming(md),
			wasAiming = getIsAiming(syncStatus.last);
		auto myFox = (TESObjectREFR *)LookupFormByID(syncStatus.myFoxID);

		auto weap = sd::GetEquippedWeapon(ac, 0);
		if (!weap)
			weap = sd::GetEquippedWeapon(ac, 1);
		if (!weap)
			return;

		if (HasEquippedBow(ac) && ac->IsWeaponDrawn())
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
					SendAnimationEvent(cdActor, md.isRPressed ? "attackStop" : "attackRelease", 1);
				}
			}
		}
	}

	void ApplyHeadtracking(ci::MovementData md, Actor *ac, SyncState &syncStatus, const Config &config)
	{
		const bool isAiming = getIsAiming(md);
		auto myFox = (TESObjectREFR *)LookupFormByID(syncStatus.myFoxID);

		if (isAiming && HasEquippedBow(ac) && myFox != nullptr)
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
	}

	void SelectBowSyncMode(SyncState &syncStatus)
	{
		if (clock() - syncStatus.lastShot > 3300 && syncStatus.lastShot != 0)
		{
			syncStatus.lastShot = 0;
			syncStatus.shotsRecordStart = 0;
			syncStatus.numShots = 0;
		}
		const float fireRate = syncStatus.numShots * 1000.0f / (clock() - syncStatus.shotsRecordStart + 1);
		syncStatus.aiDrivenBowSync = fireRate > 1.1;
	}

	void ApplyMoveAnimation(ci::MovementData md, Actor *ac, SyncState &syncStatus, const Config &config)
	{
		const float angleOffset = (float)md.angleZ - sd::GetAngleZ(ac);
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
	}

	static std::map<uint32_t, ci::MovementData> positions;

	void ApplyPosition(ci::MovementData md, Actor *ac, SyncState &syncStatus, const Config &config)
	{
		positions[ac->formID] = md;
		
		if (clock() < syncStatus.enablePosApplyMoment)
		{
			auto pos = syncStatus.lastDismountedPos;
			sd::TranslateTo(ac, pos.x, pos.y, pos.z, 0, 0, sd::GetAngleZ(ac), 10000, 10000);
			return;
		}
		//if (md.mountStage == ci::MovementData::MountStage::None)
		{
			syncStatus.lastDismountedPos = md.pos;
		}

		{
			auto vehicleRef = (TESObjectREFR *)LookupFormByID(syncStatus.myVehicleID);
			if (vehicleRef != nullptr)
			{
				if (vehicleRef->formType == FormType::Character)
				{
					char *nodeName = "SaddleBone";
					auto pos = Utility::GetNodeWorldPosition(vehicleRef, nodeName, 1);
					sd::TranslateTo(ac, pos.x, pos.y, pos.z, 0, 0, md.angleZ, 10000, 10000);
					pos = positions[vehicleRef->formID].pos;
					auto currentVPos = cd::GetPosition(vehicleRef);
					auto t = 0.1;
					auto speed = (pos - currentVPos).Length() / t;
					sd::TranslateTo(vehicleRef, pos.x, pos.y, pos.z, 0, 0, positions[vehicleRef->formID].angleZ, speed, 1000);
					return;
				}
			}
			else
			{
				syncStatus.myVehicleID = 0;
			}
		}

		const float angleOffset = (float)md.angleZ - sd::GetAngleZ(ac);

		float maxDistance = config.maxLagDistance;
		if (syncStatus.lastDirectionChangeMoment + syncStatus.translateToMSOnDirChange > clock())
			maxDistance = config.maxLagDistanceOnRotate;

		auto pos1 = md.pos, pos2 = cd::GetPosition(ac);
		pos1.z = 0.0;
		pos2.z = 0.0;
		const float distance = (pos1 - pos2).Length();
		pos1.z = md.pos.z;
		pos2.z = cd::GetPosition(ac).z;

		enum {
			WerewolfRace = 0x000CDD84,
		};
		const auto currentRace = sd::GetRace(ac);
		
		if (distance > maxDistance
			|| currentRace->formID == WerewolfRace
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
			if (currentRace->formID == WerewolfRace || md.isInJumpState)
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
						if (speed == 0)
							speed = 512;
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
	}

	void ApplyUnk(ci::MovementData md, Actor *ac, SyncState &syncStatus)
	{
		SAFE_CALL("Movement", [&] {
			if (md.runMode != syncStatus.last.runMode)
			{
				syncStatus.lastRunModeChangeMoment = clock();
				if (md.runMode != RunMode::Standing)
				{
					if (!md.isWeapDrawn && !ac->IsWeaponDrawn() && !md.isJumping)
						SendAnimationEvent(ac, "IdleForceDefaultState", syncStatus.fullyUnsafeSync);
				}
			}
		});

		SAFE_CALL("Movement", [&] {
			if (md.direction != syncStatus.last.direction)
			{
				syncStatus.translateToMSOnDirChange = 300;
				if (syncStatus.last.direction == 90 || syncStatus.last.direction == 270)
					syncStatus.translateToMSOnDirChange = 700;
				syncStatus.lastDirectionChangeMoment = clock();
			}
		});

		SAFE_CALL("Movement", [&] {
			if (syncStatus.timer1 != NULL && syncStatus.timer1 < clock())
			{
				syncStatus.timer1 = 0;
				KeepOffsetFromSelf(ac, syncStatus, { 0,0,0 }, { 0,0,0 }, 1, 1);
			}
		});
	}

	void ApplySneaking(ci::MovementData md, Actor *ac, SyncState &syncStatus, const Config &config)
	{
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
	}

	void ApplySprinting(ci::MovementData md, Actor *ac, SyncState &syncStatus, const Config &config)
	{
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
	}

	void ApplyCombat(ci::MovementData md, Actor *ac, SyncState &syncStatus, const Config &config, uint32_t ghostAxeID)
	{
		if (md.isWeapDrawn != syncStatus.last.isWeapDrawn || syncStatus.isFirstNormalApply || syncStatus.updateWeapDrawnTimer < clock())
		{
			if (md.isWeapDrawn != syncStatus.last.isWeapDrawn)
			{
				syncStatus.shotsRecordStart = 0;
				syncStatus.numShots = 0;
			}

			syncStatus.updateWeapDrawnTimer = clock() + config.weapDrawnUpdateRate;
			cd::SendAnimationEvent(ac, md.isWeapDrawn ? "Skymp_StartCombat" : "Skymp_StopCombat");

			auto val = md.isWeapDrawn == false ? 0.0f : 0.0f;
			if (sd::GetActorValue(ac, "Confidence") != val)
				sd::SetActorValue(ac, "Confidence", val);
			if (md.isWeapDrawn)
			{
				auto myFox = (TESObjectREFR *)LookupFormByID(syncStatus.myFoxID);
				if (myFox != nullptr && syncStatus.aiDrivenBowSync)
				{
					if (myFox->baseForm->formType == FormType::NPC
						&& sd::GetEquippedSpell(ac, 1) == nullptr && sd::GetEquippedSpell(ac, 0) == nullptr)
						sd::StartCombat(ac, (Actor *)myFox)
						;
				}
				else
				{
					if (sd::GetCombatTarget(ac) == myFox)
						sd::StopCombat(ac);
					auto ghostAxe = (Actor *)LookupFormByID(ghostAxeID);
					if (ghostAxe != nullptr)
						if (sd::GetEquippedSpell(ac, 1) == nullptr && sd::GetEquippedSpell(ac, 0) == nullptr)
							sd::StartCombat(ac, ghostAxe)
							;
				}
				ac->DrawSheatheWeapon(true);
			}
			else
			{
				sd::StopCombat(ac);
				ac->DrawSheatheWeapon(false);
			}
		}
	}

	void ApplyBlocking(ci::MovementData md, Actor *ac, SyncState &syncStatus, const Config &config)
	{
		if (md.isBlocking != syncStatus.last.isBlocking || syncStatus.isFirstNormalApply)
		{
			syncStatus.updateBlockingTimer = clock() + config.blockingUpdateRate;
			syncStatus.lastBlockingChangeMoment = clock();
			if (0.0 != sd::Obscript::IsBlocking(ac) != md.isBlocking)
				SendAnimationEvent(ac, md.isBlocking ? "BlockStart" : "BlockStop", syncStatus.fullyUnsafeSync);
		}
	}

	void ApplyJumping(ci::MovementData md, Actor *ac, SyncState &syncStatus, const Config &config)
	{
		char *jumpStandingStart = "JumpStandingStart";
		char *jumpDirectionalStart = "JumpDirectionalStart";
		char *jumpLand = "JumpLand";
		char *jumpDirectionalLand = "JumpLandDirectional";

		if (IsHorse(ac))
		{
			jumpStandingStart = "StandingRearUp";
			jumpDirectionalStart = "forwardJumpStart";
			jumpLand = jumpDirectionalLand = "forwardLandEnd";

			if (md.isInJumpState)
			{
				md.jumpStage = ci::MovementData::JumpStage::Jumping;
			}
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
					ac->SendAnimationEvent(md.runMode == RunMode::Standing ? jumpStandingStart : jumpDirectionalStart);
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
					ac->SendAnimationEvent(md.runMode == RunMode::Standing ? jumpLand : jumpDirectionalLand);
				}
		}
	}

	void ApplyCasting(ci::MovementData md, Actor *ac, SyncState &syncStatus)
	{
		enum {
			CastStart = 0,
			CastStop = 1,
		};
		char *const castStageAV = "Variable01";

		const auto invalidCastStage = (CastStage)-1;

		if (clock() - syncStatus.lastCastRelease > SyncOptions::GetSingleton()->GetInt("CastReleaseDelay"))
		{
			if (syncStatus.lastCastRelease != 0)
			{
				syncStatus.lastCastRelease = 0;
				for (int32_t i = 0; i <= 0; ++i)
					syncStatus.last.castStage[i] = invalidCastStage;
			}
		}

		if (md.castStage != syncStatus.last.castStage)
		{
			auto syncForHand = [&](int32_t i) {
				auto castStage = md.castStage[i];

				if (!syncStatus.isWorldSpell[i])
					castStage = CastStage::None;

				switch (castStage)
				{
				case CastStage::None:
					sd::ForceActorValue(ac, castStageAV, CastStop);
					break;
				case CastStage::Casting:
				case CastStage::Fire:
					sd::ForceActorValue(ac, castStageAV, CastStart);
					break;
				case CastStage::Release:
					if (syncStatus.last.castStage[i] != invalidCastStage)
					{
						sd::ForceActorValue(ac, castStageAV, CastStart);
						SendAnimationEvent(ac, "MRh_SpellRelease_Event", syncStatus.fullyUnsafeSync);
						syncStatus.lastCastRelease = clock();
					}
					else
						sd::ForceActorValue(ac, castStageAV, CastStop);
					break;
				}
				EvaluatePackage(ac, syncStatus.fullyUnsafeSync);
			};

			if (md.castStage[1] != CastStage::None)
				syncForHand(1);
			else
				syncForHand(0);
		}
	}

	void ApplyHorseRiding(ci::MovementData md, Actor *ac, SyncState &syncStatus)
	{
		const bool stageChanged = md.mountStage != syncStatus.last.mountStage;

		switch (md.mountStage)
		{
		case MountStage::Dismounting:
		case MountStage::None:
			if (stageChanged)
			{
				SendAnimationEvent(ac, "HorseExit", syncStatus.fullyUnsafeSync);
			}
			break;
		case MountStage::Mounting:
		case MountStage::OnMount:
			if (!syncStatus.isFirstNormalApply && stageChanged)
			{
				SendAnimationEvent(ac, "HorseEnter", syncStatus.fullyUnsafeSync);
				syncStatus.enablePosApplyMoment = clock() + 1500;
			}
			else if(syncStatus.enablePosApplyMoment < clock())
				SendAnimationEvent(ac, "HorseEnterInstant", syncStatus.fullyUnsafeSync);
			break;
		}
	}

	void StopIdles(Actor *ac, SyncState &syncStatus)
	{
		auto raceID = ((TESNPC *)ac->baseForm)->GetRace()->GetFormID();

		enum
		{
			ArgonianRace = 0x00013740,
		};

		if (raceID < ArgonianRace || raceID > ArgonianRace + 9)
		{
			if(clock() - syncStatus.lastIdleStop > 2000)
			{
				syncStatus.lastIdleStop = clock();
				SendAnimationEvent(ac, "IdleStop", syncStatus.fullyUnsafeSync);
			}
		}
	}

	void FixAfterHitAnim(SyncState &syncStatus)
	{
		syncStatus.updateUnsafeSDFuncsTimer = 0;
		syncStatus.lastRunModeChangeMoment = 0;
		syncStatus.lastDirectionChangeMoment = 0;
		syncStatus.lastTranslateToMoment = 0;
		syncStatus.strictTranslateToTimer = clock() + 5000;
	}

	void ApplyImpl(ci::MovementData md, Actor *ac, SyncState &syncStatus, uint32_t ghostAxeID)
	{
		if (!syncStatus.appliedOnce)
		{
			OnFirstApply(ac);
			syncStatus.firstApplyStartMoment = clock();
			syncStatus.appliedOnce = true;
		}

		const bool firstApplyFinished = clock() - syncStatus.firstApplyStartMoment > SyncOptions::GetSingleton()->GetInt("FirstApplyDelay") * 2;
		if (!firstApplyFinished)
			return;

		if (actorsWithErrors.find(ac->formID) != actorsWithErrors.end())
		{
			++syncStatus.fatalErrors;
			actorsWithErrors.erase(ac->formID);
		}

		if (syncStatus.firstNormalApplyMoment == 0)
		{
			syncStatus.firstNormalApplyMoment = clock();
			syncStatus.isFirstNormalApply = true;
		}

		const float angleOffset = (float)md.angleZ - sd::GetAngleZ(ac);

		if (sd::IsDead(ac))
			return;

		const auto config = GetConfig(md, ac, syncStatus);

		try {
			SelectSyncMode(md, ac, syncStatus);
			ApplyBowAttack(md, ac, syncStatus);
			ApplyHeadtracking(md, ac, syncStatus, config);
			SelectBowSyncMode(syncStatus);

			if (syncStatus.disableSyncTimer < clock())
			{
				if (syncStatus.strictTranslateToTimer >= clock())
					syncStatus.forceCallKeepOffset = true;

				ApplyMoveAnimation(md, ac, syncStatus, config);
				ApplyPosition(md, ac, syncStatus, config);
				ApplyUnk(md, ac, syncStatus);
				ApplySneaking(md, ac, syncStatus, config);
				ApplySprinting(md, ac, syncStatus, config);
				ApplyCombat(md, ac, syncStatus, config, ghostAxeID);
				ApplyBlocking(md, ac, syncStatus, config);
				ApplyJumping(md, ac, syncStatus, config);
				ApplyCasting(md, ac, syncStatus);
				ApplyHorseRiding(md, ac, syncStatus);
				StopIdles(ac, syncStatus);

				if (syncStatus.forceFixAfterHitAnim
					&& syncStatus.syncMode == SyncMode::Hard)
				{
					FixAfterHitAnim(syncStatus);
				}
			}

		}
		catch (std::exception &e)
		{
			if (std::string(e.what()).find("SendAnimationEvent") != std::string::npos)
			{
				ErrorHandling::SendError("ERROR:MovementData Fatal error");
			}
			else
				ErrorHandling::SendError("ERROR:MovementData Unk std exception");
			syncStatus.fatalErrors++;
		}
		catch (...) {
			syncStatus.fatalErrors++;
			ErrorHandling::SendError("ERROR:MovementData Unk");
		}

		static bool tdt = true;
		if (tdt)
		{
			//sd::ExecuteConsoleCommand("toggledetection", nullptr);
			tdt = false;
		}

		syncStatus.last = md;
		syncStatus.forceCallKeepOffset = false;
		syncStatus.isFirstNormalApply = false;
		syncStatus.forceFixAfterHitAnim = false;
	}

	void ApplyToThePlayer(const ci::MovementData &md)
	{
		SAFE_CALL("Movement", [&] {
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
		});
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

	void OnPlayerMountDismount(bool mount)
	{
		currentPCMountStage = mount ? MountStage::OnMount : MountStage::None;

		auto horse = sd::GetPlayersLastRiddenHorse();
		if (horse != nullptr && sd::IsDead(horse))
		{
			currentPCMountStage = MountStage::None;
		}
	}
}