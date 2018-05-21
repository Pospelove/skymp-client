/*
THIS FILE IS A PART OF THE SKYRIM DRAGON SCRIPT PROJECT
(C) Alexander Blade 2011
http://Alexander.SannyBuilder.com
*/

#pragma once

#include "types.h"
#include "invoke.h"
#include "plugin.h"

#define BSScript_StaticFunctionTag 0
using namespace sd;

extern "C" {
		__declspec(dllexport) void AddAchievement(int aiAchievementID) {
			NativeInvoke::Invoke<ScriptNone>("Game", "AddAchievement", BSScript_StaticFunctionTag, aiAchievementID);
		}

		__declspec(dllexport) void AdvanceSkill(char * asSkillName, float afMagnitude) {
			char *_asSkillName = BSString_Create(asSkillName);
			NativeInvoke::Invoke<ScriptNone>("Game", "AdvanceSkill", BSScript_StaticFunctionTag, &_asSkillName, afMagnitude);
			BSString_Free(_asSkillName);
		}

		__declspec(dllexport) bool AddHavokBallAndSocketConstraint(TESObjectREFR * arRefA, char * arRefANode, TESObjectREFR * arRefB, char * arRefBNode, float afRefALocalOffsetX, float afRefALocalOffsetY, float afRefALocalOffsetZ, float afRefBLocalOffsetX, float afRefBLocalOffsetY, float afRefBLocalOffsetZ) {
			char *_arRefANode = BSString_Create(arRefANode);
			char *_arRefBNode = BSString_Create(arRefBNode);
			bool result = NativeInvoke::Invoke<bool>("Game", "AddHavokBallAndSocketConstraint", BSScript_StaticFunctionTag, arRefA, &_arRefANode, arRefB, &_arRefBNode, afRefALocalOffsetX, afRefALocalOffsetY, afRefALocalOffsetZ, afRefBLocalOffsetX, afRefBLocalOffsetY, afRefBLocalOffsetZ);
			BSString_Free(_arRefANode);
			BSString_Free(_arRefBNode);
			return result;
		}

		__declspec(dllexport) bool RemoveHavokConstraints(TESObjectREFR * arFirstRef, char * arFirstRefNodeName, TESObjectREFR * arSecondRef, char * arSecondRefNodeName) {
			char *_arFirstRefNodeName = BSString_Create(arFirstRefNodeName);
			char *_arSecondRefNodeName = BSString_Create(arSecondRefNodeName);
			bool result = NativeInvoke::Invoke<bool>("Game", "RemoveHavokConstraints", BSScript_StaticFunctionTag, arFirstRef, &_arFirstRefNodeName, arSecondRef, &_arSecondRefNodeName);
			BSString_Free(_arFirstRefNodeName);
			BSString_Free(_arSecondRefNodeName);
			return result;
		}

		__declspec(dllexport) uint CalculateFavorCost(uint aiFavorPrice) {
			return NativeInvoke::Invoke<uint>("Game", "CalculateFavorCost", BSScript_StaticFunctionTag, aiFavorPrice);
		}

		__declspec(dllexport) void ClearTempEffects() {
			NativeInvoke::Invoke<ScriptNone>("Game", "ClearTempEffects", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void ClearPrison() {
			NativeInvoke::Invoke<ScriptNone>("Game", "ClearPrison", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void DisablePlayerControls(bool abMovement, bool abFighting, bool abCamSwitch, bool abLooking, bool abSneaking, bool abMenu, bool abActivate, bool abJournalTabs, uint aiDisablePOVType) {
			NativeInvoke::Invoke<ScriptNone>("Game", "DisablePlayerControls", BSScript_StaticFunctionTag, abMovement, abFighting, abCamSwitch, abLooking, abSneaking, abMenu, abActivate, abJournalTabs, aiDisablePOVType);
		}

		__declspec(dllexport) void EnablePlayerControls(bool abMovement, bool abFighting, bool abCamSwitch, bool abLooking, bool abSneaking, bool abMenu, bool abActivate, bool abJournalTabs, uint aiDisablePOVType) {
			NativeInvoke::Invoke<ScriptNone>("Game", "EnablePlayerControls", BSScript_StaticFunctionTag, abMovement, abFighting, abCamSwitch, abLooking, abSneaking, abMenu, abActivate, abJournalTabs, aiDisablePOVType);
		}

		__declspec(dllexport) void EnableFastTravel(bool abEnable) {
			NativeInvoke::Invoke<ScriptNone>("Game", "EnableFastTravel", BSScript_StaticFunctionTag, abEnable);
		}

		__declspec(dllexport) void FadeOutGame(bool abFadingOut, bool abBlackFade, float afSecsBeforeFade, float afFadeDuration) {
			NativeInvoke::Invoke<ScriptNone>("Game", "FadeOutGame", BSScript_StaticFunctionTag, abFadingOut, abBlackFade, afSecsBeforeFade, afFadeDuration);
		}

		__declspec(dllexport) void FastTravel(TESObjectREFR * akDestination) {
			NativeInvoke::Invoke<ScriptNone>("Game", "FastTravel", BSScript_StaticFunctionTag, akDestination);
		}

		__declspec(dllexport) TESObjectREFR * FindClosestReferenceOfType(TESForm * arBaseObject, float afX, float afY, float afZ, float afRadius) {
			return NativeInvoke::Invoke<TESObjectREFR *>("Game", "FindClosestReferenceOfType", BSScript_StaticFunctionTag, arBaseObject, afX, afY, afZ, afRadius);
		}

		__declspec(dllexport) TESObjectREFR * FindClosestReferenceOfAnyTypeInList(BGSListForm * arBaseObjects, float afX, float afY, float afZ, float afRadius) {
			return NativeInvoke::Invoke<TESObjectREFR *>("Game", "FindClosestReferenceOfAnyTypeInList", BSScript_StaticFunctionTag, arBaseObjects, afX, afY, afZ, afRadius);
		}

		__declspec(dllexport) TESObjectREFR * FindRandomReferenceOfType(TESForm * arBaseObject, float afX, float afY, float afZ, float afRadius) {
			return NativeInvoke::Invoke<TESObjectREFR *>("Game", "FindRandomReferenceOfType", BSScript_StaticFunctionTag, arBaseObject, afX, afY, afZ, afRadius);
		}

		__declspec(dllexport) TESObjectREFR * FindRandomReferenceOfAnyTypeInList(BGSListForm * arBaseObjects, float afX, float afY, float afZ, float afRadius) {
			return NativeInvoke::Invoke<TESObjectREFR *>("Game", "FindRandomReferenceOfAnyTypeInList", BSScript_StaticFunctionTag, arBaseObjects, afX, afY, afZ, afRadius);
		}

		__declspec(dllexport) CActor * FindClosestActor(float afX, float afY, float afZ, float afRadius) {
			return NativeInvoke::Invoke<CActor *>("Game", "FindClosestActor", BSScript_StaticFunctionTag, afX, afY, afZ, afRadius);
		}

		__declspec(dllexport) CActor * FindRandomActor(float afX, float afY, float afZ, float afRadius) {
			return NativeInvoke::Invoke<CActor *>("Game", "FindRandomActor", BSScript_StaticFunctionTag, afX, afY, afZ, afRadius);
		}

		__declspec(dllexport) void ForceThirdPerson() {
			NativeInvoke::Invoke<ScriptNone>("Game", "ForceThirdPerson", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void ForceFirstPerson() {
			NativeInvoke::Invoke<ScriptNone>("Game", "ForceFirstPerson", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void ShowFirstPersonGeometry(bool abShow) {
			NativeInvoke::Invoke<ScriptNone>("Game", "ShowFirstPersonGeometry", BSScript_StaticFunctionTag, abShow);
		}

		__declspec(dllexport) TESForm * GetFormById(uint aiFormID) {
			return NativeInvoke::Invoke<TESForm *>("Game", "GetForm", BSScript_StaticFunctionTag, aiFormID);
		}

		__declspec(dllexport) float GetGameSettingFloat(char * asGameSetting) {
			char *_asGameSetting = BSString_Create(asGameSetting);
			float result = NativeInvoke::Invoke<float>("Game", "GetGameSettingFloat", BSScript_StaticFunctionTag, &_asGameSetting);
			BSString_Free(_asGameSetting);
			return result;
		}

		__declspec(dllexport) int GetGameSettingInt(char * asGameSetting) {
			char *_asGameSetting = BSString_Create(asGameSetting);
			int result = NativeInvoke::Invoke<int>("Game", "GetGameSettingInt", BSScript_StaticFunctionTag, &_asGameSetting);
			BSString_Free(_asGameSetting);
			return result;
		}

		__declspec(dllexport) bool GetGameSettingString(char * asGameSetting) {
			char *_asGameSetting = BSString_Create(asGameSetting);
			bool result = NativeInvoke::Invoke<bool>("Game", "GetGameSettingString", BSScript_StaticFunctionTag, &_asGameSetting);
			BSString_Free(_asGameSetting);
			return result;
		}

		__declspec(dllexport) CActor * GetPlayer() {
			return NativeInvoke::Invoke<CActor *>("Game", "GetPlayer", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) TESObjectREFR * GetPlayerGrabbedRef() {
			return NativeInvoke::Invoke<TESObjectREFR *>("Game", "GetPlayerGrabbedRef", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) CActor * GetPlayersLastRiddenHorse() {
			return NativeInvoke::Invoke<CActor *>("Game", "GetPlayersLastRiddenHorse", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) float GetRealHoursPassed() {
			return NativeInvoke::Invoke<float>("Game", "GetRealHoursPassed", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void IncrementSkill(char * asSkillName) {
			char *_asSkillName = BSString_Create(asSkillName);
			NativeInvoke::Invoke<ScriptNone>("Game", "IncrementSkill", BSScript_StaticFunctionTag, &_asSkillName);
			BSString_Free(_asSkillName);
		}

		__declspec(dllexport) void IncrementSkillBy(char * asSkillName, uint aiCount) {
			char *_asSkillName = BSString_Create(asSkillName);
			NativeInvoke::Invoke<ScriptNone>("Game", "IncrementSkillBy", BSScript_StaticFunctionTag, &_asSkillName, aiCount);
			BSString_Free(_asSkillName);
		}

		__declspec(dllexport) void IncrementStat(char * asStatName, int aiModAmount) {
			char *_asStatName = BSString_Create(asStatName);
			NativeInvoke::Invoke<ScriptNone>("Game", "IncrementStat", BSScript_StaticFunctionTag, &_asStatName, aiModAmount);
			BSString_Free(_asStatName);
		}

		__declspec(dllexport) bool IsActivateControlsEnabled() {
			return NativeInvoke::Invoke<bool>("Game", "IsActivateControlsEnabled", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool IsCamSwitchControlsEnabled() {
			return NativeInvoke::Invoke<bool>("Game", "IsCamSwitchControlsEnabled", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool IsFastTravelEnabled() {
			return NativeInvoke::Invoke<bool>("Game", "IsFastTravelEnabled", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool IsFightingControlsEnabled() {
			return NativeInvoke::Invoke<bool>("Game", "IsFightingControlsEnabled", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool IsJournalControlsEnabled() {
			return NativeInvoke::Invoke<bool>("Game", "IsJournalControlsEnabled", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool IsLookingControlsEnabled() {
			return NativeInvoke::Invoke<bool>("Game", "IsLookingControlsEnabled", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool IsMenuControlsEnabled() {
			return NativeInvoke::Invoke<bool>("Game", "IsMenuControlsEnabled", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool IsMovementControlsEnabled() {
			return NativeInvoke::Invoke<bool>("Game", "IsMovementControlsEnabled", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool IsSneakingControlsEnabled() {
			return NativeInvoke::Invoke<bool>("Game", "IsSneakingControlsEnabled", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool IsWordUnlocked(TESWordOfPower * akWord) {
			return NativeInvoke::Invoke<bool>("Game", "IsWordUnlocked", BSScript_StaticFunctionTag, akWord);
		}

		__declspec(dllexport) void PlayBink(char * asFileName, bool abInterruptible, bool abMuteAudio, bool abMuteMusic, bool abLetterbox) {
			char *_asFileName = BSString_Create(asFileName);
			NativeInvoke::Invoke<ScriptNone>("Game", "PlayBink", BSScript_StaticFunctionTag, &_asFileName, abInterruptible, abMuteAudio, abMuteMusic, abLetterbox);
			BSString_Free(_asFileName);
		}

		__declspec(dllexport) void PrecacheCharGen() {
			NativeInvoke::Invoke<ScriptNone>("Game", "PrecacheCharGen", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void PrecacheCharGenClear() {
			NativeInvoke::Invoke<ScriptNone>("Game", "PrecacheCharGenClear", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) int QueryStat(char * asStat) {
			char *_asStat = BSString_Create(asStat);
			int result = NativeInvoke::Invoke<int>("Game", "QueryStat", BSScript_StaticFunctionTag, &_asStat);
			BSString_Free(_asStat);
			return result;
		}

		__declspec(dllexport) void QuitToMainMenu() {
			NativeInvoke::Invoke<ScriptNone>("Game", "QuitToMainMenu", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void RequestAutosave() {
			NativeInvoke::Invoke<ScriptNone>("Game", "RequestAutosave", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void RequestModel(char * asModelName) {
			char *_asModelName = BSString_Create(asModelName);
			NativeInvoke::Invoke<ScriptNone>("Game", "RequestModel", BSScript_StaticFunctionTag, &_asModelName);
			BSString_Free(_asModelName);
		}

		__declspec(dllexport) void RequestSave() {
			NativeInvoke::Invoke<ScriptNone>("Game", "RequestSave", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void SendWereWolfTransformation() {
			NativeInvoke::Invoke<ScriptNone>("Game", "SendWereWolfTransformation", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void ServeTime() {
			NativeInvoke::Invoke<ScriptNone>("Game", "ServeTime", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void SetBeastForm(bool abEntering) {
			NativeInvoke::Invoke<ScriptNone>("Game", "SetBeastForm", BSScript_StaticFunctionTag, abEntering);
		}

		__declspec(dllexport) void SetCameraTarget(CActor * arTarget) {
			NativeInvoke::Invoke<ScriptNone>("Game", "SetCameraTarget", BSScript_StaticFunctionTag, arTarget);
		}

		__declspec(dllexport) void SetHudCartMode(bool abSetCartMode) {
			NativeInvoke::Invoke<ScriptNone>("Game", "SetHudCartMode", BSScript_StaticFunctionTag, abSetCartMode);
		}

		__declspec(dllexport) void SetInChargen(bool abDisableSaving, bool abDisableWaiting, bool abShowControlsDisabledMessage) {
			NativeInvoke::Invoke<ScriptNone>("Game", "SetInChargen", BSScript_StaticFunctionTag, abDisableSaving, abDisableWaiting, abShowControlsDisabledMessage);
		}

		__declspec(dllexport) void SetPlayerAIDriven(bool abAIDriven) {
			NativeInvoke::Invoke<ScriptNone>("Game", "SetPlayerAIDriven", BSScript_StaticFunctionTag, abAIDriven);
		}

		__declspec(dllexport) void SetPlayerReportCrime(bool abReportCrime) {
			NativeInvoke::Invoke<ScriptNone>("Game", "SetPlayerReportCrime", BSScript_StaticFunctionTag, abReportCrime);
		}

		__declspec(dllexport) void SetSittingRotation(float afValue) {
			NativeInvoke::Invoke<ScriptNone>("Game", "SetSittingRotation", BSScript_StaticFunctionTag, afValue);
		}

		__declspec(dllexport) void ShakeCamera(TESObjectREFR * akSource, float afStrength, float afDuration) {
			NativeInvoke::Invoke<ScriptNone>("Game", "ShakeCamera", BSScript_StaticFunctionTag, akSource, afStrength, afDuration);
		}

		__declspec(dllexport) void ShakeController(float afSmallMotorStrength, float afBigMotorStreangth, float afDuration) {
			NativeInvoke::Invoke<ScriptNone>("Game", "ShakeController", BSScript_StaticFunctionTag, afSmallMotorStrength, afBigMotorStreangth, afDuration);
		}

		__declspec(dllexport) void ShowTitleSequenceMenu() {
			NativeInvoke::Invoke<ScriptNone>("Game", "ShowTitleSequenceMenu", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void HideTitleSequenceMenu() {
			NativeInvoke::Invoke<ScriptNone>("Game", "HideTitleSequenceMenu", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void StartTitleSequence(char * asSequenceName) {
			char *_asSequenceName = BSString_Create(asSequenceName);
			NativeInvoke::Invoke<ScriptNone>("Game", "StartTitleSequence", BSScript_StaticFunctionTag, &_asSequenceName);
			BSString_Free(_asSequenceName);
		}

		__declspec(dllexport) void ShowRaceMenu() {
			NativeInvoke::Invoke<ScriptNone>("Game", "ShowRaceMenu", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void ShowTrainingMenu(CActor * aTrainer) {
			NativeInvoke::Invoke<ScriptNone>("Game", "ShowTrainingMenu", BSScript_StaticFunctionTag, aTrainer);
		}

		__declspec(dllexport) void TeachWord(TESWordOfPower * akWord) {
			NativeInvoke::Invoke<ScriptNone>("Game", "TeachWord", BSScript_StaticFunctionTag, akWord);
		}

		__declspec(dllexport) void TriggerScreenBlood(uint aiValue) {
			NativeInvoke::Invoke<ScriptNone>("Game", "TriggerScreenBlood", BSScript_StaticFunctionTag, aiValue);
		}

		__declspec(dllexport) void UnlockWord(TESWordOfPower * akWord) {
			NativeInvoke::Invoke<ScriptNone>("Game", "UnlockWord", BSScript_StaticFunctionTag, akWord);
		}

		__declspec(dllexport) bool UsingGamepad() {
			return NativeInvoke::Invoke<bool>("Game", "UsingGamepad", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool GameTimeToString(float afGameTime) {
			return NativeInvoke::Invoke<bool>("Utility", "GameTimeToString", BSScript_StaticFunctionTag, afGameTime);
		}

		__declspec(dllexport) float GetCurrentGameTime() {
			return NativeInvoke::Invoke<float>("Utility", "GetCurrentGameTime", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) float GetCurrentRealTime() {
			return NativeInvoke::Invoke<float>("Utility", "GetCurrentRealTime", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool IsInMenuMode() {
			return NativeInvoke::Invoke<bool>("Utility", "IsInMenuMode", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) int RandomInt(int aiMin, int aiMax) {
			return NativeInvoke::Invoke<int>("Utility", "RandomInt", BSScript_StaticFunctionTag, aiMin, aiMax);
		}

		__declspec(dllexport) float RandomFloat(float afMin, float afMax) {
			return NativeInvoke::Invoke<float>("Utility", "RandomFloat", BSScript_StaticFunctionTag, afMin, afMax);
		}

		__declspec(dllexport) void SetINIBool(char * ini, bool value) {
			char *_ini = BSString_Create(ini);
			NativeInvoke::Invoke<ScriptNone>("Utility", "SetINIBool", BSScript_StaticFunctionTag, &_ini, value);
			BSString_Free(_ini);
		}

		__declspec(dllexport) void SetINIFloat(char * ini, float value) {
			char *_ini = BSString_Create(ini);
			NativeInvoke::Invoke<ScriptNone>("Utility", "SetINIFloat", BSScript_StaticFunctionTag, &_ini, value);
			BSString_Free(_ini);
		}

		__declspec(dllexport) void SetINIInt(char * ini, int value) {
			char *_ini = BSString_Create(ini);
			NativeInvoke::Invoke<ScriptNone>("Utility", "SetINIInt", BSScript_StaticFunctionTag, &_ini, value);
			BSString_Free(_ini);
		}

		__declspec(dllexport) void SetINIString(char * ini, char * value) {
			char *_ini = BSString_Create(ini);
			char *_value = BSString_Create(value);
			NativeInvoke::Invoke<ScriptNone>("Utility", "SetINIString", BSScript_StaticFunctionTag, &_ini, &_value);
			BSString_Free(_ini);
			BSString_Free(_value);
		}

		__declspec(dllexport) bool WaitGameTime(float afHours) {
			return NativeInvoke::Invoke<bool>("Utility", "WaitGameTime", BSScript_StaticFunctionTag, afHours);
		}

		__declspec(dllexport) bool WaitMenuMode(float afSeconds) {
			return NativeInvoke::Invoke<bool>("Utility", "WaitMenuMode", BSScript_StaticFunctionTag, afSeconds);
		}
	
		__declspec(dllexport) void CenterOnCell(char * asCellname) {
			char *_asCellname = BSString_Create(asCellname);
			NativeInvoke::Invoke<ScriptNone>("Debug", "CenterOnCell", BSScript_StaticFunctionTag, &_asCellname);
			BSString_Free(_asCellname);
		}

		__declspec(dllexport) bool CenterOnCellAndWait(char * asCellname) {
			char *_asCellname = BSString_Create(asCellname);
			bool result = NativeInvoke::Invoke<bool>("Debug", "CenterOnCellAndWait", BSScript_StaticFunctionTag, &_asCellname);
			BSString_Free(_asCellname);
			return result;
		}

		__declspec(dllexport) bool PlayerMoveToAndWait(char * asDestRef) {
			char *_asDestRef = BSString_Create(asDestRef);
			bool result = NativeInvoke::Invoke<bool>("Debug", "PlayerMoveToAndWait", BSScript_StaticFunctionTag, &_asDestRef);
			BSString_Free(_asDestRef);
			return result;
		}

		__declspec(dllexport) void CloseUserLog(char * asLogName) {
			char *_asLogName = BSString_Create(asLogName);
			NativeInvoke::Invoke<ScriptNone>("Debug", "CloseUserLog", BSScript_StaticFunctionTag, &_asLogName);
			BSString_Free(_asLogName);
		}

		__declspec(dllexport) void DebugChannelNotify(char * channel, char * message) {
			char *_channel = BSString_Create(channel);
			char *_message = BSString_Create(message);
			NativeInvoke::Invoke<ScriptNone>("Debug", "DebugChannelNotify", BSScript_StaticFunctionTag, &_channel, &_message);
			BSString_Free(_channel);
			BSString_Free(_message);
		}

		__declspec(dllexport) void DumpAliasData(TESQuest * akQuest) {
			NativeInvoke::Invoke<ScriptNone>("Debug", "DumpAliasData", BSScript_StaticFunctionTag, akQuest);
		}

		__declspec(dllexport) bool GetConfigName() {
			return NativeInvoke::Invoke<bool>("Debug", "GetConfigName", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool GetPlatformName() {
			return NativeInvoke::Invoke<bool>("Debug", "GetPlatformName", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) bool GetVersionNumber() {
			return NativeInvoke::Invoke<bool>("Debug", "GetVersionNumber", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void ShowMessageBox(char * asMessageBoxText) {
			char *_asMessageBoxText = BSString_Create(asMessageBoxText);
			NativeInvoke::Invoke<ScriptNone>("Debug", "MessageBox", BSScript_StaticFunctionTag, &_asMessageBoxText);
			BSString_Free(_asMessageBoxText);
		}

		__declspec(dllexport) void Notification(char * asNotificationText) {
			char *_asNotificationText = BSString_Create(asNotificationText);
			NativeInvoke::Invoke<ScriptNone>("Debug", "Notification", BSScript_StaticFunctionTag, &_asNotificationText);
			BSString_Free(_asNotificationText);
		}

		__declspec(dllexport) bool OpenUserLog(char * asLogName) {
			char *_asLogName = BSString_Create(asLogName);
			bool result = NativeInvoke::Invoke<bool>("Debug", "OpenUserLog", BSScript_StaticFunctionTag, &_asLogName);
			BSString_Free(_asLogName);
			return result;
		}

		__declspec(dllexport) void QuitGame() {
			NativeInvoke::Invoke<ScriptNone>("Debug", "QuitGame", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void SendAnimationEvent(TESObjectREFR * arRef, char * asEventName) {
			char *_asEventName = BSString_Create(asEventName);
			NativeInvoke::Invoke<ScriptNone>("Debug", "SendAnimationEvent", BSScript_StaticFunctionTag, arRef, &_asEventName);
			BSString_Free(_asEventName);
		}

		__declspec(dllexport) void SetFootIK(bool abFootIK) {
			NativeInvoke::Invoke<ScriptNone>("Debug", "SetFootIK", BSScript_StaticFunctionTag, abFootIK);
		}

		__declspec(dllexport) void SetGodMode(bool abGodMode) {
			NativeInvoke::Invoke<ScriptNone>("Debug", "SetGodMode", BSScript_StaticFunctionTag, abGodMode);
		}

		__declspec(dllexport) void StartScriptProfiling(char * asScriptName) {
			char *_asScriptName = BSString_Create(asScriptName);
			NativeInvoke::Invoke<ScriptNone>("Debug", "StartScriptProfiling", BSScript_StaticFunctionTag, &_asScriptName);
			BSString_Free(_asScriptName);
		}

		__declspec(dllexport) void StartStackProfiling() {
			NativeInvoke::Invoke<ScriptNone>("Debug", "StartStackProfiling", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void StopScriptProfiling(char * asScriptName) {
			char *_asScriptName = BSString_Create(asScriptName);
			NativeInvoke::Invoke<ScriptNone>("Debug", "StopScriptProfiling", BSScript_StaticFunctionTag, &_asScriptName);
			BSString_Free(_asScriptName);
		}

		__declspec(dllexport) void StopStackProfiling() {
			NativeInvoke::Invoke<ScriptNone>("Debug", "StopStackProfiling", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void TakeScreenshot(char * asFilename) {
			char *_asFilename = BSString_Create(asFilename);
			NativeInvoke::Invoke<ScriptNone>("Debug", "TakeScreenshot", BSScript_StaticFunctionTag, &_asFilename);
			BSString_Free(_asFilename);
		}

		__declspec(dllexport) void ToggleAI() {
			NativeInvoke::Invoke<ScriptNone>("Debug", "ToggleAI", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void ToggleCollisions() {
			NativeInvoke::Invoke<ScriptNone>("Debug", "ToggleCollisions", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void ToggleMenus() {
			NativeInvoke::Invoke<ScriptNone>("Debug", "ToggleMenus", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) void Trace(char * asTextToPrint, uint aiSeverity) {
			char *_asTextToPrint = BSString_Create(asTextToPrint);
			NativeInvoke::Invoke<ScriptNone>("Debug", "Trace", BSScript_StaticFunctionTag, &_asTextToPrint, aiSeverity);
			BSString_Free(_asTextToPrint);
		}

		__declspec(dllexport) void TraceStack(char * asTextToPrint, uint aiSeverity) {
			char *_asTextToPrint = BSString_Create(asTextToPrint);
			NativeInvoke::Invoke<ScriptNone>("Debug", "TraceStack", BSScript_StaticFunctionTag, &_asTextToPrint, aiSeverity);
			BSString_Free(_asTextToPrint);
		}

		__declspec(dllexport) bool TraceUser(char * asUserLog, char * asTextToPrint, uint aiSeverity) {
			char *_asUserLog = BSString_Create(asUserLog);
			char *_asTextToPrint = BSString_Create(asTextToPrint);
			bool result = NativeInvoke::Invoke<bool>("Debug", "TraceUser", BSScript_StaticFunctionTag, &_asUserLog, &_asTextToPrint, aiSeverity);
			BSString_Free(_asUserLog);
			BSString_Free(_asTextToPrint);
			return result;
		}

		__declspec(dllexport) uint GetFormID(TESForm * self) {
			return NativeInvoke::Invoke<uint>("Form", "GetFormID", self);
		}

		__declspec(dllexport) int GetGoldValue(TESForm * self) {
			return NativeInvoke::Invoke<int>("Form", "GetGoldValue", self);
		}

		__declspec(dllexport) bool HasKeyword(TESForm * self, BGSKeyword * akKeyword) {
			return NativeInvoke::Invoke<bool>("Form", "HasKeyword", self, akKeyword);
		}

		__declspec(dllexport) bool PlayerKnows(TESForm * self) {
			return NativeInvoke::Invoke<bool>("Form", "PlayerKnows", self);
		}

		
		__declspec(dllexport) void SendStoryEvent(BGSKeyword * self, BGSLocation * akLoc, TESObjectREFR * akRef1, TESObjectREFR * akRef2, uint aiValue1, uint aiValue2) {
			NativeInvoke::Invoke<ScriptNone>("Keyword", "SendStoryEvent", self, akLoc, akRef1, akRef2, aiValue1, aiValue2);
		}

		__declspec(dllexport) bool SendStoryEventAndWait(BGSKeyword * self, BGSLocation * akLoc, TESObjectREFR * akRef1, TESObjectREFR * akRef2, uint aiValue1, uint aiValue2) {
			return NativeInvoke::Invoke<bool>("Keyword", "SendStoryEventAndWait", self, akLoc, akRef1, akRef2, aiValue1, aiValue2);
		}
		__declspec(dllexport) float GetKeywordData(BGSLocation * self, BGSKeyword * akKeyword) {
			return NativeInvoke::Invoke<float>("Location", "GetKeywordData", self, akKeyword);
		}

		__declspec(dllexport) uint GetRefTypeAliveCount(BGSLocation * self, BGSLocation * akRefType) {
			return NativeInvoke::Invoke<uint>("Location", "GetRefTypeAliveCount", self, akRefType);
		}

		__declspec(dllexport) uint GetRefTypeDeadCount(BGSLocation * self, BGSLocation * akRefType) {
			return NativeInvoke::Invoke<uint>("Location", "GetRefTypeDeadCount", self, akRefType);
		}

		__declspec(dllexport) bool HasCommonParent(BGSLocation * self, BGSLocation * akOther, BGSKeyword * akFilter) {
			return NativeInvoke::Invoke<bool>("Location", "HasCommonParent", self, akOther, akFilter);
		}

		__declspec(dllexport) bool HasRefType(BGSLocation * self, BGSLocation * akRefType) {
			return NativeInvoke::Invoke<bool>("Location", "HasRefType", self, akRefType);
		}

		/*__declspec(dllexport) bool IsChild(BGSLocation * self, BGSLocation * akOther) {
			return NativeInvoke::Invoke<bool>("Location", "IsChild", self, akOther);
		}*/

		__declspec(dllexport) bool IsCleared(BGSLocation * self) {
			return NativeInvoke::Invoke<bool>("Location", "IsCleared", self);
		}

		__declspec(dllexport) bool IsLoaded(BGSLocation * self) {
			return NativeInvoke::Invoke<bool>("Location", "IsLoaded", self);
		}

		__declspec(dllexport) void SetCleared(BGSLocation * self, bool abCleared) {
			NativeInvoke::Invoke<ScriptNone>("Location", "SetCleared", self, abCleared);
		}

		__declspec(dllexport) void SetKeywordData(BGSLocation * self, BGSKeyword * akKeyword, float afData) {
			NativeInvoke::Invoke<ScriptNone>("Location", "SetKeywordData", self, akKeyword, afData);
		}
		__declspec(dllexport) bool Show(BGSMessage * self, float afArg1, float afArg2, float afArg3, float afArg4, float afArg5, float afArg6, float afArg7, float afArg8, float afArg9) {
			return NativeInvoke::Invoke<bool>("Message", "Show", self, afArg1, afArg2, afArg3, afArg4, afArg5, afArg6, afArg7, afArg8, afArg9);
		}

		__declspec(dllexport) void ShowAsHelpMessage(BGSMessage * self, char * asEvent, float afDuration, float afInterval, int aiMaxTimes) {
			char *_asEvent = BSString_Create(asEvent);
			NativeInvoke::Invoke<ScriptNone>("Message", "ShowAsHelpMessage", self, &_asEvent, afDuration, afInterval, aiMaxTimes);
			BSString_Free(_asEvent);
		}

		__declspec(dllexport) void ResetHelpMessage(char * asEvent) {
			char *_asEvent = BSString_Create(asEvent);
			NativeInvoke::Invoke<ScriptNone>("Message", "ResetHelpMessage", BSScript_StaticFunctionTag, &_asEvent);
			BSString_Free(_asEvent);
		}
		/*__declspec(dllexport) void Add(BGSMusicType * self) {
			NativeInvoke::Invoke<ScriptNone>("MusicType", "Add", self);
		}*/

		__declspec(dllexport) void Remove(BGSMusicType * self) {
			NativeInvoke::Invoke<ScriptNone>("MusicType", "Remove", self);
		}
		/*__declspec(dllexport) void Play(BGSReferenceEffect * self, TESObjectREFR * akObject, float afTime, TESObjectREFR * akFacingObject) {
			NativeInvoke::Invoke<ScriptNone>("VisualEffect", "Play", self, akObject, afTime, akFacingObject);
		}*/

		__declspec(dllexport) void Stop(BGSReferenceEffect * self, TESObjectREFR * akObject) {
			NativeInvoke::Invoke<ScriptNone>("VisualEffect", "Stop", self, akObject);
		}


		__declspec(dllexport) void ForceStart(BGSScene * self) {
			NativeInvoke::Invoke<ScriptNone>("Scene", "ForceStart", self);
		}

		/*__declspec(dllexport) TESQuest * GetOwningQuest(BGSScene * self) {
			return NativeInvoke::Invoke<TESQuest *>("Scene", "GetOwningQuest", self);
		}*/

		__declspec(dllexport) bool IsActionComplete(BGSScene * self, uint aiActionID) {
			return NativeInvoke::Invoke<bool>("Scene", "IsActionComplete", self, aiActionID);
		}

		__declspec(dllexport) bool IsPlaying(BGSScene * self) {
			return NativeInvoke::Invoke<bool>("Scene", "IsPlaying", self);
		}

		__declspec(dllexport) void Start(BGSScene * self) {
			NativeInvoke::Invoke<ScriptNone>("Scene", "Start", self);
		}

		/*__declspec(dllexport) void Stop(BGSScene * self) {
			NativeInvoke::Invoke<ScriptNone>("Scene", "Stop", self);
		}*/
		__declspec(dllexport) void Apply(BGSShaderParticleGeometryData * self, float afFadeTime) {
			NativeInvoke::Invoke<ScriptNone>("ShaderParticleGeometry", "Apply", self, afFadeTime);
		}

		/*__declspec(dllexport) void Remove(BGSShaderParticleGeometryData * self, float afFadeTime) {
			NativeInvoke::Invoke<ScriptNone>("ShaderParticleGeometry", "Remove", self, afFadeTime);
		}*/
		__declspec(dllexport) void Pause(BGSSoundCategory * self) {
			NativeInvoke::Invoke<ScriptNone>("SoundCategory", "Pause", self);
		}

		__declspec(dllexport) void UnPause(BGSSoundCategory * self) {
			NativeInvoke::Invoke<ScriptNone>("SoundCategory", "UnPause", self);
		}

		__declspec(dllexport) void Mute(BGSSoundCategory * self) {
			NativeInvoke::Invoke<ScriptNone>("SoundCategory", "Mute", self);
		}

		__declspec(dllexport) void UnMute(BGSSoundCategory * self) {
			NativeInvoke::Invoke<ScriptNone>("SoundCategory", "UnMute", self);
		}

		__declspec(dllexport) void SetVolume(BGSSoundCategory * self, float afVolume) {
			NativeInvoke::Invoke<ScriptNone>("SoundCategory", "SetVolume", self, afVolume);
		}

		__declspec(dllexport) void SetFrequency(BGSSoundCategory * self, float afFrequencyCoeffecient) {
			NativeInvoke::Invoke<ScriptNone>("SoundCategory", "SetFrequency", self, afFrequencyCoeffecient);
		}
		__declspec(dllexport) bool GetAssociatedSkill(EffectSetting * self) {
			return NativeInvoke::Invoke<bool>("MagicEffect", "GetAssociatedSkill", self);
		}
		/*__declspec(dllexport) void Play(TESEffectShader * self, TESObjectREFR * akObject, float afDuration) {
			NativeInvoke::Invoke<ScriptNone>("EffectShader", "Play", self, akObject, afDuration);
		}*/

		/*__declspec(dllexport) void Stop(TESEffectShader * self, TESObjectREFR * akObject) {
			NativeInvoke::Invoke<ScriptNone>("EffectShader", "Stop", self, akObject);
		}*/

		__declspec(dllexport) bool CanPayCrimeGold(TESFaction * self) {
			return NativeInvoke::Invoke<bool>("Faction", "CanPayCrimeGold", self);
		}

		__declspec(dllexport) uint GetCrimeGold(TESFaction * self) {
			return NativeInvoke::Invoke<uint>("Faction", "GetCrimeGold", self);
		}

		__declspec(dllexport) uint GetCrimeGoldNonViolent(TESFaction * self) {
			return NativeInvoke::Invoke<uint>("Faction", "GetCrimeGoldNonViolent", self);
		}

		__declspec(dllexport) uint GetCrimeGoldViolent(TESFaction * self) {
			return NativeInvoke::Invoke<uint>("Faction", "GetCrimeGoldViolent", self);
		}

		__declspec(dllexport) uint GetInfamy(TESFaction * self) {
			return NativeInvoke::Invoke<uint>("Faction", "GetInfamy", self);
		}

		__declspec(dllexport) uint GetInfamyNonViolent(TESFaction * self) {
			return NativeInvoke::Invoke<uint>("Faction", "GetInfamyNonViolent", self);
		}

		__declspec(dllexport) uint GetInfamyViolent(TESFaction * self) {
			return NativeInvoke::Invoke<uint>("Faction", "GetInfamyViolent", self);
		}

		__declspec(dllexport) int GetReaction(TESFaction * self, TESFaction * akOther) {
			return NativeInvoke::Invoke<int>("Faction", "GetReaction", self, akOther);
		}

		__declspec(dllexport) uint GetStolenItemValueCrime(TESFaction * self) {
			return NativeInvoke::Invoke<uint>("Faction", "GetStolenItemValueCrime", self);
		}

		__declspec(dllexport) uint GetStolenItemValueNoCrime(TESFaction * self) {
			return NativeInvoke::Invoke<uint>("Faction", "GetStolenItemValueNoCrime", self);
		}

		__declspec(dllexport) bool IsFactionInCrimeGroup(TESFaction * self, TESFaction * akOther) {
			return NativeInvoke::Invoke<bool>("Faction", "IsFactionInCrimeGroup", self, akOther);
		}

		__declspec(dllexport) bool IsPlayerExpelled(TESFaction * self) {
			return NativeInvoke::Invoke<bool>("Faction", "IsPlayerExpelled", self);
		}

		__declspec(dllexport) void ModCrimeGold(TESFaction * self, int aiAmount, bool abViolent) {
			NativeInvoke::Invoke<ScriptNone>("Faction", "ModCrimeGold", self, aiAmount, abViolent);
		}

		__declspec(dllexport) void ModReaction(TESFaction * self, TESFaction * akOther, int aiAmount) {
			NativeInvoke::Invoke<ScriptNone>("Faction", "ModReaction", self, akOther, aiAmount);
		}

		__declspec(dllexport) void PlayerPayCrimeGold(TESFaction * self, bool abRemoveStolenItems, bool abGoToJail) {
			NativeInvoke::Invoke<ScriptNone>("Faction", "PlayerPayCrimeGold", self, abRemoveStolenItems, abGoToJail);
		}

		/*__declspec(dllexport) void SendAssaultAlarm(TESFaction * self) {
			NativeInvoke::Invoke<ScriptNone>("Faction", "SendAssaultAlarm", self);
		}*/

		__declspec(dllexport) bool SendPlayerToJail(TESFaction * self, bool abRemoveInventory, bool abRealJail) {
			return NativeInvoke::Invoke<bool>("Faction", "SendPlayerToJail", self, abRemoveInventory, abRealJail);
		}

		__declspec(dllexport) void SetAlly(TESFaction * self, TESFaction * akOther, bool abSelfIsFriendToOther, bool abOtherIsFriendToSelf) {
			NativeInvoke::Invoke<ScriptNone>("Faction", "SetAlly", self, akOther, abSelfIsFriendToOther, abOtherIsFriendToSelf);
		}

		__declspec(dllexport) void SetCrimeGold(TESFaction * self, uint aiGold) {
			NativeInvoke::Invoke<ScriptNone>("Faction", "SetCrimeGold", self, aiGold);
		}

		__declspec(dllexport) void SetCrimeGoldViolent(TESFaction * self, uint aiGold) {
			NativeInvoke::Invoke<ScriptNone>("Faction", "SetCrimeGoldViolent", self, aiGold);
		}

		__declspec(dllexport) void SetEnemy(TESFaction * self, TESFaction * akOther, bool abSelfIsNeutralToOther, bool abOtherIsNeutralToSelf) {
			NativeInvoke::Invoke<ScriptNone>("Faction", "SetEnemy", self, akOther, abSelfIsNeutralToOther, abOtherIsNeutralToSelf);
		}

		__declspec(dllexport) void SetPlayerEnemy(TESFaction * self, bool abIsEnemy) {
			NativeInvoke::Invoke<ScriptNone>("Faction", "SetPlayerEnemy", self, abIsEnemy);
		}

		__declspec(dllexport) void SetPlayerExpelled(TESFaction * self, bool abIsExpelled) {
			NativeInvoke::Invoke<ScriptNone>("Faction", "SetPlayerExpelled", self, abIsExpelled);
		}

		__declspec(dllexport) void SetReaction(TESFaction * self, TESFaction * akOther, int aiNewValue) {
			NativeInvoke::Invoke<ScriptNone>("Faction", "SetReaction", self, akOther, aiNewValue);
		}
		__declspec(dllexport) float GetValue(TESGlobal * self) {
			return NativeInvoke::Invoke<float>("GlobalVariable", "GetValue", self);
		}

		__declspec(dllexport) void SetValue(TESGlobal * self, float afNewValue) {
			NativeInvoke::Invoke<ScriptNone>("GlobalVariable", "SetValue", self, afNewValue);
		}
		/*__declspec(dllexport) void Apply(TESImageSpaceModifier * self, float afStrength) {
			NativeInvoke::Invoke<ScriptNone>("ImageSpaceModifier", "Apply", self, afStrength);
		}*/

		__declspec(dllexport) void ApplyCrossFade(TESImageSpaceModifier * self, float afFadeDuration) {
			NativeInvoke::Invoke<ScriptNone>("ImageSpaceModifier", "ApplyCrossFade", self, afFadeDuration);
		}

		__declspec(dllexport) void PopTo(TESImageSpaceModifier * self, TESImageSpaceModifier * akNewModifier, float afStrength) {
			NativeInvoke::Invoke<ScriptNone>("ImageSpaceModifier", "PopTo", self, akNewModifier, afStrength);
		}

		/*__declspec(dllexport) void Remove(TESImageSpaceModifier * self) {
			NativeInvoke::Invoke<ScriptNone>("ImageSpaceModifier", "Remove", self);
		}*/

		__declspec(dllexport) void RemoveCrossFade(float afFadeDuration) {
			NativeInvoke::Invoke<ScriptNone>("ImageSpaceModifier", "RemoveCrossFade", BSScript_StaticFunctionTag, afFadeDuration);
		}
		/*__declspec(dllexport) void AddForm(TESLevCharacter * self, TESForm * apForm, uint aiLevel) {
			NativeInvoke::Invoke<ScriptNone>("LeveledActor", "AddForm", self, apForm, aiLevel);
		}*/

		__declspec(dllexport) void Revert(TESLevCharacter * self) {
			NativeInvoke::Invoke<ScriptNone>("LeveledActor", "Revert", self);
		}
		/*__declspec(dllexport) void AddForm(TESLevItem * self, TESForm * apForm, uint aiLevel, uint aiCount) {
			NativeInvoke::Invoke<ScriptNone>("LeveledItem", "AddForm", self, apForm, aiLevel, aiCount);
		}*/

		/*__declspec(dllexport) void Revert(TESLevItem * self) {
			NativeInvoke::Invoke<ScriptNone>("LeveledItem", "Revert", self);
		}*/
		/*__declspec(dllexport) void AddForm(TESLevSpell * self, TESForm * apForm, uint aiLevel) {
			NativeInvoke::Invoke<ScriptNone>("LeveledSpell", "AddForm", self, apForm, aiLevel);
		}*/

		/*__declspec(dllexport) void Revert(TESLevSpell * self) {
			NativeInvoke::Invoke<ScriptNone>("LeveledSpell", "Revert", self);
		}*/
		__declspec(dllexport) TESClass * GetClass(TESNPC * self) {
			return NativeInvoke::Invoke<TESClass *>("ActorBase", "GetClass", self);
		}

		__declspec(dllexport) int GetDeadCount(TESNPC * self) {
			return NativeInvoke::Invoke<int>("ActorBase", "GetDeadCount", self);
		}

		__declspec(dllexport) BGSListForm * GetGiftFilter(TESNPC * self) {
			return NativeInvoke::Invoke<BGSListForm *>("ActorBase", "GetGiftFilter", self);
		}

		/*__declspec(dllexport) TESRace * GetRace(TESNPC * self) {
			return NativeInvoke::Invoke<TESRace *>("ActorBase", "GetRace", self);
		}*/

		__declspec(dllexport) int GetSex(TESNPC * self) {
			return NativeInvoke::Invoke<int>("ActorBase", "GetSex", self);
		}

		__declspec(dllexport) bool IsEssential(TESNPC * self) {
			return NativeInvoke::Invoke<bool>("ActorBase", "IsEssential", self);
		}

		__declspec(dllexport) bool IsInvulnerable(TESNPC * self) {
			return NativeInvoke::Invoke<bool>("ActorBase", "IsInvulnerable", self);
		}

		__declspec(dllexport) bool IsProtected(TESNPC * self) {
			return NativeInvoke::Invoke<bool>("ActorBase", "IsProtected", self);
		}

		__declspec(dllexport) bool IsUnique(TESNPC * self) {
			return NativeInvoke::Invoke<bool>("ActorBase", "IsUnique", self);
		}

		__declspec(dllexport) void SetEssential(TESNPC * self, bool abEssential) {
			NativeInvoke::Invoke<ScriptNone>("ActorBase", "SetEssential", self, abEssential);
		}

		__declspec(dllexport) void SetInvulnerable(TESNPC * self, bool abInvulnerable) {
			NativeInvoke::Invoke<ScriptNone>("ActorBase", "SetInvulnerable", self, abInvulnerable);
		}

		__declspec(dllexport) void SetProtected(TESNPC * self, bool abProtected) {
			NativeInvoke::Invoke<ScriptNone>("ActorBase", "SetProtected", self, abProtected);
		}

		__declspec(dllexport) void SetOutfit(TESNPC * self, BGSOutfit * akOutfit, bool abSleepOutfit) {
			NativeInvoke::Invoke<ScriptNone>("ActorBase", "SetOutfit", self, akOutfit, abSleepOutfit);
		}
		__declspec(dllexport) TESNPC * GetActorOwner(TESObjectCELL * self) {
			return NativeInvoke::Invoke<TESNPC *>("Cell", "GetActorOwner", self);
		}

		__declspec(dllexport) TESFaction * GetFactionOwner(TESObjectCELL * self) {
			return NativeInvoke::Invoke<TESFaction *>("Cell", "GetFactionOwner", self);
		}

		__declspec(dllexport) bool IsAttached(TESObjectCELL * self) {
			return NativeInvoke::Invoke<bool>("Cell", "IsAttached", self);
		}

		__declspec(dllexport) bool IsInterior(TESObjectCELL * self) {
			return NativeInvoke::Invoke<bool>("Cell", "IsInterior", self);
		}

		__declspec(dllexport) void Reset(TESObjectCELL * self) {
			NativeInvoke::Invoke<ScriptNone>("Cell", "Reset", self);
		}

		__declspec(dllexport) void SetActorOwner(TESObjectCELL * self, TESNPC * akActor) {
			NativeInvoke::Invoke<ScriptNone>("Cell", "SetActorOwner", self, akActor);
		}

		__declspec(dllexport) void SetFactionOwner(TESObjectCELL * self, TESFaction * akFaction) {
			NativeInvoke::Invoke<ScriptNone>("Cell", "SetFactionOwner", self, akFaction);
		}

		__declspec(dllexport) void SetFogColor(TESObjectCELL * self, uint aiNearRed, uint aiNearGreen, uint aiNearBlue, uint aiFarRed, uint aiFarGreen, uint aiFarBlue) {
			NativeInvoke::Invoke<ScriptNone>("Cell", "SetFogColor", self, aiNearRed, aiNearGreen, aiNearBlue, aiFarRed, aiFarGreen, aiFarBlue);
		}

		__declspec(dllexport) void SetFogPlanes(TESObjectCELL * self, float afNear, float afFar) {
			NativeInvoke::Invoke<ScriptNone>("Cell", "SetFogPlanes", self, afNear, afFar);
		}

		__declspec(dllexport) void SetFogPower(TESObjectCELL * self, float afPower) {
			NativeInvoke::Invoke<ScriptNone>("Cell", "SetFogPower", self, afPower);
		}

		__declspec(dllexport) void SetPublic(TESObjectCELL * self, bool abPublic) {
			NativeInvoke::Invoke<ScriptNone>("Cell", "SetPublic", self, abPublic);
		}

		__declspec(dllexport) bool Activate(TESObjectREFR * self, TESObjectREFR * akActivator, bool abDefaultProcessingOnly) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "Activate", self, akActivator, abDefaultProcessingOnly);
		}

		__declspec(dllexport) void AddInventoryEventFilter(TESObjectREFR * self, TESForm * akFilter) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "AddInventoryEventFilter", self, akFilter);
		}

		__declspec(dllexport) void TetherToHorse(TESObjectREFR * self, TESObjectREFR * akHorse) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "TetherToHorse", self, akHorse);
		}

		__declspec(dllexport) bool AddDependentAnimatedObjectReference(TESObjectREFR * self, TESObjectREFR * akDependent) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "AddDependentAnimatedObjectReference", self, akDependent);
		}

		__declspec(dllexport) bool AddItem(TESObjectREFR * self, TESForm * akItemToAdd, uint aiCount, bool abSilent) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "AddItem", self, akItemToAdd, aiCount, abSilent);
		}

		__declspec(dllexport) void AddToMap(TESObjectREFR * self, bool abAllowFastTravel) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "AddToMap", self, abAllowFastTravel);
		}

		__declspec(dllexport) bool ApplyHavokImpulse(TESObjectREFR * self, float afX, float afY, float afZ, float afMagnitude) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "ApplyHavokImpulse", self, afX, afY, afZ, afMagnitude);
		}

		__declspec(dllexport) void BlockActivation(TESObjectREFR * self, bool abBlocked) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "BlockActivation", self, abBlocked);
		}

		__declspec(dllexport) uint CalculateEncounterLevel(TESObjectREFR * self, uint aiDifficulty) {
			return NativeInvoke::Invoke<uint>("ObjectReference", "CalculateEncounterLevel", self, aiDifficulty);
		}

		__declspec(dllexport) bool CanFastTravelToMarker(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "CanFastTravelToMarker", self);
		}

		__declspec(dllexport) void ClearDestruction(TESObjectREFR * self) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "ClearDestruction", self);
		}

		__declspec(dllexport) void CreateDetectionEvent(TESObjectREFR * self, CActor * akOwner, uint aiSoundLevel) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "CreateDetectionEvent", self, akOwner, aiSoundLevel);
		}

		__declspec(dllexport) bool DamageObject(TESObjectREFR * self, float afDamage) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "DamageObject", self, afDamage);
		}

		__declspec(dllexport) bool Delete(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "Delete", self);
		}

		__declspec(dllexport) bool Disable(TESObjectREFR * self, bool abFadeOut) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "Disable", self, abFadeOut);
		}

		__declspec(dllexport) void DisableNoWait(TESObjectREFR * self, bool abFadeOut) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "DisableNoWait", self, abFadeOut);
		}

		__declspec(dllexport) bool DropObject(TESObjectREFR * self, TESForm * akObject, uint aiCount) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "DropObject", self, akObject, aiCount);
		}

		__declspec(dllexport) bool Enable(TESObjectREFR * self, bool abFadeIn) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "Enable", self, abFadeIn);
		}

		/*__declspec(dllexport) void EnableFastTravel(TESObjectREFR * self, bool abEnable) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "EnableFastTravel", self, abEnable);
		}*/

		__declspec(dllexport) void EnableNoWait(TESObjectREFR * self, bool abFadeIn) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "EnableNoWait", self, abFadeIn);
		}

		__declspec(dllexport) bool ForceAddRagdollToWorld(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "ForceAddRagdollToWorld", self);
		}

		__declspec(dllexport) bool ForceRemoveRagdollFromWorld(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "ForceRemoveRagdollFromWorld", self);
		}

		/*__declspec(dllexport) TESNPC * GetActorOwner(TESObjectREFR * self) {
			return NativeInvoke::Invoke<TESNPC *>("ObjectReference", "GetActorOwner", self);
		}*/

		__declspec(dllexport) float GetAngleX(TESObjectREFR * self) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetAngleX", self);
		}

		__declspec(dllexport) float GetAngleY(TESObjectREFR * self) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetAngleY", self);
		}

		__declspec(dllexport) float GetAngleZ(TESObjectREFR * self) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetAngleZ", self);
		}

		__declspec(dllexport) bool GetAnimationVariableBool(TESObjectREFR * self, char * arVariableName) {
			char *_arVariableName = BSString_Create(arVariableName);
			bool result = NativeInvoke::Invoke<bool>("ObjectReference", "GetAnimationVariableBool", self, &_arVariableName);
			BSString_Free(_arVariableName);
			return result;
		}

		__declspec(dllexport) int GetAnimationVariableInt(TESObjectREFR * self, char * arVariableName) {
			char *_arVariableName = BSString_Create(arVariableName);
			int result = NativeInvoke::Invoke<int>("ObjectReference", "GetAnimationVariableInt", self, &_arVariableName);
			BSString_Free(_arVariableName);
			return result;
		}

		__declspec(dllexport) float GetAnimationVariableFloat(TESObjectREFR * self, char * arVariableName) {
			char *_arVariableName = BSString_Create(arVariableName);
			float result = NativeInvoke::Invoke<float>("ObjectReference", "GetAnimationVariableFloat", self, &_arVariableName);
			BSString_Free(_arVariableName);
			return result;
		}

		__declspec(dllexport) TESForm * GetBaseObject(TESObjectREFR * self) {
			return NativeInvoke::Invoke<TESForm *>("ObjectReference", "GetBaseObject", self);
		}

		__declspec(dllexport) int GetCurrentDestructionStage(TESObjectREFR * self) {
			return NativeInvoke::Invoke<int>("ObjectReference", "GetCurrentDestructionStage", self);
		}

		__declspec(dllexport) BGSLocation * GetCurrentLocation(TESObjectREFR * self) {
			return NativeInvoke::Invoke<BGSLocation *>("ObjectReference", "GetCurrentLocation", self);
		}

		__declspec(dllexport) BGSScene * GetCurrentScene(TESObjectREFR * self) {
			return NativeInvoke::Invoke<BGSScene *>("ObjectReference", "GetCurrentScene", self);
		}

		__declspec(dllexport) float GetDistance(TESObjectREFR * self, TESObjectREFR * akOther) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetDistance", self, akOther);
		}

		__declspec(dllexport) BGSLocation * GetEditorLocation(TESObjectREFR * self) {
			return NativeInvoke::Invoke<BGSLocation *>("ObjectReference", "GetEditorLocation", self);
		}

		/*__declspec(dllexport) TESFaction * GetFactionOwner(TESObjectREFR * self) {
			return NativeInvoke::Invoke<TESFaction *>("ObjectReference", "GetFactionOwner", self);
		}*/

		__declspec(dllexport) float GetHeadingAngle(TESObjectREFR * self, TESObjectREFR * akOther) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetHeadingAngle", self, akOther);
		}

		__declspec(dllexport) float GetHeight(TESObjectREFR * self) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetHeight", self);
		}

		__declspec(dllexport) uint GetItemCount(TESObjectREFR * self, TESForm * akItem) {
			return NativeInvoke::Invoke<uint>("ObjectReference", "GetItemCount", self, akItem);
		}

		__declspec(dllexport) float GetItemHealthPercent(TESObjectREFR * self) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetItemHealthPercent", self);
		}

		__declspec(dllexport) TESKey * GetKey(TESObjectREFR * self) {
			return NativeInvoke::Invoke<TESKey *>("ObjectReference", "GetKey", self);
		}

		__declspec(dllexport) float GetLength(TESObjectREFR * self) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetLength", self);
		}

		__declspec(dllexport) TESObjectREFR * GetLinkedRef(TESObjectREFR * self, BGSKeyword * apKeyword) {
			return NativeInvoke::Invoke<TESObjectREFR *>("ObjectReference", "GetLinkedRef", self, apKeyword);
		}

		__declspec(dllexport) int GetLockLevel(TESObjectREFR * self) {
			return NativeInvoke::Invoke<int>("ObjectReference", "GetLockLevel", self);
		}

		__declspec(dllexport) float GetMass(TESObjectREFR * self) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetMass", self);
		}

		__declspec(dllexport) TESObjectREFR * GetNthLinkedRef(TESObjectREFR * self, int aiLinkedRef) {
			return NativeInvoke::Invoke<TESObjectREFR *>("ObjectReference", "GetNthLinkedRef", self, aiLinkedRef);
		}

		__declspec(dllexport) uint GetOpenState(TESObjectREFR * self) {
			return NativeInvoke::Invoke<uint>("ObjectReference", "GetOpenState", self);
		}

		__declspec(dllexport) TESObjectCELL * GetParentCell(TESObjectREFR * self) {
			return NativeInvoke::Invoke<TESObjectCELL *>("ObjectReference", "GetParentCell", self);
		}

		__declspec(dllexport) float GetPositionX(TESObjectREFR * self) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetPositionX", self);
		}

		__declspec(dllexport) float GetPositionY(TESObjectREFR * self) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetPositionY", self);
		}

		__declspec(dllexport) float GetPositionZ(TESObjectREFR * self) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetPositionZ", self);
		}

		__declspec(dllexport) float GetScale(TESObjectREFR * self) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetScale", self);
		}

		__declspec(dllexport) uint GetTriggerObjectCount(TESObjectREFR * self) {
			return NativeInvoke::Invoke<uint>("ObjectReference", "GetTriggerObjectCount", self);
		}

		__declspec(dllexport) BGSVoiceType * GetVoiceType(TESObjectREFR * self) {
			return NativeInvoke::Invoke<BGSVoiceType *>("ObjectReference", "GetVoiceType", self);
		}

		__declspec(dllexport) float GetWidth(TESObjectREFR * self) {
			return NativeInvoke::Invoke<float>("ObjectReference", "GetWidth", self);
		}

		__declspec(dllexport) TESWorldSpace * GetWorldSpace(TESObjectREFR * self) {
			return NativeInvoke::Invoke<TESWorldSpace *>("ObjectReference", "GetWorldSpace", self);
		}

		__declspec(dllexport) bool HasEffectKeyword(TESObjectREFR * self, BGSKeyword * akKeyword) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "HasEffectKeyword", self, akKeyword);
		}

		__declspec(dllexport) bool HasNode(TESObjectREFR * self, char * asNodeName) {
			char *_asNodeName = BSString_Create(asNodeName);
			bool result = NativeInvoke::Invoke<bool>("ObjectReference", "HasNode", self, &_asNodeName);
			BSString_Free(_asNodeName);
			return result;
		}

		/*__declspec(dllexport) bool HasRefType(TESObjectREFR * self, BGSLocation * akRefType) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "HasRefType", self, akRefType);
		}*/

		__declspec(dllexport) void IgnoreFriendlyHits(TESObjectREFR * self, bool abIgnore) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "IgnoreFriendlyHits", self, abIgnore);
		}

		__declspec(dllexport) void InterruptCast(TESObjectREFR * self) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "InterruptCast", self);
		}

		__declspec(dllexport) bool Is3DLoaded(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "Is3DLoaded", self);
		}

		__declspec(dllexport) bool IsActivateChild(TESObjectREFR * self, TESObjectREFR * akChild) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "IsActivateChild", self, akChild);
		}

		__declspec(dllexport) bool IsActivationBlocked(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "IsActivationBlocked", self);
		}

		__declspec(dllexport) bool IsDisabled(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "IsDisabled", self);
		}

		__declspec(dllexport) bool IsFurnitureInUse(TESObjectREFR * self, bool abIgnoreReserved) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "IsFurnitureInUse", self, abIgnoreReserved);
		}

		__declspec(dllexport) bool IsFurnitureMarkerInUse(TESObjectREFR * self, uint aiMarker, bool abIgnoreReserved) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "IsFurnitureMarkerInUse", self, aiMarker, abIgnoreReserved);
		}

		__declspec(dllexport) bool IsIgnoringFriendlyHits(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "IsIgnoringFriendlyHits", self);
		}

		__declspec(dllexport) bool IsInDialogueWithPlayer(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "IsInDialogueWithPlayer", self);
		}

		__declspec(dllexport) bool IsLockBroken(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "IsLockBroken", self);
		}

		__declspec(dllexport) bool IsLocked(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "IsLocked", self);
		}

		__declspec(dllexport) bool IsMapMarkerVisible(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "IsMapMarkerVisible", self);
		}

		__declspec(dllexport) void KnockAreaEffect(TESObjectREFR * self, float afMagnitude, float afRadius) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "KnockAreaEffect", self, afMagnitude, afRadius);
		}

		__declspec(dllexport) void Lock(TESObjectREFR * self, bool abLock, bool abAsOwner) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "Lock", self, abLock, abAsOwner);
		}

		__declspec(dllexport) bool MoveTo(TESObjectREFR * self, TESObjectREFR * akTarget, float afXOffset, float afYOffset, float afZOffset, bool abMatchRotation) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "MoveTo", self, akTarget, afXOffset, afYOffset, afZOffset, abMatchRotation);
		}

		__declspec(dllexport) bool MoveToInteractionLocation(TESObjectREFR * self, TESObjectREFR * akTarget) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "MoveToInteractionLocation", self, akTarget);
		}

		__declspec(dllexport) bool MoveToMyEditorLocation(TESObjectREFR * self) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "MoveToMyEditorLocation", self);
		}

		__declspec(dllexport) bool MoveToNode(TESObjectREFR * self, TESObjectREFR * akTarget, char * asNodeName) {
			char *_asNodeName = BSString_Create(asNodeName);
			bool result = NativeInvoke::Invoke<bool>("ObjectReference", "MoveToNode", self, akTarget, &_asNodeName);
			BSString_Free(_asNodeName);
			return result;
		}

		__declspec(dllexport) TESObjectREFR * PlaceAtMe(TESObjectREFR * self, TESForm * akFormToPlace, uint aiCount, bool abForcePersist, bool abInitiallyDisabled) {
			return NativeInvoke::Invoke<TESObjectREFR *>("ObjectReference", "PlaceAtMe", self, akFormToPlace, aiCount, abForcePersist, abInitiallyDisabled);
		}

		__declspec(dllexport) CActor * PlaceActorAtMe(TESObjectREFR * self, TESNPC * akActorToPlace, uint aiLevelMod, BGSEncounterZone * akZone) {
			return NativeInvoke::Invoke<CActor *>("ObjectReference", "PlaceActorAtMe", self, akActorToPlace, aiLevelMod, akZone);
		}

		__declspec(dllexport) bool PlayAnimation(TESObjectREFR * self, char * asAnimation) {
			char *_asAnimation = BSString_Create(asAnimation);
			bool result = NativeInvoke::Invoke<bool>("ObjectReference", "PlayAnimation", self, &_asAnimation);
			BSString_Free(_asAnimation);
			return result;
		}

		__declspec(dllexport) bool PlayAnimationAndWait(TESObjectREFR * self, char * asAnimation, char * asEventName) {
			char *_asAnimation = BSString_Create(asAnimation);
			char *_asEventName = BSString_Create(asEventName);
			bool result = NativeInvoke::Invoke<bool>("ObjectReference", "PlayAnimationAndWait", self, &_asAnimation, &_asEventName);
			BSString_Free(_asAnimation);
			BSString_Free(_asEventName);
			return result;
		}

		__declspec(dllexport) bool PlayGamebryoAnimation(TESObjectREFR * self, char * asAnimation, bool abStartOver, float afEaseInTime) {
			char *_asAnimation = BSString_Create(asAnimation);
			bool result = NativeInvoke::Invoke<bool>("ObjectReference", "PlayGamebryoAnimation", self, &_asAnimation, abStartOver, afEaseInTime);
			BSString_Free(_asAnimation);
			return result;
		}

		__declspec(dllexport) bool PlayImpactEffect(TESObjectREFR * self, BGSImpactDataSet * akImpactEffect, char * asNodeName, float afPickDirX, float afPickDirY, float afPickDirZ, float afPickLength, bool abApplyNodeRotation, bool abUseNodeLocalRotation) {
			char *_asNodeName = BSString_Create(asNodeName);
			bool result = NativeInvoke::Invoke<bool>("ObjectReference", "PlayImpactEffect", self, akImpactEffect, &_asNodeName, afPickDirX, afPickDirY, afPickDirZ, afPickLength, abApplyNodeRotation, abUseNodeLocalRotation);
			BSString_Free(_asNodeName);
			return result;
		}

		__declspec(dllexport) bool PlaySyncedAnimationSS(TESObjectREFR * self, char * asAnimation1, TESObjectREFR * akObj2, char * asAnimation2) {
			char *_asAnimation1 = BSString_Create(asAnimation1);
			char *_asAnimation2 = BSString_Create(asAnimation2);
			bool result = NativeInvoke::Invoke<bool>("ObjectReference", "PlaySyncedAnimationSS", self, &_asAnimation1, akObj2, &_asAnimation2);
			BSString_Free(_asAnimation1);
			BSString_Free(_asAnimation2);
			return result;
		}

		__declspec(dllexport) bool PlaySyncedAnimationAndWaitSS(TESObjectREFR * self, char * asAnimation1, char * asEvent1, TESObjectREFR * akObj2, char * asAnimation2, char * asEvent2) {
			char *_asAnimation1 = BSString_Create(asAnimation1);
			char *_asEvent1 = BSString_Create(asEvent1);
			char *_asAnimation2 = BSString_Create(asAnimation2);
			char *_asEvent2 = BSString_Create(asEvent2);
			bool result = NativeInvoke::Invoke<bool>("ObjectReference", "PlaySyncedAnimationAndWaitSS", self, &_asAnimation1, &_asEvent1, akObj2, &_asAnimation2, &_asEvent2);
			BSString_Free(_asAnimation1);
			BSString_Free(_asEvent1);
			BSString_Free(_asAnimation2);
			BSString_Free(_asEvent2);
			return result;
		}

		__declspec(dllexport) void PlayTerrainEffect(TESObjectREFR * self, char * asEffectModelName, char * asAttachBoneName) {
			char *_asEffectModelName = BSString_Create(asEffectModelName);
			char *_asAttachBoneName = BSString_Create(asAttachBoneName);
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "PlayTerrainEffect", self, &_asEffectModelName, &_asAttachBoneName);
			BSString_Free(_asEffectModelName);
			BSString_Free(_asAttachBoneName);
		}

		__declspec(dllexport) void ProcessTrapHit(TESObjectREFR * self, TESObjectREFR * akTrap, float afDamage, float afPushback, float afXVel, float afYVel, float afZVel, float afXPos, float afYPos, float afZPos, int aeMaterial, float afStagger) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "ProcessTrapHit", self, akTrap, afDamage, afPushback, afXVel, afYVel, afZVel, afXPos, afYPos, afZPos, aeMaterial, afStagger);
		}

		__declspec(dllexport) void PushActorAway(TESObjectREFR * self, CActor * akActorToPush, float aiKnockbackForce) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "PushActorAway", self, akActorToPush, aiKnockbackForce);
		}

		__declspec(dllexport) void RemoveAllInventoryEventFilters(TESObjectREFR * self) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "RemoveAllInventoryEventFilters", self);
		}

		__declspec(dllexport) void RemoveAllItems(TESObjectREFR * self, TESObjectREFR * akTransferTo, bool abKeepOwnership, bool abRemoveQuestItems) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "RemoveAllItems", self, akTransferTo, abKeepOwnership, abRemoveQuestItems);
		}

		__declspec(dllexport) bool RemoveDependentAnimatedObjectReference(TESObjectREFR * self, TESObjectREFR * akDependent) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "RemoveDependentAnimatedObjectReference", self, akDependent);
		}

		__declspec(dllexport) void RemoveInventoryEventFilter(TESObjectREFR * self, TESForm * akFilter) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "RemoveInventoryEventFilter", self, akFilter);
		}

		__declspec(dllexport) void RemoveItem(TESObjectREFR * self, TESForm * akItemToRemove, uint aiCount, bool abSilent, TESObjectREFR * akOtherContainer) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "RemoveItem", self, akItemToRemove, aiCount, abSilent, akOtherContainer);
		}

		/*__declspec(dllexport) bool Reset(TESObjectREFR * self, TESObjectREFR * akTarget) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "Reset", self, akTarget);
		}*/

		__declspec(dllexport) void Say(TESObjectREFR * self, TESTopic * akTopicToSay, CActor * akActorToSpeakAs, bool abSpeakInPlayersHead) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "Say", self, akTopicToSay, akActorToSpeakAs, abSpeakInPlayersHead);
		}

		__declspec(dllexport) void SendStealAlarm(TESObjectREFR * self, CActor * akThief) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SendStealAlarm", self, akThief);
		}

		__declspec(dllexport) void SetActorCause(TESObjectREFR * self, CActor * akActor) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SetActorCause", self, akActor);
		}

		/*__declspec(dllexport) void SetActorOwner(TESObjectREFR * self, TESNPC * akActorBase) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SetActorOwner", self, akActorBase);
		}*/

		__declspec(dllexport) bool SetAngle(TESObjectREFR * self, float afXAngle, float afYAngle, float afZAngle) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "SetAngle", self, afXAngle, afYAngle, afZAngle);
		}

		__declspec(dllexport) void SetAnimationVariableBool(TESObjectREFR * self, char * arVariableName, bool abNewValue) {
			char *_arVariableName = BSString_Create(arVariableName);
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SetAnimationVariableBool", self, &_arVariableName, abNewValue);
			BSString_Free(_arVariableName);
		}

		__declspec(dllexport) void SetAnimationVariableInt(TESObjectREFR * self, char * arVariableName, int aiNewValue) {
			char *_arVariableName = BSString_Create(arVariableName);
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SetAnimationVariableInt", self, &_arVariableName, aiNewValue);
			BSString_Free(_arVariableName);
		}

		__declspec(dllexport) void SetAnimationVariableFloat(TESObjectREFR * self, char * arVariableName, float afNewValue) {
			char *_arVariableName = BSString_Create(arVariableName);
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SetAnimationVariableFloat", self, &_arVariableName, afNewValue);
			BSString_Free(_arVariableName);
		}

		__declspec(dllexport) void SetDestroyed(TESObjectREFR * self, bool abDestroyed) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SetDestroyed", self, abDestroyed);
		}

		__declspec(dllexport) bool SetScale(TESObjectREFR * self, float afScale) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "SetScale", self, afScale);
		}

		/*__declspec(dllexport) void SetFactionOwner(TESObjectREFR * self, TESFaction * akFaction) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SetFactionOwner", self, akFaction);
		}*/

		__declspec(dllexport) void SetLockLevel(TESObjectREFR * self, uint aiLockLevel) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SetLockLevel", self, aiLockLevel);
		}

		__declspec(dllexport) bool SetMotionType(TESObjectREFR * self, uint aeMotionType, bool abAllowActivate) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "SetMotionType", self, aeMotionType, abAllowActivate);
		}

		__declspec(dllexport) void SetNoFavorAllowed(TESObjectREFR * self, bool abNoFavor) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SetNoFavorAllowed", self, abNoFavor);
		}

		__declspec(dllexport) void SetOpen(TESObjectREFR * self, bool abOpen) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SetOpen", self, abOpen);
		}

		__declspec(dllexport) bool SetPosition(TESObjectREFR * self, float afX, float afY, float afZ) {
			return NativeInvoke::Invoke<bool>("ObjectReference", "SetPosition", self, afX, afY, afZ);
		}

		__declspec(dllexport) void TranslateTo(TESObjectREFR * self, float afX, float afY, float afZ, float afXAngle, float afYAngle, float afZAngle, float afSpeed, float afMaxRotationSpeed) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "TranslateTo", self, afX, afY, afZ, afXAngle, afYAngle, afZAngle, afSpeed, afMaxRotationSpeed);
		}

		__declspec(dllexport) void SplineTranslateTo(TESObjectREFR * self, float afX, float afY, float afZ, float afXAngle, float afYAngle, float afZAngle, float afTangentMagnitude, float afSpeed, float afMaxRotationSpeed) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SplineTranslateTo", self, afX, afY, afZ, afXAngle, afYAngle, afZAngle, afTangentMagnitude, afSpeed, afMaxRotationSpeed);
		}

		__declspec(dllexport) void SplineTranslateToRefNode(TESObjectREFR * self, TESObjectREFR * arTarget, char * arNodeName, float afTangentMagnitude, float afSpeed, float afMaxRotationSpeed) {
			char *_arNodeName = BSString_Create(arNodeName);
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "SplineTranslateToRefNode", self, arTarget, &_arNodeName, afTangentMagnitude, afSpeed, afMaxRotationSpeed);
			BSString_Free(_arNodeName);
		}

		__declspec(dllexport) void StopTranslation(TESObjectREFR * self) {
			NativeInvoke::Invoke<ScriptNone>("ObjectReference", "StopTranslation", self);
		}

		__declspec(dllexport) bool WaitForAnimationEvent(TESObjectREFR * self, char * asEventName) {
			char *_asEventName = BSString_Create(asEventName);
			bool result = NativeInvoke::Invoke<bool>("ObjectReference", "WaitForAnimationEvent", self, &_asEventName);
			BSString_Free(_asEventName);
			return result;
		}

		__declspec(dllexport) void Fire(TESObjectWEAP * self, TESObjectREFR * akSource, TESAmmo * akAmmo) {
			NativeInvoke::Invoke<ScriptNone>("Weapon", "Fire", self, akSource, akAmmo);
		}

		/*__declspec(dllexport) TESQuest * GetOwningQuest(TESPackage * self) {
			return NativeInvoke::Invoke<TESQuest *>("Package", "GetOwningQuest", self);
		}*/

		__declspec(dllexport) TESPackage * GetTemplate(TESPackage * self) {
			return NativeInvoke::Invoke<TESPackage *>("Package", "GetTemplate", self);
		}

		__declspec(dllexport) void CompleteAllObjectives(TESQuest * self) {
			NativeInvoke::Invoke<ScriptNone>("Quest", "CompleteAllObjectives", self);
		}

		__declspec(dllexport) void CompleteQuest(TESQuest * self) {
			NativeInvoke::Invoke<ScriptNone>("Quest", "CompleteQuest", self);
		}

		__declspec(dllexport) void FailAllObjectives(TESQuest * self) {
			NativeInvoke::Invoke<ScriptNone>("Quest", "FailAllObjectives", self);
		}

		__declspec(dllexport) BGSBaseAlias * GetAlias(TESQuest * self, uint aiAliasID) {
			return NativeInvoke::Invoke<BGSBaseAlias *>("Quest", "GetAlias", self, aiAliasID);
		}

		__declspec(dllexport) uint GetCurrentStageID(TESQuest * self) {
			return NativeInvoke::Invoke<uint>("Quest", "GetCurrentStageID", self);
		}

		__declspec(dllexport) bool IsActive(TESQuest * self) {
			return NativeInvoke::Invoke<bool>("Quest", "IsActive", self);
		}

		__declspec(dllexport) bool IsCompleted(TESQuest * self) {
			return NativeInvoke::Invoke<bool>("Quest", "IsCompleted", self);
		}

		__declspec(dllexport) bool IsObjectiveCompleted(TESQuest * self, uint aiObjective) {
			return NativeInvoke::Invoke<bool>("Quest", "IsObjectiveCompleted", self, aiObjective);
		}

		__declspec(dllexport) bool IsObjectiveDisplayed(TESQuest * self, uint aiObjective) {
			return NativeInvoke::Invoke<bool>("Quest", "IsObjectiveDisplayed", self, aiObjective);
		}

		__declspec(dllexport) bool IsObjectiveFailed(TESQuest * self, uint aiObjective) {
			return NativeInvoke::Invoke<bool>("Quest", "IsObjectiveFailed", self, aiObjective);
		}

		__declspec(dllexport) bool IsRunning(TESQuest * self) {
			return NativeInvoke::Invoke<bool>("Quest", "IsRunning", self);
		}

		__declspec(dllexport) bool IsStageDone(TESQuest * self, uint aiStage) {
			return NativeInvoke::Invoke<bool>("Quest", "IsStageDone", self, aiStage);
		}

		__declspec(dllexport) bool IsStarting(TESQuest * self) {
			return NativeInvoke::Invoke<bool>("Quest", "IsStarting", self);
		}

		__declspec(dllexport) bool IsStopping(TESQuest * self) {
			return NativeInvoke::Invoke<bool>("Quest", "IsStopping", self);
		}

		__declspec(dllexport) bool IsStopped(TESQuest * self) {
			return NativeInvoke::Invoke<bool>("Quest", "IsStopped", self);
		}

		/*__declspec(dllexport) void Reset(TESQuest * self) {
			NativeInvoke::Invoke<ScriptNone>("Quest", "Reset", self);
		}*/

		__declspec(dllexport) void SetActive(TESQuest * self, bool abActive) {
			NativeInvoke::Invoke<ScriptNone>("Quest", "SetActive", self, abActive);
		}

		__declspec(dllexport) bool SetCurrentStageID(TESQuest * self, uint aiStageID) {
			return NativeInvoke::Invoke<bool>("Quest", "SetCurrentStageID", self, aiStageID);
		}

		__declspec(dllexport) void SetObjectiveCompleted(TESQuest * self, uint aiObjective, bool abCompleted) {
			NativeInvoke::Invoke<ScriptNone>("Quest", "SetObjectiveCompleted", self, aiObjective, abCompleted);
		}

		__declspec(dllexport) void SetObjectiveDisplayed(TESQuest * self, uint aiObjective, bool abDisplayed, bool abForce) {
			NativeInvoke::Invoke<ScriptNone>("Quest", "SetObjectiveDisplayed", self, aiObjective, abDisplayed, abForce);
		}

		__declspec(dllexport) void SetObjectiveFailed(TESQuest * self, uint aiObjective, bool abFailed) {
			NativeInvoke::Invoke<ScriptNone>("Quest", "SetObjectiveFailed", self, aiObjective, abFailed);
		}

		/*__declspec(dllexport) bool Start(TESQuest * self) {
			return NativeInvoke::Invoke<bool>("Quest", "Start", self);
		}*/

		/*__declspec(dllexport) void Stop(TESQuest * self) {
			NativeInvoke::Invoke<ScriptNone>("Quest", "Stop", self);
		}*/

		__declspec(dllexport) bool UpdateCurrentInstanceGlobal(TESQuest * self, TESGlobal * aUpdateGlobal) {
			return NativeInvoke::Invoke<bool>("Quest", "UpdateCurrentInstanceGlobal", self, aUpdateGlobal);
		}

		/*__declspec(dllexport) uint Play(TESSound * self, TESObjectREFR * akSource) {
			return NativeInvoke::Invoke<uint>("Sound", "Play", self, akSource);
		}*/

		__declspec(dllexport) bool PlayAndWait(TESSound * self, TESObjectREFR * akSource) {
			return NativeInvoke::Invoke<bool>("Sound", "PlayAndWait", self, akSource);
		}

		__declspec(dllexport) void StopInstance(uint aiPlaybackInstance) {
			NativeInvoke::Invoke<ScriptNone>("Sound", "StopInstance", BSScript_StaticFunctionTag, aiPlaybackInstance);
		}

		__declspec(dllexport) void SetInstanceVolume(uint aiPlaybackInstance, float afVolume) {
			NativeInvoke::Invoke<ScriptNone>("Sound", "SetInstanceVolume", BSScript_StaticFunctionTag, aiPlaybackInstance, afVolume);
		}

		/*__declspec(dllexport) void Add(TESTopic * self) {
			NativeInvoke::Invoke<ScriptNone>("Topic", "Add", self);
		}*/

		/*__declspec(dllexport) TESQuest * GetOwningQuest(TESTopic * self) {
			return NativeInvoke::Invoke<TESQuest *>("TopicInfo", "GetOwningQuest", self);
		}*/

		__declspec(dllexport) void AddPerk(CActor * self, BGSPerk * akPerk) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "AddPerk", self, akPerk);
		}

		__declspec(dllexport) bool AddShout(CActor * self, TESShout * akShout) {
			return NativeInvoke::Invoke<bool>("Actor", "AddShout", self, akShout);
		}

		__declspec(dllexport) bool AddSpell(CActor * self, SpellItem * akSpell, bool abVerbose) {
			return NativeInvoke::Invoke<bool>("Actor", "AddSpell", self, akSpell, abVerbose);
		}

		__declspec(dllexport) void AllowBleedoutDialogue(CActor * self, bool abCanTalk) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "AllowBleedoutDialogue", self, abCanTalk);
		}

		__declspec(dllexport) void AllowPCDialogue(CActor * self, bool abTalk) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "AllowPCDialogue", self, abTalk);
		}

		__declspec(dllexport) void AttachAshPile(CActor * self, TESForm * akAshPileBase) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "AttachAshPile", self, akAshPileBase);
		}

		__declspec(dllexport) void ClearArrested(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "ClearArrested", self);
		}

		__declspec(dllexport) void ClearExtraArrows(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "ClearExtraArrows", self);
		}

		__declspec(dllexport) void ClearKeepOffsetFromActor(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "ClearKeepOffsetFromActor", self);
		}

		__declspec(dllexport) void ClearLookAt(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "ClearLookAt", self);
		}

		__declspec(dllexport) void DamageActorValue(CActor * self, char * asValueName, float afDamage) {
			char *_asValueName = BSString_Create(asValueName);
			NativeInvoke::Invoke<ScriptNone>("Actor", "DamageActorValue", self, &_asValueName, afDamage);
			BSString_Free(_asValueName);
		}

		__declspec(dllexport) void DispelAllSpells(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "DispelAllSpells", self);
		}

		__declspec(dllexport) bool DispelSpell(CActor * self, SpellItem * akSpell) {
			return NativeInvoke::Invoke<bool>("Actor", "DispelSpell", self, akSpell);
		}

		__declspec(dllexport) void DoCombatSpellApply(CActor * self, SpellItem * akSpell, TESObjectREFR * akTarget) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "DoCombatSpellApply", self, akSpell, akTarget);
		}

		__declspec(dllexport) void EnableAI(CActor * self, bool abEnable) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "EnableAI", self, abEnable);
		}

		__declspec(dllexport) void EquipItem(CActor * self, TESForm * akItem, bool abPreventRemoval, bool abSilent) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "EquipItem", self, akItem, abPreventRemoval, abSilent);
		}

		__declspec(dllexport) void EquipShout(CActor * self, TESShout * akShout) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "EquipShout", self, akShout);
		}

		__declspec(dllexport) void EquipSpell(CActor * self, SpellItem * akSpell, uint aiSource) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "EquipSpell", self, akSpell, aiSource);
		}

		__declspec(dllexport) void EvaluatePackage(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "EvaluatePackage", self);
		}

		__declspec(dllexport) void ForceActorValue(CActor * self, char * asValueName, float afNewValue) {
			char *_asValueName = BSString_Create(asValueName);
			NativeInvoke::Invoke<ScriptNone>("Actor", "ForceActorValue", self, &_asValueName, afNewValue);
			BSString_Free(_asValueName);
		}

		__declspec(dllexport) float GetActorValue(CActor * self, char * asValueName) {
			char *_asValueName = BSString_Create(asValueName);
			float result = NativeInvoke::Invoke<float>("Actor", "GetActorValue", self, &_asValueName);
			BSString_Free(_asValueName);
			return result;
		}

		__declspec(dllexport) float GetActorValuePercentage(CActor * self, char * asValueName) {
			char *_asValueName = BSString_Create(asValueName);
			float result = NativeInvoke::Invoke<float>("Actor", "GetActorValuePercentage", self, &_asValueName);
			BSString_Free(_asValueName);
			return result;
		}

		__declspec(dllexport) float GetBaseActorValue(CActor * self, char * asValueName) {
			char *_asValueName = BSString_Create(asValueName);
			float result = NativeInvoke::Invoke<float>("Actor", "GetBaseActorValue", self, &_asValueName);
			BSString_Free(_asValueName);
			return result;
		}

		__declspec(dllexport) uint GetBribeAmount(CActor * self) {
			return NativeInvoke::Invoke<uint>("Actor", "GetBribeAmount", self);
		}

		__declspec(dllexport) TESFaction * GetCrimeFaction(CActor * self) {
			return NativeInvoke::Invoke<TESFaction *>("Actor", "GetCrimeFaction", self);
		}

		__declspec(dllexport) uint GetCombatState(CActor * self) {
			return NativeInvoke::Invoke<uint>("Actor", "GetCombatState", self);
		}

		__declspec(dllexport) CActor * GetCombatTarget(CActor * self) {
			return NativeInvoke::Invoke<CActor *>("Actor", "GetCombatTarget", self);
		}

		__declspec(dllexport) TESPackage * GetCurrentPackage(CActor * self) {
			return NativeInvoke::Invoke<TESPackage *>("Actor", "GetCurrentPackage", self);
		}

		__declspec(dllexport) CActor * GetDialogueTarget(CActor * self) {
			return NativeInvoke::Invoke<CActor *>("Actor", "GetDialogueTarget", self);
		}

		__declspec(dllexport) TESShout * GetEquippedShout(CActor * self) {
			return NativeInvoke::Invoke<TESShout *>("Actor", "GetEquippedShout", self);
		}

		__declspec(dllexport) SpellItem * GetEquippedSpell(CActor * self, uint aiSource) {
			return NativeInvoke::Invoke<SpellItem *>("Actor", "GetEquippedSpell", self, aiSource);
		}

		__declspec(dllexport) TESObjectWEAP * GetEquippedWeapon(CActor * self, bool abLeftHand) {
			return NativeInvoke::Invoke<TESObjectWEAP *>("Actor", "GetEquippedWeapon", self, abLeftHand);
		}

		__declspec(dllexport) TESObjectARMO * GetEquippedShield(CActor * self) {
			return NativeInvoke::Invoke<TESObjectARMO *>("Actor", "GetEquippedShield", self);
		}

		__declspec(dllexport) int GetEquippedItemType(CActor * self, uint aiHand) {
			return NativeInvoke::Invoke<int>("Actor", "GetEquippedItemType", self, aiHand);
		}

		__declspec(dllexport) int GetFactionRank(CActor * self, TESFaction * akFaction) {
			return NativeInvoke::Invoke<int>("Actor", "GetFactionRank", self, akFaction);
		}

		__declspec(dllexport) uint GetFactionReaction(CActor * self, CActor * akOther) {
			return NativeInvoke::Invoke<uint>("Actor", "GetFactionReaction", self, akOther);
		}

		__declspec(dllexport) uint GetFlyingState(CActor * self) {
			return NativeInvoke::Invoke<uint>("Actor", "GetFlyingState", self);
		}

		__declspec(dllexport) int GetGoldAmount(CActor * self) {
			return NativeInvoke::Invoke<int>("Actor", "GetGoldAmount", self);
		}

		__declspec(dllexport) int GetHighestRelationshipRank(CActor * self) {
			return NativeInvoke::Invoke<int>("Actor", "GetHighestRelationshipRank", self);
		}

		__declspec(dllexport) CActor * GetKiller(CActor * self) {
			return NativeInvoke::Invoke<CActor *>("Actor", "GetKiller", self);
		}

		__declspec(dllexport) TESObjectREFR * GetForcedLandingMarker(CActor * self) {
			return NativeInvoke::Invoke<TESObjectREFR *>("Actor", "GetForcedLandingMarker", self);
		}

		__declspec(dllexport) uint GetLevel(CActor * self) {
			return NativeInvoke::Invoke<uint>("Actor", "GetLevel", self);
		}

		__declspec(dllexport) TESNPC * GetLeveledActorBase(CActor * self) {
			return NativeInvoke::Invoke<TESNPC *>("Actor", "GetLeveledActorBase", self);
		}

		__declspec(dllexport) float GetLightLevel(CActor * self) {
			return NativeInvoke::Invoke<float>("Actor", "GetLightLevel", self);
		}

		__declspec(dllexport) int GetLowestRelationshipRank(CActor * self) {
			return NativeInvoke::Invoke<int>("Actor", "GetLowestRelationshipRank", self);
		}

		__declspec(dllexport) bool GetNoBleedoutRecovery(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "GetNoBleedoutRecovery", self);
		}

		__declspec(dllexport) bool GetPlayerControls(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "GetPlayerControls", self);
		}

		/*__declspec(dllexport) TESRace * GetRace(CActor * self) {
			return NativeInvoke::Invoke<TESRace *>("Actor", "GetRace", self);
		}*/

		__declspec(dllexport) int GetRelationshipRank(CActor * self, CActor * akOther) {
			return NativeInvoke::Invoke<int>("Actor", "GetRelationshipRank", self, akOther);
		}

		__declspec(dllexport) uint GetSitState(CActor * self) {
			return NativeInvoke::Invoke<uint>("Actor", "GetSitState", self);
		}

		__declspec(dllexport) uint GetSleepState(CActor * self) {
			return NativeInvoke::Invoke<uint>("Actor", "GetSleepState", self);
		}

		__declspec(dllexport) float GetVoiceRecoveryTime(CActor * self) {
			return NativeInvoke::Invoke<float>("Actor", "GetVoiceRecoveryTime", self);
		}

		__declspec(dllexport) bool HasAssociation(CActor * self, BGSAssociationType * akAssociation, CActor * akOther) {
			return NativeInvoke::Invoke<bool>("Actor", "HasAssociation", self, akAssociation, akOther);
		}

		__declspec(dllexport) bool HasFamilyRelationship(CActor * self, CActor * akOther) {
			return NativeInvoke::Invoke<bool>("Actor", "HasFamilyRelationship", self, akOther);
		}

		__declspec(dllexport) bool HasLOS(CActor * self, TESObjectREFR * akOther) {
			return NativeInvoke::Invoke<bool>("Actor", "HasLOS", self, akOther);
		}

		__declspec(dllexport) bool HasMagicEffect(CActor * self, EffectSetting * akEffect) {
			return NativeInvoke::Invoke<bool>("Actor", "HasMagicEffect", self, akEffect);
		}

		__declspec(dllexport) bool HasMagicEffectWithKeyword(CActor * self, BGSKeyword * akKeyword) {
			return NativeInvoke::Invoke<bool>("Actor", "HasMagicEffectWithKeyword", self, akKeyword);
		}

		__declspec(dllexport) bool HasParentRelationship(CActor * self, CActor * akOther) {
			return NativeInvoke::Invoke<bool>("Actor", "HasParentRelationship", self, akOther);
		}

		__declspec(dllexport) bool HasPerk(CActor * self, BGSPerk * akPerk) {
			return NativeInvoke::Invoke<bool>("Actor", "HasPerk", self, akPerk);
		}

		__declspec(dllexport) bool HasSpell(CActor * self, TESForm * akForm) {
			return NativeInvoke::Invoke<bool>("Actor", "HasSpell", self, akForm);
		}

		__declspec(dllexport) bool IsAlarmed(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsAlarmed", self);
		}

		__declspec(dllexport) bool IsAlerted(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsAlerted", self);
		}

		__declspec(dllexport) bool IsAllowedToFly(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsAllowedToFly", self);
		}

		__declspec(dllexport) bool IsArrested(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsArrested", self);
		}

		__declspec(dllexport) bool IsArrestingTarget(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsArrestingTarget", self);
		}

		__declspec(dllexport) bool IsBleedingOut(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsBleedingOut", self);
		}

		__declspec(dllexport) bool IsBribed(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsBribed", self);
		}

		/*__declspec(dllexport) bool IsChild(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsChild", self);
		}*/

		__declspec(dllexport) bool IsCommandedActor(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsCommandedActor", self);
		}

		__declspec(dllexport) bool IsDead(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsDead", self);
		}

		__declspec(dllexport) bool IsDetectedBy(CActor * self, CActor * akOther) {
			return NativeInvoke::Invoke<bool>("Actor", "IsDetectedBy", self, akOther);
		}

		__declspec(dllexport) bool IsDoingFavor(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsDoingFavor", self);
		}

		__declspec(dllexport) bool IsEquipped(CActor * self, TESForm * akItem) {
			return NativeInvoke::Invoke<bool>("Actor", "IsEquipped", self, akItem);
		}

		/*__declspec(dllexport) bool IsEssential(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsEssential", self);
		}*/

		__declspec(dllexport) bool IsFlying(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsFlying", self);
		}

		__declspec(dllexport) bool IsGhost(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsGhost", self);
		}

		__declspec(dllexport) bool IsGuard(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsGuard", self);
		}

		__declspec(dllexport) bool IsHostileToActor(CActor * self, CActor * akActor) {
			return NativeInvoke::Invoke<bool>("Actor", "IsHostileToActor", self, akActor);
		}

		__declspec(dllexport) bool IsInCombat(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsInCombat", self);
		}

		__declspec(dllexport) bool IsInFaction(CActor * self, TESFaction * akFaction) {
			return NativeInvoke::Invoke<bool>("Actor", "IsInFaction", self, akFaction);
		}

		__declspec(dllexport) bool IsInKillMove(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsInKillMove", self);
		}

		__declspec(dllexport) bool IsIntimidated(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsIntimidated", self);
		}

		__declspec(dllexport) bool IsPlayersLastRiddenHorse(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsPlayersLastRiddenHorse", self);
		}

		__declspec(dllexport) bool IsPlayerTeammate(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsPlayerTeammate", self);
		}

		/*__declspec(dllexport) bool IsRunning(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsRunning", self);
		}*/

		__declspec(dllexport) bool IsSneaking(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsSneaking", self);
		}

		__declspec(dllexport) bool IsSprinting(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsSprinting", self);
		}

		__declspec(dllexport) bool IsTrespassing(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsTrespassing", self);
		}

		__declspec(dllexport) bool IsUnconscious(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsUnconscious", self);
		}

		__declspec(dllexport) bool IsWeaponDrawn(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "IsWeaponDrawn", self);
		}

		__declspec(dllexport) void KeepOffsetFromActor(CActor * self, CActor * arTarget, float afOffsetX, float afOffsetY, float afOffsetZ, float afOffsetAngleX, float afOffsetAngleY, float afOffsetAngleZ, float afCatchUpRadius, float afFollowRadius) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "KeepOffsetFromActor", self, arTarget, afOffsetX, afOffsetY, afOffsetZ, afOffsetAngleX, afOffsetAngleY, afOffsetAngleZ, afCatchUpRadius, afFollowRadius);
		}

		__declspec(dllexport) void KillSilent(CActor * self, CActor * akKiller) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "KillSilent", self, akKiller);
		}

		__declspec(dllexport) void Kill(CActor * self, CActor * akKiller) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "Kill", self, akKiller);
		}

		__declspec(dllexport) void ModActorValue(CActor * self, char * asValueName, float afAmount) {
			char *_asValueName = BSString_Create(asValueName);
			NativeInvoke::Invoke<ScriptNone>("Actor", "ModActorValue", self, &_asValueName, afAmount);
			BSString_Free(_asValueName);
		}

		__declspec(dllexport) void ModFactionRank(CActor * self, TESFaction * akFaction, int aiMod) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "ModFactionRank", self, akFaction, aiMod);
		}

		__declspec(dllexport) bool MoveToPackageLocation(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "MoveToPackageLocation", self);
		}

		__declspec(dllexport) void OpenInventory(CActor * self, bool abForceOpen) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "OpenInventory", self, abForceOpen);
		}

		__declspec(dllexport) bool PathToReference(CActor * self, TESObjectREFR * aTarget, float afWalkRunPercent) {
			return NativeInvoke::Invoke<bool>("Actor", "PathToReference", self, aTarget, afWalkRunPercent);
		}

		__declspec(dllexport) bool PlayIdle(CActor * self, TESIdleForm * akIdle) {
			return NativeInvoke::Invoke<bool>("Actor", "PlayIdle", self, akIdle);
		}

		__declspec(dllexport) bool PlayIdleWithTarget(CActor * self, TESIdleForm * akIdle, TESObjectREFR * akTarget) {
			return NativeInvoke::Invoke<bool>("Actor", "PlayIdleWithTarget", self, akIdle, akTarget);
		}

		__declspec(dllexport) void PlaySubGraphAnimation(CActor * self, char * asEventName) {
			char *_asEventName = BSString_Create(asEventName);
			NativeInvoke::Invoke<ScriptNone>("Actor", "PlaySubGraphAnimation", self, &_asEventName);
			BSString_Free(_asEventName);
		}

		__declspec(dllexport) void RemoveFromFaction(CActor * self, TESFaction * akFaction) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "RemoveFromFaction", self, akFaction);
		}

		__declspec(dllexport) void RemoveFromAllFactions(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "RemoveFromAllFactions", self);
		}

		__declspec(dllexport) void RemovePerk(CActor * self, BGSPerk * akPerk) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "RemovePerk", self, akPerk);
		}

		__declspec(dllexport) bool RemoveShout(CActor * self, TESShout * akShout) {
			return NativeInvoke::Invoke<bool>("Actor", "RemoveShout", self, akShout);
		}

		__declspec(dllexport) bool RemoveSpell(CActor * self, SpellItem * akSpell) {
			return NativeInvoke::Invoke<bool>("Actor", "RemoveSpell", self, akSpell);
		}

		__declspec(dllexport) void ResetHealthAndLimbs(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "ResetHealthAndLimbs", self);
		}

		__declspec(dllexport) bool Resurrect(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "Resurrect", self);
		}

		__declspec(dllexport) void RestoreActorValue(CActor * self, char * asValueName, float afAmount) {
			char *_asValueName = BSString_Create(asValueName);
			NativeInvoke::Invoke<ScriptNone>("Actor", "RestoreActorValue", self, &_asValueName, afAmount);
			BSString_Free(_asValueName);
		}

		__declspec(dllexport) void SendAssaultAlarm(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SendAssaultAlarm", self);
		}

		__declspec(dllexport) void SendTrespassAlarm(CActor * self, CActor * akCriminal) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SendTrespassAlarm", self, akCriminal);
		}

		__declspec(dllexport) void SetActorValue(CActor * self, char * asValueName, float afValue) {
			char *_asValueName = BSString_Create(asValueName);
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetActorValue", self, &_asValueName, afValue);
			BSString_Free(_asValueName);
		}

		__declspec(dllexport) void SetAlert(CActor * self, bool abAlerted) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetAlert", self, abAlerted);
		}

		__declspec(dllexport) void SetAllowFlying(CActor * self, bool abAllowed) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetAllowFlying", self, abAllowed);
		}

		__declspec(dllexport) void SetAlpha(CActor * self, float afTargetAlpha, bool abFade) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetAlpha", self, afTargetAlpha, abFade);
		}

		__declspec(dllexport) void SetAttackActorOnSight(CActor * self, bool abAttackOnSight) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetAttackActorOnSight", self, abAttackOnSight);
		}

		__declspec(dllexport) void SetBribed(CActor * self, bool abBribe) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetBribed", self, abBribe);
		}

		__declspec(dllexport) void SetCrimeFaction(CActor * self, TESFaction * akFaction) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetCrimeFaction", self, akFaction);
		}

		__declspec(dllexport) void SetCriticalStage(CActor * self, uint aiStage) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetCriticalStage", self, aiStage);
		}

		__declspec(dllexport) void SetDoingFavor(CActor * self, bool abDoingFavor) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetDoingFavor", self, abDoingFavor);
		}

		__declspec(dllexport) void SetFactionRank(CActor * self, TESFaction * akFaction, int aiRank) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetFactionRank", self, akFaction, aiRank);
		}

		__declspec(dllexport) void SetGhost(CActor * self, bool abIsGhost) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetGhost", self, abIsGhost);
		}

		__declspec(dllexport) void SetHeadTracking(CActor * self, bool abEnable) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetHeadTracking", self, abEnable);
		}

		__declspec(dllexport) void SetForcedLandingMarker(CActor * self, TESObjectREFR * aMarker) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetForcedLandingMarker", self, aMarker);
		}

		__declspec(dllexport) void SetIntimidated(CActor * self, bool abIntimidate) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetIntimidated", self, abIntimidate);
		}

		__declspec(dllexport) void SetLookAt(CActor * self, TESObjectREFR * akTarget, bool abPathingLookAt) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetLookAt", self, akTarget, abPathingLookAt);
		}

		__declspec(dllexport) void SetNoBleedoutRecovery(CActor * self, bool abAllowed) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetNoBleedoutRecovery", self, abAllowed);
		}

		__declspec(dllexport) void SetNotShowOnStealthMeter(CActor * self, bool abNotShow) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetNotShowOnStealthMeter", self, abNotShow);
		}

		/*__declspec(dllexport) void SetOutfit(CActor * self, BGSOutfit * akOutfit, bool abSleepOutfit) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetOutfit", self, akOutfit, abSleepOutfit);
		}*/

		__declspec(dllexport) void SetPlayerControls(CActor * self, bool abControls) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetPlayerControls", self, abControls);
		}

		__declspec(dllexport) void SetPlayerResistingArrest(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetPlayerResistingArrest", self);
		}

		__declspec(dllexport) void SetPlayerTeammate(CActor * self, bool abTeammate, bool abCanDoFavor) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetPlayerTeammate", self, abTeammate, abCanDoFavor);
		}

		__declspec(dllexport) void SetRace(CActor * self, TESRace * akRace) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetRace", self, akRace);
		}

		__declspec(dllexport) void SetRelationshipRank(CActor * self, CActor * akOther, int aiRank) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetRelationshipRank", self, akOther, aiRank);
		}

		__declspec(dllexport) void SetRestrained(CActor * self, bool abRestrained) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetRestrained", self, abRestrained);
		}

		__declspec(dllexport) void SetUnconscious(CActor * self, bool abUnconscious) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetUnconscious", self, abUnconscious);
		}

		__declspec(dllexport) void SetVehicle(CActor * self, TESObjectREFR * akVehicle) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetVehicle", self, akVehicle);
		}

		__declspec(dllexport) void SetVoiceRecoveryTime(CActor * self, float afTime) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "SetVoiceRecoveryTime", self, afTime);
		}

		__declspec(dllexport) void ShowBarterMenu(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "ShowBarterMenu", self);
		}

		__declspec(dllexport) bool ShowGiftMenu(CActor * self, bool abGivingGift, BGSListForm * apFilterList, bool abShowStolenItems, bool abUseFavorPoints) {
			return NativeInvoke::Invoke<bool>("Actor", "ShowGiftMenu", self, abGivingGift, apFilterList, abShowStolenItems, abUseFavorPoints);
		}

		__declspec(dllexport) void StartCannibal(CActor * self, CActor * akTarget) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "StartCannibal", self, akTarget);
		}

		__declspec(dllexport) void StartCombat(CActor * self, CActor * akTarget) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "StartCombat", self, akTarget);
		}

		__declspec(dllexport) void StartVampireFeed(CActor * self, CActor * akTarget) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "StartVampireFeed", self, akTarget);
		}

		__declspec(dllexport) void StopCombat(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "StopCombat", self);
		}

		__declspec(dllexport) void StopCombatAlarm(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "StopCombatAlarm", self);
		}

		__declspec(dllexport) bool TrapSoul(CActor * self, CActor * akTarget) {
			return NativeInvoke::Invoke<bool>("Actor", "TrapSoul", self, akTarget);
		}

		__declspec(dllexport) void UnequipAll(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "UnequipAll", self);
		}

		__declspec(dllexport) void UnequipItem(CActor * self, TESForm * akItem, bool abPreventEquip, bool abSilent) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "UnequipItem", self, akItem, abPreventEquip, abSilent);
		}

		__declspec(dllexport) void UnequipShout(CActor * self, TESShout * akShout) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "UnequipShout", self, akShout);
		}

		__declspec(dllexport) void UnequipSpell(CActor * self, SpellItem * akSpell, uint aiSource) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "UnequipSpell", self, akSpell, aiSource);
		}

		__declspec(dllexport) void UnlockOwnedDoorsInCell(CActor * self) {
			NativeInvoke::Invoke<ScriptNone>("Actor", "UnlockOwnedDoorsInCell", self);
		}

		__declspec(dllexport) bool WillIntimidateSucceed(CActor * self) {
			return NativeInvoke::Invoke<bool>("Actor", "WillIntimidateSucceed", self);
		}

		__declspec(dllexport) bool WornHasKeyword(CActor * self, BGSKeyword * akKeyword) {
			return NativeInvoke::Invoke<bool>("Actor", "WornHasKeyword", self, akKeyword);
		}
	
		/*__declspec(dllexport) TESQuest * GetOwningQuest(BGSBaseAlias * self) {
			return NativeInvoke::Invoke<TESQuest *>("Alias", "GetOwningQuest", self);
		}*/

		/*__declspec(dllexport) void AddInventoryEventFilter(BGSRefAlias * self, TESForm * akFilter) {
			NativeInvoke::Invoke<ScriptNone>("ReferenceAlias", "AddInventoryEventFilter", self, akFilter);
		}*/

		__declspec(dllexport) void Clear(BGSRefAlias * self) {
			NativeInvoke::Invoke<ScriptNone>("ReferenceAlias", "Clear", self);
		}

		__declspec(dllexport) TESObjectREFR * GetReference(BGSRefAlias * self) {
			return NativeInvoke::Invoke<TESObjectREFR *>("ReferenceAlias", "GetReference", self);
		}

		__declspec(dllexport) void ForceRefTo(BGSRefAlias * self, TESObjectREFR * akNewRef) {
			NativeInvoke::Invoke<ScriptNone>("ReferenceAlias", "ForceRefTo", self, akNewRef);
		}

		/*__declspec(dllexport) void RemoveAllInventoryEventFilters(BGSRefAlias * self) {
			NativeInvoke::Invoke<ScriptNone>("ReferenceAlias", "RemoveAllInventoryEventFilters", self);
		}

		__declspec(dllexport) void RemoveInventoryEventFilter(BGSRefAlias * self, TESForm * akFilter) {
			NativeInvoke::Invoke<ScriptNone>("ReferenceAlias", "RemoveInventoryEventFilter", self, akFilter);
		}*/

		/*__declspec(dllexport) void Clear(BGSLocAlias * self) {
			NativeInvoke::Invoke<ScriptNone>("LocationAlias", "Clear", self);
		}*/

		__declspec(dllexport) BGSLocation * GetLocation(BGSLocAlias * self) {
			return NativeInvoke::Invoke<BGSLocation *>("LocationAlias", "GetLocation", self);
		}

		__declspec(dllexport) void ForceLocationTo(BGSLocAlias * self, BGSLocation * akNewLocation) {
			NativeInvoke::Invoke<ScriptNone>("LocationAlias", "ForceLocationTo", self, akNewLocation);
		}

		/*__declspec(dllexport) void AddForm(BGSListForm * self, TESForm * apForm) {
			NativeInvoke::Invoke<ScriptNone>("FormList", "AddForm", self, apForm);
		}*/

		/*__declspec(dllexport) void RemoveAddedForm(BGSListForm * self, TESForm * apForm) {
			NativeInvoke::Invoke<ScriptNone>("FormList", "RemoveAddedForm", self, apForm);
		}*/

		__declspec(dllexport) TESForm * GetAt(BGSListForm * self, uint aiIndex) {
			return NativeInvoke::Invoke<TESForm *>("FormList", "GetAt", self, aiIndex);
		}

		__declspec(dllexport) uint GetSize(BGSListForm * self) {
			return NativeInvoke::Invoke<uint>("FormList", "GetSize", self);
		}

		__declspec(dllexport) bool HasForm(BGSListForm * self, TESForm * akForm) {
			return NativeInvoke::Invoke<bool>("FormList", "HasForm", self, akForm);
		}

		/*__declspec(dllexport) void Revert(BGSListForm * self) {
			NativeInvoke::Invoke<ScriptNone>("FormList", "Revert", self);
		}*/

		__declspec(dllexport) bool Cast(SpellItem * self, TESObjectREFR * akSource, TESObjectREFR * akTarget) {
			return NativeInvoke::Invoke<bool>("Spell", "Cast", self, akSource, akTarget);
		}

		__declspec(dllexport) bool RemoteCast(SpellItem * self, TESObjectREFR * akSource, CActor * akBlameActor, TESObjectREFR * akTarget) {
			return NativeInvoke::Invoke<bool>("Spell", "RemoteCast", self, akSource, akBlameActor, akTarget);
		}

		/*__declspec(dllexport) bool IsHostile(SpellItem * self) {
			return NativeInvoke::Invoke<bool>("Spell", "IsHostile", self);
		}*/

		/*__declspec(dllexport) bool Cast(ScrollItem * self, TESObjectREFR * akSource, TESObjectREFR * akTarget) {
			return NativeInvoke::Invoke<bool>("Scroll", "Cast", self, akSource, akTarget);
		}*/

		/*__declspec(dllexport) void AddInventoryEventFilter(ActiveEffect * self, TESForm * akFilter) {
			NativeInvoke::Invoke<ScriptNone>("ActiveMagicEffect", "AddInventoryEventFilter", self, akFilter);
		}*/

		__declspec(dllexport) void Dispel(ActiveEffect * self) {
			NativeInvoke::Invoke<ScriptNone>("ActiveMagicEffect", "Dispel", self);
		}

		/*__declspec(dllexport) EffectSetting * GetBaseObject(ActiveEffect * self) {
			return NativeInvoke::Invoke<EffectSetting *>("ActiveMagicEffect", "GetBaseObject", self);
		}*/

		__declspec(dllexport) CActor * GetCasterActor(ActiveEffect * self) {
			return NativeInvoke::Invoke<CActor *>("ActiveMagicEffect", "GetCasterActor", self);
		}

		__declspec(dllexport) CActor * GetTargetActor(ActiveEffect * self) {
			return NativeInvoke::Invoke<CActor *>("ActiveMagicEffect", "GetTargetActor", self);
		}

		
		__declspec(dllexport) void ReleaseOverride() {
			NativeInvoke::Invoke<ScriptNone>("Weather", "ReleaseOverride", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) TESWeather * GetCurrentWeather() {
			return NativeInvoke::Invoke<TESWeather *>("Weather", "GetCurrentWeather", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) TESWeather * GetOutgoingWeather() {
			return NativeInvoke::Invoke<TESWeather *>("Weather", "GetOutgoingWeather", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) float GetCurrentWeatherTransition() {
			return NativeInvoke::Invoke<float>("Weather", "GetCurrentWeatherTransition", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) uint GetSkyMode() {
			return NativeInvoke::Invoke<uint>("Weather", "GetSkyMode", BSScript_StaticFunctionTag);
		}

		__declspec(dllexport) TESWeather * FindWeather(uint auiType) {
			return NativeInvoke::Invoke<TESWeather *>("Weather", "FindWeather", BSScript_StaticFunctionTag, auiType);
		}

		__declspec(dllexport) void ForceActive(TESWeather * self, bool abOverride) {
			NativeInvoke::Invoke<ScriptNone>("Weather", "ForceActive", self, abOverride);
		}

		/*__declspec(dllexport) void SetActive(TESWeather * self, bool abOverride, bool abAccelerate) {
			NativeInvoke::Invoke<ScriptNone>("Weather", "SetActive", self, abOverride, abAccelerate);
		}*/

		__declspec(dllexport) int GetClassification(TESWeather * self) {
			return NativeInvoke::Invoke<int>("Weather", "GetClassification", self);
		}
		/*__declspec(dllexport) bool IsHostile(IngredientItem * self) {
			return NativeInvoke::Invoke<bool>("Ingredient", "IsHostile", self);
		}*/

		__declspec(dllexport) void LearnEffect(IngredientItem * self, uint aiIndex) {
			NativeInvoke::Invoke<ScriptNone>("Ingredient", "LearnEffect", self, aiIndex);
		}

		__declspec(dllexport) uint LearnNextEffect(IngredientItem * self) {
			return NativeInvoke::Invoke<uint>("Ingredient", "LearnNextEffect", self);
		}

		__declspec(dllexport) void LearnAllEffects(IngredientItem * self) {
			NativeInvoke::Invoke<ScriptNone>("Ingredient", "LearnAllEffects", self);
		}
		/*__declspec(dllexport) bool IsHostile(AlchemyItem * self) {
			return NativeInvoke::Invoke<bool>("Potion", "IsHostile", self);
		}
		__declspec(dllexport) bool IsHostile(EnchantmentItem * self) {
			return NativeInvoke::Invoke<bool>("Enchantment", "IsHostile", self);
		}*/

};