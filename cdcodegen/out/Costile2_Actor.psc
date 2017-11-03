Scriptname Costile2_Actor Hidden

;
;
;
; script Actor.psc
;
;
;

; void Actor::ModFavorPoints(...)
function Actor_ModFavorPoints() global
	int session = Costile2.GetSession("Actor_ModFavorPoints")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_iFavorPoints = Costile2.GetInt(session, 1)
	this.ModFavorPoints(cdcg_iFavorPoints)
endFunction

; void Actor::ModFavorPointsWithGlobal(...)
function Actor_ModFavorPointsWithGlobal() global
	int session = Costile2.GetSession("Actor_ModFavorPointsWithGlobal")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	globalvariable cdcg_FavorPointsGlobal = Costile2.GetTESForm(session, 1) as globalvariable
	this.ModFavorPointsWithGlobal(cdcg_FavorPointsGlobal)
endFunction

; void Actor::MakePlayerFriend(...)
function Actor_MakePlayerFriend() global
	int session = Costile2.GetSession("Actor_MakePlayerFriend")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.MakePlayerFriend()
endFunction

; void Actor::AddPerk(...) native
function Actor_AddPerk() global
	int session = Costile2.GetSession("Actor_AddPerk")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	perk cdcg_akPerk = Costile2.GetTESForm(session, 1) as perk
	this.AddPerk(cdcg_akPerk)
endFunction

; bool Actor::AddShout(...) native
bool function Actor_AddShout() global
	int session = Costile2.GetSession("Actor_AddShout")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	shout cdcg_akShout = Costile2.GetTESForm(session, 1) as shout
	return this.AddShout(cdcg_akShout)
endFunction

; bool Actor::AddSpell(...) native
bool function Actor_AddSpell() global
	int session = Costile2.GetSession("Actor_AddSpell")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	spell cdcg_akSpell = Costile2.GetTESForm(session, 1) as spell
	bool cdcg_abVerbose = Costile2.GetBool(session, 2)
	return this.AddSpell(cdcg_akSpell, cdcg_abVerbose)
endFunction

; void Actor::AllowBleedoutDialogue(...) native
function Actor_AllowBleedoutDialogue() global
	int session = Costile2.GetSession("Actor_AllowBleedoutDialogue")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abCanTalk = Costile2.GetBool(session, 1)
	this.AllowBleedoutDialogue(cdcg_abCanTalk)
endFunction

; void Actor::AllowPCDialogue(...) native
function Actor_AllowPCDialogue() global
	int session = Costile2.GetSession("Actor_AllowPCDialogue")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abTalk = Costile2.GetBool(session, 1)
	this.AllowPCDialogue(cdcg_abTalk)
endFunction

; void Actor::AttachAshPile(...) native
function Actor_AttachAshPile() global
	int session = Costile2.GetSession("Actor_AttachAshPile")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	form cdcg_akAshPileBase = Costile2.GetTESForm(session, 1) as form
	this.AttachAshPile(cdcg_akAshPileBase)
endFunction

; bool Actor::CanFlyHere(...) native
bool function Actor_CanFlyHere() global
	int session = Costile2.GetSession("Actor_CanFlyHere")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.CanFlyHere()
endFunction

; void Actor::ClearArrested(...) native
function Actor_ClearArrested() global
	int session = Costile2.GetSession("Actor_ClearArrested")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.ClearArrested()
endFunction

; void Actor::ClearExpressionOverride(...) native
function Actor_ClearExpressionOverride() global
	int session = Costile2.GetSession("Actor_ClearExpressionOverride")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.ClearExpressionOverride()
endFunction

; void Actor::ClearExtraArrows(...) native
function Actor_ClearExtraArrows() global
	int session = Costile2.GetSession("Actor_ClearExtraArrows")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.ClearExtraArrows()
endFunction

; void Actor::ClearForcedLandingMarker(...)
function Actor_ClearForcedLandingMarker() global
	int session = Costile2.GetSession("Actor_ClearForcedLandingMarker")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.ClearForcedLandingMarker()
endFunction

; void Actor::ClearKeepOffsetFromActor(...) native
function Actor_ClearKeepOffsetFromActor() global
	int session = Costile2.GetSession("Actor_ClearKeepOffsetFromActor")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.ClearKeepOffsetFromActor()
endFunction

; void Actor::ClearLookAt(...) native
function Actor_ClearLookAt() global
	int session = Costile2.GetSession("Actor_ClearLookAt")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.ClearLookAt()
endFunction

; void Actor::DamageActorValue(...) native
function Actor_DamageActorValue() global
	int session = Costile2.GetSession("Actor_DamageActorValue")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	float cdcg_afDamage = Costile2.GetFloat(session, 2)
	this.DamageActorValue(cdcg_asValueName, cdcg_afDamage)
endFunction

; void Actor::DamageAV(...)
function Actor_DamageAV() global
	int session = Costile2.GetSession("Actor_DamageAV")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	float cdcg_afDamage = Costile2.GetFloat(session, 2)
	this.DamageAV(cdcg_asValueName, cdcg_afDamage)
endFunction

; bool Actor::Dismount(...) native
bool function Actor_Dismount() global
	int session = Costile2.GetSession("Actor_Dismount")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.Dismount()
endFunction

; void Actor::DispelAllSpells(...) native
function Actor_DispelAllSpells() global
	int session = Costile2.GetSession("Actor_DispelAllSpells")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.DispelAllSpells()
endFunction

; bool Actor::DispelSpell(...) native
bool function Actor_DispelSpell() global
	int session = Costile2.GetSession("Actor_DispelSpell")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	spell cdcg_akSpell = Costile2.GetTESForm(session, 1) as spell
	return this.DispelSpell(cdcg_akSpell)
endFunction

; void Actor::DoCombatSpellApply(...) native
function Actor_DoCombatSpellApply() global
	int session = Costile2.GetSession("Actor_DoCombatSpellApply")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	spell cdcg_akSpell = Costile2.GetTESForm(session, 1) as spell
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 2) as objectreference
	this.DoCombatSpellApply(cdcg_akSpell, cdcg_akTarget)
endFunction

; void Actor::EnableAI(...) native
function Actor_EnableAI() global
	int session = Costile2.GetSession("Actor_EnableAI")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abEnable = Costile2.GetBool(session, 1)
	this.EnableAI(cdcg_abEnable)
endFunction

; void Actor::EndDeferredKill(...) native
function Actor_EndDeferredKill() global
	int session = Costile2.GetSession("Actor_EndDeferredKill")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.EndDeferredKill()
endFunction

; void Actor::EquipItem(...) native
function Actor_EquipItem() global
	int session = Costile2.GetSession("Actor_EquipItem")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	form cdcg_akItem = Costile2.GetTESForm(session, 1) as form
	bool cdcg_abPreventRemoval = Costile2.GetBool(session, 2)
	bool cdcg_abSilent = Costile2.GetBool(session, 3)
	this.EquipItem(cdcg_akItem, cdcg_abPreventRemoval, cdcg_abSilent)
endFunction

; void Actor::EquipShout(...) native
function Actor_EquipShout() global
	int session = Costile2.GetSession("Actor_EquipShout")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	shout cdcg_akShout = Costile2.GetTESForm(session, 1) as shout
	this.EquipShout(cdcg_akShout)
endFunction

; void Actor::EquipSpell(...) native
function Actor_EquipSpell() global
	int session = Costile2.GetSession("Actor_EquipSpell")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	spell cdcg_akSpell = Costile2.GetTESForm(session, 1) as spell
	int cdcg_aiSource = Costile2.GetInt(session, 2)
	this.EquipSpell(cdcg_akSpell, cdcg_aiSource)
endFunction

; void Actor::EvaluatePackage(...) native
function Actor_EvaluatePackage() global
	int session = Costile2.GetSession("Actor_EvaluatePackage")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.EvaluatePackage()
endFunction

; void Actor::ForceActorValue(...) native
function Actor_ForceActorValue() global
	int session = Costile2.GetSession("Actor_ForceActorValue")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	float cdcg_afNewValue = Costile2.GetFloat(session, 2)
	this.ForceActorValue(cdcg_asValueName, cdcg_afNewValue)
endFunction

; void Actor::ForceAV(...)
function Actor_ForceAV() global
	int session = Costile2.GetSession("Actor_ForceAV")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	float cdcg_afNewValue = Costile2.GetFloat(session, 2)
	this.ForceAV(cdcg_asValueName, cdcg_afNewValue)
endFunction

; actorbase Actor::GetActorBase(...)
actorbase function Actor_GetActorBase() global
	int session = Costile2.GetSession("Actor_GetActorBase")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetActorBase()
endFunction

; float Actor::GetActorValue(...) native
float function Actor_GetActorValue() global
	int session = Costile2.GetSession("Actor_GetActorValue")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	return this.GetActorValue(cdcg_asValueName)
endFunction

; float Actor::GetActorValuePercentage(...) native
float function Actor_GetActorValuePercentage() global
	int session = Costile2.GetSession("Actor_GetActorValuePercentage")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	return this.GetActorValuePercentage(cdcg_asValueName)
endFunction

; float Actor::GetAV(...)
float function Actor_GetAV() global
	int session = Costile2.GetSession("Actor_GetAV")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	return this.GetAV(cdcg_asValueName)
endFunction

; float Actor::GetAVPercentage(...)
float function Actor_GetAVPercentage() global
	int session = Costile2.GetSession("Actor_GetAVPercentage")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	return this.GetAVPercentage(cdcg_asValueName)
endFunction

; float Actor::GetBaseActorValue(...) native
float function Actor_GetBaseActorValue() global
	int session = Costile2.GetSession("Actor_GetBaseActorValue")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	return this.GetBaseActorValue(cdcg_asValueName)
endFunction

; float Actor::GetBaseAV(...)
float function Actor_GetBaseAV() global
	int session = Costile2.GetSession("Actor_GetBaseAV")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	return this.GetBaseAV(cdcg_asValueName)
endFunction

; int Actor::GetBribeAmount(...) native
int function Actor_GetBribeAmount() global
	int session = Costile2.GetSession("Actor_GetBribeAmount")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetBribeAmount()
endFunction

; faction Actor::GetCrimeFaction(...) native
faction function Actor_GetCrimeFaction() global
	int session = Costile2.GetSession("Actor_GetCrimeFaction")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetCrimeFaction()
endFunction

; int Actor::GetCombatState(...) native
int function Actor_GetCombatState() global
	int session = Costile2.GetSession("Actor_GetCombatState")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetCombatState()
endFunction

; actor Actor::GetCombatTarget(...) native
actor function Actor_GetCombatTarget() global
	int session = Costile2.GetSession("Actor_GetCombatTarget")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetCombatTarget()
endFunction

; package Actor::GetCurrentPackage(...) native
package function Actor_GetCurrentPackage() global
	int session = Costile2.GetSession("Actor_GetCurrentPackage")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetCurrentPackage()
endFunction

; actor Actor::GetDialogueTarget(...) native
actor function Actor_GetDialogueTarget() global
	int session = Costile2.GetSession("Actor_GetDialogueTarget")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetDialogueTarget()
endFunction

; int Actor::GetEquippedItemType(...) native
int function Actor_GetEquippedItemType() global
	int session = Costile2.GetSession("Actor_GetEquippedItemType")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_aiHand = Costile2.GetInt(session, 1)
	return this.GetEquippedItemType(cdcg_aiHand)
endFunction

; shout Actor::GetEquippedShout(...) native
shout function Actor_GetEquippedShout() global
	int session = Costile2.GetSession("Actor_GetEquippedShout")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetEquippedShout()
endFunction

; weapon Actor::GetEquippedWeapon(...) native
weapon function Actor_GetEquippedWeapon() global
	int session = Costile2.GetSession("Actor_GetEquippedWeapon")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abLeftHand = Costile2.GetBool(session, 1)
	return this.GetEquippedWeapon(cdcg_abLeftHand)
endFunction

; armor Actor::GetEquippedShield(...) native
armor function Actor_GetEquippedShield() global
	int session = Costile2.GetSession("Actor_GetEquippedShield")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetEquippedShield()
endFunction

; spell Actor::GetEquippedSpell(...) native
spell function Actor_GetEquippedSpell() global
	int session = Costile2.GetSession("Actor_GetEquippedSpell")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_aiSource = Costile2.GetInt(session, 1)
	return this.GetEquippedSpell(cdcg_aiSource)
endFunction

; int Actor::GetFactionRank(...) native
int function Actor_GetFactionRank() global
	int session = Costile2.GetSession("Actor_GetFactionRank")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	faction cdcg_akFaction = Costile2.GetTESForm(session, 1) as faction
	return this.GetFactionRank(cdcg_akFaction)
endFunction

; int Actor::GetFactionReaction(...) native
int function Actor_GetFactionReaction() global
	int session = Costile2.GetSession("Actor_GetFactionReaction")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akOther = Costile2.GetTESForm(session, 1) as actor
	return this.GetFactionReaction(cdcg_akOther)
endFunction

; int Actor::GetFlyingState(...) native
int function Actor_GetFlyingState() global
	int session = Costile2.GetSession("Actor_GetFlyingState")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetFlyingState()
endFunction

; objectreference Actor::GetForcedLandingMarker(...) native
objectreference function Actor_GetForcedLandingMarker() global
	int session = Costile2.GetSession("Actor_GetForcedLandingMarker")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetForcedLandingMarker()
endFunction

; int Actor::GetGoldAmount(...) native
int function Actor_GetGoldAmount() global
	int session = Costile2.GetSession("Actor_GetGoldAmount")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetGoldAmount()
endFunction

; int Actor::GetHighestRelationshipRank(...) native
int function Actor_GetHighestRelationshipRank() global
	int session = Costile2.GetSession("Actor_GetHighestRelationshipRank")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetHighestRelationshipRank()
endFunction

; actor Actor::GetKiller(...) native
actor function Actor_GetKiller() global
	int session = Costile2.GetSession("Actor_GetKiller")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetKiller()
endFunction

; int Actor::GetLevel(...) native
int function Actor_GetLevel() global
	int session = Costile2.GetSession("Actor_GetLevel")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetLevel()
endFunction

; float Actor::GetLightLevel(...) native
float function Actor_GetLightLevel() global
	int session = Costile2.GetSession("Actor_GetLightLevel")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetLightLevel()
endFunction

; int Actor::GetLowestRelationshipRank(...) native
int function Actor_GetLowestRelationshipRank() global
	int session = Costile2.GetSession("Actor_GetLowestRelationshipRank")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetLowestRelationshipRank()
endFunction

; actorbase Actor::GetLeveledActorBase(...) native
actorbase function Actor_GetLeveledActorBase() global
	int session = Costile2.GetSession("Actor_GetLeveledActorBase")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetLeveledActorBase()
endFunction

; bool Actor::GetNoBleedoutRecovery(...) native
bool function Actor_GetNoBleedoutRecovery() global
	int session = Costile2.GetSession("Actor_GetNoBleedoutRecovery")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetNoBleedoutRecovery()
endFunction

; bool Actor::GetPlayerControls(...) native
bool function Actor_GetPlayerControls() global
	int session = Costile2.GetSession("Actor_GetPlayerControls")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetPlayerControls()
endFunction

; race Actor::GetRace(...) native
race function Actor_GetRace() global
	int session = Costile2.GetSession("Actor_GetRace")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetRace()
endFunction

; int Actor::GetRelationshipRank(...) native
int function Actor_GetRelationshipRank() global
	int session = Costile2.GetSession("Actor_GetRelationshipRank")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akOther = Costile2.GetTESForm(session, 1) as actor
	return this.GetRelationshipRank(cdcg_akOther)
endFunction

; int Actor::GetSitState(...) native
int function Actor_GetSitState() global
	int session = Costile2.GetSession("Actor_GetSitState")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetSitState()
endFunction

; int Actor::GetSleepState(...) native
int function Actor_GetSleepState() global
	int session = Costile2.GetSession("Actor_GetSleepState")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetSleepState()
endFunction

; float Actor::GetVoiceRecoveryTime(...) native
float function Actor_GetVoiceRecoveryTime() global
	int session = Costile2.GetSession("Actor_GetVoiceRecoveryTime")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetVoiceRecoveryTime()
endFunction

; bool Actor::HasAssociation(...) native
bool function Actor_HasAssociation() global
	int session = Costile2.GetSession("Actor_HasAssociation")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	associationtype cdcg_akAssociation = Costile2.GetTESForm(session, 1) as associationtype
	actor cdcg_akOther = Costile2.GetTESForm(session, 2) as actor
	return this.HasAssociation(cdcg_akAssociation, cdcg_akOther)
endFunction

; bool Actor::HasFamilyRelationship(...) native
bool function Actor_HasFamilyRelationship() global
	int session = Costile2.GetSession("Actor_HasFamilyRelationship")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akOther = Costile2.GetTESForm(session, 1) as actor
	return this.HasFamilyRelationship(cdcg_akOther)
endFunction

; bool Actor::HasLOS(...) native
bool function Actor_HasLOS() global
	int session = Costile2.GetSession("Actor_HasLOS")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	objectreference cdcg_akOther = Costile2.GetTESForm(session, 1) as objectreference
	return this.HasLOS(cdcg_akOther)
endFunction

; bool Actor::HasMagicEffect(...) native
bool function Actor_HasMagicEffect() global
	int session = Costile2.GetSession("Actor_HasMagicEffect")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	magiceffect cdcg_akEffect = Costile2.GetTESForm(session, 1) as magiceffect
	return this.HasMagicEffect(cdcg_akEffect)
endFunction

; bool Actor::HasMagicEffectWithKeyword(...) native
bool function Actor_HasMagicEffectWithKeyword() global
	int session = Costile2.GetSession("Actor_HasMagicEffectWithKeyword")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	keyword cdcg_akKeyword = Costile2.GetTESForm(session, 1) as keyword
	return this.HasMagicEffectWithKeyword(cdcg_akKeyword)
endFunction

; bool Actor::HasParentRelationship(...) native
bool function Actor_HasParentRelationship() global
	int session = Costile2.GetSession("Actor_HasParentRelationship")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akOther = Costile2.GetTESForm(session, 1) as actor
	return this.HasParentRelationship(cdcg_akOther)
endFunction

; bool Actor::HasPerk(...) native
bool function Actor_HasPerk() global
	int session = Costile2.GetSession("Actor_HasPerk")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	perk cdcg_akPerk = Costile2.GetTESForm(session, 1) as perk
	return this.HasPerk(cdcg_akPerk)
endFunction

; bool Actor::HasSpell(...) native
bool function Actor_HasSpell() global
	int session = Costile2.GetSession("Actor_HasSpell")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	form cdcg_akForm = Costile2.GetTESForm(session, 1) as form
	return this.HasSpell(cdcg_akForm)
endFunction

; bool Actor::IsAlarmed(...) native
bool function Actor_IsAlarmed() global
	int session = Costile2.GetSession("Actor_IsAlarmed")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsAlarmed()
endFunction

; bool Actor::IsAlerted(...) native
bool function Actor_IsAlerted() global
	int session = Costile2.GetSession("Actor_IsAlerted")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsAlerted()
endFunction

; bool Actor::IsAllowedToFly(...) native
bool function Actor_IsAllowedToFly() global
	int session = Costile2.GetSession("Actor_IsAllowedToFly")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsAllowedToFly()
endFunction

; bool Actor::IsArrested(...) native
bool function Actor_IsArrested() global
	int session = Costile2.GetSession("Actor_IsArrested")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsArrested()
endFunction

; bool Actor::IsArrestingTarget(...) native
bool function Actor_IsArrestingTarget() global
	int session = Costile2.GetSession("Actor_IsArrestingTarget")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsArrestingTarget()
endFunction

; bool Actor::IsBeingRidden(...) native
bool function Actor_IsBeingRidden() global
	int session = Costile2.GetSession("Actor_IsBeingRidden")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsBeingRidden()
endFunction

; bool Actor::IsBleedingOut(...) native
bool function Actor_IsBleedingOut() global
	int session = Costile2.GetSession("Actor_IsBleedingOut")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsBleedingOut()
endFunction

; bool Actor::IsBribed(...) native
bool function Actor_IsBribed() global
	int session = Costile2.GetSession("Actor_IsBribed")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsBribed()
endFunction

; bool Actor::IsChild(...) native
bool function Actor_IsChild() global
	int session = Costile2.GetSession("Actor_IsChild")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsChild()
endFunction

; bool Actor::IsCommandedActor(...) native
bool function Actor_IsCommandedActor() global
	int session = Costile2.GetSession("Actor_IsCommandedActor")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsCommandedActor()
endFunction

; bool Actor::IsDead(...) native
bool function Actor_IsDead() global
	int session = Costile2.GetSession("Actor_IsDead")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsDead()
endFunction

; bool Actor::IsDetectedBy(...) native
bool function Actor_IsDetectedBy() global
	int session = Costile2.GetSession("Actor_IsDetectedBy")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akOther = Costile2.GetTESForm(session, 1) as actor
	return this.IsDetectedBy(cdcg_akOther)
endFunction

; bool Actor::IsDoingFavor(...) native
bool function Actor_IsDoingFavor() global
	int session = Costile2.GetSession("Actor_IsDoingFavor")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsDoingFavor()
endFunction

; bool Actor::IsEquipped(...) native
bool function Actor_IsEquipped() global
	int session = Costile2.GetSession("Actor_IsEquipped")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	form cdcg_akItem = Costile2.GetTESForm(session, 1) as form
	return this.IsEquipped(cdcg_akItem)
endFunction

; bool Actor::IsEssential(...) native
bool function Actor_IsEssential() global
	int session = Costile2.GetSession("Actor_IsEssential")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsEssential()
endFunction

; bool Actor::IsFlying(...) native
bool function Actor_IsFlying() global
	int session = Costile2.GetSession("Actor_IsFlying")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsFlying()
endFunction

; bool Actor::IsGuard(...) native
bool function Actor_IsGuard() global
	int session = Costile2.GetSession("Actor_IsGuard")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsGuard()
endFunction

; bool Actor::IsGhost(...) native
bool function Actor_IsGhost() global
	int session = Costile2.GetSession("Actor_IsGhost")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsGhost()
endFunction

; bool Actor::IsHostileToActor(...) native
bool function Actor_IsHostileToActor() global
	int session = Costile2.GetSession("Actor_IsHostileToActor")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akActor = Costile2.GetTESForm(session, 1) as actor
	return this.IsHostileToActor(cdcg_akActor)
endFunction

; bool Actor::IsInCombat(...) native
bool function Actor_IsInCombat() global
	int session = Costile2.GetSession("Actor_IsInCombat")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsInCombat()
endFunction

; bool Actor::IsInFaction(...) native
bool function Actor_IsInFaction() global
	int session = Costile2.GetSession("Actor_IsInFaction")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	faction cdcg_akFaction = Costile2.GetTESForm(session, 1) as faction
	return this.IsInFaction(cdcg_akFaction)
endFunction

; bool Actor::IsInKillMove(...) native
bool function Actor_IsInKillMove() global
	int session = Costile2.GetSession("Actor_IsInKillMove")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsInKillMove()
endFunction

; bool Actor::IsIntimidated(...) native
bool function Actor_IsIntimidated() global
	int session = Costile2.GetSession("Actor_IsIntimidated")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsIntimidated()
endFunction

; bool Actor::IsOnMount(...) native
bool function Actor_IsOnMount() global
	int session = Costile2.GetSession("Actor_IsOnMount")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsOnMount()
endFunction

; bool Actor::IsPlayersLastRiddenHorse(...) native
bool function Actor_IsPlayersLastRiddenHorse() global
	int session = Costile2.GetSession("Actor_IsPlayersLastRiddenHorse")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsPlayersLastRiddenHorse()
endFunction

; bool Actor::IsPlayerTeammate(...) native
bool function Actor_IsPlayerTeammate() global
	int session = Costile2.GetSession("Actor_IsPlayerTeammate")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsPlayerTeammate()
endFunction

; bool Actor::IsRunning(...) native
bool function Actor_IsRunning() global
	int session = Costile2.GetSession("Actor_IsRunning")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsRunning()
endFunction

; bool Actor::IsSneaking(...) native
bool function Actor_IsSneaking() global
	int session = Costile2.GetSession("Actor_IsSneaking")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsSneaking()
endFunction

; bool Actor::IsSprinting(...) native
bool function Actor_IsSprinting() global
	int session = Costile2.GetSession("Actor_IsSprinting")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsSprinting()
endFunction

; bool Actor::IsTrespassing(...) native
bool function Actor_IsTrespassing() global
	int session = Costile2.GetSession("Actor_IsTrespassing")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsTrespassing()
endFunction

; bool Actor::IsUnconscious(...) native
bool function Actor_IsUnconscious() global
	int session = Costile2.GetSession("Actor_IsUnconscious")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsUnconscious()
endFunction

; bool Actor::IsWeaponDrawn(...) native
bool function Actor_IsWeaponDrawn() global
	int session = Costile2.GetSession("Actor_IsWeaponDrawn")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsWeaponDrawn()
endFunction

; void Actor::KeepOffsetFromActor(...) native
function Actor_KeepOffsetFromActor() global
	int session = Costile2.GetSession("Actor_KeepOffsetFromActor")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_arTarget = Costile2.GetTESForm(session, 1) as actor
	float cdcg_afOffsetX = Costile2.GetFloat(session, 2)
	float cdcg_afOffsetY = Costile2.GetFloat(session, 3)
	float cdcg_afOffsetZ = Costile2.GetFloat(session, 4)
	float cdcg_afOffsetAngleX = Costile2.GetFloat(session, 5)
	float cdcg_afOffsetAngleY = Costile2.GetFloat(session, 6)
	float cdcg_afOffsetAngleZ = Costile2.GetFloat(session, 7)
	float cdcg_afCatchUpRadius = Costile2.GetFloat(session, 8)
	float cdcg_afFollowRadius = Costile2.GetFloat(session, 9)
	this.KeepOffsetFromActor(cdcg_arTarget, cdcg_afOffsetX, cdcg_afOffsetY, cdcg_afOffsetZ, cdcg_afOffsetAngleX, cdcg_afOffsetAngleY, cdcg_afOffsetAngleZ, cdcg_afCatchUpRadius, cdcg_afFollowRadius)
endFunction

; void Actor::Kill(...) native
function Actor_Kill() global
	int session = Costile2.GetSession("Actor_Kill")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akKiller = Costile2.GetTESForm(session, 1) as actor
	this.Kill(cdcg_akKiller)
endFunction

; void Actor::KillEssential(...)
function Actor_KillEssential() global
	int session = Costile2.GetSession("Actor_KillEssential")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akKiller = Costile2.GetTESForm(session, 1) as actor
	this.KillEssential(cdcg_akKiller)
endFunction

; void Actor::KillSilent(...) native
function Actor_KillSilent() global
	int session = Costile2.GetSession("Actor_KillSilent")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akKiller = Costile2.GetTESForm(session, 1) as actor
	this.KillSilent(cdcg_akKiller)
endFunction

; void Actor::ModActorValue(...) native
function Actor_ModActorValue() global
	int session = Costile2.GetSession("Actor_ModActorValue")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	float cdcg_afAmount = Costile2.GetFloat(session, 2)
	this.ModActorValue(cdcg_asValueName, cdcg_afAmount)
endFunction

; void Actor::ModAV(...)
function Actor_ModAV() global
	int session = Costile2.GetSession("Actor_ModAV")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	float cdcg_afAmount = Costile2.GetFloat(session, 2)
	this.ModAV(cdcg_asValueName, cdcg_afAmount)
endFunction

; void Actor::ModFactionRank(...) native
function Actor_ModFactionRank() global
	int session = Costile2.GetSession("Actor_ModFactionRank")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	faction cdcg_akFaction = Costile2.GetTESForm(session, 1) as faction
	int cdcg_aiMod = Costile2.GetInt(session, 2)
	this.ModFactionRank(cdcg_akFaction, cdcg_aiMod)
endFunction

; void Actor::MoveToPackageLocation(...) native
function Actor_MoveToPackageLocation() global
	int session = Costile2.GetSession("Actor_MoveToPackageLocation")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.MoveToPackageLocation()
endFunction

; void Actor::OpenInventory(...) native
function Actor_OpenInventory() global
	int session = Costile2.GetSession("Actor_OpenInventory")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abForceOpen = Costile2.GetBool(session, 1)
	this.OpenInventory(cdcg_abForceOpen)
endFunction

; bool Actor::PathToReference(...) native
bool function Actor_PathToReference() global
	int session = Costile2.GetSession("Actor_PathToReference")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	objectreference cdcg_aTarget = Costile2.GetTESForm(session, 1) as objectreference
	float cdcg_afWalkRunPercent = Costile2.GetFloat(session, 2)
	return this.PathToReference(cdcg_aTarget, cdcg_afWalkRunPercent)
endFunction

; bool Actor::PlayIdle(...) native
bool function Actor_PlayIdle() global
	int session = Costile2.GetSession("Actor_PlayIdle")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	idle cdcg_akIdle = Costile2.GetTESForm(session, 1) as idle
	return this.PlayIdle(cdcg_akIdle)
endFunction

; bool Actor::PlayIdleWithTarget(...) native
bool function Actor_PlayIdleWithTarget() global
	int session = Costile2.GetSession("Actor_PlayIdleWithTarget")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	idle cdcg_akIdle = Costile2.GetTESForm(session, 1) as idle
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 2) as objectreference
	return this.PlayIdleWithTarget(cdcg_akIdle, cdcg_akTarget)
endFunction

; void Actor::PlaySubGraphAnimation(...) native
function Actor_PlaySubGraphAnimation() global
	int session = Costile2.GetSession("Actor_PlaySubGraphAnimation")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asEventName = Costile2.GetString(session, 1)
	this.PlaySubGraphAnimation(cdcg_asEventName)
endFunction

; void Actor::RemoveFromFaction(...) native
function Actor_RemoveFromFaction() global
	int session = Costile2.GetSession("Actor_RemoveFromFaction")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	faction cdcg_akFaction = Costile2.GetTESForm(session, 1) as faction
	this.RemoveFromFaction(cdcg_akFaction)
endFunction

; void Actor::RemoveFromAllFactions(...) native
function Actor_RemoveFromAllFactions() global
	int session = Costile2.GetSession("Actor_RemoveFromAllFactions")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.RemoveFromAllFactions()
endFunction

; void Actor::RemovePerk(...) native
function Actor_RemovePerk() global
	int session = Costile2.GetSession("Actor_RemovePerk")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	perk cdcg_akPerk = Costile2.GetTESForm(session, 1) as perk
	this.RemovePerk(cdcg_akPerk)
endFunction

; bool Actor::RemoveShout(...) native
bool function Actor_RemoveShout() global
	int session = Costile2.GetSession("Actor_RemoveShout")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	shout cdcg_akShout = Costile2.GetTESForm(session, 1) as shout
	return this.RemoveShout(cdcg_akShout)
endFunction

; bool Actor::RemoveSpell(...) native
bool function Actor_RemoveSpell() global
	int session = Costile2.GetSession("Actor_RemoveSpell")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	spell cdcg_akSpell = Costile2.GetTESForm(session, 1) as spell
	return this.RemoveSpell(cdcg_akSpell)
endFunction

; void Actor::ResetHealthAndLimbs(...) native
function Actor_ResetHealthAndLimbs() global
	int session = Costile2.GetSession("Actor_ResetHealthAndLimbs")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.ResetHealthAndLimbs()
endFunction

; void Actor::RestoreActorValue(...) native
function Actor_RestoreActorValue() global
	int session = Costile2.GetSession("Actor_RestoreActorValue")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	float cdcg_afAmount = Costile2.GetFloat(session, 2)
	this.RestoreActorValue(cdcg_asValueName, cdcg_afAmount)
endFunction

; void Actor::Resurrect(...) native
function Actor_Resurrect() global
	int session = Costile2.GetSession("Actor_Resurrect")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.Resurrect()
endFunction

; void Actor::RestoreAV(...)
function Actor_RestoreAV() global
	int session = Costile2.GetSession("Actor_RestoreAV")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	float cdcg_afAmount = Costile2.GetFloat(session, 2)
	this.RestoreAV(cdcg_asValueName, cdcg_afAmount)
endFunction

; void Actor::SendAssaultAlarm(...) native
function Actor_SendAssaultAlarm() global
	int session = Costile2.GetSession("Actor_SendAssaultAlarm")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.SendAssaultAlarm()
endFunction

; void Actor::SendTrespassAlarm(...) native
function Actor_SendTrespassAlarm() global
	int session = Costile2.GetSession("Actor_SendTrespassAlarm")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akCriminal = Costile2.GetTESForm(session, 1) as actor
	this.SendTrespassAlarm(cdcg_akCriminal)
endFunction

; void Actor::SetActorValue(...) native
function Actor_SetActorValue() global
	int session = Costile2.GetSession("Actor_SetActorValue")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	float cdcg_afValue = Costile2.GetFloat(session, 2)
	this.SetActorValue(cdcg_asValueName, cdcg_afValue)
endFunction

; void Actor::SetAlert(...) native
function Actor_SetAlert() global
	int session = Costile2.GetSession("Actor_SetAlert")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abAlerted = Costile2.GetBool(session, 1)
	this.SetAlert(cdcg_abAlerted)
endFunction

; void Actor::SetAllowFlying(...) native
function Actor_SetAllowFlying() global
	int session = Costile2.GetSession("Actor_SetAllowFlying")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abAllowed = Costile2.GetBool(session, 1)
	this.SetAllowFlying(cdcg_abAllowed)
endFunction

; void Actor::SetAllowFlyingEx(...) native
function Actor_SetAllowFlyingEx() global
	int session = Costile2.GetSession("Actor_SetAllowFlyingEx")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abAllowed = Costile2.GetBool(session, 1)
	bool cdcg_abAllowCrash = Costile2.GetBool(session, 2)
	bool cdcg_abAllowSearch = Costile2.GetBool(session, 3)
	this.SetAllowFlyingEx(cdcg_abAllowed, cdcg_abAllowCrash, cdcg_abAllowSearch)
endFunction

; void Actor::SetAlpha(...) native
function Actor_SetAlpha() global
	int session = Costile2.GetSession("Actor_SetAlpha")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	float cdcg_afTargetAlpha = Costile2.GetFloat(session, 1)
	bool cdcg_abFade = Costile2.GetBool(session, 2)
	this.SetAlpha(cdcg_afTargetAlpha, cdcg_abFade)
endFunction

; void Actor::SetAttackActorOnSight(...) native
function Actor_SetAttackActorOnSight() global
	int session = Costile2.GetSession("Actor_SetAttackActorOnSight")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abAttackOnSight = Costile2.GetBool(session, 1)
	this.SetAttackActorOnSight(cdcg_abAttackOnSight)
endFunction

; void Actor::SetAV(...)
function Actor_SetAV() global
	int session = Costile2.GetSession("Actor_SetAV")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asValueName = Costile2.GetString(session, 1)
	float cdcg_afValue = Costile2.GetFloat(session, 2)
	this.SetAV(cdcg_asValueName, cdcg_afValue)
endFunction

; void Actor::SetBribed(...) native
function Actor_SetBribed() global
	int session = Costile2.GetSession("Actor_SetBribed")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abBribe = Costile2.GetBool(session, 1)
	this.SetBribed(cdcg_abBribe)
endFunction

; void Actor::SetCrimeFaction(...) native
function Actor_SetCrimeFaction() global
	int session = Costile2.GetSession("Actor_SetCrimeFaction")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	faction cdcg_akFaction = Costile2.GetTESForm(session, 1) as faction
	this.SetCrimeFaction(cdcg_akFaction)
endFunction

; void Actor::SetCriticalStage(...) native
function Actor_SetCriticalStage() global
	int session = Costile2.GetSession("Actor_SetCriticalStage")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_aiStage = Costile2.GetInt(session, 1)
	this.SetCriticalStage(cdcg_aiStage)
endFunction

; void Actor::SetDoingFavor(...) native
function Actor_SetDoingFavor() global
	int session = Costile2.GetSession("Actor_SetDoingFavor")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abDoingFavor = Costile2.GetBool(session, 1)
	this.SetDoingFavor(cdcg_abDoingFavor)
endFunction

; void Actor::SetDontMove(...) native
function Actor_SetDontMove() global
	int session = Costile2.GetSession("Actor_SetDontMove")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abDontMove = Costile2.GetBool(session, 1)
	this.SetDontMove(cdcg_abDontMove)
endFunction

; void Actor::SetExpressionOverride(...) native
function Actor_SetExpressionOverride() global
	int session = Costile2.GetSession("Actor_SetExpressionOverride")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_aiMood = Costile2.GetInt(session, 1)
	int cdcg_aiStrength = Costile2.GetInt(session, 2)
	this.SetExpressionOverride(cdcg_aiMood, cdcg_aiStrength)
endFunction

; void Actor::SetEyeTexture(...) native
function Actor_SetEyeTexture() global
	int session = Costile2.GetSession("Actor_SetEyeTexture")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	textureset cdcg_akNewTexture = Costile2.GetTESForm(session, 1) as textureset
	this.SetEyeTexture(cdcg_akNewTexture)
endFunction

; void Actor::SetFactionRank(...) native
function Actor_SetFactionRank() global
	int session = Costile2.GetSession("Actor_SetFactionRank")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	faction cdcg_akFaction = Costile2.GetTESForm(session, 1) as faction
	int cdcg_aiRank = Costile2.GetInt(session, 2)
	this.SetFactionRank(cdcg_akFaction, cdcg_aiRank)
endFunction

; void Actor::SetForcedLandingMarker(...) native
function Actor_SetForcedLandingMarker() global
	int session = Costile2.GetSession("Actor_SetForcedLandingMarker")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	objectreference cdcg_aMarker = Costile2.GetTESForm(session, 1) as objectreference
	this.SetForcedLandingMarker(cdcg_aMarker)
endFunction

; void Actor::SetGhost(...) native
function Actor_SetGhost() global
	int session = Costile2.GetSession("Actor_SetGhost")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abIsGhost = Costile2.GetBool(session, 1)
	this.SetGhost(cdcg_abIsGhost)
endFunction

; void Actor::AddToFaction(...)
function Actor_AddToFaction() global
	int session = Costile2.GetSession("Actor_AddToFaction")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	faction cdcg_akFaction = Costile2.GetTESForm(session, 1) as faction
	this.AddToFaction(cdcg_akFaction)
endFunction

; void Actor::SetHeadTracking(...) native
function Actor_SetHeadTracking() global
	int session = Costile2.GetSession("Actor_SetHeadTracking")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abEnable = Costile2.GetBool(session, 1)
	this.SetHeadTracking(cdcg_abEnable)
endFunction

; void Actor::SetIntimidated(...) native
function Actor_SetIntimidated() global
	int session = Costile2.GetSession("Actor_SetIntimidated")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abIntimidate = Costile2.GetBool(session, 1)
	this.SetIntimidated(cdcg_abIntimidate)
endFunction

; void Actor::SetLookAt(...) native
function Actor_SetLookAt() global
	int session = Costile2.GetSession("Actor_SetLookAt")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 1) as objectreference
	bool cdcg_abPathingLookAt = Costile2.GetBool(session, 2)
	this.SetLookAt(cdcg_akTarget, cdcg_abPathingLookAt)
endFunction

; void Actor::SetNoBleedoutRecovery(...) native
function Actor_SetNoBleedoutRecovery() global
	int session = Costile2.GetSession("Actor_SetNoBleedoutRecovery")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abAllowed = Costile2.GetBool(session, 1)
	this.SetNoBleedoutRecovery(cdcg_abAllowed)
endFunction

; void Actor::SetNotShowOnStealthMeter(...) native
function Actor_SetNotShowOnStealthMeter() global
	int session = Costile2.GetSession("Actor_SetNotShowOnStealthMeter")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abNotShow = Costile2.GetBool(session, 1)
	this.SetNotShowOnStealthMeter(cdcg_abNotShow)
endFunction

; void Actor::SetOutfit(...) native
function Actor_SetOutfit() global
	int session = Costile2.GetSession("Actor_SetOutfit")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	outfit cdcg_akOutfit = Costile2.GetTESForm(session, 1) as outfit
	bool cdcg_abSleepOutfit = Costile2.GetBool(session, 2)
	this.SetOutfit(cdcg_akOutfit, cdcg_abSleepOutfit)
endFunction

; void Actor::SetPlayerControls(...) native
function Actor_SetPlayerControls() global
	int session = Costile2.GetSession("Actor_SetPlayerControls")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abControls = Costile2.GetBool(session, 1)
	this.SetPlayerControls(cdcg_abControls)
endFunction

; void Actor::SetPlayerResistingArrest(...) native
function Actor_SetPlayerResistingArrest() global
	int session = Costile2.GetSession("Actor_SetPlayerResistingArrest")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.SetPlayerResistingArrest()
endFunction

; void Actor::SetPlayerTeammate(...) native
function Actor_SetPlayerTeammate() global
	int session = Costile2.GetSession("Actor_SetPlayerTeammate")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abTeammate = Costile2.GetBool(session, 1)
	bool cdcg_abCanDoFavor = Costile2.GetBool(session, 2)
	this.SetPlayerTeammate(cdcg_abTeammate, cdcg_abCanDoFavor)
endFunction

; void Actor::SetRace(...) native
function Actor_SetRace() global
	int session = Costile2.GetSession("Actor_SetRace")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	race cdcg_akRace = Costile2.GetTESForm(session, 1) as race
	this.SetRace(cdcg_akRace)
endFunction

; void Actor::SetRelationshipRank(...) native
function Actor_SetRelationshipRank() global
	int session = Costile2.GetSession("Actor_SetRelationshipRank")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akOther = Costile2.GetTESForm(session, 1) as actor
	int cdcg_aiRank = Costile2.GetInt(session, 2)
	this.SetRelationshipRank(cdcg_akOther, cdcg_aiRank)
endFunction

; void Actor::SetRestrained(...) native
function Actor_SetRestrained() global
	int session = Costile2.GetSession("Actor_SetRestrained")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abRestrained = Costile2.GetBool(session, 1)
	this.SetRestrained(cdcg_abRestrained)
endFunction

; void Actor::SetSubGraphFloatVariable(...) native
function Actor_SetSubGraphFloatVariable() global
	int session = Costile2.GetSession("Actor_SetSubGraphFloatVariable")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	string cdcg_asVariableName = Costile2.GetString(session, 1)
	float cdcg_afValue = Costile2.GetFloat(session, 2)
	this.SetSubGraphFloatVariable(cdcg_asVariableName, cdcg_afValue)
endFunction

; void Actor::SetUnconscious(...) native
function Actor_SetUnconscious() global
	int session = Costile2.GetSession("Actor_SetUnconscious")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abUnconscious = Costile2.GetBool(session, 1)
	this.SetUnconscious(cdcg_abUnconscious)
endFunction

; void Actor::SetVehicle(...) native
function Actor_SetVehicle() global
	int session = Costile2.GetSession("Actor_SetVehicle")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	objectreference cdcg_akVehicle = Costile2.GetTESForm(session, 1) as objectreference
	this.SetVehicle(cdcg_akVehicle)
endFunction

; void Actor::SetVoiceRecoveryTime(...) native
function Actor_SetVoiceRecoveryTime() global
	int session = Costile2.GetSession("Actor_SetVoiceRecoveryTime")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	float cdcg_afTime = Costile2.GetFloat(session, 1)
	this.SetVoiceRecoveryTime(cdcg_afTime)
endFunction

; void Actor::ShowBarterMenu(...) native
function Actor_ShowBarterMenu() global
	int session = Costile2.GetSession("Actor_ShowBarterMenu")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.ShowBarterMenu()
endFunction

; int Actor::ShowGiftMenu(...) native
int function Actor_ShowGiftMenu() global
	int session = Costile2.GetSession("Actor_ShowGiftMenu")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	bool cdcg_abGivingGift = Costile2.GetBool(session, 1)
	formlist cdcg_apFilterList = Costile2.GetTESForm(session, 2) as formlist
	bool cdcg_abShowStolenItems = Costile2.GetBool(session, 3)
	bool cdcg_abUseFavorPoints = Costile2.GetBool(session, 4)
	return this.ShowGiftMenu(cdcg_abGivingGift, cdcg_apFilterList, cdcg_abShowStolenItems, cdcg_abUseFavorPoints)
endFunction

; void Actor::StartCannibal(...) native
function Actor_StartCannibal() global
	int session = Costile2.GetSession("Actor_StartCannibal")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akTarget = Costile2.GetTESForm(session, 1) as actor
	this.StartCannibal(cdcg_akTarget)
endFunction

; void Actor::StartCombat(...) native
function Actor_StartCombat() global
	int session = Costile2.GetSession("Actor_StartCombat")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akTarget = Costile2.GetTESForm(session, 1) as actor
	this.StartCombat(cdcg_akTarget)
endFunction

; void Actor::StartDeferredKill(...) native
function Actor_StartDeferredKill() global
	int session = Costile2.GetSession("Actor_StartDeferredKill")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.StartDeferredKill()
endFunction

; void Actor::StartVampireFeed(...) native
function Actor_StartVampireFeed() global
	int session = Costile2.GetSession("Actor_StartVampireFeed")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akTarget = Costile2.GetTESForm(session, 1) as actor
	this.StartVampireFeed(cdcg_akTarget)
endFunction

; void Actor::StopCombat(...) native
function Actor_StopCombat() global
	int session = Costile2.GetSession("Actor_StopCombat")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.StopCombat()
endFunction

; void Actor::StopCombatAlarm(...) native
function Actor_StopCombatAlarm() global
	int session = Costile2.GetSession("Actor_StopCombatAlarm")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.StopCombatAlarm()
endFunction

; bool Actor::TrapSoul(...) native
bool function Actor_TrapSoul() global
	int session = Costile2.GetSession("Actor_TrapSoul")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	actor cdcg_akTarget = Costile2.GetTESForm(session, 1) as actor
	return this.TrapSoul(cdcg_akTarget)
endFunction

; void Actor::UnequipAll(...) native
function Actor_UnequipAll() global
	int session = Costile2.GetSession("Actor_UnequipAll")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.UnequipAll()
endFunction

; void Actor::UnequipItem(...) native
function Actor_UnequipItem() global
	int session = Costile2.GetSession("Actor_UnequipItem")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	form cdcg_akItem = Costile2.GetTESForm(session, 1) as form
	bool cdcg_abPreventEquip = Costile2.GetBool(session, 2)
	bool cdcg_abSilent = Costile2.GetBool(session, 3)
	this.UnequipItem(cdcg_akItem, cdcg_abPreventEquip, cdcg_abSilent)
endFunction

; void Actor::UnequipItemSlot(...) native
function Actor_UnequipItemSlot() global
	int session = Costile2.GetSession("Actor_UnequipItemSlot")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_aiSlot = Costile2.GetInt(session, 1)
	this.UnequipItemSlot(cdcg_aiSlot)
endFunction

; void Actor::UnequipShout(...) native
function Actor_UnequipShout() global
	int session = Costile2.GetSession("Actor_UnequipShout")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	shout cdcg_akShout = Costile2.GetTESForm(session, 1) as shout
	this.UnequipShout(cdcg_akShout)
endFunction

; void Actor::UnequipSpell(...) native
function Actor_UnequipSpell() global
	int session = Costile2.GetSession("Actor_UnequipSpell")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	spell cdcg_akSpell = Costile2.GetTESForm(session, 1) as spell
	int cdcg_aiSource = Costile2.GetInt(session, 2)
	this.UnequipSpell(cdcg_akSpell, cdcg_aiSource)
endFunction

; void Actor::UnLockOwnedDoorsInCell(...) native
function Actor_UnLockOwnedDoorsInCell() global
	int session = Costile2.GetSession("Actor_UnLockOwnedDoorsInCell")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.UnLockOwnedDoorsInCell()
endFunction

; bool Actor::WillIntimidateSucceed(...) native
bool function Actor_WillIntimidateSucceed() global
	int session = Costile2.GetSession("Actor_WillIntimidateSucceed")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.WillIntimidateSucceed()
endFunction

; bool Actor::WornHasKeyword(...) native
bool function Actor_WornHasKeyword() global
	int session = Costile2.GetSession("Actor_WornHasKeyword")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	keyword cdcg_akKeyword = Costile2.GetTESForm(session, 1) as keyword
	return this.WornHasKeyword(cdcg_akKeyword)
endFunction

; void Actor::StartSneaking(...) native
function Actor_StartSneaking() global
	int session = Costile2.GetSession("Actor_StartSneaking")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.StartSneaking()
endFunction

; void Actor::DrawWeapon(...) native
function Actor_DrawWeapon() global
	int session = Costile2.GetSession("Actor_DrawWeapon")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.DrawWeapon()
endFunction

; void Actor::ForceMovementDirection(...) native
function Actor_ForceMovementDirection() global
	int session = Costile2.GetSession("Actor_ForceMovementDirection")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	float cdcg_afXAngle = Costile2.GetFloat(session, 1)
	float cdcg_afYAngle = Costile2.GetFloat(session, 2)
	float cdcg_afZAngle = Costile2.GetFloat(session, 3)
	this.ForceMovementDirection(cdcg_afXAngle, cdcg_afYAngle, cdcg_afZAngle)
endFunction

; void Actor::ForceMovementSpeed(...) native
function Actor_ForceMovementSpeed() global
	int session = Costile2.GetSession("Actor_ForceMovementSpeed")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	float cdcg_afSpeedMult = Costile2.GetFloat(session, 1)
	this.ForceMovementSpeed(cdcg_afSpeedMult)
endFunction

; void Actor::ForceMovementRotationSpeed(...) native
function Actor_ForceMovementRotationSpeed() global
	int session = Costile2.GetSession("Actor_ForceMovementRotationSpeed")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	float cdcg_afXMult = Costile2.GetFloat(session, 1)
	float cdcg_afYMult = Costile2.GetFloat(session, 2)
	float cdcg_afZMult = Costile2.GetFloat(session, 3)
	this.ForceMovementRotationSpeed(cdcg_afXMult, cdcg_afYMult, cdcg_afZMult)
endFunction

; void Actor::ForceMovementDirectionRamp(...) native
function Actor_ForceMovementDirectionRamp() global
	int session = Costile2.GetSession("Actor_ForceMovementDirectionRamp")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	float cdcg_afXAngle = Costile2.GetFloat(session, 1)
	float cdcg_afYAngle = Costile2.GetFloat(session, 2)
	float cdcg_afZAngle = Costile2.GetFloat(session, 3)
	float cdcg_afRampTime = Costile2.GetFloat(session, 4)
	this.ForceMovementDirectionRamp(cdcg_afXAngle, cdcg_afYAngle, cdcg_afZAngle, cdcg_afRampTime)
endFunction

; void Actor::ForceMovementSpeedRamp(...) native
function Actor_ForceMovementSpeedRamp() global
	int session = Costile2.GetSession("Actor_ForceMovementSpeedRamp")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	float cdcg_afSpeedMult = Costile2.GetFloat(session, 1)
	float cdcg_afRampTime = Costile2.GetFloat(session, 2)
	this.ForceMovementSpeedRamp(cdcg_afSpeedMult, cdcg_afRampTime)
endFunction

; void Actor::ForceMovementRotationSpeedRamp(...) native
function Actor_ForceMovementRotationSpeedRamp() global
	int session = Costile2.GetSession("Actor_ForceMovementRotationSpeedRamp")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	float cdcg_afXMult = Costile2.GetFloat(session, 1)
	float cdcg_afYMult = Costile2.GetFloat(session, 2)
	float cdcg_afZMult = Costile2.GetFloat(session, 3)
	float cdcg_afRampTime = Costile2.GetFloat(session, 4)
	this.ForceMovementRotationSpeedRamp(cdcg_afXMult, cdcg_afYMult, cdcg_afZMult, cdcg_afRampTime)
endFunction

; void Actor::ForceTargetDirection(...) native
function Actor_ForceTargetDirection() global
	int session = Costile2.GetSession("Actor_ForceTargetDirection")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	float cdcg_afXAngle = Costile2.GetFloat(session, 1)
	float cdcg_afYAngle = Costile2.GetFloat(session, 2)
	float cdcg_afZAngle = Costile2.GetFloat(session, 3)
	this.ForceTargetDirection(cdcg_afXAngle, cdcg_afYAngle, cdcg_afZAngle)
endFunction

; void Actor::ForceTargetSpeed(...) native
function Actor_ForceTargetSpeed() global
	int session = Costile2.GetSession("Actor_ForceTargetSpeed")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	float cdcg_afSpeed = Costile2.GetFloat(session, 1)
	this.ForceTargetSpeed(cdcg_afSpeed)
endFunction

; void Actor::ForceTargetAngle(...) native
function Actor_ForceTargetAngle() global
	int session = Costile2.GetSession("Actor_ForceTargetAngle")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	float cdcg_afXAngle = Costile2.GetFloat(session, 1)
	float cdcg_afYAngle = Costile2.GetFloat(session, 2)
	float cdcg_afZAngle = Costile2.GetFloat(session, 3)
	this.ForceTargetAngle(cdcg_afXAngle, cdcg_afYAngle, cdcg_afZAngle)
endFunction

; void Actor::ClearForcedMovement(...) native
function Actor_ClearForcedMovement() global
	int session = Costile2.GetSession("Actor_ClearForcedMovement")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.ClearForcedMovement()
endFunction

; form Actor::GetWornForm(...) native
form function Actor_GetWornForm() global
	int session = Costile2.GetSession("Actor_GetWornForm")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_slotMask = Costile2.GetInt(session, 1)
	return this.GetWornForm(cdcg_slotMask)
endFunction

; int Actor::GetWornItemId(...) native
int function Actor_GetWornItemId() global
	int session = Costile2.GetSession("Actor_GetWornItemId")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_slotMask = Costile2.GetInt(session, 1)
	return this.GetWornItemId(cdcg_slotMask)
endFunction

; form Actor::GetEquippedObject(...) native
form function Actor_GetEquippedObject() global
	int session = Costile2.GetSession("Actor_GetEquippedObject")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_location = Costile2.GetInt(session, 1)
	return this.GetEquippedObject(cdcg_location)
endFunction

; int Actor::GetEquippedItemId(...) native
int function Actor_GetEquippedItemId() global
	int session = Costile2.GetSession("Actor_GetEquippedItemId")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_location = Costile2.GetInt(session, 1)
	return this.GetEquippedItemId(cdcg_location)
endFunction

; int Actor::GetSpellCount(...) native
int function Actor_GetSpellCount() global
	int session = Costile2.GetSession("Actor_GetSpellCount")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetSpellCount()
endFunction

; spell Actor::GetNthSpell(...) native
spell function Actor_GetNthSpell() global
	int session = Costile2.GetSession("Actor_GetNthSpell")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthSpell(cdcg_n)
endFunction

; void Actor::QueueNiNodeUpdate(...) native
function Actor_QueueNiNodeUpdate() global
	int session = Costile2.GetSession("Actor_QueueNiNodeUpdate")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.QueueNiNodeUpdate()
endFunction

; void Actor::RegenerateHead(...) native
function Actor_RegenerateHead() global
	int session = Costile2.GetSession("Actor_RegenerateHead")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.RegenerateHead()
endFunction

; void Actor::EquipItemEx(...) native
function Actor_EquipItemEx() global
	int session = Costile2.GetSession("Actor_EquipItemEx")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	form cdcg_item = Costile2.GetTESForm(session, 1) as form
	int cdcg_equipSlot = Costile2.GetInt(session, 2)
	bool cdcg_preventUnequip = Costile2.GetBool(session, 3)
	bool cdcg_equipSound = Costile2.GetBool(session, 4)
	this.EquipItemEx(cdcg_item, cdcg_equipSlot, cdcg_preventUnequip, cdcg_equipSound)
endFunction

; void Actor::EquipItemById(...) native
function Actor_EquipItemById() global
	int session = Costile2.GetSession("Actor_EquipItemById")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	form cdcg_item = Costile2.GetTESForm(session, 1) as form
	int cdcg_itemId = Costile2.GetInt(session, 2)
	int cdcg_equipSlot = Costile2.GetInt(session, 3)
	bool cdcg_preventUnequip = Costile2.GetBool(session, 4)
	bool cdcg_equipSound = Costile2.GetBool(session, 5)
	this.EquipItemById(cdcg_item, cdcg_itemId, cdcg_equipSlot, cdcg_preventUnequip, cdcg_equipSound)
endFunction

; void Actor::UnequipItemEx(...) native
function Actor_UnequipItemEx() global
	int session = Costile2.GetSession("Actor_UnequipItemEx")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	form cdcg_item = Costile2.GetTESForm(session, 1) as form
	int cdcg_equipSlot = Costile2.GetInt(session, 2)
	bool cdcg_preventEquip = Costile2.GetBool(session, 3)
	this.UnequipItemEx(cdcg_item, cdcg_equipSlot, cdcg_preventEquip)
endFunction

; void Actor::ChangeHeadPart(...) native
function Actor_ChangeHeadPart() global
	int session = Costile2.GetSession("Actor_ChangeHeadPart")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	headpart cdcg_hPart = Costile2.GetTESForm(session, 1) as headpart
	this.ChangeHeadPart(cdcg_hPart)
endFunction

; void Actor::ReplaceHeadPart(...) native
function Actor_ReplaceHeadPart() global
	int session = Costile2.GetSession("Actor_ReplaceHeadPart")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	headpart cdcg_oPart = Costile2.GetTESForm(session, 1) as headpart
	headpart cdcg_newPart = Costile2.GetTESForm(session, 2) as headpart
	this.ReplaceHeadPart(cdcg_oPart, cdcg_newPart)
endFunction

; void Actor::UpdateWeight(...) native
function Actor_UpdateWeight() global
	int session = Costile2.GetSession("Actor_UpdateWeight")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	float cdcg_neckDelta = Costile2.GetFloat(session, 1)
	this.UpdateWeight(cdcg_neckDelta)
endFunction

; bool Actor::IsAIEnabled(...) native
bool function Actor_IsAIEnabled() global
	int session = Costile2.GetSession("Actor_IsAIEnabled")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsAIEnabled()
endFunction

; void Actor::ResetAI(...) native
function Actor_ResetAI() global
	int session = Costile2.GetSession("Actor_ResetAI")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.ResetAI()
endFunction

; bool Actor::IsSwimming(...) native
bool function Actor_IsSwimming() global
	int session = Costile2.GetSession("Actor_IsSwimming")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.IsSwimming()
endFunction

; void Actor::SheatheWeapon(...) native
function Actor_SheatheWeapon() global
	int session = Costile2.GetSession("Actor_SheatheWeapon")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.SheatheWeapon()
endFunction

; objectreference Actor::GetFurnitureReference(...) native
objectreference function Actor_GetFurnitureReference() global
	int session = Costile2.GetSession("Actor_GetFurnitureReference")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	return this.GetFurnitureReference()
endFunction

; void Actor::SetExpressionPhoneme(...) native
function Actor_SetExpressionPhoneme() global
	int session = Costile2.GetSession("Actor_SetExpressionPhoneme")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_index = Costile2.GetInt(session, 1)
	float cdcg_value = Costile2.GetFloat(session, 2)
	this.SetExpressionPhoneme(cdcg_index, cdcg_value)
endFunction

; void Actor::SetExpressionModifier(...) native
function Actor_SetExpressionModifier() global
	int session = Costile2.GetSession("Actor_SetExpressionModifier")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	int cdcg_index = Costile2.GetInt(session, 1)
	float cdcg_value = Costile2.GetFloat(session, 2)
	this.SetExpressionModifier(cdcg_index, cdcg_value)
endFunction

; void Actor::ResetExpressionOverrides(...) native
function Actor_ResetExpressionOverrides() global
	int session = Costile2.GetSession("Actor_ResetExpressionOverrides")
	Actor this = Costile2.GetTESForm(session, 0) as Actor
	this.ResetExpressionOverrides()
endFunction

