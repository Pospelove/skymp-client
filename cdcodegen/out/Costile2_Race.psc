Scriptname Costile2_Race Hidden

;
;
;
; script Race.psc
;
;
;

; int Race::GetSpellCount(...) native
int function Race_GetSpellCount() global
	int session = Costile2.GetSession("Race_GetSpellCount")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.GetSpellCount()
endFunction

; spell Race::GetNthSpell(...) native
spell function Race_GetNthSpell() global
	int session = Costile2.GetSession("Race_GetNthSpell")
	Race this = Costile2.GetTESForm(session, 0) as Race
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthSpell(cdcg_n)
endFunction

; bool Race::IsRaceFlagSet(...) native
bool function Race_IsRaceFlagSet() global
	int session = Costile2.GetSession("Race_IsRaceFlagSet")
	Race this = Costile2.GetTESForm(session, 0) as Race
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.IsRaceFlagSet(cdcg_n)
endFunction

; void Race::SetRaceFlag(...) native
function Race_SetRaceFlag() global
	int session = Costile2.GetSession("Race_SetRaceFlag")
	Race this = Costile2.GetTESForm(session, 0) as Race
	int cdcg_n = Costile2.GetInt(session, 1)
	this.SetRaceFlag(cdcg_n)
endFunction

; void Race::ClearRaceFlag(...) native
function Race_ClearRaceFlag() global
	int session = Costile2.GetSession("Race_ClearRaceFlag")
	Race this = Costile2.GetTESForm(session, 0) as Race
	int cdcg_n = Costile2.GetInt(session, 1)
	this.ClearRaceFlag(cdcg_n)
endFunction

; voicetype Race::GetDefaultVoiceType(...) native
voicetype function Race_GetDefaultVoiceType() global
	int session = Costile2.GetSession("Race_GetDefaultVoiceType")
	Race this = Costile2.GetTESForm(session, 0) as Race
	bool cdcg_female = Costile2.GetBool(session, 1)
	return this.GetDefaultVoiceType(cdcg_female)
endFunction

; void Race::SetDefaultVoiceType(...) native
function Race_SetDefaultVoiceType() global
	int session = Costile2.GetSession("Race_SetDefaultVoiceType")
	Race this = Costile2.GetTESForm(session, 0) as Race
	bool cdcg_female = Costile2.GetBool(session, 1)
	voicetype cdcg_voice = Costile2.GetTESForm(session, 2) as voicetype
	this.SetDefaultVoiceType(cdcg_female, cdcg_voice)
endFunction

; armor Race::GetSkin(...) native
armor function Race_GetSkin() global
	int session = Costile2.GetSession("Race_GetSkin")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.GetSkin()
endFunction

; void Race::SetSkin(...) native
function Race_SetSkin() global
	int session = Costile2.GetSession("Race_SetSkin")
	Race this = Costile2.GetTESForm(session, 0) as Race
	armor cdcg_skin = Costile2.GetTESForm(session, 1) as armor
	this.SetSkin(cdcg_skin)
endFunction

; int Race::GetNumPlayableRaces(...) native global
int function Race_GetNumPlayableRaces() global
	int session = Costile2.GetSession("Race_GetNumPlayableRaces")
	return Race.GetNumPlayableRaces()
endFunction

; race Race::GetNthPlayableRace(...) native global
race function Race_GetNthPlayableRace() global
	int session = Costile2.GetSession("Race_GetNthPlayableRace")
	int cdcg_n = Costile2.GetInt(session, 0)
	return Race.GetNthPlayableRace(cdcg_n)
endFunction

; race Race::GetRace(...) native global
race function Race_GetRace() global
	int session = Costile2.GetSession("Race_GetRace")
	string cdcg_editorId = Costile2.GetString(session, 0)
	return Race.GetRace(cdcg_editorId)
endFunction

; bool Race::IsPlayable(...)
bool function Race_IsPlayable() global
	int session = Costile2.GetSession("Race_IsPlayable")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.IsPlayable()
endFunction

; void Race::MakePlayable(...)
function Race_MakePlayable() global
	int session = Costile2.GetSession("Race_MakePlayable")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakePlayable()
endFunction

; void Race::MakeUnplayable(...)
function Race_MakeUnplayable() global
	int session = Costile2.GetSession("Race_MakeUnplayable")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeUnplayable()
endFunction

; bool Race::IsChildRace(...)
bool function Race_IsChildRace() global
	int session = Costile2.GetSession("Race_IsChildRace")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.IsChildRace()
endFunction

; void Race::MakeChildRace(...)
function Race_MakeChildRace() global
	int session = Costile2.GetSession("Race_MakeChildRace")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeChildRace()
endFunction

; void Race::MakeNonChildRace(...)
function Race_MakeNonChildRace() global
	int session = Costile2.GetSession("Race_MakeNonChildRace")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeNonChildRace()
endFunction

; bool Race::CanFly(...)
bool function Race_CanFly() global
	int session = Costile2.GetSession("Race_CanFly")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.CanFly()
endFunction

; void Race::MakeCanFly(...)
function Race_MakeCanFly() global
	int session = Costile2.GetSession("Race_MakeCanFly")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeCanFly()
endFunction

; void Race::MakeNonFlying(...)
function Race_MakeNonFlying() global
	int session = Costile2.GetSession("Race_MakeNonFlying")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeNonFlying()
endFunction

; bool Race::CanSwim(...)
bool function Race_CanSwim() global
	int session = Costile2.GetSession("Race_CanSwim")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.CanSwim()
endFunction

; void Race::MakeCanSwim(...)
function Race_MakeCanSwim() global
	int session = Costile2.GetSession("Race_MakeCanSwim")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeCanSwim()
endFunction

; void Race::MakeNonSwimming(...)
function Race_MakeNonSwimming() global
	int session = Costile2.GetSession("Race_MakeNonSwimming")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeNonSwimming()
endFunction

; bool Race::CanWalk(...)
bool function Race_CanWalk() global
	int session = Costile2.GetSession("Race_CanWalk")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.CanWalk()
endFunction

; void Race::MakeCanWalk(...)
function Race_MakeCanWalk() global
	int session = Costile2.GetSession("Race_MakeCanWalk")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeCanWalk()
endFunction

; void Race::MakeNonWalking(...)
function Race_MakeNonWalking() global
	int session = Costile2.GetSession("Race_MakeNonWalking")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeNonWalking()
endFunction

; bool Race::IsImmobile(...)
bool function Race_IsImmobile() global
	int session = Costile2.GetSession("Race_IsImmobile")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.IsImmobile()
endFunction

; void Race::MakeImmobile(...)
function Race_MakeImmobile() global
	int session = Costile2.GetSession("Race_MakeImmobile")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeImmobile()
endFunction

; void Race::MakeMobile(...)
function Race_MakeMobile() global
	int session = Costile2.GetSession("Race_MakeMobile")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeMobile()
endFunction

; bool Race::IsNotPushable(...)
bool function Race_IsNotPushable() global
	int session = Costile2.GetSession("Race_IsNotPushable")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.IsNotPushable()
endFunction

; void Race::MakeNotPushable(...)
function Race_MakeNotPushable() global
	int session = Costile2.GetSession("Race_MakeNotPushable")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeNotPushable()
endFunction

; void Race::MakePushable(...)
function Race_MakePushable() global
	int session = Costile2.GetSession("Race_MakePushable")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakePushable()
endFunction

; bool Race::NoKnockdowns(...)
bool function Race_NoKnockdowns() global
	int session = Costile2.GetSession("Race_NoKnockdowns")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.NoKnockdowns()
endFunction

; void Race::MakeNoKnockdowns(...)
function Race_MakeNoKnockdowns() global
	int session = Costile2.GetSession("Race_MakeNoKnockdowns")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.MakeNoKnockdowns()
endFunction

; void Race::ClearNoKNockdowns(...)
function Race_ClearNoKNockdowns() global
	int session = Costile2.GetSession("Race_ClearNoKNockdowns")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.ClearNoKNockdowns()
endFunction

; bool Race::NoCombatInWater(...)
bool function Race_NoCombatInWater() global
	int session = Costile2.GetSession("Race_NoCombatInWater")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.NoCombatInWater()
endFunction

; void Race::SetNoCombatInWater(...)
function Race_SetNoCombatInWater() global
	int session = Costile2.GetSession("Race_SetNoCombatInWater")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.SetNoCombatInWater()
endFunction

; void Race::ClearNoCombatInWater(...)
function Race_ClearNoCombatInWater() global
	int session = Costile2.GetSession("Race_ClearNoCombatInWater")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.ClearNoCombatInWater()
endFunction

; bool Race::AvoidsRoads(...)
bool function Race_AvoidsRoads() global
	int session = Costile2.GetSession("Race_AvoidsRoads")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.AvoidsRoads()
endFunction

; void Race::SetAvoidsRoads(...)
function Race_SetAvoidsRoads() global
	int session = Costile2.GetSession("Race_SetAvoidsRoads")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.SetAvoidsRoads()
endFunction

; void Race::ClearAvoidsRoads(...)
function Race_ClearAvoidsRoads() global
	int session = Costile2.GetSession("Race_ClearAvoidsRoads")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.ClearAvoidsRoads()
endFunction

; bool Race::AllowPickpocket(...)
bool function Race_AllowPickpocket() global
	int session = Costile2.GetSession("Race_AllowPickpocket")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.AllowPickpocket()
endFunction

; void Race::SetAllowPickpocket(...)
function Race_SetAllowPickpocket() global
	int session = Costile2.GetSession("Race_SetAllowPickpocket")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.SetAllowPickpocket()
endFunction

; void Race::ClearAllowPickpocket(...)
function Race_ClearAllowPickpocket() global
	int session = Costile2.GetSession("Race_ClearAllowPickpocket")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.ClearAllowPickpocket()
endFunction

; bool Race::AllowPCDialogue(...)
bool function Race_AllowPCDialogue() global
	int session = Costile2.GetSession("Race_AllowPCDialogue")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.AllowPCDialogue()
endFunction

; void Race::SetAllowPCDialogue(...)
function Race_SetAllowPCDialogue() global
	int session = Costile2.GetSession("Race_SetAllowPCDialogue")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.SetAllowPCDialogue()
endFunction

; void Race::ClearAllowPCDialogue(...)
function Race_ClearAllowPCDialogue() global
	int session = Costile2.GetSession("Race_ClearAllowPCDialogue")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.ClearAllowPCDialogue()
endFunction

; bool Race::CantOpenDoors(...)
bool function Race_CantOpenDoors() global
	int session = Costile2.GetSession("Race_CantOpenDoors")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.CantOpenDoors()
endFunction

; void Race::SetCantOpenDoors(...)
function Race_SetCantOpenDoors() global
	int session = Costile2.GetSession("Race_SetCantOpenDoors")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.SetCantOpenDoors()
endFunction

; void Race::ClearCantOpenDoors(...)
function Race_ClearCantOpenDoors() global
	int session = Costile2.GetSession("Race_ClearCantOpenDoors")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.ClearCantOpenDoors()
endFunction

; bool Race::NoShadow(...)
bool function Race_NoShadow() global
	int session = Costile2.GetSession("Race_NoShadow")
	Race this = Costile2.GetTESForm(session, 0) as Race
	return this.NoShadow()
endFunction

; void Race::SetNoShadow(...)
function Race_SetNoShadow() global
	int session = Costile2.GetSession("Race_SetNoShadow")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.SetNoShadow()
endFunction

; void Race::ClearNoShadow(...)
function Race_ClearNoShadow() global
	int session = Costile2.GetSession("Race_ClearNoShadow")
	Race this = Costile2.GetTESForm(session, 0) as Race
	this.ClearNoShadow()
endFunction

