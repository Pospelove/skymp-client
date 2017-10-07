#pragma once

#include "CostileDragon.h"
#include "CostileInvoke2.h"

#include <utility>

template <class T> 
struct Selector1
{
	using type = T;
};

template <>
struct Selector1<void>
{
	using type = SInt32;
};

#undef GetForm
#undef AddForm
#define Value cd::Value

#define DEFVOID_START(className, funcName, ...)\
	void cd::funcName(__VA_ARGS__, std::function<void(void)> callback)\
	{\
		auto className_ = #className, funcName_ = #funcName;


#define DEFVOID_START_ZEROARGS(className, funcName)\
	void cd::funcName(std::function<void(void)> callback)\
	{\
		auto className_ = #className, funcName_ = #funcName;

//CostileInvoke::Invoke(CostileInvoke::Async, /*(cd::Callback)*/callback, className_, funcName_, __VA_ARGS__);
#define DEFVOID_END(...)\
		CostileInvoke2::Invoke<void>::Exec(true, callback, className_, funcName_, __VA_ARGS__);\
	}

#define DEFVAL_START(returnValueType, className, funcName, ...)\
	returnValueType cd::funcName(__VA_ARGS__, std::function<void(returnValueType)> callback)\
	{\
		using Type = Selector1<returnValueType>::type;\
		auto className_ = #className,\
			funcName_ = #funcName;

#define DEFVAL_START_ZEROARGS(returnValueType, className, funcName)\
	returnValueType cd::funcName(std::function<void(returnValueType)> callback)\
	{\
		using Type = Selector1<returnValueType>::type;\
		auto className_ = #className,\
			funcName_ = #funcName;

//return CostileInvoke::Invoke(callback ? CostileInvoke::Async : 0, /*(cd::Callback)*/callback, className_, funcName_, __VA_ARGS__);
#define DEFVAL_END(...)\
		return CostileInvoke2::Invoke<Type>::Exec(callback != nullptr, callback, className_, funcName_, __VA_ARGS__);\
	}



// Action.psc

// Activator.psc

// ActiveMagicEffect.psc

// Actor.psc

DEFVOID_START(Actor, ModFavorPoints, Value<Actor> self, SInt32 iFavorPoints);
DEFVOID_END(self, iFavorPoints);

DEFVOID_START(Actor, ModFavorPointsWithGlobal, Value<Actor> self, Value<TESGlobal> FavorPointsGlobal);
DEFVOID_END(self, FavorPointsGlobal);

DEFVOID_START(Actor, MakePlayerFriend, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, AddPerk, Value<Actor> self, Value<BGSPerk> akPerk);
DEFVOID_END(self, akPerk);

DEFVAL_START(bool, Actor, AddShout, Value<Actor> self, Value<TESShout> akShout);
DEFVAL_END(self, akShout);

DEFVAL_START(bool, Actor, AddSpell, Value<Actor> self, Value<SpellItem> akSpell, bool abVerbose);
DEFVAL_END(self, akSpell, abVerbose);

DEFVOID_START(Actor, AllowBleedoutDialogue, Value<Actor> self, bool abCanTalk);
DEFVOID_END(self, abCanTalk);

DEFVOID_START(Actor, AllowPCDialogue, Value<Actor> self, bool abTalk);
DEFVOID_END(self, abTalk);

DEFVOID_START(Actor, AttachAshPile, Value<Actor> self, Value<TESForm> akAshPileBase);
DEFVOID_END(self, akAshPileBase);

DEFVAL_START(bool, Actor, CanFlyHere, Value<Actor> self);
DEFVAL_END(self);

DEFVOID_START(Actor, ClearArrested, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, ClearExpressionOverride, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, ClearExtraArrows, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, ClearForcedLandingMarker, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, ClearKeepOffsetFromActor, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, ClearLookAt, Value<Actor> self);
DEFVOID_END(self);

//DEFVOID_START(Actor, DamageActorValue, Value<Actor> self, std::string asValueName, float afDamage);
//DEFVOID_END(self, asValueName, afDamage);

//DEFVOID_START(Actor, DamageAV, Value<Actor> self, std::string asValueName, float afDamage);
//DEFVOID_END(self, asValueName, afDamage);

DEFVAL_START(bool, Actor, Dismount, Value<Actor> self);
DEFVAL_END(self);

DEFVOID_START(Actor, DispelAllSpells, Value<Actor> self);
DEFVOID_END(self);

DEFVAL_START(bool, Actor, DispelSpell, Value<Actor> self, Value<SpellItem> akSpell);
DEFVAL_END(self, akSpell);

DEFVOID_START(Actor, DoCombatSpellApply, Value<Actor> self, Value<SpellItem> akSpell, Value<TESObjectREFR> akTarget);
DEFVOID_END(self, akSpell, akTarget);

DEFVOID_START(Actor, EnableAI, Value<Actor> self, bool abEnable);
DEFVOID_END(self, abEnable);

DEFVOID_START(Actor, EndDeferredKill, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, EquipItem, Value<Actor> self, Value<TESForm> akItem, bool abPreventRemoval, bool abSilent);
DEFVOID_END(self, akItem, abPreventRemoval, abSilent);

DEFVOID_START(Actor, EquipShout, Value<Actor> self, Value<TESShout> akShout);
DEFVOID_END(self, akShout);

DEFVOID_START(Actor, EquipSpell, Value<Actor> self, Value<SpellItem> akSpell, SInt32 aiSource);
DEFVOID_END(self, akSpell, aiSource);

DEFVOID_START(Actor, EvaluatePackage, Value<Actor> self);
DEFVOID_END(self);

//DEFVOID_START(Actor, ForceAV, Value<Actor> self, std::string asValueName, float afNewValue);
//DEFVOID_END(self, asValueName, afNewValue);

DEFVAL_START(Value<TESNPC>, Actor, GetActorBase, Value<Actor> self);
DEFVAL_END(self);

//DEFVAL_START(float, Actor, GetActorValue, Value<Actor> self, std::string asValueName);
//DEFVAL_END(self, asValueName);

//DEFVAL_START(float, Actor, GetActorValuePercentage, Value<Actor> self, std::string asValueName);
//DEFVAL_END(self, asValueName);

//DEFVAL_START(float, Actor, GetAV, Value<Actor> self, std::string asValueName);
//DEFVAL_END(self, asValueName);

//DEFVAL_START(float, Actor, GetAVPercentage, Value<Actor> self, std::string asValueName);
//DEFVAL_END(self, asValueName);

//DEFVAL_START(float, Actor, GetBaseActorValue, Value<Actor> self, std::string asValueName);
//DEFVAL_END(self, asValueName);

//DEFVAL_START(float, Actor, GetBaseAV, Value<Actor> self, std::string asValueName);
//DEFVAL_END(self, asValueName);

DEFVAL_START(SInt32, Actor, GetBribeAmount, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESFaction>, Actor, GetCrimeFaction, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Actor, GetCombatState, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(Value<Actor>, Actor, GetCombatTarget, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESPackage>, Actor, GetCurrentPackage, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(Value<Actor>, Actor, GetDialogueTarget, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Actor, GetEquippedItemType, Value<Actor> self, SInt32 aiHand);
DEFVAL_END(self, aiHand);

DEFVAL_START(Value<TESShout>, Actor, GetEquippedShout, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESObjectWEAP>, Actor, GetEquippedWeapon, Value<Actor> self, bool abLeftHand);
DEFVAL_END(self, abLeftHand);

DEFVAL_START(Value<TESObjectARMO>, Actor, GetEquippedShield, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(Value<SpellItem>, Actor, GetEquippedSpell, Value<Actor> self, SInt32 aiSource);
DEFVAL_END(self, aiSource);

DEFVAL_START(SInt32, Actor, GetFactionRank, Value<Actor> self, Value<TESFaction> akFaction);
DEFVAL_END(self, akFaction);

DEFVAL_START(SInt32, Actor, GetFactionReaction, Value<Actor> self, Value<Actor> akOther);
DEFVAL_END(self, akOther);

DEFVAL_START(SInt32, Actor, GetFlyingState, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESObjectREFR>, Actor, GetForcedLandingMarker, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Actor, GetGoldAmount, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Actor, GetHighestRelationshipRank, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(Value<Actor>, Actor, GetKiller, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Actor, GetLevel, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(float, Actor, GetLightLevel, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Actor, GetLowestRelationshipRank, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESNPC>, Actor, GetLeveledActorBase, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, GetNoBleedoutRecovery, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, GetPlayerControls, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESRace>, Actor, GetRace, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Actor, GetRelationshipRank, Value<Actor> self, Value<Actor> akOther);
DEFVAL_END(self, akOther);

DEFVAL_START(SInt32, Actor, GetSitState, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Actor, GetSleepState, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(float, Actor, GetVoiceRecoveryTime, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, HasAssociation, Value<Actor> self, Value<BGSAssociationType> akAssociation, Value<Actor> akOther);
DEFVAL_END(self, akAssociation, akOther);

DEFVAL_START(bool, Actor, HasFamilyRelationship, Value<Actor> self, Value<Actor> akOther);
DEFVAL_END(self, akOther);

DEFVAL_START(bool, Actor, HasLOS, Value<Actor> self, Value<TESObjectREFR> akOther);
DEFVAL_END(self, akOther);

DEFVAL_START(bool, Actor, HasMagicEffect, Value<Actor> self, Value<EffectSetting> akEffect);
DEFVAL_END(self, akEffect);

DEFVAL_START(bool, Actor, HasMagicEffectWithKeyword, Value<Actor> self, Value<BGSKeyword> akKeyword);
DEFVAL_END(self, akKeyword);

DEFVAL_START(bool, Actor, HasParentRelationship, Value<Actor> self, Value<Actor> akOther);
DEFVAL_END(self, akOther);

DEFVAL_START(bool, Actor, HasPerk, Value<Actor> self, Value<BGSPerk> akPerk);
DEFVAL_END(self, akPerk);

DEFVAL_START(bool, Actor, HasSpell, Value<Actor> self, Value<TESForm> akForm);
DEFVAL_END(self, akForm);

DEFVAL_START(bool, Actor, IsAlarmed, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsAlerted, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsAllowedToFly, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsArrested, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsArrestingTarget, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsBeingRidden, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsBleedingOut, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsBribed, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsChild, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsCommandedActor, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsDead, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsDetectedBy, Value<Actor> self, Value<Actor> akOther);
DEFVAL_END(self, akOther);

DEFVAL_START(bool, Actor, IsDoingFavor, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsEquipped, Value<Actor> self, Value<TESForm> akItem);
DEFVAL_END(self, akItem);

DEFVAL_START(bool, Actor, IsEssential, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsFlying, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsGuard, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsGhost, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsHostileToActor, Value<Actor> self, Value<Actor> akActor);
DEFVAL_END(self, akActor);

DEFVAL_START(bool, Actor, IsInCombat, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsInFaction, Value<Actor> self, Value<TESFaction> akFaction);
DEFVAL_END(self, akFaction);

DEFVAL_START(bool, Actor, IsInKillMove, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsIntimidated, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsOnMount, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsPlayersLastRiddenHorse, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, IsPlayerTeammate, Value<Actor> self);
DEFVAL_END(self);

DEFVOID_START(Actor, KeepOffsetFromActor, Value<Actor> self, Value<Actor> arTarget, float afOffsetX, float afOffsetY, float afOffsetZ, float afOffsetAngleX, float afOffsetAngleY, float afOffsetAngleZ, float afCatchUpRadius, float afFollowRadius);
DEFVOID_END(self, arTarget, afOffsetX, afOffsetY, afOffsetZ, afOffsetAngleX, afOffsetAngleY, afOffsetAngleZ, afCatchUpRadius, afFollowRadius);

DEFVOID_START(Actor, Kill, Value<Actor> self, Value<Actor> akKiller);
DEFVOID_END(self, akKiller);

DEFVOID_START(Actor, KillEssential, Value<Actor> self, Value<Actor> akKiller);
DEFVOID_END(self, akKiller);

DEFVOID_START(Actor, KillSilent, Value<Actor> self, Value<Actor> akKiller);
DEFVOID_END(self, akKiller);

//DEFVOID_START(Actor, ModAV, Value<Actor> self, std::string asValueName, float afAmount);
//DEFVOID_END(self, asValueName, afAmount);

DEFVOID_START(Actor, ModFactionRank, Value<Actor> self, Value<TESFaction> akFaction, SInt32 aiMod);
DEFVOID_END(self, akFaction, aiMod);

DEFVOID_START(Actor, MoveToPackageLocation, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, OpenInventory, Value<Actor> self, bool abForceOpen);
DEFVOID_END(self, abForceOpen);

DEFVAL_START(bool, Actor, PathToReference, Value<Actor> self, Value<TESObjectREFR> aTarget, float afWalkRunPercent);
DEFVAL_END(self, aTarget, afWalkRunPercent);

DEFVAL_START(bool, Actor, PlayIdle, Value<Actor> self, Value<TESIdleForm> akIdle);
DEFVAL_END(self, akIdle);

DEFVAL_START(bool, Actor, PlayIdleWithTarget, Value<Actor> self, Value<TESIdleForm> akIdle, Value<TESObjectREFR> akTarget);
DEFVAL_END(self, akIdle, akTarget);

DEFVOID_START(Actor, PlaySubGraphAnimation, Value<Actor> self, std::string asEventName);
DEFVOID_END(self, asEventName);

DEFVOID_START(Actor, RemoveFromFaction, Value<Actor> self, Value<TESFaction> akFaction);
DEFVOID_END(self, akFaction);

DEFVOID_START(Actor, RemoveFromAllFactions, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, RemovePerk, Value<Actor> self, Value<BGSPerk> akPerk);
DEFVOID_END(self, akPerk);

DEFVAL_START(bool, Actor, RemoveShout, Value<Actor> self, Value<TESShout> akShout);
DEFVAL_END(self, akShout);

DEFVAL_START(bool, Actor, RemoveSpell, Value<Actor> self, Value<SpellItem> akSpell);
DEFVAL_END(self, akSpell);

DEFVOID_START(Actor, ResetHealthAndLimbs, Value<Actor> self);
DEFVOID_END(self);

//DEFVOID_START(Actor, RestoreActorValue, Value<Actor> self, std::string asValueName, float afAmount);
//DEFVOID_END(self, asValueName, afAmount);

DEFVOID_START(Actor, Resurrect, Value<Actor> self);
DEFVOID_END(self);

//DEFVOID_START(Actor, RestoreAV, Value<Actor> self, std::string asValueName, float afAmount);
//DEFVOID_END(self, asValueName, afAmount);

DEFVOID_START(Actor, SendAssaultAlarm, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, SendTrespassAlarm, Value<Actor> self, Value<Actor> akCriminal);
DEFVOID_END(self, akCriminal);

DEFVOID_START(Actor, SetAlert, Value<Actor> self, bool abAlerted);
DEFVOID_END(self, abAlerted);

DEFVOID_START(Actor, SetAllowFlying, Value<Actor> self, bool abAllowed);
DEFVOID_END(self, abAllowed);

DEFVOID_START(Actor, SetAllowFlyingEx, Value<Actor> self, bool abAllowed, bool abAllowCrash, bool abAllowSearch);
DEFVOID_END(self, abAllowed, abAllowCrash, abAllowSearch);

DEFVOID_START(Actor, SetAlpha, Value<Actor> self, float afTargetAlpha, bool abFade);
DEFVOID_END(self, afTargetAlpha, abFade);

DEFVOID_START(Actor, SetAttackActorOnSight, Value<Actor> self, bool abAttackOnSight);
DEFVOID_END(self, abAttackOnSight);

//DEFVOID_START(Actor, SetAV, Value<Actor> self, std::string asValueName, float afValue);
//DEFVOID_END(self, asValueName, afValue);

DEFVOID_START(Actor, SetBribed, Value<Actor> self, bool abBribe);
DEFVOID_END(self, abBribe);

DEFVOID_START(Actor, SetCrimeFaction, Value<Actor> self, Value<TESFaction> akFaction);
DEFVOID_END(self, akFaction);

DEFVOID_START(Actor, SetCriticalStage, Value<Actor> self, SInt32 aiStage);
DEFVOID_END(self, aiStage);

DEFVOID_START(Actor, SetDoingFavor, Value<Actor> self, bool abDoingFavor);
DEFVOID_END(self, abDoingFavor);

DEFVOID_START(Actor, SetDontMove, Value<Actor> self, bool abDontMove);
DEFVOID_END(self, abDontMove);

DEFVOID_START(Actor, SetExpressionOverride, Value<Actor> self, SInt32 aiMood, SInt32 aiStrength);
DEFVOID_END(self, aiMood, aiStrength);

//DEFVOID_START(Actor, SetEyeTexture, Value<Actor> self, Value<BGSTextureSet> akNewTexture);
//DEFVOID_END(self, akNewTexture);

DEFVOID_START(Actor, SetFactionRank, Value<Actor> self, Value<TESFaction> akFaction, SInt32 aiRank);
DEFVOID_END(self, akFaction, aiRank);

DEFVOID_START(Actor, SetForcedLandingMarker, Value<Actor> self, Value<TESObjectREFR> aMarker);
DEFVOID_END(self, aMarker);

DEFVOID_START(Actor, SetGhost, Value<Actor> self, bool abIsGhost);
DEFVOID_END(self, abIsGhost);

DEFVOID_START(Actor, AddToFaction, Value<Actor> self, Value<TESFaction> akFaction);
DEFVOID_END(self, akFaction);

DEFVOID_START(Actor, SetHeadTracking, Value<Actor> self, bool abEnable);
DEFVOID_END(self, abEnable);

DEFVOID_START(Actor, SetIntimidated, Value<Actor> self, bool abIntimidate);
DEFVOID_END(self, abIntimidate);

DEFVOID_START(Actor, SetLookAt, Value<Actor> self, Value<TESObjectREFR> akTarget, bool abPathingLookAt);
DEFVOID_END(self, akTarget, abPathingLookAt);

DEFVOID_START(Actor, SetNoBleedoutRecovery, Value<Actor> self, bool abAllowed);
DEFVOID_END(self, abAllowed);

DEFVOID_START(Actor, SetNotShowOnStealthMeter, Value<Actor> self, bool abNotShow);
DEFVOID_END(self, abNotShow);

DEFVOID_START(Actor, SetOutfit, Value<Actor> self, Value<BGSOutfit> akOutfit, bool abSleepOutfit);
DEFVOID_END(self, akOutfit, abSleepOutfit);

DEFVOID_START(Actor, SetPlayerControls, Value<Actor> self, bool abControls);
DEFVOID_END(self, abControls);

DEFVOID_START(Actor, SetPlayerResistingArrest, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, SetPlayerTeammate, Value<Actor> self, bool abTeammate, bool abCanDoFavor);
DEFVOID_END(self, abTeammate, abCanDoFavor);

DEFVOID_START(Actor, SetRace, Value<Actor> self, Value<TESRace> akRace);
DEFVOID_END(self, akRace);

DEFVOID_START(Actor, SetRelationshipRank, Value<Actor> self, Value<Actor> akOther, SInt32 aiRank);
DEFVOID_END(self, akOther, aiRank);

DEFVOID_START(Actor, SetRestrained, Value<Actor> self, bool abRestrained);
DEFVOID_END(self, abRestrained);

DEFVOID_START(Actor, SetSubGraphFloatVariable, Value<Actor> self, std::string asVariableName, float afValue);
DEFVOID_END(self, asVariableName, afValue);

DEFVOID_START(Actor, SetUnconscious, Value<Actor> self, bool abUnconscious);
DEFVOID_END(self, abUnconscious);

DEFVOID_START(Actor, SetVehicle, Value<Actor> self, Value<TESObjectREFR> akVehicle);
DEFVOID_END(self, akVehicle);

DEFVOID_START(Actor, SetVoiceRecoveryTime, Value<Actor> self, float afTime);
DEFVOID_END(self, afTime);

DEFVOID_START(Actor, ShowBarterMenu, Value<Actor> self);
DEFVOID_END(self);

DEFVAL_START(SInt32, Actor, ShowGiftMenu, Value<Actor> self, bool abGivingGift, Value<BGSListForm> apFilterList, bool abShowStolenItems, bool abUseFavorPoints);
DEFVAL_END(self, abGivingGift, apFilterList, abShowStolenItems, abUseFavorPoints);

DEFVOID_START(Actor, StartCannibal, Value<Actor> self, Value<Actor> akTarget);
DEFVOID_END(self, akTarget);

DEFVOID_START(Actor, StartCombat, Value<Actor> self, Value<Actor> akTarget);
DEFVOID_END(self, akTarget);

DEFVOID_START(Actor, StartDeferredKill, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, StartVampireFeed, Value<Actor> self, Value<Actor> akTarget);
DEFVOID_END(self, akTarget);

DEFVOID_START(Actor, StopCombat, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, StopCombatAlarm, Value<Actor> self);
DEFVOID_END(self);

DEFVAL_START(bool, Actor, TrapSoul, Value<Actor> self, Value<Actor> akTarget);
DEFVAL_END(self, akTarget);

DEFVOID_START(Actor, UnequipAll, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, UnequipItem, Value<Actor> self, Value<TESForm> akItem, bool abPreventEquip, bool abSilent);
DEFVOID_END(self, akItem, abPreventEquip, abSilent);

DEFVOID_START(Actor, UnequipItemSlot, Value<Actor> self, SInt32 aiSlot);
DEFVOID_END(self, aiSlot);

DEFVOID_START(Actor, UnequipShout, Value<Actor> self, Value<TESShout> akShout);
DEFVOID_END(self, akShout);

DEFVOID_START(Actor, UnequipSpell, Value<Actor> self, Value<SpellItem> akSpell, SInt32 aiSource);
DEFVOID_END(self, akSpell, aiSource);

DEFVOID_START(Actor, UnLockOwnedDoorsInCell, Value<Actor> self);
DEFVOID_END(self);

DEFVAL_START(bool, Actor, WillIntimidateSucceed, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(bool, Actor, WornHasKeyword, Value<Actor> self, Value<BGSKeyword> akKeyword);
DEFVAL_END(self, akKeyword);

DEFVOID_START(Actor, StartSneaking, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, DrawWeapon, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, ForceMovementDirection, Value<Actor> self, float afXAngle, float afYAngle, float afZAngle);
DEFVOID_END(self, afXAngle, afYAngle, afZAngle);

DEFVOID_START(Actor, ForceMovementSpeed, Value<Actor> self, float afSpeedMult);
DEFVOID_END(self, afSpeedMult);

DEFVOID_START(Actor, ForceMovementRotationSpeed, Value<Actor> self, float afXMult, float afYMult, float afZMult);
DEFVOID_END(self, afXMult, afYMult, afZMult);

DEFVOID_START(Actor, ForceMovementDirectionRamp, Value<Actor> self, float afXAngle, float afYAngle, float afZAngle, float afRampTime);
DEFVOID_END(self, afXAngle, afYAngle, afZAngle, afRampTime);

DEFVOID_START(Actor, ForceMovementSpeedRamp, Value<Actor> self, float afSpeedMult, float afRampTime);
DEFVOID_END(self, afSpeedMult, afRampTime);

DEFVOID_START(Actor, ForceMovementRotationSpeedRamp, Value<Actor> self, float afXMult, float afYMult, float afZMult, float afRampTime);
DEFVOID_END(self, afXMult, afYMult, afZMult, afRampTime);

DEFVOID_START(Actor, ForceTargetDirection, Value<Actor> self, float afXAngle, float afYAngle, float afZAngle);
DEFVOID_END(self, afXAngle, afYAngle, afZAngle);

DEFVOID_START(Actor, ForceTargetSpeed, Value<Actor> self, float afSpeed);
DEFVOID_END(self, afSpeed);

DEFVOID_START(Actor, ForceTargetAngle, Value<Actor> self, float afXAngle, float afYAngle, float afZAngle);
DEFVOID_END(self, afXAngle, afYAngle, afZAngle);

DEFVOID_START(Actor, ClearForcedMovement, Value<Actor> self);
DEFVOID_END(self);

DEFVAL_START(Value<TESForm>, Actor, GetWornForm, Value<Actor> self, SInt32 slotMask);
DEFVAL_END(self, slotMask);

DEFVAL_START(SInt32, Actor, GetWornItemId, Value<Actor> self, SInt32 slotMask);
DEFVAL_END(self, slotMask);

DEFVAL_START(Value<TESForm>, Actor, GetEquippedObject, Value<Actor> self, SInt32 location);
DEFVAL_END(self, location);

DEFVAL_START(SInt32, Actor, GetEquippedItemId, Value<Actor> self, SInt32 location);
DEFVAL_END(self, location);

DEFVAL_START(SInt32, Actor, GetSpellCount, Value<Actor> self);
DEFVAL_END(self);

DEFVAL_START(Value<SpellItem>, Actor, GetNthSpell, Value<Actor> self, SInt32 n);
DEFVAL_END(self, n);

DEFVOID_START(Actor, QueueNiNodeUpdate, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, RegenerateHead, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, EquipItemEx, Value<Actor> self, Value<TESForm> item, SInt32 equipSlot, bool preventUnequip, bool equipSound);
DEFVOID_END(self, item, equipSlot, preventUnequip, equipSound);

DEFVOID_START(Actor, EquipItemById, Value<Actor> self, Value<TESForm> item, SInt32 itemId, SInt32 equipSlot, bool preventUnequip, bool equipSound);
DEFVOID_END(self, item, itemId, equipSlot, preventUnequip, equipSound);

DEFVOID_START(Actor, UnequipItemEx, Value<Actor> self, Value<TESForm> item, SInt32 equipSlot, bool preventEquip);
DEFVOID_END(self, item, equipSlot, preventEquip);

DEFVOID_START(Actor, ChangeHeadPart, Value<Actor> self, Value<BGSHeadPart> hPart);
DEFVOID_END(self, hPart);

DEFVOID_START(Actor, ReplaceHeadPart, Value<Actor> self, Value<BGSHeadPart> oPart, Value<BGSHeadPart> newPart);
DEFVOID_END(self, oPart, newPart);

DEFVOID_START(Actor, UpdateWeight, Value<Actor> self, float neckDelta);
DEFVOID_END(self, neckDelta);

DEFVAL_START(bool, Actor, IsAIEnabled, Value<Actor> self);
DEFVAL_END(self);

DEFVOID_START(Actor, ResetAI, Value<Actor> self);
DEFVOID_END(self);

DEFVOID_START(Actor, SheatheWeapon, Value<Actor> self);
DEFVOID_END(self);

DEFVAL_START(Value<TESObjectREFR>, Actor, GetFurnitureReference, Value<Actor> self);
DEFVAL_END(self);

DEFVOID_START(Actor, SetExpressionPhoneme, Value<Actor> self, SInt32 index, float value);
DEFVOID_END(self, index, value);

DEFVOID_START(Actor, SetExpressionModifier, Value<Actor> self, SInt32 index, float value);
DEFVOID_END(self, index, value);

DEFVOID_START(Actor, ResetExpressionOverrides, Value<Actor> self);
DEFVOID_END(self);

// ActorBase.psc

DEFVAL_START(Value<TESClass>, ActorBase, GetClass, Value<TESNPC> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, ActorBase, GetDeadCount, Value<TESNPC> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSListForm>, ActorBase, GetGiftFilter, Value<TESNPC> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESRace>, ActorBase, GetRace, Value<TESNPC> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, ActorBase, GetSex, Value<TESNPC> self);
DEFVAL_END(self);

DEFVAL_START(bool, ActorBase, IsEssential, Value<TESNPC> self);
DEFVAL_END(self);

DEFVAL_START(bool, ActorBase, IsInvulnerable, Value<TESNPC> self);
DEFVAL_END(self);

DEFVAL_START(bool, ActorBase, IsProtected, Value<TESNPC> self);
DEFVAL_END(self);

DEFVAL_START(bool, ActorBase, IsUnique, Value<TESNPC> self);
DEFVAL_END(self);

DEFVOID_START(ActorBase, SetEssential, Value<TESNPC> self, bool abEssential);
DEFVOID_END(self, abEssential);

DEFVOID_START(ActorBase, SetInvulnerable, Value<TESNPC> self, bool abInvulnerable);
DEFVOID_END(self, abInvulnerable);

DEFVOID_START(ActorBase, SetProtected, Value<TESNPC> self, bool abProtected);
DEFVOID_END(self, abProtected);

DEFVOID_START(ActorBase, SetOutfit, Value<TESNPC> self, Value<BGSOutfit> akOutfit, bool abSleepOutfit);
DEFVOID_END(self, akOutfit, abSleepOutfit);

DEFVAL_START(Value<TESCombatStyle>, ActorBase, GetCombatStyle, Value<TESNPC> self);
DEFVAL_END(self);

DEFVOID_START(ActorBase, SetCombatStyle, Value<TESNPC> self, Value<TESCombatStyle> cs);
DEFVOID_END(self, cs);

DEFVAL_START(Value<BGSOutfit>, ActorBase, GetOutfit, Value<TESNPC> self, bool bSleepOutfit);
DEFVAL_END(self, bSleepOutfit);

DEFVOID_START(ActorBase, SetClass, Value<TESNPC> self, Value<TESClass> c);
DEFVOID_END(self, c);

DEFVAL_START(float, ActorBase, GetHeight, Value<TESNPC> self);
DEFVAL_END(self);

DEFVOID_START(ActorBase, SetHeight, Value<TESNPC> self, float height);
DEFVOID_END(self, height);

DEFVAL_START(float, ActorBase, GetWeight, Value<TESNPC> self);
DEFVAL_END(self);

DEFVOID_START(ActorBase, SetWeight, Value<TESNPC> self, float weight);
DEFVOID_END(self, weight);

DEFVAL_START(SInt32, ActorBase, GetNumHeadParts, Value<TESNPC> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSHeadPart>, ActorBase, GetNthHeadPart, Value<TESNPC> self, SInt32 slotPart);
DEFVAL_END(self, slotPart);

DEFVOID_START(ActorBase, SetNthHeadPart, Value<TESNPC> self, Value<BGSHeadPart> headPart, SInt32 slotPart);
DEFVOID_END(self, headPart, slotPart);

DEFVAL_START(SInt32, ActorBase, GetIndexOfHeadPartByType, Value<TESNPC> self, SInt32 type);
DEFVAL_END(self, type);

DEFVAL_START(SInt32, ActorBase, GetNumOverlayHeadParts, Value<TESNPC> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSHeadPart>, ActorBase, GetNthOverlayHeadPart, Value<TESNPC> self, SInt32 slotPart);
DEFVAL_END(self, slotPart);

DEFVAL_START(SInt32, ActorBase, GetIndexOfOverlayHeadPartByType, Value<TESNPC> self, SInt32 type);
DEFVAL_END(self, type);

DEFVAL_START(float, ActorBase, GetFaceMorph, Value<TESNPC> self, SInt32 index);
DEFVAL_END(self, index);

DEFVOID_START(ActorBase, SetFaceMorph, Value<TESNPC> self, float value, SInt32 index);
DEFVOID_END(self, value, index);

DEFVAL_START(SInt32, ActorBase, GetFacePreset, Value<TESNPC> self, SInt32 index);
DEFVAL_END(self, index);

DEFVOID_START(ActorBase, SetFacePreset, Value<TESNPC> self, SInt32 value, SInt32 index);
DEFVOID_END(self, value, index);

DEFVAL_START(Value<BGSColorForm>, ActorBase, GetHairColor, Value<TESNPC> self);
DEFVAL_END(self);

DEFVOID_START(ActorBase, SetHairColor, Value<TESNPC> self, Value<BGSColorForm> color);
DEFVOID_END(self, color);

DEFVAL_START(SInt32, ActorBase, GetSpellCount, Value<TESNPC> self);
DEFVAL_END(self);

DEFVAL_START(Value<SpellItem>, ActorBase, GetNthSpell, Value<TESNPC> self, SInt32 n);
DEFVAL_END(self, n);

//DEFVAL_START(Value<BGSTextureSet>, ActorBase, GetFaceTextureSet, Value<TESNPC> self);
//DEFVAL_END(self);

//DEFVOID_START(ActorBase, SetFaceTextureSet, Value<TESNPC> self, Value<BGSTextureSet> textures);
//DEFVOID_END(self, textures);

DEFVAL_START(Value<BGSVoiceType>, ActorBase, GetVoiceType, Value<TESNPC> self);
DEFVAL_END(self);

DEFVOID_START(ActorBase, SetVoiceType, Value<TESNPC> self, Value<BGSVoiceType> nVoice);
DEFVOID_END(self, nVoice);

DEFVAL_START(Value<TESObjectARMO>, ActorBase, GetSkin, Value<TESNPC> self);
DEFVAL_END(self);

DEFVOID_START(ActorBase, SetSkin, Value<TESNPC> self, Value<TESObjectARMO> skin);
DEFVOID_END(self, skin);

DEFVAL_START(Value<TESObjectARMO>, ActorBase, GetSkinFar, Value<TESNPC> self);
DEFVAL_END(self);

DEFVOID_START(ActorBase, SetSkinFar, Value<TESNPC> self, Value<TESObjectARMO> skin);
DEFVOID_END(self, skin);

DEFVAL_START(Value<TESNPC>, ActorBase, GetTemplate, Value<TESNPC> self);
DEFVAL_END(self);

// ActorValueInfo.psc

DEFVAL_START(Value<ActorValueInfo>, ActorValueInfo, GetActorValueInfoByName, std::string avName);
DEFVAL_END(avName);

DEFVAL_START(Value<ActorValueInfo>, ActorValueInfo, GetAVIByName, std::string avName);
DEFVAL_END(avName);

DEFVAL_START(Value<ActorValueInfo>, ActorValueInfo, GetActorValueInfoByID, SInt32 id);
DEFVAL_END(id);

DEFVAL_START(Value<ActorValueInfo>, ActorValueInfo, GetAVIByID, SInt32 id);
DEFVAL_END(id);

DEFVAL_START(bool, ActorValueInfo, IsSkill, Value<ActorValueInfo> self);
DEFVAL_END(self);

DEFVAL_START(float, ActorValueInfo, GetSkillUseMult, Value<ActorValueInfo> self);
DEFVAL_END(self);

DEFVOID_START(ActorValueInfo, SetSkillUseMult, Value<ActorValueInfo> self, float value);
DEFVOID_END(self, value);

DEFVAL_START(float, ActorValueInfo, GetSkillOffsetMult, Value<ActorValueInfo> self);
DEFVAL_END(self);

DEFVOID_START(ActorValueInfo, SetSkillOffsetMult, Value<ActorValueInfo> self, float value);
DEFVOID_END(self, value);

DEFVAL_START(float, ActorValueInfo, GetSkillImproveMult, Value<ActorValueInfo> self);
DEFVAL_END(self);

DEFVOID_START(ActorValueInfo, SetSkillImproveMult, Value<ActorValueInfo> self, float value);
DEFVOID_END(self, value);

DEFVAL_START(float, ActorValueInfo, GetSkillImproveOffset, Value<ActorValueInfo> self);
DEFVAL_END(self);

DEFVOID_START(ActorValueInfo, SetSkillImproveOffset, Value<ActorValueInfo> self, float value);
DEFVOID_END(self, value);

DEFVAL_START(float, ActorValueInfo, GetSkillExperience, Value<ActorValueInfo> self);
DEFVAL_END(self);

DEFVOID_START(ActorValueInfo, SetSkillExperience, Value<ActorValueInfo> self, float exp);
DEFVOID_END(self, exp);

DEFVOID_START(ActorValueInfo, AddSkillExperience, Value<ActorValueInfo> self, float exp);
DEFVOID_END(self, exp);

DEFVAL_START(float, ActorValueInfo, GetExperienceForLevel, Value<ActorValueInfo> self, SInt32 currentLevel);
DEFVAL_END(self, currentLevel);

DEFVAL_START(SInt32, ActorValueInfo, GetSkillLegendaryLevel, Value<ActorValueInfo> self);
DEFVAL_END(self);

DEFVOID_START(ActorValueInfo, SetSkillLegendaryLevel, Value<ActorValueInfo> self, SInt32 level);
DEFVOID_END(self, level);

DEFVOID_START(ActorValueInfo, GetPerkTree, Value<ActorValueInfo> self, Value<BGSListForm> list, Value<Actor> akActor, bool unowned, bool allRanks);
DEFVOID_END(self, list, akActor, unowned, allRanks);

DEFVAL_START(float, ActorValueInfo, GetCurrentValue, Value<ActorValueInfo> self, Value<Actor> akActor);
DEFVAL_END(self, akActor);

DEFVAL_START(float, ActorValueInfo, GetBaseValue, Value<ActorValueInfo> self, Value<Actor> akActor);
DEFVAL_END(self, akActor);

DEFVAL_START(float, ActorValueInfo, GetMaximumValue, Value<ActorValueInfo> self, Value<Actor> akActor);
DEFVAL_END(self, akActor);

// Ammo.psc

DEFVAL_START(bool, Ammo, IsBolt, Value<TESAmmo> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSProjectile>, Ammo, GetProjectile, Value<TESAmmo> self);
DEFVAL_END(self);

DEFVAL_START(float, Ammo, GetDamage, Value<TESAmmo> self);
DEFVAL_END(self);

// Apparatus.psc

DEFVAL_START(SInt32, Apparatus, GetQuality, Value<BGSApparatus> self);
DEFVAL_END(self);

DEFVOID_START(Apparatus, SetQuality, Value<BGSApparatus> self, SInt32 quality);
DEFVOID_END(self, quality);

// Armor.psc

DEFVAL_START(SInt32, Armor, GetArmorRating, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Armor, GetAR, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVOID_START(Armor, SetArmorRating, Value<TESObjectARMO> self, SInt32 armorRating);
DEFVOID_END(self, armorRating);

DEFVOID_START(Armor, SetAR, Value<TESObjectARMO> self, SInt32 armorRating);
DEFVOID_END(self, armorRating);

DEFVOID_START(Armor, ModArmorRating, Value<TESObjectARMO> self, SInt32 modBy);
DEFVOID_END(self, modBy);

DEFVOID_START(Armor, ModAR, Value<TESObjectARMO> self, SInt32 modBy);
DEFVOID_END(self, modBy);

DEFVAL_START(std::string, Armor, GetModelPath, Value<TESObjectARMO> self, bool bFemalePath);
DEFVAL_END(self, bFemalePath);

DEFVOID_START(Armor, SetModelPath, Value<TESObjectARMO> self, std::string path, bool bFemalePath);
DEFVOID_END(self, path, bFemalePath);

DEFVAL_START(std::string, Armor, GetIconPath, Value<TESObjectARMO> self, bool bFemalePath);
DEFVAL_END(self, bFemalePath);

DEFVOID_START(Armor, SetIconPath, Value<TESObjectARMO> self, std::string path, bool bFemalePath);
DEFVOID_END(self, path, bFemalePath);

DEFVAL_START(std::string, Armor, GetMessageIconPath, Value<TESObjectARMO> self, bool bFemalePath);
DEFVAL_END(self, bFemalePath);

DEFVOID_START(Armor, SetMessageIconPath, Value<TESObjectARMO> self, std::string path, bool bFemalePath);
DEFVOID_END(self, path, bFemalePath);

DEFVAL_START(SInt32, Armor, GetWeightClass, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVOID_START(Armor, SetWeightClass, Value<TESObjectARMO> self, SInt32 weightClass);
DEFVOID_END(self, weightClass);

DEFVAL_START(Value<EnchantmentItem>, Armor, GetEnchantment, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVOID_START(Armor, SetEnchantment, Value<TESObjectARMO> self, Value<EnchantmentItem> e);
DEFVOID_END(self, e);

DEFVAL_START(bool, Armor, IsLightArmor, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsHeavyArmor, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsClothing, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsBoots, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsCuirass, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsGauntlets, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsHelmet, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsShield, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsJewelry, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsClothingHead, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsClothingBody, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsClothingFeet, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsClothingHands, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsClothingRing, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsClothingRich, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(bool, Armor, IsClothingPoor, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Armor, GetSlotMask, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVOID_START(Armor, SetSlotMask, Value<TESObjectARMO> self, SInt32 slotMask);
DEFVOID_END(self, slotMask);

DEFVAL_START(SInt32, Armor, AddSlotToMask, Value<TESObjectARMO> self, SInt32 slotMask);
DEFVAL_END(self, slotMask);

DEFVAL_START(SInt32, Armor, RemoveSlotFromMask, Value<TESObjectARMO> self, SInt32 slotMask);
DEFVAL_END(self, slotMask);

DEFVAL_START(SInt32, Armor, GetMaskForSlot, SInt32 slot);
DEFVAL_END(slot);

DEFVAL_START(SInt32, Armor, GetNumArmorAddons, Value<TESObjectARMO> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESObjectARMA>, Armor, GetNthArmorAddon, Value<TESObjectARMO> self, SInt32 n);
DEFVAL_END(self, n);

// ArmorAddon.psc

DEFVAL_START(std::string, ArmorAddon, GetModelPath, Value<TESObjectARMA> self, bool firstPerson, bool female);
DEFVAL_END(self, firstPerson, female);

DEFVOID_START(ArmorAddon, SetModelPath, Value<TESObjectARMA> self, std::string path, bool firstPerson, bool female);
DEFVOID_END(self, path, firstPerson, female);

DEFVAL_START(SInt32, ArmorAddon, GetModelNumTextureSets, Value<TESObjectARMA> self, bool first, bool female);
DEFVAL_END(self, first, female);

//DEFVAL_START(Value<BGSTextureSet>, ArmorAddon, GetModelNthTextureSet, Value<TESObjectARMA> self, SInt32 n, bool first, bool female);
//DEFVAL_END(self, n, first, female);

//DEFVOID_START(ArmorAddon, SetModelNthTextureSet, Value<TESObjectARMA> self, Value<BGSTextureSet> texture, SInt32 n, bool first, bool female);
//DEFVOID_END(self, texture, n, first, female);

DEFVAL_START(SInt32, ArmorAddon, GetNumAdditionalRaces, Value<TESObjectARMA> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESRace>, ArmorAddon, GetNthAdditionalRace, Value<TESObjectARMA> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(SInt32, ArmorAddon, GetSlotMask, Value<TESObjectARMA> self);
DEFVAL_END(self);

DEFVOID_START(ArmorAddon, SetSlotMask, Value<TESObjectARMA> self, SInt32 slotMask);
DEFVOID_END(self, slotMask);

DEFVAL_START(SInt32, ArmorAddon, AddSlotToMask, Value<TESObjectARMA> self, SInt32 slotMask);
DEFVAL_END(self, slotMask);

DEFVAL_START(SInt32, ArmorAddon, RemoveSlotFromMask, Value<TESObjectARMA> self, SInt32 slotMask);
DEFVAL_END(self, slotMask);

//DEFVAL_START(SInt32, ArmorAddon, GetMaskForSlot, SInt32 slot);
//DEFVAL_END(slot);

// Art.psc

DEFVAL_START(std::string, Art, GetModelPath, Value<BGSArtObject> self);
DEFVAL_END(self);

DEFVOID_START(Art, SetModelPath, Value<BGSArtObject> self, std::string path);
DEFVOID_END(self, path);

// AssociationType.psc

// Book.psc

DEFVAL_START(Value<SpellItem>, Book, GetSpell, Value<TESObjectBOOK> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Book, GetSkill, Value<TESObjectBOOK> self);
DEFVAL_END(self);

DEFVAL_START(bool, Book, IsRead, Value<TESObjectBOOK> self);
DEFVAL_END(self);

DEFVAL_START(bool, Book, IsTakeable, Value<TESObjectBOOK> self);
DEFVAL_END(self);

// Cell.psc

DEFVAL_START(Value<TESNPC>, Cell, GetActorOwner, Value<TESObjectCELL> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESFaction>, Cell, GetFactionOwner, Value<TESObjectCELL> self);
DEFVAL_END(self);

DEFVAL_START(bool, Cell, IsAttached, Value<TESObjectCELL> self);
DEFVAL_END(self);

DEFVAL_START(bool, Cell, IsInterior, Value<TESObjectCELL> self);
DEFVAL_END(self);

DEFVOID_START(Cell, Reset, Value<TESObjectCELL> self);
DEFVOID_END(self);

DEFVOID_START(Cell, SetActorOwner, Value<TESObjectCELL> self, Value<TESNPC> akActor);
DEFVOID_END(self, akActor);

DEFVOID_START(Cell, SetFactionOwner, Value<TESObjectCELL> self, Value<TESFaction> akFaction);
DEFVOID_END(self, akFaction);

DEFVOID_START(Cell, SetFogColor, Value<TESObjectCELL> self, SInt32 aiNearRed, SInt32 aiNearGreen, SInt32 aiNearBlue, SInt32 aiFarRed, SInt32 aiFarGreen, SInt32 aiFarBlue);
DEFVOID_END(self, aiNearRed, aiNearGreen, aiNearBlue, aiFarRed, aiFarGreen, aiFarBlue);

DEFVOID_START(Cell, SetFogPlanes, Value<TESObjectCELL> self, float afNear, float afFar);
DEFVOID_END(self, afNear, afFar);

DEFVOID_START(Cell, SetFogPower, Value<TESObjectCELL> self, float afPower);
DEFVOID_END(self, afPower);

DEFVOID_START(Cell, SetPublic, Value<TESObjectCELL> self, bool abPublic);
DEFVOID_END(self, abPublic);

DEFVAL_START(SInt32, Cell, GetNumRefs, Value<TESObjectCELL> self, SInt32 formTypeFilter);
DEFVAL_END(self, formTypeFilter);

DEFVAL_START(Value<TESObjectREFR>, Cell, GetNthRef, Value<TESObjectCELL> self, SInt32 n, SInt32 formTypeFilter);
DEFVAL_END(self, n, formTypeFilter);

DEFVAL_START(float, Cell, GetWaterLevel, Value<TESObjectCELL> self);
DEFVAL_END(self);

// Class.psc

// ColorForm.psc

DEFVAL_START(SInt32, ColorForm, GetColor, Value<BGSColorForm> self);
DEFVAL_END(self);

DEFVOID_START(ColorForm, SetColor, Value<BGSColorForm> self, SInt32 color);
DEFVOID_END(self, color);

DEFVAL_START(SInt32, ColorForm, GetRed, Value<BGSColorForm> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, ColorForm, GetGreen, Value<BGSColorForm> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, ColorForm, GetBlue, Value<BGSColorForm> self);
DEFVAL_END(self);

DEFVAL_START(float, ColorForm, GetHue, Value<BGSColorForm> self);
DEFVAL_END(self);

DEFVAL_START(float, ColorForm, GetSaturation, Value<BGSColorForm> self);
DEFVAL_END(self);

DEFVAL_START(float, ColorForm, GetValue, Value<BGSColorForm> self);
DEFVAL_END(self);

// CombatStyle.psc

DEFVAL_START(float, CombatStyle, GetOffensiveMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetDefensiveMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetGroupOffensiveMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetAvoidThreatChance, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetMeleeMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetRangedMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetMagicMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetShoutMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetStaffMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetUnarmedMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVOID_START(CombatStyle, SetOffensiveMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetDefensiveMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetGroupOffensiveMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetAvoidThreatChance, Value<TESCombatStyle> self, float chance);
DEFVOID_END(self, chance);

DEFVOID_START(CombatStyle, SetMeleeMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetRangedMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetMagicMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetShoutMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetStaffMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetUnarmedMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVAL_START(float, CombatStyle, GetMeleeAttackStaggeredMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetMeleePowerAttackStaggeredMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetMeleePowerAttackBlockingMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetMeleeBashMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetMeleeBashRecoiledMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetMeleeBashAttackMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetMeleeBashPowerAttackMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetMeleeSpecialAttackMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(bool, CombatStyle, GetAllowDualWielding, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVOID_START(CombatStyle, SetMeleeAttackStaggeredMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetMeleePowerAttackStaggeredMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetMeleePowerAttackBlockingMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetMeleeBashMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetMeleeBashRecoiledMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetMeleeBashAttackMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetMeleeBashPowerAttackMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetMeleeSpecialAttackMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetAllowDualWielding, Value<TESCombatStyle> self, bool allow);
DEFVOID_END(self, allow);

DEFVAL_START(float, CombatStyle, GetCloseRangeDuelingCircleMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetCloseRangeDuelingFallbackMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetCloseRangeFlankingFlankDistance, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetCloseRangeFlankingStalkTime, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVOID_START(CombatStyle, SetCloseRangeDuelingCircleMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetCloseRangeDuelingFallbackMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetCloseRangeFlankingFlankDistance, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVOID_START(CombatStyle, SetCloseRangeFlankingStalkTime, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVAL_START(float, CombatStyle, GetLongRangeStrafeMult, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVOID_START(CombatStyle, SetLongRangeStrafeMult, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

DEFVAL_START(float, CombatStyle, GetFlightHoverChance, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetFlightDiveBombChance, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVAL_START(float, CombatStyle, GetFlightFlyingAttackChance, Value<TESCombatStyle> self);
DEFVAL_END(self);

DEFVOID_START(CombatStyle, SetFlightHoverChance, Value<TESCombatStyle> self, float chance);
DEFVOID_END(self, chance);

DEFVOID_START(CombatStyle, SetFlightDiveBombChance, Value<TESCombatStyle> self, float chance);
DEFVOID_END(self, chance);

DEFVOID_START(CombatStyle, SetFlightFlyingAttackChance, Value<TESCombatStyle> self, float mult);
DEFVOID_END(self, mult);

// ConstructibleObject.psc

DEFVAL_START(Value<TESForm>, ConstructibleObject, GetResult, Value<BGSConstructibleObject> self);
DEFVAL_END(self);

DEFVOID_START(ConstructibleObject, SetResult, Value<BGSConstructibleObject> self, Value<TESForm> result);
DEFVOID_END(self, result);

DEFVAL_START(SInt32, ConstructibleObject, GetResultQuantity, Value<BGSConstructibleObject> self);
DEFVAL_END(self);

DEFVOID_START(ConstructibleObject, SetResultQuantity, Value<BGSConstructibleObject> self, SInt32 quantity);
DEFVOID_END(self, quantity);

DEFVAL_START(SInt32, ConstructibleObject, GetNumIngredients, Value<BGSConstructibleObject> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESForm>, ConstructibleObject, GetNthIngredient, Value<BGSConstructibleObject> self, SInt32 n);
DEFVAL_END(self, n);

DEFVOID_START(ConstructibleObject, SetNthIngredient, Value<BGSConstructibleObject> self, Value<TESForm> required, SInt32 n);
DEFVOID_END(self, required, n);

DEFVAL_START(SInt32, ConstructibleObject, GetNthIngredientQuantity, Value<BGSConstructibleObject> self, SInt32 n);
DEFVAL_END(self, n);

DEFVOID_START(ConstructibleObject, SetNthIngredientQuantity, Value<BGSConstructibleObject> self, SInt32 value, SInt32 n);
DEFVOID_END(self, value, n);

DEFVAL_START(Value<BGSKeyword>, ConstructibleObject, GetWorkbenchKeyword, Value<BGSConstructibleObject> self);
DEFVAL_END(self);

DEFVOID_START(ConstructibleObject, SetWorkbenchKeyword, Value<BGSConstructibleObject> self, Value<BGSKeyword> aKeyword);
DEFVOID_END(self, aKeyword);

// Container.psc

// Debug.psc

DEFVAL_START_ZEROARGS(std::string, Debug, GetPlatformName);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(std::string, Debug, GetVersionNumber);
DEFVAL_END(NULL);

/*DEFVOID_START(Debug, MessageBox, std::string asMessageBoxText);
DEFVOID_END(asMessageBoxText);

DEFVOID_START(Debug, Notification, std::string asNotificationText);
DEFVOID_END(asNotificationText);*/

DEFVOID_START_ZEROARGS(Debug, QuitGame);
DEFVOID_END(NULL);

DEFVOID_START(Debug, SetFootIK, bool abFootIK);
DEFVOID_END(abFootIK);

DEFVOID_START(Debug, SetGodMode, bool abGodMode);
DEFVOID_END(abGodMode);

DEFVOID_START(Debug, SendAnimationEvent, Value<TESObjectREFR> arRef, std::string asEventName);
DEFVOID_END(arRef, asEventName);

DEFVOID_START(Debug, TakeScreenshot, std::string asFilename);
DEFVOID_END(asFilename);

DEFVOID_START_ZEROARGS(Debug, ToggleAI);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Debug, ToggleCollisions);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Debug, ToggleMenus);
DEFVOID_END(NULL);

// Door.psc

// EffectShader.psc

DEFVOID_START(EffectShader, Play, Value<TESEffectShader> self, Value<TESObjectREFR> akObject, float afDuration);
DEFVOID_END(self, akObject, afDuration);

DEFVOID_START(EffectShader, Stop, Value<TESEffectShader> self, Value<TESObjectREFR> akObject);
DEFVOID_END(self, akObject);

// Enchantment.psc

DEFVAL_START(bool, Enchantment, IsHostile, Value<EnchantmentItem> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Enchantment, GetNumEffects, Value<EnchantmentItem> self);
DEFVAL_END(self);

DEFVAL_START(float, Enchantment, GetNthEffectMagnitude, Value<EnchantmentItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Enchantment, GetNthEffectArea, Value<EnchantmentItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Enchantment, GetNthEffectDuration, Value<EnchantmentItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(Value<EffectSetting>, Enchantment, GetNthEffectMagicEffect, Value<EnchantmentItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Enchantment, GetCostliestEffectIndex, Value<EnchantmentItem> self);
DEFVAL_END(self);

DEFVOID_START(Enchantment, SetNthEffectMagnitude, Value<EnchantmentItem> self, SInt32 index, float value);
DEFVOID_END(self, index, value);

DEFVOID_START(Enchantment, SetNthEffectArea, Value<EnchantmentItem> self, SInt32 index, SInt32 value);
DEFVOID_END(self, index, value);

DEFVOID_START(Enchantment, SetNthEffectDuration, Value<EnchantmentItem> self, SInt32 index, SInt32 value);
DEFVOID_END(self, index, value);

DEFVAL_START(Value<EnchantmentItem>, Enchantment, GetBaseEnchantment, Value<EnchantmentItem> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSListForm>, Enchantment, GetKeywordRestrictions, Value<EnchantmentItem> self);
DEFVAL_END(self);

DEFVOID_START(Enchantment, SetKeywordRestrictions, Value<EnchantmentItem> self, Value<BGSListForm> newKeywordList);
DEFVOID_END(self, newKeywordList);

// EquipSlot.psc

DEFVAL_START(SInt32, EquipSlot, GetNumParents, Value<BGSEquipSlot> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSEquipSlot>, EquipSlot, GetNthParent, Value<BGSEquipSlot> self, SInt32 n);
DEFVAL_END(self, n);

// Explosion.psc

// Faction.psc

DEFVAL_START(bool, Faction, CanPayCrimeGold, Value<TESFaction> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Faction, GetCrimeGold, Value<TESFaction> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Faction, GetCrimeGoldNonViolent, Value<TESFaction> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Faction, GetCrimeGoldViolent, Value<TESFaction> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Faction, GetInfamy, Value<TESFaction> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Faction, GetInfamyNonViolent, Value<TESFaction> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Faction, GetInfamyViolent, Value<TESFaction> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Faction, GetReaction, Value<TESFaction> self, Value<TESFaction> akOther);
DEFVAL_END(self, akOther);

DEFVAL_START(SInt32, Faction, GetStolenItemValueCrime, Value<TESFaction> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Faction, GetStolenItemValueNoCrime, Value<TESFaction> self);
DEFVAL_END(self);

DEFVAL_START(bool, Faction, IsFactionInCrimeGroup, Value<TESFaction> self, Value<TESFaction> akOther);
DEFVAL_END(self, akOther);

DEFVAL_START(bool, Faction, IsPlayerExpelled, Value<TESFaction> self);
DEFVAL_END(self);

DEFVOID_START(Faction, ModCrimeGold, Value<TESFaction> self, SInt32 aiAmount, bool abViolent);
DEFVOID_END(self, aiAmount, abViolent);

DEFVOID_START(Faction, ModReaction, Value<TESFaction> self, Value<TESFaction> akOther, SInt32 aiAmount);
DEFVOID_END(self, akOther, aiAmount);

DEFVOID_START(Faction, PlayerPayCrimeGold, Value<TESFaction> self, bool abRemoveStolenItems, bool abGoToJail);
DEFVOID_END(self, abRemoveStolenItems, abGoToJail);

DEFVOID_START(Faction, SendAssaultAlarm, Value<TESFaction> self);
DEFVOID_END(self);

DEFVOID_START(Faction, SendPlayerToJail, Value<TESFaction> self, bool abRemoveInventory, bool abRealJail);
DEFVOID_END(self, abRemoveInventory, abRealJail);

DEFVOID_START(Faction, SetAlly, Value<TESFaction> self, Value<TESFaction> akOther, bool abSelfIsFriendToOther, bool abOtherIsFriendToSelf);
DEFVOID_END(self, akOther, abSelfIsFriendToOther, abOtherIsFriendToSelf);

DEFVOID_START(Faction, SetCrimeGold, Value<TESFaction> self, SInt32 aiGold);
DEFVOID_END(self, aiGold);

DEFVOID_START(Faction, SetCrimeGoldViolent, Value<TESFaction> self, SInt32 aiGold);
DEFVOID_END(self, aiGold);

DEFVOID_START(Faction, SetEnemy, Value<TESFaction> self, Value<TESFaction> akOther, bool abSelfIsNeutralToOther, bool abOtherIsNeutralToSelf);
DEFVOID_END(self, akOther, abSelfIsNeutralToOther, abOtherIsNeutralToSelf);

DEFVOID_START(Faction, SetPlayerEnemy, Value<TESFaction> self, bool abIsEnemy);
DEFVOID_END(self, abIsEnemy);

DEFVOID_START(Faction, SetPlayerExpelled, Value<TESFaction> self, bool abIsExpelled);
DEFVOID_END(self, abIsExpelled);

DEFVOID_START(Faction, SetReaction, Value<TESFaction> self, Value<TESFaction> akOther, SInt32 aiNewValue);
DEFVOID_END(self, akOther, aiNewValue);

DEFVOID_START(Faction, MakeVendor, Value<TESFaction> self);
DEFVOID_END(self);

DEFVAL_START(bool, Faction, IsVendor, Value<TESFaction> self);
DEFVAL_END(self);

DEFVOID_START(Faction, ClearVendor, Value<TESFaction> self);
DEFVOID_END(self);

DEFVAL_START(bool, Faction, IsFactionFlagSet, Value<TESFaction> self, SInt32 flag);
DEFVAL_END(self, flag);

DEFVOID_START(Faction, SetFactionFlag, Value<TESFaction> self, SInt32 flag);
DEFVOID_END(self, flag);

DEFVOID_START(Faction, ClearFactionFlag, Value<TESFaction> self, SInt32 flag);
DEFVOID_END(self, flag);

DEFVAL_START(bool, Faction, OnlyBuysStolenItems, Value<TESFaction> self);
DEFVAL_END(self);

DEFVOID_START(Faction, SetOnlyBuysStolenItems, Value<TESFaction> self, bool onlyStolen);
DEFVOID_END(self, onlyStolen);

DEFVAL_START(SInt32, Faction, GetVendorStartHour, Value<TESFaction> self);
DEFVAL_END(self);

DEFVOID_START(Faction, SetVendorStartHour, Value<TESFaction> self, SInt32 hour);
DEFVOID_END(self, hour);

DEFVAL_START(SInt32, Faction, GetVendorEndHour, Value<TESFaction> self);
DEFVAL_END(self);

DEFVOID_START(Faction, SetVendorEndHour, Value<TESFaction> self, SInt32 hour);
DEFVOID_END(self, hour);

DEFVAL_START(SInt32, Faction, GetVendorRadius, Value<TESFaction> self);
DEFVAL_END(self);

DEFVOID_START(Faction, SetVendorRadius, Value<TESFaction> self, SInt32 radius);
DEFVOID_END(self, radius);

DEFVAL_START(Value<TESObjectREFR>, Faction, GetMerchantContainer, Value<TESFaction> self);
DEFVAL_END(self);

DEFVOID_START(Faction, SetMerchantContainer, Value<TESFaction> self, Value<TESObjectREFR> akContainer);
DEFVOID_END(self, akContainer);

DEFVAL_START(bool, Faction, IsNotSellBuy, Value<TESFaction> self);
DEFVAL_END(self);

DEFVOID_START(Faction, SetNotSellBuy, Value<TESFaction> self, bool notSellBuy);
DEFVOID_END(self, notSellBuy);

DEFVAL_START(Value<BGSListForm>, Faction, GetBuySellList, Value<TESFaction> self);
DEFVAL_END(self);

DEFVOID_START(Faction, SetBuySellList, Value<TESFaction> self, Value<BGSListForm> akList);
DEFVOID_END(self, akList);

// Flora.psc

DEFVAL_START(Value<BGSSoundDescriptorForm>, Flora, GetHarvestSound, Value<TESFlora> self);
DEFVAL_END(self);

DEFVOID_START(Flora, SetHarvestSound, Value<TESFlora> self, Value<BGSSoundDescriptorForm> akSoundDescriptor);
DEFVOID_END(self, akSoundDescriptor);

DEFVAL_START(Value<TESForm>, Flora, GetIngredient, Value<TESFlora> self);
DEFVAL_END(self);

DEFVOID_START(Flora, SetIngredient, Value<TESFlora> self, Value<TESForm> akIngredient);
DEFVOID_END(self, akIngredient);

// Form.psc

DEFVAL_START(UInt32, Form, GetFormID, Value<TESForm> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Form, GetGoldValue, Value<TESForm> self);
DEFVAL_END(self);

DEFVAL_START(bool, Form, HasKeyword, Value<TESForm> self, Value<BGSKeyword> akKeyword);
DEFVAL_END(self, akKeyword);

DEFVAL_START(bool, Form, PlayerKnows, Value<TESForm> self);
DEFVAL_END(self);

DEFVAL_START(bool, Form, RegisterForAnimationEvent, Value<TESForm> self, Value<TESObjectREFR> akSender, std::string asEventName);
DEFVAL_END(self, akSender, asEventName);

DEFVOID_START(Form, RegisterForLOS, Value<TESForm> self, Value<Actor> akViewer, Value<TESObjectREFR> akTarget);
DEFVOID_END(self, akViewer, akTarget);

DEFVOID_START(Form, RegisterForSingleLOSGain, Value<TESForm> self, Value<Actor> akViewer, Value<TESObjectREFR> akTarget);
DEFVOID_END(self, akViewer, akTarget);

DEFVOID_START(Form, RegisterForSingleLOSLost, Value<TESForm> self, Value<Actor> akViewer, Value<TESObjectREFR> akTarget);
DEFVOID_END(self, akViewer, akTarget);

DEFVOID_START(Form, RegisterForSingleUpdate, Value<TESForm> self, float afInterval);
DEFVOID_END(self, afInterval);

DEFVOID_START(Form, RegisterForSleep, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, RegisterForTrackedStatsEvent, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, RegisterForUpdate, Value<TESForm> self, float afInterval);
DEFVOID_END(self, afInterval);

DEFVOID_START(Form, RegisterForUpdateGameTime, Value<TESForm> self, float afInterval);
DEFVOID_END(self, afInterval);

DEFVOID_START(Form, RegisterForSingleUpdateGameTime, Value<TESForm> self, float afInterval);
DEFVOID_END(self, afInterval);

DEFVOID_START(Form, StartObjectProfiling, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, StopObjectProfiling, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, UnregisterForAnimationEvent, Value<TESForm> self, Value<TESObjectREFR> akSender, std::string asEventName);
DEFVOID_END(self, akSender, asEventName);

DEFVOID_START(Form, UnregisterForLOS, Value<TESForm> self, Value<Actor> akViewer, Value<TESObjectREFR> akTarget);
DEFVOID_END(self, akViewer, akTarget);

DEFVOID_START(Form, UnregisterForSleep, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, UnregisterForTrackedStatsEvent, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, UnregisterForUpdate, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, UnregisterForUpdateGameTime, Value<TESForm> self);
DEFVOID_END(self);

DEFVAL_START(SInt32, Form, GetType, Value<TESForm> self);
DEFVAL_END(self);

DEFVAL_START(std::string, Form, GetName, Value<TESForm> self);
DEFVAL_END(self);

DEFVOID_START(Form, SetName, Value<TESForm> self, std::string name);
DEFVOID_END(self, name);

DEFVAL_START(float, Form, GetWeight, Value<TESForm> self);
DEFVAL_END(self);

DEFVOID_START(Form, SetWeight, Value<TESForm> self, float weight);
DEFVOID_END(self, weight);

DEFVOID_START(Form, SetGoldValue, Value<TESForm> self, SInt32 value);
DEFVOID_END(self, value);

DEFVAL_START(SInt32, Form, GetNumKeywords, Value<TESForm> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSKeyword>, Form, GetNthKeyword, Value<TESForm> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(bool, Form, HasKeywordString, Value<TESForm> self, std::string s);
DEFVAL_END(self, s);

DEFVOID_START(Form, SetPlayerKnows, Value<TESForm> self, bool knows);
DEFVOID_END(self, knows);

DEFVOID_START(Form, RegisterForKey, Value<TESForm> self, SInt32 keyCode);
DEFVOID_END(self, keyCode);

DEFVOID_START(Form, UnregisterForKey, Value<TESForm> self, SInt32 keyCode);
DEFVOID_END(self, keyCode);

DEFVOID_START(Form, UnregisterForAllKeys, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, RegisterForControl, Value<TESForm> self, std::string control);
DEFVOID_END(self, control);

DEFVOID_START(Form, UnregisterForControl, Value<TESForm> self, std::string control);
DEFVOID_END(self, control);

DEFVOID_START(Form, UnregisterForAllControls, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, RegisterForMenu, Value<TESForm> self, std::string menuName);
DEFVOID_END(self, menuName);

DEFVOID_START(Form, UnregisterForMenu, Value<TESForm> self, std::string menuName);
DEFVOID_END(self, menuName);

DEFVOID_START(Form, UnregisterForAllMenus, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, RegisterForModEvent, Value<TESForm> self, std::string eventName, std::string callbackName);
DEFVOID_END(self, eventName, callbackName);

DEFVOID_START(Form, UnregisterForModEvent, Value<TESForm> self, std::string eventName);
DEFVOID_END(self, eventName);

DEFVOID_START(Form, UnregisterForAllModEvents, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, SendModEvent, Value<TESForm> self, std::string eventName, std::string strArg, float numArg);
DEFVOID_END(self, eventName, strArg, numArg);

DEFVOID_START(Form, RegisterForCameraState, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, UnregisterForCameraState, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, RegisterForCrosshairRef, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, UnregisterForCrosshairRef, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, RegisterForActorAction, Value<TESForm> self, SInt32 actionType);
DEFVOID_END(self, actionType);

DEFVOID_START(Form, UnregisterForActorAction, Value<TESForm> self, SInt32 actionType);
DEFVOID_END(self, actionType);

DEFVOID_START(Form, RegisterForNiNodeUpdate, Value<TESForm> self);
DEFVOID_END(self);

DEFVOID_START(Form, UnregisterForNiNodeUpdate, Value<TESForm> self);
DEFVOID_END(self);

DEFVAL_START(Value<TESForm>, Form, TempClone, Value<TESForm> self);
DEFVAL_END(self);

DEFVAL_START(bool, Form, HasWorldModel, Value<TESForm> self);
DEFVAL_END(self);

DEFVAL_START(std::string, Form, GetWorldModelPath, Value<TESForm> self);
DEFVAL_END(self);

DEFVOID_START(Form, SetWorldModelPath, Value<TESForm> self, std::string path);
DEFVOID_END(self, path);

DEFVAL_START(SInt32, Form, GetWorldModelNumTextureSets, Value<TESForm> self);
DEFVAL_END(self);

//DEFVAL_START(Value<BGSTextureSet>, Form, GetWorldModelNthTextureSet, Value<TESForm> self, SInt32 n);
//DEFVAL_END(self, n);

//DEFVOID_START(Form, SetWorldModelNthTextureSet, Value<TESForm> self, Value<BGSTextureSet> nSet, SInt32 n);
//DEFVOID_END(self, nSet, n);

DEFVAL_START(bool, Form, IsPlayable, Value<TESForm> self);
DEFVAL_END(self);

// FormList.psc

DEFVOID_START(FormList, AddForm, Value<BGSListForm> self, Value<TESForm> apForm);
DEFVOID_END(self, apForm);

DEFVAL_START(SInt32, FormList, Find, Value<BGSListForm> self, Value<TESForm> apForm);
DEFVAL_END(self, apForm);

DEFVAL_START(SInt32, FormList, GetSize, Value<BGSListForm> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESForm>, FormList, GetAt, Value<BGSListForm> self, SInt32 aiIndex);
DEFVAL_END(self, aiIndex);

DEFVAL_START(bool, FormList, HasForm, Value<BGSListForm> self, Value<TESForm> akForm);
DEFVAL_END(self, akForm);

DEFVOID_START(FormList, RemoveAddedForm, Value<BGSListForm> self, Value<TESForm> apForm);
DEFVOID_END(self, apForm);

DEFVOID_START(FormList, Revert, Value<BGSListForm> self);
DEFVOID_END(self);

// Furniture.psc

// Game.psc

DEFVOID_START(Game, AddAchievement, SInt32 aiAchievementID);
DEFVOID_END(aiAchievementID);

DEFVOID_START(Game, AddPerkPoints, SInt32 aiPerkPoints);
DEFVOID_END(aiPerkPoints);

DEFVOID_START(Game, AdvanceSkill, std::string asSkillName, float afMagnitude);
DEFVOID_END(asSkillName, afMagnitude);

DEFVAL_START(bool, Game, AddHavokBallAndSocketConstraint, Value<TESObjectREFR> arRefA, std::string arRefANode, Value<TESObjectREFR> arRefB, std::string arRefBNode, float afRefALocalOffsetX, float afRefALocalOffsetY, float afRefALocalOffsetZ, float afRefBLocalOffsetX, float afRefBLocalOffsetY, float afRefBLocalOffsetZ);
DEFVAL_END(arRefA, arRefANode, arRefB, arRefBNode, afRefALocalOffsetX, afRefALocalOffsetY, afRefALocalOffsetZ, afRefBLocalOffsetX, afRefBLocalOffsetY, afRefBLocalOffsetZ);

DEFVAL_START(bool, Game, RemoveHavokConstraints, Value<TESObjectREFR> arFirstRef, std::string arFirstRefNodeName, Value<TESObjectREFR> arSecondRef, std::string arSecondRefNodeName);
DEFVAL_END(arFirstRef, arFirstRefNodeName, arSecondRef, arSecondRefNodeName);

DEFVAL_START(SInt32, Game, CalculateFavorCost, SInt32 aiFavorPrice);
DEFVAL_END(aiFavorPrice);

DEFVOID_START_ZEROARGS(Game, ClearPrison);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Game, ClearTempEffects);
DEFVOID_END(NULL);

DEFVOID_START(Game, DisablePlayerControls, bool abMovement, bool abFighting, bool abCamSwitch, bool abLooking, bool abSneaking, bool abMenu, bool abActivate, bool abJournalTabs, SInt32 aiDisablePOVType);
DEFVOID_END(abMovement, abFighting, abCamSwitch, abLooking, abSneaking, abMenu, abActivate, abJournalTabs, aiDisablePOVType);

DEFVOID_START(Game, EnablePlayerControls, bool abMovement, bool abFighting, bool abCamSwitch, bool abLooking, bool abSneaking, bool abMenu, bool abActivate, bool abJournalTabs, SInt32 aiDisablePOVType);
DEFVOID_END(abMovement, abFighting, abCamSwitch, abLooking, abSneaking, abMenu, abActivate, abJournalTabs, aiDisablePOVType);

DEFVOID_START(Game, EnableFastTravel, bool abEnable);
DEFVOID_END(abEnable);

DEFVOID_START(Game, FadeOutGame, bool abFadingOut, bool abBlackFade, float afSecsBeforeFade, float afFadeDuration);
DEFVOID_END(abFadingOut, abBlackFade, afSecsBeforeFade, afFadeDuration);

DEFVOID_START(Game, FastTravel, Value<TESObjectREFR> akDestination);
DEFVOID_END(akDestination);

DEFVAL_START(Value<TESObjectREFR>, Game, FindClosestReferenceOfType, Value<TESForm> arBaseObject, float afX, float afY, float afZ, float afRadius);
DEFVAL_END(arBaseObject, afX, afY, afZ, afRadius);

DEFVAL_START(Value<TESObjectREFR>, Game, FindRandomReferenceOfType, Value<TESForm> arBaseObject, float afX, float afY, float afZ, float afRadius);
DEFVAL_END(arBaseObject, afX, afY, afZ, afRadius);

DEFVAL_START(Value<TESObjectREFR>, Game, FindClosestReferenceOfAnyTypeInList, Value<BGSListForm> arBaseObjects, float afX, float afY, float afZ, float afRadius);
DEFVAL_END(arBaseObjects, afX, afY, afZ, afRadius);

DEFVAL_START(Value<TESObjectREFR>, Game, FindRandomReferenceOfAnyTypeInList, Value<BGSListForm> arBaseObjects, float afX, float afY, float afZ, float afRadius);
DEFVAL_END(arBaseObjects, afX, afY, afZ, afRadius);

DEFVAL_START(Value<TESObjectREFR>, Game, FindClosestReferenceOfTypeFromRef, Value<TESForm> arBaseObject, Value<TESObjectREFR> arCenter, float afRadius);
DEFVAL_END(arBaseObject, arCenter, afRadius);

DEFVAL_START(Value<TESObjectREFR>, Game, FindRandomReferenceOfTypeFromRef, Value<TESForm> arBaseObject, Value<TESObjectREFR> arCenter, float afRadius);
DEFVAL_END(arBaseObject, arCenter, afRadius);

DEFVAL_START(Value<TESObjectREFR>, Game, FindClosestReferenceOfAnyTypeInListFromRef, Value<BGSListForm> arBaseObjects, Value<TESObjectREFR> arCenter, float afRadius);
DEFVAL_END(arBaseObjects, arCenter, afRadius);

DEFVAL_START(Value<TESObjectREFR>, Game, FindRandomReferenceOfAnyTypeInListFromRef, Value<BGSListForm> arBaseObjects, Value<TESObjectREFR> arCenter, float afRadius);
DEFVAL_END(arBaseObjects, arCenter, afRadius);

DEFVAL_START(Value<Actor>, Game, FindClosestActor, float afX, float afY, float afZ, float afRadius);
DEFVAL_END(afX, afY, afZ, afRadius);

DEFVAL_START(Value<Actor>, Game, FindRandomActor, float afX, float afY, float afZ, float afRadius);
DEFVAL_END(afX, afY, afZ, afRadius);

DEFVAL_START(Value<Actor>, Game, FindClosestActorFromRef, Value<TESObjectREFR> arCenter, float afRadius);
DEFVAL_END(arCenter, afRadius);

DEFVAL_START(Value<Actor>, Game, FindRandomActorFromRef, Value<TESObjectREFR> arCenter, float afRadius);
DEFVAL_END(arCenter, afRadius);

DEFVOID_START_ZEROARGS(Game, ForceThirdPerson);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Game, ForceFirstPerson);
DEFVOID_END(NULL);

DEFVOID_START(Game, ShowFirstPersonGeometry, bool abShow);
DEFVOID_END(abShow);

DEFVAL_START(Value<TESForm>, Game, GetForm, UInt32 aiFormID);
DEFVAL_END(aiFormID);

DEFVAL_START(Value<TESForm>, Game, GetFormFromFile, UInt32 aiFormID, std::string asFilename);
DEFVAL_END(aiFormID, asFilename);

DEFVAL_START(float, Game, GetGameSettingFloat, std::string asGameSetting);
DEFVAL_END(asGameSetting);

DEFVAL_START(SInt32, Game, GetGameSettingInt, std::string asGameSetting);
DEFVAL_END(asGameSetting);

DEFVAL_START(std::string, Game, GetGameSettingString, std::string asGameSetting);
DEFVAL_END(asGameSetting);

//DEFVAL_START_ZEROARGS(Value<Actor>, Game, GetPlayer);
//DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(Value<TESObjectREFR>, Game, GetPlayerGrabbedRef);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(Value<Actor>, Game, GetPlayersLastRiddenHorse);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(float, Game, GetSunPositionX);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(float, Game, GetSunPositionY);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(float, Game, GetSunPositionZ);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(float, Game, GetRealHoursPassed);
DEFVAL_END(NULL);

DEFVOID_START(Game, IncrementSkill, std::string asSkillName);
DEFVOID_END(asSkillName);

DEFVOID_START(Game, IncrementSkillBy, std::string asSkillName, SInt32 aiCount);
DEFVOID_END(asSkillName, aiCount);

DEFVOID_START(Game, IncrementStat, std::string asStatName, SInt32 aiModAmount);
DEFVOID_END(asStatName, aiModAmount);

DEFVAL_START_ZEROARGS(bool, Game, IsActivateControlsEnabled);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(bool, Game, IsCamSwitchControlsEnabled);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(bool, Game, IsFastTravelControlsEnabled);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(bool, Game, IsFastTravelEnabled);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(bool, Game, IsFightingControlsEnabled);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(bool, Game, IsJournalControlsEnabled);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(bool, Game, IsLookingControlsEnabled);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(bool, Game, IsMenuControlsEnabled);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(bool, Game, IsMovementControlsEnabled);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(bool, Game, IsPlayerSungazing);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(bool, Game, IsSneakingControlsEnabled);
DEFVAL_END(NULL);

DEFVAL_START(bool, Game, IsWordUnlocked, Value<TESWordOfPower> akWord);
DEFVAL_END(akWord);

DEFVOID_START(Game, PlayBink, std::string asFileName, bool abInterruptible, bool abMuteAudio, bool abMuteMusic, bool abLetterbox);
DEFVOID_END(asFileName, abInterruptible, abMuteAudio, abMuteMusic, abLetterbox);

DEFVOID_START_ZEROARGS(Game, PrecacheCharGen);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Game, PrecacheCharGenClear);
DEFVOID_END(NULL);

DEFVAL_START(SInt32, Game, QueryStat, std::string asStat);
DEFVAL_END(asStat);

DEFVOID_START_ZEROARGS(Game, QuitToMainMenu);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Game, RequestAutoSave);
DEFVOID_END(NULL);

DEFVOID_START(Game, RequestModel, std::string asModelName);
DEFVOID_END(asModelName);

DEFVOID_START_ZEROARGS(Game, RequestSave);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Game, ServeTime);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Game, SendWereWolfTransformation);
DEFVOID_END(NULL);

DEFVOID_START(Game, SetBeastForm, bool abEntering);
DEFVOID_END(abEntering);

DEFVOID_START(Game, SetCameraTarget, Value<Actor> arTarget);
DEFVOID_END(arTarget);

DEFVOID_START(Game, SetHudCartMode, bool abSetCartMode);
DEFVOID_END(abSetCartMode);

DEFVOID_START(Game, SetInChargen, bool abDisableSaving, bool abDisableWaiting, bool abShowControlsDisabledMessage);
DEFVOID_END(abDisableSaving, abDisableWaiting, abShowControlsDisabledMessage);

DEFVOID_START(Game, SetPlayerAIDriven, bool abAIDriven);
DEFVOID_END(abAIDriven);

DEFVOID_START(Game, SetPlayerReportCrime, bool abReportCrime);
DEFVOID_END(abReportCrime);

DEFVOID_START(Game, SetSittingRotation, float afValue);
DEFVOID_END(afValue);

DEFVOID_START(Game, ShakeCamera, Value<TESObjectREFR> akSource, float afStrength, float afDuration);
DEFVOID_END(akSource, afStrength, afDuration);

DEFVOID_START(Game, ShakeController, float afSmallMotorStrength, float afBigMotorStreangth, float afDuration);
DEFVOID_END(afSmallMotorStrength, afBigMotorStreangth, afDuration);

DEFVOID_START_ZEROARGS(Game, ShowRaceMenu);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Game, ShowLimitedRaceMenu);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Game, ShowTitleSequenceMenu);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Game, HideTitleSequenceMenu);
DEFVOID_END(NULL);

DEFVOID_START(Game, StartTitleSequence, std::string asSequenceName);
DEFVOID_END(asSequenceName);

DEFVOID_START(Game, SetAllowFlyingMountLandingRequests, bool abAllow);
DEFVOID_END(abAllow);

DEFVOID_START(Game, SetSunGazeImageSpaceModifier, Value<TESImageSpaceModifier> apImod);
DEFVOID_END(apImod);

DEFVOID_START(Game, ShowTrainingMenu, Value<Actor> aTrainer);
DEFVOID_END(aTrainer);

DEFVOID_START(Game, TeachWord, Value<TESWordOfPower> akWord);
DEFVOID_END(akWord);

DEFVOID_START(Game, TriggerScreenBlood, SInt32 aiValue);
DEFVOID_END(aiValue);

DEFVOID_START(Game, UnlockWord, Value<TESWordOfPower> akWord);
DEFVOID_END(akWord);

DEFVAL_START_ZEROARGS(bool, Game, UsingGamepad);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(SInt32, Game, GetPerkPoints);
DEFVAL_END(NULL);

DEFVOID_START(Game, SetPerkPoints, SInt32 perkPoints);
DEFVOID_END(perkPoints);

DEFVOID_START(Game, ModPerkPoints, SInt32 perkPoints);
DEFVOID_END(perkPoints);

DEFVAL_START_ZEROARGS(SInt32, Game, GetModCount);
DEFVAL_END(NULL);

DEFVAL_START(SInt32, Game, GetModByName, std::string name);
DEFVAL_END(name);

DEFVAL_START(std::string, Game, GetModName, SInt32 modIndex);
DEFVAL_END(modIndex);

DEFVAL_START(std::string, Game, GetModAuthor, SInt32 modIndex);
DEFVAL_END(modIndex);

DEFVAL_START(std::string, Game, GetModDescription, SInt32 modIndex);
DEFVAL_END(modIndex);

DEFVAL_START(SInt32, Game, GetModDependencyCount, SInt32 modIndex);
DEFVAL_END(modIndex);

DEFVAL_START(SInt32, Game, GetNthModDependency, SInt32 modIndex, SInt32 n);
DEFVAL_END(modIndex, n);

DEFVOID_START(Game, SetGameSettingFloat, std::string setting, float value);
DEFVOID_END(setting, value);

DEFVOID_START(Game, SetGameSettingInt, std::string setting, SInt32 value);
DEFVOID_END(setting, value);

DEFVOID_START(Game, SetGameSettingBool, std::string setting, bool value);
DEFVOID_END(setting, value);

DEFVOID_START(Game, SetGameSettingString, std::string setting, std::string value);
DEFVOID_END(setting, value);

DEFVOID_START(Game, SaveGame, std::string name);
DEFVOID_END(name);

DEFVOID_START(Game, LoadGame, std::string name);
DEFVOID_END(name);

DEFVAL_START_ZEROARGS(SInt32, Game, GetNumTintMasks);
DEFVAL_END(NULL);

DEFVAL_START(SInt32, Game, GetNthTintMaskColor, SInt32 n);
DEFVAL_END(n);

DEFVAL_START(SInt32, Game, GetNthTintMaskType, SInt32 n);
DEFVAL_END(n);

DEFVOID_START(Game, SetNthTintMaskColor, SInt32 n, SInt32 color);
DEFVOID_END(n, color);

DEFVAL_START(std::string, Game, GetNthTintMaskTexturePath, SInt32 n);
DEFVAL_END(n);

DEFVOID_START(Game, SetNthTintMaskTexturePath, std::string path, SInt32 n);
DEFVOID_END(path, n);

DEFVAL_START(SInt32, Game, GetNumTintsByType, SInt32 type);
DEFVAL_END(type);

DEFVAL_START(SInt32, Game, GetTintMaskColor, SInt32 type, SInt32 index);
DEFVAL_END(type, index);

DEFVOID_START(Game, SetTintMaskColor, SInt32 color, SInt32 type, SInt32 index);
DEFVOID_END(color, type, index);

DEFVAL_START(std::string, Game, GetTintMaskTexturePath, SInt32 type, SInt32 index);
DEFVAL_END(type, index);

DEFVOID_START(Game, SetTintMaskTexturePath, std::string path, SInt32 type, SInt32 index);
DEFVOID_END(path, type, index);

DEFVOID_START_ZEROARGS(Game, UpdateTintMaskColors);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Game, UpdateHairColor);
DEFVOID_END(NULL);

DEFVAL_START_ZEROARGS(SInt32, Game, GetCameraState);
DEFVAL_END(NULL);

DEFVOID_START(Game, SetMiscStat, std::string name, SInt32 value);
DEFVOID_END(name, value);

DEFVOID_START(Game, SetPlayersLastRiddenHorse, Value<Actor> horse);
DEFVOID_END(horse);

DEFVAL_START(SInt32, Game, GetSkillLegendaryLevel, std::string actorValue);
DEFVAL_END(actorValue);

DEFVOID_START(Game, SetSkillLegendaryLevel, std::string actorValue, SInt32 level);
DEFVOID_END(actorValue, level);

DEFVAL_START_ZEROARGS(float, Game, GetPlayerExperience);
DEFVAL_END(NULL);

DEFVOID_START(Game, SetPlayerExperience, float exp);
DEFVOID_END(exp);

DEFVAL_START(float, Game, GetExperienceForLevel, SInt32 currentLevel);
DEFVAL_END(currentLevel);

DEFVAL_START_ZEROARGS(bool, Game, GetPlayerMovementMode);
DEFVAL_END(NULL);

DEFVOID_START_ZEROARGS(Game, UpdateThirdPerson);
DEFVOID_END(NULL);

DEFVOID_START(Game, UnbindObjectHotkey, SInt32 hotkey);
DEFVOID_END(hotkey);

DEFVAL_START(Value<TESForm>, Game, GetHotkeyBoundObject, SInt32 hotkey);
DEFVAL_END(hotkey);

DEFVAL_START(bool, Game, IsObjectFavorited, Value<TESForm> form);
DEFVAL_END(form);

//DEFVAL_START(Value<TESForm>, Game, GetFormEx, SInt32 formId);
//DEFVAL_END(formId);

DEFVOID_START(Game, SetPlayerLevel, SInt32 level);
DEFVOID_END(level);

// GlobalVariable.psc

DEFVAL_START(float, GlobalVariable, GetValue, Value<TESGlobal> self);
DEFVAL_END(self);

DEFVOID_START(GlobalVariable, SetValue, Value<TESGlobal> self, float afNewValue);
DEFVOID_END(self, afNewValue);

DEFVAL_START(SInt32, GlobalVariable, GetValueInt, Value<TESGlobal> self);
DEFVAL_END(self);

DEFVOID_START(GlobalVariable, SetValueInt, Value<TESGlobal> self, SInt32 aiNewValue);
DEFVOID_END(self, aiNewValue);

DEFVAL_START(float, GlobalVariable, Mod, Value<TESGlobal> self, float afHowMuch);
DEFVAL_END(self, afHowMuch);

// Hazard.psc

// HeadPart.psc

DEFVAL_START(Value<BGSHeadPart>, HeadPart, GetHeadPart, std::string name);
DEFVAL_END(name);

DEFVAL_START(SInt32, HeadPart, GetType, Value<BGSHeadPart> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, HeadPart, GetNumExtraParts, Value<BGSHeadPart> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSHeadPart>, HeadPart, GetNthExtraPart, Value<BGSHeadPart> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(bool, HeadPart, HasExtraPart, Value<BGSHeadPart> self, Value<BGSHeadPart> p);
DEFVAL_END(self, p);

DEFVAL_START(SInt32, HeadPart, GetIndexOfExtraPart, Value<BGSHeadPart> self, Value<BGSHeadPart> p);
DEFVAL_END(self, p);

DEFVAL_START(Value<BGSListForm>, HeadPart, GetValidRaces, Value<BGSHeadPart> self);
DEFVAL_END(self);

DEFVOID_START(HeadPart, SetValidRaces, Value<BGSHeadPart> self, Value<BGSListForm> vRaces);
DEFVOID_END(self, vRaces);

DEFVAL_START(bool, HeadPart, IsExtraPart, Value<BGSHeadPart> self);
DEFVAL_END(self);

// Idle.psc

// ImageSpaceModifier.psc

DEFVOID_START(ImageSpaceModifier, Apply, Value<TESImageSpaceModifier> self, float afStrength);
DEFVOID_END(self, afStrength);

DEFVOID_START(ImageSpaceModifier, ApplyCrossFade, Value<TESImageSpaceModifier> self, float afFadeDuration);
DEFVOID_END(self, afFadeDuration);

DEFVOID_START(ImageSpaceModifier, PopTo, Value<TESImageSpaceModifier> self, Value<TESImageSpaceModifier> akNewModifier, float afStrength);
DEFVOID_END(self, akNewModifier, afStrength);

DEFVOID_START(ImageSpaceModifier, Remove, Value<TESImageSpaceModifier> self);
DEFVOID_END(self);

DEFVOID_START(ImageSpaceModifier, RemoveCrossFade, float afFadeDuration);
DEFVOID_END(afFadeDuration);

// ImpactDataSet.psc

// Ingredient.psc

DEFVAL_START(bool, Ingredient, IsHostile, Value<IngredientItem> self);
DEFVAL_END(self);

DEFVOID_START(Ingredient, LearnEffect, Value<IngredientItem> self, SInt32 aiIndex);
DEFVOID_END(self, aiIndex);

DEFVAL_START(SInt32, Ingredient, LearnNextEffect, Value<IngredientItem> self);
DEFVAL_END(self);

DEFVOID_START(Ingredient, LearnAllEffects, Value<IngredientItem> self);
DEFVOID_END(self);

DEFVAL_START(SInt32, Ingredient, GetNumEffects, Value<IngredientItem> self);
DEFVAL_END(self);

DEFVAL_START(float, Ingredient, GetNthEffectMagnitude, Value<IngredientItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Ingredient, GetNthEffectArea, Value<IngredientItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Ingredient, GetNthEffectDuration, Value<IngredientItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(Value<EffectSetting>, Ingredient, GetNthEffectMagicEffect, Value<IngredientItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Ingredient, GetCostliestEffectIndex, Value<IngredientItem> self);
DEFVAL_END(self);

DEFVOID_START(Ingredient, SetNthEffectMagnitude, Value<IngredientItem> self, SInt32 index, float value);
DEFVOID_END(self, index, value);

DEFVOID_START(Ingredient, SetNthEffectArea, Value<IngredientItem> self, SInt32 index, SInt32 value);
DEFVOID_END(self, index, value);

DEFVOID_START(Ingredient, SetNthEffectDuration, Value<IngredientItem> self, SInt32 index, SInt32 value);
DEFVOID_END(self, index, value);

DEFVAL_START(bool, Ingredient, GetIsNthEffectKnown, Value<IngredientItem> self, SInt32 index);
DEFVAL_END(self, index);

// Input.psc

DEFVAL_START(bool, Input, IsKeyPressed, SInt32 dxKeycode);
DEFVAL_END(dxKeycode);

DEFVOID_START(Input, TapKey, SInt32 dxKeycode);
DEFVOID_END(dxKeycode);

DEFVOID_START(Input, HoldKey, SInt32 dxKeycode);
DEFVOID_END(dxKeycode);

DEFVOID_START(Input, ReleaseKey, SInt32 dxKeycode);
DEFVOID_END(dxKeycode);

DEFVAL_START_ZEROARGS(SInt32, Input, GetNumKeysPressed);
DEFVAL_END(NULL);

DEFVAL_START(SInt32, Input, GetNthKeyPressed, SInt32 n);
DEFVAL_END(n);

DEFVAL_START(SInt32, Input, GetMappedKey, std::string control, SInt32 deviceType);
DEFVAL_END(control, deviceType);

DEFVAL_START(std::string, Input, GetMappedControl, SInt32 keycode);
DEFVAL_END(keycode);

// Key.psc

// Keyword.psc

DEFVOID_START(Keyword, SendStoryEvent, Value<BGSKeyword> self, Value<BGSLocation> akLoc, Value<TESObjectREFR> akRef1, Value<TESObjectREFR> akRef2, SInt32 aiValue1, SInt32 aiValue2);
DEFVOID_END(self, akLoc, akRef1, akRef2, aiValue1, aiValue2);

DEFVAL_START(bool, Keyword, SendStoryEventAndWait, Value<BGSKeyword> self, Value<BGSLocation> akLoc, Value<TESObjectREFR> akRef1, Value<TESObjectREFR> akRef2, SInt32 aiValue1, SInt32 aiValue2);
DEFVAL_END(self, akLoc, akRef1, akRef2, aiValue1, aiValue2);

DEFVAL_START(Value<BGSKeyword>, Keyword, GetKeyword, std::string key);
DEFVAL_END(key);

DEFVAL_START(std::string, Keyword, GetString, Value<BGSKeyword> self);
DEFVAL_END(self);

// LeveledActor.psc

DEFVOID_START(LeveledActor, AddForm, Value<TESLevCharacter> self, Value<TESForm> apForm, SInt32 aiLevel);
DEFVOID_END(self, apForm, aiLevel);

DEFVOID_START(LeveledActor, Revert, Value<TESLevCharacter> self);
DEFVOID_END(self);

DEFVAL_START(SInt32, LeveledActor, GetNumForms, Value<TESLevCharacter> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESForm>, LeveledActor, GetNthForm, Value<TESLevCharacter> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(SInt32, LeveledActor, GetNthLevel, Value<TESLevCharacter> self, SInt32 n);
DEFVAL_END(self, n);

DEFVOID_START(LeveledActor, SetNthLevel, Value<TESLevCharacter> self, SInt32 n, SInt32 level);
DEFVOID_END(self, n, level);

DEFVAL_START(SInt32, LeveledActor, GetNthCount, Value<TESLevCharacter> self, SInt32 n);
DEFVAL_END(self, n);

DEFVOID_START(LeveledActor, SetNthCount, Value<TESLevCharacter> self, SInt32 n, SInt32 count);
DEFVOID_END(self, n, count);

// LeveledItem.psc

DEFVOID_START(LeveledItem, AddForm, Value<TESLevItem> self, Value<TESForm> apForm, SInt32 aiLevel, SInt32 aiCount);
DEFVOID_END(self, apForm, aiLevel, aiCount);

DEFVOID_START(LeveledItem, Revert, Value<TESLevItem> self);
DEFVOID_END(self);

DEFVAL_START(SInt32, LeveledItem, GetChanceNone, Value<TESLevItem> self);
DEFVAL_END(self);

DEFVOID_START(LeveledItem, SetChanceNone, Value<TESLevItem> self, SInt32 chance);
DEFVOID_END(self, chance);

DEFVAL_START(Value<TESGlobal>, LeveledItem, GetChanceGlobal, Value<TESLevItem> self);
DEFVAL_END(self);

DEFVOID_START(LeveledItem, SetChanceGlobal, Value<TESLevItem> self, Value<TESGlobal> glob);
DEFVOID_END(self, glob);

DEFVAL_START(SInt32, LeveledItem, GetNumForms, Value<TESLevItem> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESForm>, LeveledItem, GetNthForm, Value<TESLevItem> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(SInt32, LeveledItem, GetNthLevel, Value<TESLevItem> self, SInt32 n);
DEFVAL_END(self, n);

DEFVOID_START(LeveledItem, SetNthLevel, Value<TESLevItem> self, SInt32 n, SInt32 level);
DEFVOID_END(self, n, level);

DEFVAL_START(SInt32, LeveledItem, GetNthCount, Value<TESLevItem> self, SInt32 n);
DEFVAL_END(self, n);

DEFVOID_START(LeveledItem, SetNthCount, Value<TESLevItem> self, SInt32 n, SInt32 count);
DEFVOID_END(self, n, count);

// LeveledSpell.psc

DEFVOID_START(LeveledSpell, AddForm, Value<TESLevSpell> self, Value<TESForm> apForm, SInt32 aiLevel);
DEFVOID_END(self, apForm, aiLevel);

DEFVOID_START(LeveledSpell, Revert, Value<TESLevSpell> self);
DEFVOID_END(self);

DEFVAL_START(SInt32, LeveledSpell, GetChanceNone, Value<TESLevSpell> self);
DEFVAL_END(self);

DEFVOID_START(LeveledSpell, SetChanceNone, Value<TESLevSpell> self, SInt32 chance);
DEFVOID_END(self, chance);

DEFVAL_START(SInt32, LeveledSpell, GetNumForms, Value<TESLevSpell> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESForm>, LeveledSpell, GetNthForm, Value<TESLevSpell> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(SInt32, LeveledSpell, GetNthLevel, Value<TESLevSpell> self, SInt32 n);
DEFVAL_END(self, n);

DEFVOID_START(LeveledSpell, SetNthLevel, Value<TESLevSpell> self, SInt32 n, SInt32 level);
DEFVOID_END(self, n, level);

// Location.psc

DEFVAL_START(float, Location, GetKeywordData, Value<BGSLocation> self, Value<BGSKeyword> akKeyword);
DEFVAL_END(self, akKeyword);

DEFVAL_START(SInt32, Location, GetRefTypeAliveCount, Value<BGSLocation> self, Value<BGSLocationRefType> akRefType);
DEFVAL_END(self, akRefType);

DEFVAL_START(SInt32, Location, GetRefTypeDeadCount, Value<BGSLocation> self, Value<BGSLocationRefType> akRefType);
DEFVAL_END(self, akRefType);

DEFVAL_START(bool, Location, HasCommonParent, Value<BGSLocation> self, Value<BGSLocation> akOther, Value<BGSKeyword> akFilter);
DEFVAL_END(self, akOther, akFilter);

DEFVAL_START(bool, Location, HasRefType, Value<BGSLocation> self, Value<BGSLocationRefType> akRefType);
DEFVAL_END(self, akRefType);

DEFVAL_START(bool, Location, IsCleared, Value<BGSLocation> self);
DEFVAL_END(self);

DEFVAL_START(bool, Location, IsChild, Value<BGSLocation> self, Value<BGSLocation> akOther);
DEFVAL_END(self, akOther);

DEFVAL_START(bool, Location, IsLoaded, Value<BGSLocation> self);
DEFVAL_END(self);

DEFVAL_START(bool, Location, IsSameLocation, Value<BGSLocation> self, Value<BGSLocation> akOtherLocation, Value<BGSKeyword> akKeyword);
DEFVAL_END(self, akOtherLocation, akKeyword);

DEFVOID_START(Location, SetKeywordData, Value<BGSLocation> self, Value<BGSKeyword> akKeyword, float afData);
DEFVOID_END(self, akKeyword, afData);

DEFVOID_START(Location, SetCleared, Value<BGSLocation> self, bool abCleared);
DEFVOID_END(self, abCleared);

// LocationRefType.psc

// MagicEffect.psc

DEFVAL_START(std::string, MagicEffect, GetAssociatedSkill, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetAssociatedSkill, Value<EffectSetting> self, std::string skill);
DEFVOID_END(self, skill);

DEFVAL_START(std::string, MagicEffect, GetResistance, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetResistance, Value<EffectSetting> self, std::string skill);
DEFVOID_END(self, skill);

DEFVAL_START(bool, MagicEffect, IsEffectFlagSet, Value<EffectSetting> self, SInt32 flag);
DEFVAL_END(self, flag);

DEFVOID_START(MagicEffect, SetEffectFlag, Value<EffectSetting> self, SInt32 flag);
DEFVOID_END(self, flag);

DEFVOID_START(MagicEffect, ClearEffectFlag, Value<EffectSetting> self, SInt32 flag);
DEFVOID_END(self, flag);

DEFVAL_START(float, MagicEffect, GetCastTime, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetCastTime, Value<EffectSetting> self, float castTime);
DEFVOID_END(self, castTime);

DEFVAL_START(SInt32, MagicEffect, GetSkillLevel, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetSkillLevel, Value<EffectSetting> self, SInt32 level);
DEFVOID_END(self, level);

DEFVAL_START(SInt32, MagicEffect, GetArea, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetArea, Value<EffectSetting> self, SInt32 area);
DEFVOID_END(self, area);

DEFVAL_START(float, MagicEffect, GetSkillUsageMult, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetSkillUsageMult, Value<EffectSetting> self, float usageMult);
DEFVOID_END(self, usageMult);

DEFVAL_START(float, MagicEffect, GetBaseCost, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetBaseCost, Value<EffectSetting> self, float cost);
DEFVOID_END(self, cost);

DEFVAL_START(Value<TESObjectLIGH>, MagicEffect, GetLight, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetLight, Value<EffectSetting> self, Value<TESObjectLIGH> obj);
DEFVOID_END(self, obj);

DEFVAL_START(Value<TESEffectShader>, MagicEffect, GetHitShader, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetHitShader, Value<EffectSetting> self, Value<TESEffectShader> obj);
DEFVOID_END(self, obj);

DEFVAL_START(Value<TESEffectShader>, MagicEffect, GetEnchantShader, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetEnchantShader, Value<EffectSetting> self, Value<TESEffectShader> obj);
DEFVOID_END(self, obj);

DEFVAL_START(Value<BGSProjectile>, MagicEffect, GetProjectile, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetProjectile, Value<EffectSetting> self, Value<BGSProjectile> obj);
DEFVOID_END(self, obj);

DEFVAL_START(Value<BGSExplosion>, MagicEffect, GetExplosion, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetExplosion, Value<EffectSetting> self, Value<BGSExplosion> obj);
DEFVOID_END(self, obj);

DEFVAL_START(Value<BGSArtObject>, MagicEffect, GetCastingArt, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetCastingArt, Value<EffectSetting> self, Value<BGSArtObject> obj);
DEFVOID_END(self, obj);

DEFVAL_START(Value<BGSArtObject>, MagicEffect, GetHitEffectArt, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetHitEffectArt, Value<EffectSetting> self, Value<BGSArtObject> obj);
DEFVOID_END(self, obj);

DEFVAL_START(Value<BGSArtObject>, MagicEffect, GetEnchantArt, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetEnchantArt, Value<EffectSetting> self, Value<BGSArtObject> obj);
DEFVOID_END(self, obj);

DEFVAL_START(Value<BGSImpactDataSet>, MagicEffect, GetImpactDataSet, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetImpactDataSet, Value<EffectSetting> self, Value<BGSImpactDataSet> obj);
DEFVOID_END(self, obj);

DEFVAL_START(Value<SpellItem>, MagicEffect, GetEquipAbility, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetEquipAbility, Value<EffectSetting> self, Value<SpellItem> obj);
DEFVOID_END(self, obj);

DEFVAL_START(Value<TESImageSpaceModifier>, MagicEffect, GetImageSpaceMod, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetImageSpaceMod, Value<EffectSetting> self, Value<TESImageSpaceModifier> obj);
DEFVOID_END(self, obj);

DEFVAL_START(Value<BGSPerk>, MagicEffect, GetPerk, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVOID_START(MagicEffect, SetPerk, Value<EffectSetting> self, Value<BGSPerk> obj);
DEFVOID_END(self, obj);

DEFVAL_START(SInt32, MagicEffect, GetCastingType, Value<EffectSetting> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, MagicEffect, GetDeliveryType, Value<EffectSetting> self);
DEFVAL_END(self);

// Math.psc

DEFVAL_START(float, Math, abs, float afValue);
DEFVAL_END(afValue);

DEFVAL_START(float, Math, acos, float afValue);
DEFVAL_END(afValue);

DEFVAL_START(float, Math, asin, float afValue);
DEFVAL_END(afValue);

DEFVAL_START(float, Math, atan, float afValue);
DEFVAL_END(afValue);

DEFVAL_START(SInt32, Math, Ceiling, float afValue);
DEFVAL_END(afValue);

DEFVAL_START(float, Math, cos, float afValue);
DEFVAL_END(afValue);

DEFVAL_START(float, Math, DegreesToRadians, float afDegrees);
DEFVAL_END(afDegrees);

DEFVAL_START(SInt32, Math, Floor, float afValue);
DEFVAL_END(afValue);

DEFVAL_START(float, Math, pow, float x, float y);
DEFVAL_END(x, y);

DEFVAL_START(float, Math, RadiansToDegrees, float afRadians);
DEFVAL_END(afRadians);

DEFVAL_START(float, Math, sin, float afValue);
DEFVAL_END(afValue);

DEFVAL_START(float, Math, sqrt, float afValue);
DEFVAL_END(afValue);

DEFVAL_START(float, Math, tan, float afValue);
DEFVAL_END(afValue);

DEFVAL_START(SInt32, Math, LeftShift, SInt32 value, SInt32 shiftBy);
DEFVAL_END(value, shiftBy);

DEFVAL_START(SInt32, Math, RightShift, SInt32 value, SInt32 shiftBy);
DEFVAL_END(value, shiftBy);

DEFVAL_START(SInt32, Math, LogicalAnd, SInt32 arg1, SInt32 arg2);
DEFVAL_END(arg1, arg2);

DEFVAL_START(SInt32, Math, LogicalOr, SInt32 arg1, SInt32 arg2);
DEFVAL_END(arg1, arg2);

DEFVAL_START(SInt32, Math, LogicalXor, SInt32 arg1, SInt32 arg2);
DEFVAL_END(arg1, arg2);

DEFVAL_START(SInt32, Math, LogicalNot, SInt32 arg1);
DEFVAL_END(arg1);

DEFVAL_START(float, Math, Log, float arg1);
DEFVAL_END(arg1);

// Message.psc

DEFVAL_START(SInt32, Message, Show, Value<BGSMessage> self, float afArg1, float afArg2, float afArg3, float afArg4, float afArg5, float afArg6, float afArg7, float afArg8, float afArg9);
DEFVAL_END(self, afArg1, afArg2, afArg3, afArg4, afArg5, afArg6, afArg7, afArg8, afArg9);

DEFVOID_START(Message, ShowAsHelpMessage, Value<BGSMessage> self, std::string asEvent, float afDuration, float afInterval, SInt32 aiMaxTimes);
DEFVOID_END(self, asEvent, afDuration, afInterval, aiMaxTimes);

DEFVOID_START(Message, ResetHelpMessage, std::string asEvent);
DEFVOID_END(asEvent);

// MiscObject.psc

// MusicType.psc

DEFVOID_START(MusicType, Add, Value<BGSMusicType> self);
DEFVOID_END(self);

DEFVOID_START(MusicType, Remove, Value<BGSMusicType> self);
DEFVOID_END(self);

// ObjectReference.psc

DEFVAL_START(bool, ObjectReference, rampRumble, Value<TESObjectREFR> self, float power, float duration, float falloff);
DEFVAL_END(self, power, duration, falloff);

DEFVAL_START(bool, ObjectReference, IsNearPlayer, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, IsInInterior, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, MoveToIfUnloaded, Value<TESObjectREFR> self, Value<TESObjectREFR> akTarget, float afXOffset, float afYOffset, float afZOffset);
DEFVAL_END(self, akTarget, afXOffset, afYOffset, afZOffset);

DEFVOID_START(ObjectReference, MoveToWhenUnloaded, Value<TESObjectREFR> self, Value<TESObjectREFR> akTarget, float afXOffset, float afYOffset, float afZOffset);
DEFVOID_END(self, akTarget, afXOffset, afYOffset, afZOffset);

DEFVOID_START(ObjectReference, DeleteWhenAble, Value<TESObjectREFR> self);
DEFVOID_END(self);

DEFVOID_START(ObjectReference, AddKeyIfNeeded, Value<TESObjectREFR> self, Value<TESObjectREFR> ObjectWithNeededKey);
DEFVOID_END(self, ObjectWithNeededKey);

DEFVAL_START(bool, ObjectReference, Activate, Value<TESObjectREFR> self, Value<TESObjectREFR> akActivator, bool abDefaultProcessingOnly);
DEFVAL_END(self, akActivator, abDefaultProcessingOnly);

DEFVAL_START(bool, ObjectReference, AddDependentAnimatedObjectReference, Value<TESObjectREFR> self, Value<TESObjectREFR> akDependent);
DEFVAL_END(self, akDependent);

DEFVOID_START(ObjectReference, AddInventoryEventFilter, Value<TESObjectREFR> self, Value<TESForm> akFilter);
DEFVOID_END(self, akFilter);

DEFVOID_START(ObjectReference, AddItem, Value<TESObjectREFR> self, Value<TESForm> akItemToAdd, SInt32 aiCount, bool abSilent);
DEFVOID_END(self, akItemToAdd, aiCount, abSilent);

DEFVOID_START(ObjectReference, AddToMap, Value<TESObjectREFR> self, bool abAllowFastTravel);
DEFVOID_END(self, abAllowFastTravel);

DEFVOID_START(ObjectReference, ApplyHavokImpulse, Value<TESObjectREFR> self, float afX, float afY, float afZ, float afMagnitude);
DEFVOID_END(self, afX, afY, afZ, afMagnitude);

DEFVOID_START(ObjectReference, BlockActivation, Value<TESObjectREFR> self, bool abBlocked);
DEFVOID_END(self, abBlocked);

DEFVAL_START(SInt32, ObjectReference, CalculateEncounterLevel, Value<TESObjectREFR> self, SInt32 aiDifficulty);
DEFVAL_END(self, aiDifficulty);

DEFVAL_START(bool, ObjectReference, CanFastTravelToMarker, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVOID_START(ObjectReference, ClearDestruction, Value<TESObjectREFR> self);
DEFVOID_END(self);

DEFVOID_START(ObjectReference, CreateDetectionEvent, Value<TESObjectREFR> self, Value<Actor> akOwner, SInt32 aiSoundLevel);
DEFVOID_END(self, akOwner, aiSoundLevel);

DEFVOID_START(ObjectReference, DamageObject, Value<TESObjectREFR> self, float afDamage);
DEFVOID_END(self, afDamage);

DEFVOID_START(ObjectReference, Delete, Value<TESObjectREFR> self);
DEFVOID_END(self);

DEFVOID_START(ObjectReference, Disable, Value<TESObjectREFR> self, bool abFadeOut);
DEFVOID_END(self, abFadeOut);

DEFVOID_START(ObjectReference, DisableNoWait, Value<TESObjectREFR> self, bool abFadeOut);
DEFVOID_END(self, abFadeOut);

DEFVAL_START(Value<TESObjectREFR>, ObjectReference, DropObject, Value<TESObjectREFR> self, Value<TESForm> akObject, SInt32 aiCount);
DEFVAL_END(self, akObject, aiCount);

DEFVOID_START(ObjectReference, Enable, Value<TESObjectREFR> self, bool abFadeIn);
DEFVOID_END(self, abFadeIn);

DEFVOID_START(ObjectReference, EnableFastTravel, Value<TESObjectREFR> self, bool abEnable);
DEFVOID_END(self, abEnable);

DEFVOID_START(ObjectReference, EnableNoWait, Value<TESObjectREFR> self, bool abFadeIn);
DEFVOID_END(self, abFadeIn);

DEFVOID_START(ObjectReference, ForceAddRagdollToWorld, Value<TESObjectREFR> self);
DEFVOID_END(self);

DEFVOID_START(ObjectReference, ForceRemoveRagdollFromWorld, Value<TESObjectREFR> self);
DEFVOID_END(self);

DEFVAL_START(Value<TESNPC>, ObjectReference, GetActorOwner, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, GetAnimationVariableBool, Value<TESObjectREFR> self, std::string arVariableName);
DEFVAL_END(self, arVariableName);

DEFVAL_START(SInt32, ObjectReference, GetAnimationVariableInt, Value<TESObjectREFR> self, std::string arVariableName);
DEFVAL_END(self, arVariableName);

DEFVAL_START(float, ObjectReference, GetAnimationVariableFloat, Value<TESObjectREFR> self, std::string arVariableName);
DEFVAL_END(self, arVariableName);

DEFVAL_START(Value<TESForm>, ObjectReference, GetBaseObject, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, ObjectReference, GetCurrentDestructionStage, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSLocation>, ObjectReference, GetCurrentLocation, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSScene>, ObjectReference, GetCurrentScene, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(float, ObjectReference, GetDistance, Value<TESObjectREFR> self, Value<TESObjectREFR> akOther);
DEFVAL_END(self, akOther);

DEFVAL_START(Value<BGSLocation>, ObjectReference, GetEditorLocation, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESFaction>, ObjectReference, GetFactionOwner, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(float, ObjectReference, GetHeadingAngle, Value<TESObjectREFR> self, Value<TESObjectREFR> akOther);
DEFVAL_END(self, akOther);

DEFVAL_START(float, ObjectReference, GetHeight, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, ObjectReference, GetItemCount, Value<TESObjectREFR> self, Value<TESForm> akItem);
DEFVAL_END(self, akItem);

DEFVAL_START(float, ObjectReference, GetItemHealthPercent, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESKey>, ObjectReference, GetKey, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(float, ObjectReference, GetLength, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESObjectREFR>, ObjectReference, GetLinkedRef, Value<TESObjectREFR> self, Value<BGSKeyword> apKeyword);
DEFVAL_END(self, apKeyword);

DEFVAL_START(SInt32, ObjectReference, GetLockLevel, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, ObjectReference, countLinkedRefChain, Value<TESObjectREFR> self, Value<BGSKeyword> apKeyword, SInt32 maxExpectedLinkedRefs);
DEFVAL_END(self, apKeyword, maxExpectedLinkedRefs);

DEFVAL_START(Value<TESObjectREFR>, ObjectReference, GetNthLinkedRef, Value<TESObjectREFR> self, SInt32 aiLinkedRef);
DEFVAL_END(self, aiLinkedRef);

DEFVOID_START(ObjectReference, EnableLinkChain, Value<TESObjectREFR> self, Value<BGSKeyword> apKeyword);
DEFVOID_END(self, apKeyword);

DEFVOID_START(ObjectReference, DisableLinkChain, Value<TESObjectREFR> self, Value<BGSKeyword> apKeyword, bool abFadeOut);
DEFVOID_END(self, apKeyword, abFadeOut);

DEFVAL_START(float, ObjectReference, GetMass, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, ObjectReference, GetOpenState, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESObjectCELL>, ObjectReference, GetParentCell, Value<TESObjectREFR> self);
DEFVAL_END(self);

/*DEFVAL_START(float, ObjectReference, GetPositionX, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(float, ObjectReference, GetPositionY, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(float, ObjectReference, GetPositionZ, Value<TESObjectREFR> self);
DEFVAL_END(self);*/

DEFVAL_START(float, ObjectReference, GetScale, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, ObjectReference, GetTriggerObjectCount, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSVoiceType>, ObjectReference, GetVoiceType, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(float, ObjectReference, GetWidth, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESWorldSpace>, ObjectReference, GetWorldSpace, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(Value<Actor>, ObjectReference, GetSelfAsActor, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, HasEffectKeyword, Value<TESObjectREFR> self, Value<BGSKeyword> akKeyword);
DEFVAL_END(self, akKeyword);

DEFVAL_START(bool, ObjectReference, HasNode, Value<TESObjectREFR> self, std::string asNodeName);
DEFVAL_END(self, asNodeName);

DEFVAL_START(bool, ObjectReference, HasRefType, Value<TESObjectREFR> self, Value<BGSLocationRefType> akRefType);
DEFVAL_END(self, akRefType);

DEFVOID_START(ObjectReference, IgnoreFriendlyHits, Value<TESObjectREFR> self, bool abIgnore);
DEFVOID_END(self, abIgnore);

DEFVOID_START(ObjectReference, InterruptCast, Value<TESObjectREFR> self);
DEFVOID_END(self);

DEFVAL_START(bool, ObjectReference, IsActivateChild, Value<TESObjectREFR> self, Value<TESObjectREFR> akChild);
DEFVAL_END(self, akChild);

DEFVAL_START(bool, ObjectReference, IsActivationBlocked, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, Is3DLoaded, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, IsDeleted, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, IsDisabled, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, IsEnabled, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, IsFurnitureInUse, Value<TESObjectREFR> self, bool abIgnoreReserved);
DEFVAL_END(self, abIgnoreReserved);

DEFVAL_START(bool, ObjectReference, IsFurnitureMarkerInUse, Value<TESObjectREFR> self, SInt32 aiMarker, bool abIgnoreReserved);
DEFVAL_END(self, aiMarker, abIgnoreReserved);

DEFVAL_START(bool, ObjectReference, IsIgnoringFriendlyHits, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, IsInDialogueWithPlayer, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, IsLockBroken, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, IsLocked, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, IsMapMarkerVisible, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVOID_START(ObjectReference, KnockAreaEffect, Value<TESObjectREFR> self, float afMagnitude, float afRadius);
DEFVOID_END(self, afMagnitude, afRadius);

DEFVOID_START(ObjectReference, Lock, Value<TESObjectREFR> self, bool abLock, bool abAsOwner);
DEFVOID_END(self, abLock, abAsOwner);

DEFVOID_START(ObjectReference, MoveTo, Value<TESObjectREFR> self, Value<TESObjectREFR> akTarget, float afXOffset, float afYOffset, float afZOffset, bool abMatchRotation);
DEFVOID_END(self, akTarget, afXOffset, afYOffset, afZOffset, abMatchRotation);

DEFVOID_START(ObjectReference, MoveToInteractionLocation, Value<TESObjectREFR> self, Value<TESObjectREFR> akTarget);
DEFVOID_END(self, akTarget);

DEFVOID_START(ObjectReference, MoveToMyEditorLocation, Value<TESObjectREFR> self);
DEFVOID_END(self);

DEFVOID_START(ObjectReference, MoveToNode, Value<TESObjectREFR> self, Value<TESObjectREFR> akTarget, std::string asNodeName);
DEFVOID_END(self, akTarget, asNodeName);

DEFVAL_START(Value<TESObjectREFR>, ObjectReference, PlaceAtMe, Value<TESObjectREFR> self, Value<TESForm> akFormToPlace, SInt32 aiCount, bool abForcePersist, bool abInitiallyDisabled);
DEFVAL_END(self, akFormToPlace, aiCount, abForcePersist, abInitiallyDisabled);

DEFVAL_START(Value<Actor>, ObjectReference, PlaceActorAtMe, Value<TESObjectREFR> self, Value<TESNPC> akActorToPlace, SInt32 aiLevelMod, Value<BGSEncounterZone> akZone);
DEFVAL_END(self, akActorToPlace, aiLevelMod, akZone);

DEFVAL_START(bool, ObjectReference, PlayAnimation, Value<TESObjectREFR> self, std::string asAnimation);
DEFVAL_END(self, asAnimation);

DEFVAL_START(bool, ObjectReference, PlayAnimationAndWait, Value<TESObjectREFR> self, std::string asAnimation, std::string asEventName);
DEFVAL_END(self, asAnimation, asEventName);

DEFVAL_START(bool, ObjectReference, PlayGamebryoAnimation, Value<TESObjectREFR> self, std::string asAnimation, bool abStartOver, float afEaseInTime);
DEFVAL_END(self, asAnimation, abStartOver, afEaseInTime);

DEFVAL_START(bool, ObjectReference, PlayImpactEffect, Value<TESObjectREFR> self, Value<BGSImpactDataSet> akImpactEffect, std::string asNodeName, float afPickDirX, float afPickDirY, float afPickDirZ, float afPickLength, bool abApplyNodeRotation, bool abUseNodeLocalRotation);
DEFVAL_END(self, akImpactEffect, asNodeName, afPickDirX, afPickDirY, afPickDirZ, afPickLength, abApplyNodeRotation, abUseNodeLocalRotation);

DEFVAL_START(bool, ObjectReference, PlaySyncedAnimationSS, Value<TESObjectREFR> self, std::string asAnimation1, Value<TESObjectREFR> akObj2, std::string asAnimation2);
DEFVAL_END(self, asAnimation1, akObj2, asAnimation2);

DEFVAL_START(bool, ObjectReference, PlaySyncedAnimationAndWaitSS, Value<TESObjectREFR> self, std::string asAnimation1, std::string asEvent1, Value<TESObjectREFR> akObj2, std::string asAnimation2, std::string asEvent2);
DEFVAL_END(self, asAnimation1, asEvent1, akObj2, asAnimation2, asEvent2);

DEFVOID_START(ObjectReference, PlayTerrainEffect, Value<TESObjectREFR> self, std::string asEffectModelName, std::string asAttachBoneName);
DEFVOID_END(self, asEffectModelName, asAttachBoneName);

DEFVOID_START(ObjectReference, ProcessTrapHit, Value<TESObjectREFR> self, Value<TESObjectREFR> akTrap, float afDamage, float afPushback, float afXVel, float afYVel, float afZVel, float afXPos, float afYPos, float afZPos, SInt32 aeMaterial, float afStagger);
DEFVOID_END(self, akTrap, afDamage, afPushback, afXVel, afYVel, afZVel, afXPos, afYPos, afZPos, aeMaterial, afStagger);

DEFVOID_START(ObjectReference, PushActorAway, Value<TESObjectREFR> self, Value<Actor> akActorToPush, float aiKnockbackForce);
DEFVOID_END(self, akActorToPush, aiKnockbackForce);

DEFVOID_START(ObjectReference, RemoveAllInventoryEventFilters, Value<TESObjectREFR> self);
DEFVOID_END(self);

DEFVOID_START(ObjectReference, RemoveAllItems, Value<TESObjectREFR> self, Value<TESObjectREFR> akTransferTo, bool abKeepOwnership, bool abRemoveQuestItems);
DEFVOID_END(self, akTransferTo, abKeepOwnership, abRemoveQuestItems);

DEFVOID_START(ObjectReference, RemoveInventoryEventFilter, Value<TESObjectREFR> self, Value<TESForm> akFilter);
DEFVOID_END(self, akFilter);

DEFVOID_START(ObjectReference, RemoveItem, Value<TESObjectREFR> self, Value<TESForm> akItemToRemove, SInt32 aiCount, bool abSilent, Value<TESObjectREFR> akOtherContainer);
DEFVOID_END(self, akItemToRemove, aiCount, abSilent, akOtherContainer);

DEFVAL_START(bool, ObjectReference, RemoveDependentAnimatedObjectReference, Value<TESObjectREFR> self, Value<TESObjectREFR> akDependent);
DEFVAL_END(self, akDependent);

DEFVOID_START(ObjectReference, Reset, Value<TESObjectREFR> self, Value<TESObjectREFR> akTarget);
DEFVOID_END(self, akTarget);

//DEFVOID_START(ObjectReference, Say, Value<TESObjectREFR> self, Value<TESTopic> akTopicToSay, Value<Actor> akActorToSpeakAs, bool abSpeakInPlayersHead);
//DEFVOID_END(self, akTopicToSay, akActorToSpeakAs, abSpeakInPlayersHead);

DEFVOID_START(ObjectReference, SendStealAlarm, Value<TESObjectREFR> self, Value<Actor> akThief);
DEFVOID_END(self, akThief);

DEFVOID_START(ObjectReference, SetActorCause, Value<TESObjectREFR> self, Value<Actor> akActor);
DEFVOID_END(self, akActor);

DEFVOID_START(ObjectReference, SetActorOwner, Value<TESObjectREFR> self, Value<TESNPC> akActorBase);
DEFVOID_END(self, akActorBase);

DEFVOID_START(ObjectReference, SetAngle, Value<TESObjectREFR> self, float afXAngle, float afYAngle, float afZAngle);
DEFVOID_END(self, afXAngle, afYAngle, afZAngle);

DEFVOID_START(ObjectReference, SetAnimationVariableBool, Value<TESObjectREFR> self, std::string arVariableName, bool abNewValue);
DEFVOID_END(self, arVariableName, abNewValue);

DEFVOID_START(ObjectReference, SetAnimationVariableInt, Value<TESObjectREFR> self, std::string arVariableName, SInt32 aiNewValue);
DEFVOID_END(self, arVariableName, aiNewValue);

DEFVOID_START(ObjectReference, SetAnimationVariableFloat, Value<TESObjectREFR> self, std::string arVariableName, float afNewValue);
DEFVOID_END(self, arVariableName, afNewValue);

DEFVOID_START(ObjectReference, SetDestroyed, Value<TESObjectREFR> self, bool abDestroyed);
DEFVOID_END(self, abDestroyed);

DEFVOID_START(ObjectReference, SetFactionOwner, Value<TESObjectREFR> self, Value<TESFaction> akFaction);
DEFVOID_END(self, akFaction);

DEFVOID_START(ObjectReference, SetLockLevel, Value<TESObjectREFR> self, SInt32 aiLockLevel);
DEFVOID_END(self, aiLockLevel);

DEFVOID_START(ObjectReference, SetMotionType, Value<TESObjectREFR> self, SInt32 aeMotionType, bool abAllowActivate);
DEFVOID_END(self, aeMotionType, abAllowActivate);

DEFVOID_START(ObjectReference, SetNoFavorAllowed, Value<TESObjectREFR> self, bool abNoFavor);
DEFVOID_END(self, abNoFavor);

DEFVOID_START(ObjectReference, SetOpen, Value<TESObjectREFR> self, bool abOpen);
DEFVOID_END(self, abOpen);

DEFVOID_START(ObjectReference, SetPosition, Value<TESObjectREFR> self, float afX, float afY, float afZ);
DEFVOID_END(self, afX, afY, afZ);

DEFVOID_START(ObjectReference, SetScale, Value<TESObjectREFR> self, float afScale);
DEFVOID_END(self, afScale);

DEFVOID_START(ObjectReference, TranslateTo, Value<TESObjectREFR> self, float afX, float afY, float afZ, float afXAngle, float afYAngle, float afZAngle, float afSpeed, float afMaxRotationSpeed);
DEFVOID_END(self, afX, afY, afZ, afXAngle, afYAngle, afZAngle, afSpeed, afMaxRotationSpeed);

DEFVOID_START(ObjectReference, SplineTranslateTo, Value<TESObjectREFR> self, float afX, float afY, float afZ, float afXAngle, float afYAngle, float afZAngle, float afTangentMagnitude, float afSpeed, float afMaxRotationSpeed);
DEFVOID_END(self, afX, afY, afZ, afXAngle, afYAngle, afZAngle, afTangentMagnitude, afSpeed, afMaxRotationSpeed);

DEFVOID_START(ObjectReference, SplineTranslateToRefNode, Value<TESObjectREFR> self, Value<TESObjectREFR> arTarget, std::string arNodeName, float afTangentMagnitude, float afSpeed, float afMaxRotationSpeed);
DEFVOID_END(self, arTarget, arNodeName, afTangentMagnitude, afSpeed, afMaxRotationSpeed);

DEFVOID_START(ObjectReference, StopTranslation, Value<TESObjectREFR> self);
DEFVOID_END(self);

DEFVOID_START(ObjectReference, TranslateToRef, Value<TESObjectREFR> self, Value<TESObjectREFR> arTarget, float afSpeed, float afMaxRotationSpeed);
DEFVOID_END(self, arTarget, afSpeed, afMaxRotationSpeed);

DEFVOID_START(ObjectReference, SplineTranslateToRef, Value<TESObjectREFR> self, Value<TESObjectREFR> arTarget, float afTangentMagnitude, float afSpeed, float afMaxRotationSpeed);
DEFVOID_END(self, arTarget, afTangentMagnitude, afSpeed, afMaxRotationSpeed);

DEFVOID_START(ObjectReference, TetherToHorse, Value<TESObjectREFR> self, Value<TESObjectREFR> akHorse);
DEFVOID_END(self, akHorse);

DEFVAL_START(bool, ObjectReference, WaitForAnimationEvent, Value<TESObjectREFR> self, std::string asEventName);
DEFVAL_END(self, asEventName);

DEFVAL_START(bool, ObjectReference, IsInLocation, Value<TESObjectREFR> self, Value<BGSLocation> akLocation);
DEFVAL_END(self, akLocation);

DEFVAL_START(SInt32, ObjectReference, GetNumItems, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESForm>, ObjectReference, GetNthForm, Value<TESObjectREFR> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(float, ObjectReference, GetTotalItemWeight, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(float, ObjectReference, GetTotalArmorWeight, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, IsHarvested, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVOID_START(ObjectReference, SetHarvested, Value<TESObjectREFR> self, bool harvested);
DEFVOID_END(self, harvested);

DEFVOID_START(ObjectReference, SetItemHealthPercent, Value<TESObjectREFR> self, float health);
DEFVOID_END(self, health);

DEFVOID_START(ObjectReference, SetItemMaxCharge, Value<TESObjectREFR> self, float maxCharge);
DEFVOID_END(self, maxCharge);

DEFVAL_START(float, ObjectReference, GetItemMaxCharge, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(float, ObjectReference, GetItemCharge, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVOID_START(ObjectReference, SetItemCharge, Value<TESObjectREFR> self, float charge);
DEFVOID_END(self, charge);

DEFVOID_START(ObjectReference, ResetInventory, Value<TESObjectREFR> self);
DEFVOID_END(self);

DEFVAL_START(bool, ObjectReference, IsOffLimits, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(std::string, ObjectReference, GetDisplayName, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(bool, ObjectReference, SetDisplayName, Value<TESObjectREFR> self, std::string name, bool force);
DEFVAL_END(self, name, force);

DEFVAL_START(Value<TESObjectREFR>, ObjectReference, GetEnableParent, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(Value<EnchantmentItem>, ObjectReference, GetEnchantment, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVOID_START(ObjectReference, SetEnchantment, Value<TESObjectREFR> self, Value<EnchantmentItem> source, float maxCharge);
DEFVOID_END(self, source, maxCharge);

DEFVAL_START(SInt32, ObjectReference, GetNumReferenceAliases, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESForm>, ObjectReference, GetNthReferenceAlias, Value<TESObjectREFR> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(Value<AlchemyItem>, ObjectReference, GetPoison, Value<TESObjectREFR> self);
DEFVAL_END(self);

DEFVOID_START(ObjectReference, GetAllForms, Value<TESObjectREFR> self, Value<BGSListForm> toFill);
DEFVOID_END(self, toFill);

// Outfit.psc

DEFVAL_START(SInt32, Outfit, GetNumParts, Value<BGSOutfit> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESForm>, Outfit, GetNthPart, Value<BGSOutfit> self, SInt32 n);
DEFVAL_END(self, n);

// Package.psc

DEFVAL_START(Value<TESQuest>, Package, GetOwningQuest, Value<TESPackage> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESPackage>, Package, GetTemplate, Value<TESPackage> self);
DEFVAL_END(self);

// Perk.psc

DEFVAL_START(Value<BGSPerk>, Perk, GetNextPerk, Value<BGSPerk> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Perk, GetNumEntries, Value<BGSPerk> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Perk, GetNthEntryRank, Value<BGSPerk> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(bool, Perk, SetNthEntryRank, Value<BGSPerk> self, SInt32 n, SInt32 rank);
DEFVAL_END(self, n, rank);

DEFVAL_START(SInt32, Perk, GetNthEntryPriority, Value<BGSPerk> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(bool, Perk, SetNthEntryPriority, Value<BGSPerk> self, SInt32 n, SInt32 priority);
DEFVAL_END(self, n, priority);

DEFVAL_START(Value<TESQuest>, Perk, GetNthEntryQuest, Value<BGSPerk> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(bool, Perk, SetNthEntryQuest, Value<BGSPerk> self, SInt32 n, Value<TESQuest> newQuest);
DEFVAL_END(self, n, newQuest);

DEFVAL_START(SInt32, Perk, GetNthEntryStage, Value<BGSPerk> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(bool, Perk, SetNthEntryStage, Value<BGSPerk> self, SInt32 n, SInt32 stage);
DEFVAL_END(self, n, stage);

DEFVAL_START(Value<SpellItem>, Perk, GetNthEntrySpell, Value<BGSPerk> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(bool, Perk, SetNthEntrySpell, Value<BGSPerk> self, SInt32 n, Value<SpellItem> newSpell);
DEFVAL_END(self, n, newSpell);

DEFVAL_START(Value<TESLevItem>, Perk, GetNthEntryLeveledList, Value<BGSPerk> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(bool, Perk, SetNthEntryLeveledList, Value<BGSPerk> self, SInt32 n, Value<TESLevItem> lList);
DEFVAL_END(self, n, lList);

DEFVAL_START(std::string, Perk, GetNthEntryText, Value<BGSPerk> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(bool, Perk, SetNthEntryText, Value<BGSPerk> self, SInt32 n, std::string newText);
DEFVAL_END(self, n, newText);

DEFVAL_START(float, Perk, GetNthEntryValue, Value<BGSPerk> self, SInt32 n, SInt32 i);
DEFVAL_END(self, n, i);

DEFVAL_START(bool, Perk, SetNthEntryValue, Value<BGSPerk> self, SInt32 n, SInt32 i, float value);
DEFVAL_END(self, n, i, value);

// Potion.psc

DEFVAL_START(bool, Potion, IsHostile, Value<AlchemyItem> self);
DEFVAL_END(self);

DEFVAL_START(bool, Potion, IsFood, Value<AlchemyItem> self);
DEFVAL_END(self);

DEFVAL_START(bool, Potion, IsPoison, Value<AlchemyItem> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Potion, GetNumEffects, Value<AlchemyItem> self);
DEFVAL_END(self);

DEFVAL_START(float, Potion, GetNthEffectMagnitude, Value<AlchemyItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Potion, GetNthEffectArea, Value<AlchemyItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Potion, GetNthEffectDuration, Value<AlchemyItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(Value<EffectSetting>, Potion, GetNthEffectMagicEffect, Value<AlchemyItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Potion, GetCostliestEffectIndex, Value<AlchemyItem> self);
DEFVAL_END(self);

DEFVOID_START(Potion, SetNthEffectMagnitude, Value<AlchemyItem> self, SInt32 index, float value);
DEFVOID_END(self, index, value);

DEFVOID_START(Potion, SetNthEffectArea, Value<AlchemyItem> self, SInt32 index, SInt32 value);
DEFVOID_END(self, index, value);

DEFVOID_START(Potion, SetNthEffectDuration, Value<AlchemyItem> self, SInt32 index, SInt32 value);
DEFVOID_END(self, index, value);

DEFVAL_START(Value<BGSSoundDescriptorForm>, Potion, GetUseSound, Value<AlchemyItem> self);
DEFVAL_END(self);

// Projectile.psc

// Quest.psc

DEFVAL_START(bool, Quest, ModObjectiveGlobal, Value<TESQuest> self, float afModValue, Value<TESGlobal> aModGlobal, SInt32 aiObjectiveID, float afTargetValue, bool abCountingUp, bool abCompleteObjective, bool abRedisplayObjective);
DEFVAL_END(self, afModValue, aModGlobal, aiObjectiveID, afTargetValue, abCountingUp, abCompleteObjective, abRedisplayObjective);

DEFVOID_START(Quest, CompleteAllObjectives, Value<TESQuest> self);
DEFVOID_END(self);

DEFVOID_START(Quest, CompleteQuest, Value<TESQuest> self);
DEFVOID_END(self);

DEFVOID_START(Quest, FailAllObjectives, Value<TESQuest> self);
DEFVOID_END(self);

DEFVAL_START(Value<TESForm>, Quest, GetAlias, Value<TESQuest> self, SInt32 aiAliasID);
DEFVAL_END(self, aiAliasID);

DEFVAL_START(SInt32, Quest, GetCurrentStageID, Value<TESQuest> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Quest, GetStage, Value<TESQuest> self);
DEFVAL_END(self);

DEFVAL_START(bool, Quest, GetStageDone, Value<TESQuest> self, SInt32 aiStage);
DEFVAL_END(self, aiStage);

DEFVAL_START(bool, Quest, IsActive, Value<TESQuest> self);
DEFVAL_END(self);

DEFVAL_START(bool, Quest, IsCompleted, Value<TESQuest> self);
DEFVAL_END(self);

DEFVAL_START(bool, Quest, IsObjectiveCompleted, Value<TESQuest> self, SInt32 aiObjective);
DEFVAL_END(self, aiObjective);

DEFVAL_START(bool, Quest, IsObjectiveDisplayed, Value<TESQuest> self, SInt32 aiObjective);
DEFVAL_END(self, aiObjective);

DEFVAL_START(bool, Quest, IsObjectiveFailed, Value<TESQuest> self, SInt32 aiObjective);
DEFVAL_END(self, aiObjective);

DEFVAL_START(bool, Quest, IsRunning, Value<TESQuest> self);
DEFVAL_END(self);

DEFVAL_START(bool, Quest, IsStageDone, Value<TESQuest> self, SInt32 aiStage);
DEFVAL_END(self, aiStage);

DEFVAL_START(bool, Quest, IsStarting, Value<TESQuest> self);
DEFVAL_END(self);

DEFVAL_START(bool, Quest, IsStopping, Value<TESQuest> self);
DEFVAL_END(self);

DEFVAL_START(bool, Quest, IsStopped, Value<TESQuest> self);
DEFVAL_END(self);

DEFVOID_START(Quest, Reset, Value<TESQuest> self);
DEFVOID_END(self);

DEFVOID_START(Quest, SetActive, Value<TESQuest> self, bool abActive);
DEFVOID_END(self, abActive);

DEFVAL_START(bool, Quest, SetCurrentStageID, Value<TESQuest> self, SInt32 aiStageID);
DEFVAL_END(self, aiStageID);

DEFVOID_START(Quest, SetObjectiveCompleted, Value<TESQuest> self, SInt32 aiObjective, bool abCompleted);
DEFVOID_END(self, aiObjective, abCompleted);

DEFVOID_START(Quest, SetObjectiveDisplayed, Value<TESQuest> self, SInt32 aiObjective, bool abDisplayed, bool abForce);
DEFVOID_END(self, aiObjective, abDisplayed, abForce);

DEFVOID_START(Quest, SetObjectiveFailed, Value<TESQuest> self, SInt32 aiObjective, bool abFailed);
DEFVOID_END(self, aiObjective, abFailed);

DEFVAL_START(bool, Quest, SetStage, Value<TESQuest> self, SInt32 aiStage);
DEFVAL_END(self, aiStage);

DEFVAL_START(bool, Quest, Start, Value<TESQuest> self);
DEFVAL_END(self);

DEFVOID_START(Quest, Stop, Value<TESQuest> self);
DEFVOID_END(self);

DEFVAL_START(bool, Quest, UpdateCurrentInstanceGlobal, Value<TESQuest> self, Value<TESGlobal> aUpdateGlobal);
DEFVAL_END(self, aUpdateGlobal);

DEFVAL_START(Value<TESQuest>, Quest, GetQuest, std::string editorId);
DEFVAL_END(editorId);

DEFVAL_START(std::string, Quest, GetID, Value<TESQuest> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Quest, GetPriority, Value<TESQuest> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Quest, GetNumAliases, Value<TESQuest> self);
DEFVAL_END(self);

DEFVAL_START(Value<TESForm>, Quest, GetNthAlias, Value<TESQuest> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(Value<TESForm>, Quest, GetAliasByName, Value<TESQuest> self, std::string name);
DEFVAL_END(self, name);

DEFVAL_START(Value<TESForm>, Quest, GetAliasById, Value<TESQuest> self, SInt32 aliasId);
DEFVAL_END(self, aliasId);

// Race.psc

DEFVAL_START(SInt32, Race, GetSpellCount, Value<TESRace> self);
DEFVAL_END(self);

DEFVAL_START(Value<SpellItem>, Race, GetNthSpell, Value<TESRace> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(bool, Race, IsRaceFlagSet, Value<TESRace> self, SInt32 n);
DEFVAL_END(self, n);

DEFVOID_START(Race, SetRaceFlag, Value<TESRace> self, SInt32 n);
DEFVOID_END(self, n);

DEFVOID_START(Race, ClearRaceFlag, Value<TESRace> self, SInt32 n);
DEFVOID_END(self, n);

DEFVAL_START(Value<BGSVoiceType>, Race, GetDefaultVoiceType, Value<TESRace> self, bool female);
DEFVAL_END(self, female);

DEFVOID_START(Race, SetDefaultVoiceType, Value<TESRace> self, bool female, Value<BGSVoiceType> voice);
DEFVOID_END(self, female, voice);

DEFVAL_START(Value<TESObjectARMO>, Race, GetSkin, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, SetSkin, Value<TESRace> self, Value<TESObjectARMO> skin);
DEFVOID_END(self, skin);

DEFVAL_START_ZEROARGS(SInt32, Race, GetNumPlayableRaces);
DEFVAL_END(NULL);

DEFVAL_START(Value<TESRace>, Race, GetNthPlayableRace, SInt32 n);
DEFVAL_END(n);

DEFVAL_START(Value<TESRace>, Race, GetRace, std::string editorId);
DEFVAL_END(editorId);

DEFVAL_START(bool, Race, IsPlayable, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, MakePlayable, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, MakeUnplayable, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, IsChildRace, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, MakeChildRace, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, MakeNonChildRace, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, CanFly, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, MakeCanFly, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, MakeNonFlying, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, CanSwim, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, MakeCanSwim, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, MakeNonSwimming, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, CanWalk, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, MakeCanWalk, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, MakeNonWalking, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, IsImmobile, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, MakeImmobile, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, MakeMobile, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, IsNotPushable, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, MakeNotPushable, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, MakePushable, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, NoKnockdowns, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, MakeNoKnockdowns, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, ClearNoKNockdowns, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, NoCombatInWater, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, SetNoCombatInWater, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, ClearNoCombatInWater, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, AvoidsRoads, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, SetAvoidsRoads, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, ClearAvoidsRoads, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, AllowPickpocket, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, SetAllowPickpocket, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, ClearAllowPickpocket, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, AllowPCDialogue, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, SetAllowPCDialogue, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, ClearAllowPCDialogue, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, CantOpenDoors, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, SetCantOpenDoors, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, ClearCantOpenDoors, Value<TESRace> self);
DEFVOID_END(self);

DEFVAL_START(bool, Race, NoShadow, Value<TESRace> self);
DEFVAL_END(self);

DEFVOID_START(Race, SetNoShadow, Value<TESRace> self);
DEFVOID_END(self);

DEFVOID_START(Race, ClearNoShadow, Value<TESRace> self);
DEFVOID_END(self);

// Scroll.psc

DEFVOID_START(Scroll, Cast, Value<ScrollItem> self, Value<TESObjectREFR> akSource, Value<TESObjectREFR> akTarget);
DEFVOID_END(self, akSource, akTarget);

DEFVAL_START(float, Scroll, GetCastTime, Value<ScrollItem> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSPerk>, Scroll, GetPerk, Value<ScrollItem> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Scroll, GetNumEffects, Value<ScrollItem> self);
DEFVAL_END(self);

DEFVAL_START(float, Scroll, GetNthEffectMagnitude, Value<ScrollItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Scroll, GetNthEffectArea, Value<ScrollItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Scroll, GetNthEffectDuration, Value<ScrollItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(Value<EffectSetting>, Scroll, GetNthEffectMagicEffect, Value<ScrollItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Scroll, GetCostliestEffectIndex, Value<ScrollItem> self);
DEFVAL_END(self);

DEFVOID_START(Scroll, SetNthEffectMagnitude, Value<ScrollItem> self, SInt32 index, float value);
DEFVOID_END(self, index, value);

DEFVOID_START(Scroll, SetNthEffectArea, Value<ScrollItem> self, SInt32 index, SInt32 value);
DEFVOID_END(self, index, value);

DEFVOID_START(Scroll, SetNthEffectDuration, Value<ScrollItem> self, SInt32 index, SInt32 value);
DEFVOID_END(self, index, value);

DEFVAL_START(Value<BGSEquipSlot>, Scroll, GetEquipType, Value<ScrollItem> self);
DEFVAL_END(self);

DEFVOID_START(Scroll, SetEquipType, Value<ScrollItem> self, Value<BGSEquipSlot> type);
DEFVOID_END(self, type);

// Shout.psc

DEFVAL_START(Value<TESWordOfPower>, Shout, GetNthWordOfPower, Value<TESShout> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(Value<SpellItem>, Shout, GetNthSpell, Value<TESShout> self, SInt32 n);
DEFVAL_END(self, n);

DEFVAL_START(float, Shout, GetNthRecoveryTime, Value<TESShout> self, SInt32 n);
DEFVAL_END(self, n);

DEFVOID_START(Shout, SetNthWordOfPower, Value<TESShout> self, SInt32 n, Value<TESWordOfPower> aWoop);
DEFVOID_END(self, n, aWoop);

DEFVOID_START(Shout, SetNthSpell, Value<TESShout> self, SInt32 n, Value<SpellItem> aSpell);
DEFVOID_END(self, n, aSpell);

DEFVOID_START(Shout, SetNthRecoveryTime, Value<TESShout> self, SInt32 n, float time);
DEFVOID_END(self, n, time);

// SoulGem.psc

DEFVAL_START(SInt32, SoulGem, GetSoulSize, Value<TESSoulGem> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, SoulGem, GetGemSize, Value<TESSoulGem> self);
DEFVAL_END(self);

// Sound.psc

DEFVAL_START(SInt32, Sound, Play, Value<TESSound> self, Value<TESObjectREFR> akSource);
DEFVAL_END(self, akSource);

DEFVAL_START(bool, Sound, PlayAndWait, Value<TESSound> self, Value<TESObjectREFR> akSource);
DEFVAL_END(self, akSource);

DEFVOID_START(Sound, StopInstance, SInt32 aiPlaybackInstance);
DEFVOID_END(aiPlaybackInstance);

DEFVOID_START(Sound, SetInstanceVolume, SInt32 aiPlaybackInstance, float afVolume);
DEFVOID_END(aiPlaybackInstance, afVolume);

DEFVAL_START(Value<BGSSoundDescriptorForm>, Sound, GetDescriptor, Value<TESSound> self);
DEFVAL_END(self);

// Spell.psc

DEFVOID_START(Spell, Cast, Value<SpellItem> self, Value<TESObjectREFR> akSource, Value<TESObjectREFR> akTarget);
DEFVOID_END(self, akSource, akTarget);

DEFVOID_START(Spell, RemoteCast, Value<SpellItem> self, Value<TESObjectREFR> akSource, Value<Actor> akBlameActor, Value<TESObjectREFR> akTarget);
DEFVOID_END(self, akSource, akBlameActor, akTarget);

DEFVAL_START(bool, Spell, IsHostile, Value<SpellItem> self);
DEFVAL_END(self);

DEFVOID_START(Spell, Preload, Value<SpellItem> self);
DEFVOID_END(self);

DEFVOID_START(Spell, Unload, Value<SpellItem> self);
DEFVOID_END(self);

DEFVAL_START(float, Spell, GetCastTime, Value<SpellItem> self);
DEFVAL_END(self);

DEFVAL_START(Value<BGSPerk>, Spell, GetPerk, Value<SpellItem> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Spell, GetNumEffects, Value<SpellItem> self);
DEFVAL_END(self);

DEFVAL_START(float, Spell, GetNthEffectMagnitude, Value<SpellItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Spell, GetNthEffectArea, Value<SpellItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Spell, GetNthEffectDuration, Value<SpellItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(Value<EffectSetting>, Spell, GetNthEffectMagicEffect, Value<SpellItem> self, SInt32 index);
DEFVAL_END(self, index);

DEFVAL_START(SInt32, Spell, GetCostliestEffectIndex, Value<SpellItem> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Spell, GetMagickaCost, Value<SpellItem> self);
DEFVAL_END(self);

DEFVAL_START(SInt32, Spell, GetEffectiveMagickaCost, Value<SpellItem> self, Value<Actor> caster);
DEFVAL_END(self, caster);

DEFVOID_START(Spell, SetNthEffectMagnitude, Value<SpellItem> self, SInt32 index, float value);
DEFVOID_END(self, index, value);

DEFVOID_START(Spell, SetNthEffectArea, Value<SpellItem> self, SInt32 index, SInt32 value);
DEFVOID_END(self, index, value);

DEFVOID_START(Spell, SetNthEffectDuration, Value<SpellItem> self, SInt32 index, SInt32 value);
DEFVOID_END(self, index, value);

DEFVAL_START(Value<BGSEquipSlot>, Spell, GetEquipType, Value<SpellItem> self);
DEFVAL_END(self);

DEFVOID_START(Spell, SetEquipType, Value<SpellItem> self, Value<BGSEquipSlot> type);
DEFVOID_END(self, type);

// Static.psc

// TalkingActivator.psc

// Topic.psc

DEFVOID_START(Topic, Add, Value<TESTopic> self);
DEFVOID_END(self);

// TreeObject.psc

DEFVAL_START(Value<BGSSoundDescriptorForm>, TreeObject, GetHarvestSound, Value<TESObjectTREE> self);
DEFVAL_END(self);

DEFVOID_START(TreeObject, SetHarvestSound, Value<TESObjectTREE> self, Value<BGSSoundDescriptorForm> akSoundDescriptor);
DEFVOID_END(self, akSoundDescriptor);

DEFVAL_START(Value<TESForm>, TreeObject, GetIngredient, Value<TESObjectTREE> self);
DEFVAL_END(self);

DEFVOID_START(TreeObject, SetIngredient, Value<TESObjectTREE> self, Value<TESForm> akIngredient);
DEFVOID_END(self, akIngredient);

// Utility.psc

DEFVAL_START(std::string, Utility, GameTimeToString, float afGameTime);
DEFVAL_END(afGameTime);

DEFVAL_START_ZEROARGS(float, Utility, GetCurrentGameTime);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(float, Utility, GetCurrentRealTime);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(bool, Utility, IsInMenuMode);
DEFVAL_END(NULL);

DEFVAL_START(SInt32, Utility, RandomInt, SInt32 aiMin, SInt32 aiMax);
DEFVAL_END(aiMin, aiMax);

DEFVAL_START(float, Utility, RandomFloat, float afMin, float afMax);
DEFVAL_END(afMin, afMax);

DEFVOID_START(Utility, SetINIFloat, std::string ini, float value);
DEFVOID_END(ini, value);

DEFVOID_START(Utility, SetINIInt, std::string ini, SInt32 value);
DEFVOID_END(ini, value);

DEFVOID_START(Utility, SetINIBool, std::string ini, bool value);
DEFVOID_END(ini, value);

DEFVOID_START(Utility, SetINIString, std::string ini, std::string value);
DEFVOID_END(ini, value);

DEFVOID_START(Utility, Wait, float afSeconds);
DEFVOID_END(afSeconds);

DEFVOID_START(Utility, WaitGameTime, float afHours);
DEFVOID_END(afHours);

DEFVOID_START(Utility, WaitMenuMode, float afSeconds);
DEFVOID_END(afSeconds);

DEFVAL_START(std::string, Utility, CaptureFrameRate, SInt32 numFrames);
DEFVAL_END(numFrames);

DEFVOID_START_ZEROARGS(Utility, StartFrameRateCapture);
DEFVOID_END(NULL);

DEFVOID_START_ZEROARGS(Utility, EndFrameRateCapture);
DEFVOID_END(NULL);

DEFVAL_START_ZEROARGS(float, Utility, GetAverageFrameRate);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(float, Utility, GetMinFrameRate);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(float, Utility, GetMaxFrameRate);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(SInt32, Utility, GetCurrentMemory);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(SInt32, Utility, GetBudgetCount);
DEFVAL_END(NULL);

DEFVAL_START(SInt32, Utility, GetCurrentBudget, SInt32 aiBudgetNumber);
DEFVAL_END(aiBudgetNumber);

DEFVAL_START(bool, Utility, OverBudget, SInt32 aiBudgetNumber);
DEFVAL_END(aiBudgetNumber);

DEFVAL_START(std::string, Utility, GetBudgetName, SInt32 aiBudgetNumber);
DEFVAL_END(aiBudgetNumber);

DEFVAL_START(float, Utility, GetINIFloat, std::string ini);
DEFVAL_END(ini);

DEFVAL_START(SInt32, Utility, GetINIInt, std::string ini);
DEFVAL_END(ini);

DEFVAL_START(bool, Utility, GetINIBool, std::string ini);
DEFVAL_END(ini);

DEFVAL_START(std::string, Utility, GetINIString, std::string ini);
DEFVAL_END(ini);

// VoiceType.psc

// Weapon.psc

DEFVOID_START(Weapon, Fire, Value<TESObjectWEAP> self, Value<TESObjectREFR> akSource, Value<TESAmmo> akAmmo);
DEFVOID_END(self, akSource, akAmmo);

DEFVAL_START(SInt32, Weapon, GetBaseDamage, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetBaseDamage, Value<TESObjectWEAP> self, SInt32 damage);
DEFVOID_END(self, damage);

DEFVAL_START(SInt32, Weapon, GetCritDamage, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetCritDamage, Value<TESObjectWEAP> self, SInt32 damage);
DEFVOID_END(self, damage);

DEFVAL_START(float, Weapon, GetReach, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetReach, Value<TESObjectWEAP> self, float reach);
DEFVOID_END(self, reach);

DEFVAL_START(float, Weapon, GetMinRange, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetMinRange, Value<TESObjectWEAP> self, float minRange);
DEFVOID_END(self, minRange);

DEFVAL_START(float, Weapon, GetMaxRange, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetMaxRange, Value<TESObjectWEAP> self, float maxRange);
DEFVOID_END(self, maxRange);

DEFVAL_START(float, Weapon, GetSpeed, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetSpeed, Value<TESObjectWEAP> self, float speed);
DEFVOID_END(self, speed);

DEFVAL_START(float, Weapon, GetStagger, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetStagger, Value<TESObjectWEAP> self, float stagger);
DEFVOID_END(self, stagger);

DEFVAL_START(SInt32, Weapon, GetWeaponType, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetWeaponType, Value<TESObjectWEAP> self, SInt32 type);
DEFVOID_END(self, type);

DEFVAL_START(std::string, Weapon, GetModelPath, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetModelPath, Value<TESObjectWEAP> self, std::string path);
DEFVOID_END(self, path);

DEFVAL_START(std::string, Weapon, GetIconPath, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetIconPath, Value<TESObjectWEAP> self, std::string path);
DEFVOID_END(self, path);

DEFVAL_START(std::string, Weapon, GetMessageIconPath, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetMessageIconPath, Value<TESObjectWEAP> self, std::string path);
DEFVOID_END(self, path);

DEFVAL_START(Value<EnchantmentItem>, Weapon, GetEnchantment, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetEnchantment, Value<TESObjectWEAP> self, Value<EnchantmentItem> e);
DEFVOID_END(self, e);

DEFVAL_START(SInt32, Weapon, GetEnchantmentValue, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetEnchantmentValue, Value<TESObjectWEAP> self, SInt32 value);
DEFVOID_END(self, value);

DEFVAL_START(Value<TESObjectSTAT>, Weapon, GetEquippedModel, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetEquippedModel, Value<TESObjectWEAP> self, Value<TESObjectSTAT> model);
DEFVOID_END(self, model);

DEFVAL_START(Value<BGSEquipSlot>, Weapon, GetEquipType, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetEquipType, Value<TESObjectWEAP> self, Value<BGSEquipSlot> type);
DEFVOID_END(self, type);

DEFVAL_START(std::string, Weapon, GetSkill, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetSkill, Value<TESObjectWEAP> self, std::string skill);
DEFVOID_END(self, skill);

DEFVAL_START(std::string, Weapon, GetResist, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetResist, Value<TESObjectWEAP> self, std::string resist);
DEFVOID_END(self, resist);

DEFVAL_START(Value<SpellItem>, Weapon, GetCritEffect, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetCritEffect, Value<TESObjectWEAP> self, Value<SpellItem> ce);
DEFVOID_END(self, ce);

DEFVAL_START(bool, Weapon, GetCritEffectOnDeath, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetCritEffectOnDeath, Value<TESObjectWEAP> self, bool ceod);
DEFVOID_END(self, ceod);

DEFVAL_START(float, Weapon, GetCritMultiplier, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVOID_START(Weapon, SetCritMultiplier, Value<TESObjectWEAP> self, float crit);
DEFVOID_END(self, crit);

DEFVAL_START(Value<TESObjectWEAP>, Weapon, GetTemplate, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVAL_START(bool, Weapon, IsBattleaxe, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVAL_START(bool, Weapon, IsBow, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVAL_START(bool, Weapon, IsDagger, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVAL_START(bool, Weapon, IsGreatsword, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVAL_START(bool, Weapon, IsMace, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVAL_START(bool, Weapon, IsStaff, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVAL_START(bool, Weapon, IsSword, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVAL_START(bool, Weapon, IsWarhammer, Value<TESObjectWEAP> self);
DEFVAL_END(self);

DEFVAL_START(bool, Weapon, IsWarAxe, Value<TESObjectWEAP> self);
DEFVAL_END(self);

// Weather.psc

DEFVOID_START_ZEROARGS(Weather, ReleaseOverride);
DEFVOID_END(NULL);

DEFVOID_START(Weather, ForceActive, Value<TESWeather> self, bool abOverride);
DEFVOID_END(self, abOverride);

DEFVOID_START(Weather, SetActive, Value<TESWeather> self, bool abOverride, bool abAccelerate);
DEFVOID_END(self, abOverride, abAccelerate);

DEFVAL_START(Value<TESWeather>, Weather, FindWeather, SInt32 auiType);
DEFVAL_END(auiType);

DEFVAL_START(SInt32, Weather, GetClassification, Value<TESWeather> self);
DEFVAL_END(self);

DEFVAL_START_ZEROARGS(Value<TESWeather>, Weather, GetCurrentWeather);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(Value<TESWeather>, Weather, GetOutgoingWeather);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(float, Weather, GetCurrentWeatherTransition);
DEFVAL_END(NULL);

DEFVAL_START_ZEROARGS(SInt32, Weather, GetSkyMode);
DEFVAL_END(NULL);

DEFVAL_START(float, Weather, GetSunGlare, Value<TESWeather> self);
DEFVAL_END(self);

DEFVAL_START(float, Weather, GetSunDamage, Value<TESWeather> self);
DEFVAL_END(self);

DEFVAL_START(float, Weather, GetWindDirection, Value<TESWeather> self);
DEFVAL_END(self);

DEFVAL_START(float, Weather, GetWindDirectionRange, Value<TESWeather> self);
DEFVAL_END(self);

DEFVAL_START(float, Weather, GetFogDistance, Value<TESWeather> self, bool day, SInt32 type);
DEFVAL_END(self, day, type);

// WordOfPower.psc

// WorldSpace.psc

/*UInt32 cd::GetFormID(Value<TESForm> self, std::function<void(UInt32)> callback)
{
	TESForm *rawPtr = (TESForm *)self;
	if (!callback)
	{
		if (rawPtr)
			return rawPtr->formID;
	}
	else
	{
		if (rawPtr)
			callback(rawPtr->formID);
	}
	return NULL;
}*/ //danger?

/*Value<TESForm> cd::GetForm(UInt32 aiFormID, std::function<void(Value<TESForm>)> callback)
{
	const Value<TESForm> result = LookupFormByID(aiFormID);

	if (callback)
	{
		callback(result);
		return {};
	}

	return result;
}*/ // danger?

Value<Actor> cd::GetPlayer(std::function<void(Value<Actor>)> callback)
{
	if (callback)
	{
		callback(g_thePlayer);
		return {};
	}
	else
		return g_thePlayer;
}

namespace cd
{
	const std::vector<std::pair<UInt32, const char *>> avs = {
		{ 0, "Aggression" },{ 1, "Confidence" },{ 2, "Energy" },{ 3, "Morality" },{ 4, "Mood" },
		{ 5, "Assistance" },{ 6, "OneHanded" },{ 7, "TwoHanded" },{ 8, "Marksman" },{ 9, "Block" },{ 10, "Smithing" },{ 11, "HeavyArmor" },{ 12, "LightArmor" },
		{ 13, "Pickpocket" },{ 14, "LockPicking" },{ 15, "Sneak" },{ 16, "Alchemy" },{ 17, "SpeechCraft" },{ 18, "Alteration" },{ 19, "Conjuration" },{ 20, "Destruction" },
		{ 21, "Illusion" },{ 22, "Restoration" },{ 23, "Enchanting" },{ 24, "Health" },{ 25, "Magicka" },{ 26, "Stamina" },{ 27, "HealRate" },{ 28, "MagickaRate" },
		{ 29, "StaminaRate" },{ 30, "SpeedMult" },{ 31, "InventoryWeight" },{ 32, "CarryWeight" },{ 33, "CritChance" },{ 34, "MeleeDamage" },{ 35, "UnarmedDamage" },{ 36, "Mass" },
		{ 37, "VoicePoints" },{ 38, "VoiceRate" },{ 39, "DamageResist" },{ 40, "PoisonResist" },{ 41, "FireResist" },{ 42, "ElectricResist" },{ 43, "FrostResist" },{ 44, "MagicResist" },
		{ 45, "NormalWeaponsResist" },{ 46, "PerceptionCondition" },{ 47, "EnduranceCondition" },{ 48, "LeftAttackCondition" },{ 49, "RightAttackCondition" },{ 50, "LeftMobilityCondition" },
		{ 51, "RightMobilityCondition" },{ 52, "BrainCondition" },{ 53, "Paralysis" },{ 54, "Invisibility" },{ 55, "NightEye" },{ 56, "DetectLifeRange" },{ 57, "WaterBreathing" },
		{ 58, "WaterWalking" },{ 59, "IgnoreCrippleLimbs" },{ 60, "Fame" },{ 61, "Infamy" },{ 62, "JumpingBonus" },{ 63, "WardPower" },{ 64, "EquippedItemCharge" },{ 65, "ArmorPerks" },
		{ 66, "ShieldPerks" },{ 67, "WardDeflection" },{ 68, "Variable01" },{ 69, "Variable02" },{ 70, "Variable03" },{ 71, "Variable04" },{ 72, "Variable05" },{ 73, "Variable06" },
		{ 74, "Variable07" },{ 75, "Variable08" },{ 76, "Variable09" },{ 77, "Variable10" },{ 78, "BowSpeedBonus" },{ 79, "FavorActive" },{ 80, "FavorsPerDay" },{ 81, "FavorsPerDayTimer" },
		{ 82, "EquippedStaffCharge" },{ 83, "AbsorbChance" },{ 84, "Blindness" },{ 85, "WeaponSpeedMult" },{ 86, "ShoutRecoveryMult" },{ 87, "BowStaggerBonus" },{ 88, "Telekinesis" },
		{ 89, "FavorPointsBonus" },{ 90, "LastBribedIntimidated" },{ 91, "LastFlattered" },{ 92,"Muffled" },{ 93, "BypassVendorStolenCheck" },{ 94, "BypassVendorKeywordCheck" },
		{ 95, "WaitingForPlayer" },{ 96, "OneHandedMod" },{ 97, "TwoHandedMod" },{ 98, "MarksmanMod" },{ 99, "BlockMod" },{ 100, "SmithingMod" },{ 101, "HeavyArmorMod" },
		{ 102, "LightArmorMod" },{ 103, "PickPocketMod" },{ 104, "LockPickingMod" },{ 105, "SneakMod" },{ 106, "AlchemyMod" },{ 107, "SpeechcraftMod" },{ 108	,"AlterationMod" },
		{ 109, "ConjurationMod" },{ 110, "DestructionMod" },{ 111, "IllusionMod" },{ 112, "RestorationMod" },{ 113, "EnchantingMod" },{ 114, "OneHandedSkillAdvance" },
		{ 115, "TwoHandedSkillAdvance" },{ 116, "MarksmanSkillAdvance" },{ 117, "BlockSkillAdvance" },{ 118, "SmithingSkillAdvance" },{ 119, "HeavyArmorSkillAdvance" },
		{ 120, "LightArmorSkillAdvance" },{ 121, "PickPocketSkillAdvance" },{ 122, "LockPickingSkillAdvance" },{ 123, "SneakSkillAdvance" },{ 124, "AlchemySkillAdvance" },
		{ 125, "SpeechcraftSkillAdvance" },{ 126, "AlterationSkillAdvance" },{ 127, "ConjurationSkillAdvance" },{ 128, "DestructionSkillAdvance" },{ 129, "IllusionSkillAdvance" },
		{ 130, "RestorationSkillAdvance" },{ 131, "EnchantingSkillAdvance" },{ 132, "LeftWeaponSpeedMult" },{ 133	,"DragonSouls" },{ 134, "CombatHealthRegenMult" },
		{ 135, "OneHandedPowerMod" },{ 136, "TwoHandedPowerMod" },{ 137, "MarksmanPowerMod" },{ 138, "BlockPowerMod" },{ 139, "SmithingPowerMod" },{ 140, "HeavyArmorPowerMod" },
		{ 141, "LightArmorPowerMod" },{ 142, "PickPocketPowerMod" },{ 143, "LockPickingPowerMod" },{ 144, "SneakPowerMod" },{ 145, "AlchemyPowerMod" },{ 146, "SpeechcraftPowerMod" },
		{ 147, "AlterationPowerMod" },{ 148, "ConjurationPowerMod" },{ 149, "DestructionPowerMod" },{ 150, "IllusionPowerMod" },{ 151, "RestorationPowerMod" },
		{ 152, "EnchantingPowerMod" },{ 153, "DragonRend" },{ 154, "AttackDamageMult" },{ 155, "CombatHealthRegenMultMod" },{ 156, "CombatHealthRegenMultPowerMod" },
		{ 157, "StaminaRateMult" },{ 158, "HealRatePowerMod" },{ 159, "MagickaRateMod" },{ 160, "GrabActorOffset" },{ 161, "Grabbed" },{ 162, "" },{ 163, "ReflectDamage" }
	};
}

UInt32 cd::GetAVID(std::string asActorValueName, std::function<void(UInt32)> callback)
{
	std::transform(asActorValueName.begin(), asActorValueName.end(), asActorValueName.begin(), ::tolower);

	UInt32 result = ~0;

	std::for_each(avs.begin(), avs.end(), [&](std::pair<UInt32, const char *> val) {
		std::string thisName = val.second;
		std::transform(thisName.begin(), thisName.end(), thisName.begin(), ::tolower);
		if (thisName == asActorValueName)
			result = val.first;
	});

	if (callback)
	{
		callback(result);
		return ~0;
	}
	return result;
}

float cd::GetBaseAV(Value<Actor> self, UInt32 avid, std::function<void(float)> callback)
{
	Actor *ac = self;
	const float result = ac ? ac->GetActorValueBase(avid) : 0;

	if (callback)
	{
		callback(result);
		return 0.0f;
	}
	return result;
}

namespace cd
{
	void SetActorValue(Value<Actor> self, std::string asValueName, float afValue, std::function<void(void)> callback = {});
	DEFVOID_START(Actor, SetActorValue, Value<Actor> self, std::string asValueName, float afValue);
	DEFVOID_END(self, asValueName, afValue);

	void ModActorValue(Value<Actor> self, std::string asValueName, float afValue, std::function<void(void)> callback = {});
	DEFVOID_START(Actor, ModActorValue, Value<Actor> self, std::string asValueName, float afAmount);
	DEFVOID_END(self, asValueName, afAmount);
}

void cd::SetBaseAV(Value<Actor> self, UInt32 avid, float value, std::function<void(void)> callback)
{
	const char *avName = "";
	std::for_each(avs.begin(), avs.end(), [&](std::pair<UInt32, const char *> val) {
		if (avid == val.first)
			avName = val.second;
	});

	SetActorValue(self, avName, value, callback);
}

void cd::ModBaseAV(Value<Actor> self, UInt32 avid, float value, bool colored, std::function<void(void)> callback)
{
	const char *avName = "";
	std::for_each(avs.begin(), avs.end(), [&](std::pair<UInt32, const char *> val) {
		if (avid == val.first)
			avName = val.second;
	});

	if (colored)
		ModActorValue(self, avName, value, callback);
	else
	{
		auto resultValue = GetBaseAV(self, avid) + value;
		SetBaseAV(self, avid, resultValue);
		while ((SInt32)resultValue != (SInt32)GetBaseAV(self, avid))
		{
			if (GetCurrentThreadId() == g_SDThreadId)
				sd::Wait(0);
		}
	}
}

float cd::GetCurrentAV(Value<Actor> self, UInt32 avid, std::function<void(float)> callback)
{
	Actor *ac = self;
	const float result = ac ? ac->GetActorValueCurrent(avid) : 0;

	if (callback)
	{
		callback(result);
		return 0.0f;
	}
	return result;
}

namespace cd
{
	void ForceActorValue(Value<Actor> self, std::string asValueName, float afValue, std::function<void(void)> callback = {});
	DEFVOID_START(Actor, ForceActorValue, Value<Actor> self, std::string asValueName, float afNewValue);
	DEFVOID_END(self, asValueName, afNewValue);

	void RestoreActorValue(Value<Actor> self, std::string asValueName, float afValue, std::function<void(void)> callback = {});
	DEFVOID_START(Actor, RestoreActorValue, Value<Actor> self, std::string asValueName, float afNewValue);
	DEFVOID_END(self, asValueName, afNewValue);

	void DamageActorValue(Value<Actor> self, std::string asValueName, float afValue, std::function<void(void)> callback = {});
	DEFVOID_START(Actor, DamageActorValue, Value<Actor> self, std::string asValueName, float afNewValue);
	DEFVOID_END(self, asValueName, afNewValue);
}

void cd::SetCurrentAV(Value<Actor> self, UInt32 avid, float value, std::function<void(void)> callback)
{
	const char *avName = "";
	std::for_each(avs.begin(), avs.end(), [&](std::pair<UInt32, const char *> val) {
		if (avid == val.first)
			avName = val.second;
	});

	ForceActorValue(self, avName, value, callback);
}

void cd::ModCurrentAV(Value<Actor> self, UInt32 avid, float value, std::function<void(void)> callback)
{
	const char *avName = "";
	std::for_each(avs.begin(), avs.end(), [&](std::pair<UInt32, const char *> val) {
		if (avid == val.first)
			avName = val.second;
	});


	if(value > 0)
		RestoreActorValue(self, avName, value, callback);
	else
		DamageActorValue(self, avName, -value, callback);
}

float cd::GetPositionX(Value<TESObjectREFR> self, std::function<void(float)> callback)
{
	TESObjectREFR *refr = self;
	float result = 0;
	if (refr)
		result = refr->GetPositionX();

	if (callback)
	{
		callback(result);
		return 0.0f;
	}
	return result;
}

float cd::GetPositionY(Value<TESObjectREFR> self, std::function<void(float)> callback)
{
	TESObjectREFR *refr = self;
	float result = 0;
	if (refr)
		result = refr->GetPositionY();

	if (callback)
	{
		callback(result);
		return 0.0f;
	}
	return result;
}

float cd::GetPositionZ(Value<TESObjectREFR> self, std::function<void(float)> callback)
{
	TESObjectREFR *refr = self;
	float result = 0;
	if (refr)
		result = refr->GetPositionZ();

	if (callback)
	{
		callback(result);
		return 0.0f;
	}
	return result;
}

NiPoint3 cd::GetPosition(Value<TESObjectREFR> self, std::function<void(NiPoint3)> callback)
{
	const NiPoint3 result = { GetPositionX(self), GetPositionY(self), GetPositionZ(self) };

	if (callback)
	{
		callback(result);
		return {};
	}
	return result;
}

float cd::GetAngleX(Value<TESObjectREFR> self, std::function<void(float)> callback)
{
	TESObjectREFR *refr = self;
	float result = 0;
	if (refr)
		result = refr->rot.x;

	if (callback)
	{
		callback(result);
		return 0.0f;
	}
	return result;
}

float cd::GetAngleY(Value<TESObjectREFR> self, std::function<void(float)> callback)
{
	TESObjectREFR *refr = self;
	float result = 0;
	if (refr)
		result = refr->rot.y;

	if (callback)
	{
		callback(result);
		return 0.0f;
	}
	return result;
}

float cd::GetAngleZ(Value<TESObjectREFR> self, std::function<void(float)> callback)
{
	TESObjectREFR *refr = self;
	float result = 0;
	if (refr)
		result = refr->rot.z;

	if (callback)
	{
		callback(result);
		return 0.0f;
	}
	return result;
}

NiPoint3 cd::GetAngle(Value<TESObjectREFR> self, std::function<void(NiPoint3)> callback)
{
	const NiPoint3 result = { GetAngleX(self), GetAngleY(self), GetAngleZ(self) };

	if (callback)
	{
		callback(result);
		return {};
	}
	return result;
}

bool cd::IsSneaking(Value<Actor> self, std::function<void(bool)> callback)
{
	Actor *ac = self;
	const bool result = ac && ac->IsSneaking();

	if (callback)
	{
		callback(result);
		return false;
	}
	return result;
}

bool cd::IsRunning(Value<Actor> self, std::function<void(bool)> callback)
{
	Actor *ac = self;
	const bool result = ac && ac->IsRunning();

	if (callback)
	{
		callback(result);
		return false;
	}
	return result;
}

bool cd::IsSprinting(Value<Actor> self, std::function<void(bool)> callback)
{
	Actor *ac = self;
	const bool result = ac && ac->IsSprinting();

	if (callback)
	{
		callback(result);
		return false;
	}
	return result;
}

bool cd::IsTrespassing(Value<Actor> self, std::function<void(bool)> callback)
{
	Actor *ac = self;
	const bool result = ac && ac->IsTrespassing();

	if (callback)
	{
		callback(result);
		return false;
	}
	return result;
}

bool cd::IsUnconscious(Value<Actor> self, std::function<void(bool)> callback)
{
	Actor *ac = self;
	const bool result = ac && ac->IsUnconscious();

	if (callback)
	{
		callback(result);
		return false;
	}
	return result;
}

bool cd::IsWeaponDrawn(Value<Actor> self, std::function<void(bool)> callback)
{
	Actor *ac = self;
	const bool result = ac && ac->IsWeaponDrawn();

	if (callback)
	{
		callback(result);
		return false;
	}
	return result;
}

bool cd::IsSwimming(Value<Actor> self, std::function<void(bool)> callback)
{
	Actor *ac = self;
	const bool result = ac && ac->IsSwimming();

	if (callback)
	{
		callback(result);
		return false;
	}
	return result;
}

Value<TESObjectREFR> cd::GetCurrentConsoleRef(std::function<void(Value<TESObjectREFR>)> callback)
{
	Value<TESObjectREFR> result = sd::GetConsoleSelectedRef();

	if (callback)
	{
		callback(result);
		return {};
	}
	return result;
}

DEFVAL_START_ZEROARGS(Value<TESObjectREFR>, Game, GetCurrentCrosshairRef);
DEFVAL_END(NULL);

void cd::SetPosition(Value<TESObjectREFR> self, NiPoint3 pos, std::function<void(void)> callback)
{
	cd::SetPosition(self, pos.x, pos.y, pos.z, callback);
}

Value<Actor> cd::FindClosestActor(NiPoint3 pos, float afRadius, std::function<void(Value<Actor>)> callback)
{
	return cd::FindClosestActor(pos.x, pos.y, pos.z, afRadius, callback);
}

Value<Actor> cd::FindRandomActor(NiPoint3 pos, float afRadius, std::function<void(Value<Actor>)> callback)
{
	return cd::FindRandomActor(pos.x, pos.y, pos.z, afRadius, callback);
}

FormType cd::GetFormTypeID(Value<TESForm> self, std::function<void(FormType)> callback)
{
	FormType result = FormType::None;
	if (self != nullptr)
	{
		TESForm *form = self;
		if (form != nullptr)
			result = form->formType;
	}

	if (callback)
	{
		callback(result);
		return FormType::None;
	}
	return result;
}

/*Value<TESForm> cd::GetBaseForm(Value<TESObjectREFR> self, std::function<void(Value<TESForm>)> callback)
{
	Value<TESForm> result;
	if (self != nullptr)
	{
		TESObjectREFR *form = self;
		if (form != nullptr)
			result = form->baseForm;
	}

	if (callback)
	{
		callback(result);
		return {};
	}
	return result;
}*/ //DANGER!!!

DEFVAL_START(int32_t, UILIB_1, ShowList, int32_t dummy);
DEFVAL_END(dummy);

DEFVAL_START(int32_t, UILIB_1, ShowListResult, bool dummy);
DEFVAL_END(dummy);

DEFVAL_START(int32_t, UILIB_1, ShowInput, int32_t dummy);
DEFVAL_END(dummy);

DEFVAL_START(std::string, UILIB_1, ShowInputResult, bool dummy);
DEFVAL_END(dummy);

DEFVAL_START(int32_t, UILIB_1, SetDataInput, int32_t dummy);
DEFVAL_END(dummy);

DEFVAL_START(int32_t, UILIB_1, SetDataList, int32_t dummy);
DEFVAL_END(dummy);