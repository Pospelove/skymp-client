Scriptname Costile2_CombatStyle Hidden

;
;
;
; script CombatStyle.psc
;
;
;

; float CombatStyle::GetOffensiveMult(...) native
float function CombatStyle_GetOffensiveMult() global
	int session = Costile2.GetSession("CombatStyle_GetOffensiveMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetOffensiveMult()
endFunction

; float CombatStyle::GetDefensiveMult(...) native
float function CombatStyle_GetDefensiveMult() global
	int session = Costile2.GetSession("CombatStyle_GetDefensiveMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetDefensiveMult()
endFunction

; float CombatStyle::GetGroupOffensiveMult(...) native
float function CombatStyle_GetGroupOffensiveMult() global
	int session = Costile2.GetSession("CombatStyle_GetGroupOffensiveMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetGroupOffensiveMult()
endFunction

; float CombatStyle::GetAvoidThreatChance(...) native
float function CombatStyle_GetAvoidThreatChance() global
	int session = Costile2.GetSession("CombatStyle_GetAvoidThreatChance")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetAvoidThreatChance()
endFunction

; float CombatStyle::GetMeleeMult(...) native
float function CombatStyle_GetMeleeMult() global
	int session = Costile2.GetSession("CombatStyle_GetMeleeMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetMeleeMult()
endFunction

; float CombatStyle::GetRangedMult(...) native
float function CombatStyle_GetRangedMult() global
	int session = Costile2.GetSession("CombatStyle_GetRangedMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetRangedMult()
endFunction

; float CombatStyle::GetMagicMult(...) native
float function CombatStyle_GetMagicMult() global
	int session = Costile2.GetSession("CombatStyle_GetMagicMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetMagicMult()
endFunction

; float CombatStyle::GetShoutMult(...) native
float function CombatStyle_GetShoutMult() global
	int session = Costile2.GetSession("CombatStyle_GetShoutMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetShoutMult()
endFunction

; float CombatStyle::GetStaffMult(...) native
float function CombatStyle_GetStaffMult() global
	int session = Costile2.GetSession("CombatStyle_GetStaffMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetStaffMult()
endFunction

; float CombatStyle::GetUnarmedMult(...) native
float function CombatStyle_GetUnarmedMult() global
	int session = Costile2.GetSession("CombatStyle_GetUnarmedMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetUnarmedMult()
endFunction

; void CombatStyle::SetOffensiveMult(...) native
function CombatStyle_SetOffensiveMult() global
	int session = Costile2.GetSession("CombatStyle_SetOffensiveMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetOffensiveMult(cdcg_mult)
endFunction

; void CombatStyle::SetDefensiveMult(...) native
function CombatStyle_SetDefensiveMult() global
	int session = Costile2.GetSession("CombatStyle_SetDefensiveMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetDefensiveMult(cdcg_mult)
endFunction

; void CombatStyle::SetGroupOffensiveMult(...) native
function CombatStyle_SetGroupOffensiveMult() global
	int session = Costile2.GetSession("CombatStyle_SetGroupOffensiveMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetGroupOffensiveMult(cdcg_mult)
endFunction

; void CombatStyle::SetAvoidThreatChance(...) native
function CombatStyle_SetAvoidThreatChance() global
	int session = Costile2.GetSession("CombatStyle_SetAvoidThreatChance")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_chance = Costile2.GetFloat(session, 1)
	this.SetAvoidThreatChance(cdcg_chance)
endFunction

; void CombatStyle::SetMeleeMult(...) native
function CombatStyle_SetMeleeMult() global
	int session = Costile2.GetSession("CombatStyle_SetMeleeMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetMeleeMult(cdcg_mult)
endFunction

; void CombatStyle::SetRangedMult(...) native
function CombatStyle_SetRangedMult() global
	int session = Costile2.GetSession("CombatStyle_SetRangedMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetRangedMult(cdcg_mult)
endFunction

; void CombatStyle::SetMagicMult(...) native
function CombatStyle_SetMagicMult() global
	int session = Costile2.GetSession("CombatStyle_SetMagicMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetMagicMult(cdcg_mult)
endFunction

; void CombatStyle::SetShoutMult(...) native
function CombatStyle_SetShoutMult() global
	int session = Costile2.GetSession("CombatStyle_SetShoutMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetShoutMult(cdcg_mult)
endFunction

; void CombatStyle::SetStaffMult(...) native
function CombatStyle_SetStaffMult() global
	int session = Costile2.GetSession("CombatStyle_SetStaffMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetStaffMult(cdcg_mult)
endFunction

; void CombatStyle::SetUnarmedMult(...) native
function CombatStyle_SetUnarmedMult() global
	int session = Costile2.GetSession("CombatStyle_SetUnarmedMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetUnarmedMult(cdcg_mult)
endFunction

; float CombatStyle::GetMeleeAttackStaggeredMult(...) native
float function CombatStyle_GetMeleeAttackStaggeredMult() global
	int session = Costile2.GetSession("CombatStyle_GetMeleeAttackStaggeredMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetMeleeAttackStaggeredMult()
endFunction

; float CombatStyle::GetMeleePowerAttackStaggeredMult(...) native
float function CombatStyle_GetMeleePowerAttackStaggeredMult() global
	int session = Costile2.GetSession("CombatStyle_GetMeleePowerAttackStaggeredMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetMeleePowerAttackStaggeredMult()
endFunction

; float CombatStyle::GetMeleePowerAttackBlockingMult(...) native
float function CombatStyle_GetMeleePowerAttackBlockingMult() global
	int session = Costile2.GetSession("CombatStyle_GetMeleePowerAttackBlockingMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetMeleePowerAttackBlockingMult()
endFunction

; float CombatStyle::GetMeleeBashMult(...) native
float function CombatStyle_GetMeleeBashMult() global
	int session = Costile2.GetSession("CombatStyle_GetMeleeBashMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetMeleeBashMult()
endFunction

; float CombatStyle::GetMeleeBashRecoiledMult(...) native
float function CombatStyle_GetMeleeBashRecoiledMult() global
	int session = Costile2.GetSession("CombatStyle_GetMeleeBashRecoiledMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetMeleeBashRecoiledMult()
endFunction

; float CombatStyle::GetMeleeBashAttackMult(...) native
float function CombatStyle_GetMeleeBashAttackMult() global
	int session = Costile2.GetSession("CombatStyle_GetMeleeBashAttackMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetMeleeBashAttackMult()
endFunction

; float CombatStyle::GetMeleeBashPowerAttackMult(...) native
float function CombatStyle_GetMeleeBashPowerAttackMult() global
	int session = Costile2.GetSession("CombatStyle_GetMeleeBashPowerAttackMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetMeleeBashPowerAttackMult()
endFunction

; float CombatStyle::GetMeleeSpecialAttackMult(...) native
float function CombatStyle_GetMeleeSpecialAttackMult() global
	int session = Costile2.GetSession("CombatStyle_GetMeleeSpecialAttackMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetMeleeSpecialAttackMult()
endFunction

; bool CombatStyle::GetAllowDualWielding(...) native
bool function CombatStyle_GetAllowDualWielding() global
	int session = Costile2.GetSession("CombatStyle_GetAllowDualWielding")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetAllowDualWielding()
endFunction

; void CombatStyle::SetMeleeAttackStaggeredMult(...) native
function CombatStyle_SetMeleeAttackStaggeredMult() global
	int session = Costile2.GetSession("CombatStyle_SetMeleeAttackStaggeredMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetMeleeAttackStaggeredMult(cdcg_mult)
endFunction

; void CombatStyle::SetMeleePowerAttackStaggeredMult(...) native
function CombatStyle_SetMeleePowerAttackStaggeredMult() global
	int session = Costile2.GetSession("CombatStyle_SetMeleePowerAttackStaggeredMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetMeleePowerAttackStaggeredMult(cdcg_mult)
endFunction

; void CombatStyle::SetMeleePowerAttackBlockingMult(...) native
function CombatStyle_SetMeleePowerAttackBlockingMult() global
	int session = Costile2.GetSession("CombatStyle_SetMeleePowerAttackBlockingMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetMeleePowerAttackBlockingMult(cdcg_mult)
endFunction

; void CombatStyle::SetMeleeBashMult(...) native
function CombatStyle_SetMeleeBashMult() global
	int session = Costile2.GetSession("CombatStyle_SetMeleeBashMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetMeleeBashMult(cdcg_mult)
endFunction

; void CombatStyle::SetMeleeBashRecoiledMult(...) native
function CombatStyle_SetMeleeBashRecoiledMult() global
	int session = Costile2.GetSession("CombatStyle_SetMeleeBashRecoiledMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetMeleeBashRecoiledMult(cdcg_mult)
endFunction

; void CombatStyle::SetMeleeBashAttackMult(...) native
function CombatStyle_SetMeleeBashAttackMult() global
	int session = Costile2.GetSession("CombatStyle_SetMeleeBashAttackMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetMeleeBashAttackMult(cdcg_mult)
endFunction

; void CombatStyle::SetMeleeBashPowerAttackMult(...) native
function CombatStyle_SetMeleeBashPowerAttackMult() global
	int session = Costile2.GetSession("CombatStyle_SetMeleeBashPowerAttackMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetMeleeBashPowerAttackMult(cdcg_mult)
endFunction

; void CombatStyle::SetMeleeSpecialAttackMult(...) native
function CombatStyle_SetMeleeSpecialAttackMult() global
	int session = Costile2.GetSession("CombatStyle_SetMeleeSpecialAttackMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetMeleeSpecialAttackMult(cdcg_mult)
endFunction

; void CombatStyle::SetAllowDualWielding(...) native
function CombatStyle_SetAllowDualWielding() global
	int session = Costile2.GetSession("CombatStyle_SetAllowDualWielding")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	bool cdcg_allow = Costile2.GetBool(session, 1)
	this.SetAllowDualWielding(cdcg_allow)
endFunction

; float CombatStyle::GetCloseRangeDuelingCircleMult(...) native
float function CombatStyle_GetCloseRangeDuelingCircleMult() global
	int session = Costile2.GetSession("CombatStyle_GetCloseRangeDuelingCircleMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetCloseRangeDuelingCircleMult()
endFunction

; float CombatStyle::GetCloseRangeDuelingFallbackMult(...) native
float function CombatStyle_GetCloseRangeDuelingFallbackMult() global
	int session = Costile2.GetSession("CombatStyle_GetCloseRangeDuelingFallbackMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetCloseRangeDuelingFallbackMult()
endFunction

; float CombatStyle::GetCloseRangeFlankingFlankDistance(...) native
float function CombatStyle_GetCloseRangeFlankingFlankDistance() global
	int session = Costile2.GetSession("CombatStyle_GetCloseRangeFlankingFlankDistance")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetCloseRangeFlankingFlankDistance()
endFunction

; float CombatStyle::GetCloseRangeFlankingStalkTime(...) native
float function CombatStyle_GetCloseRangeFlankingStalkTime() global
	int session = Costile2.GetSession("CombatStyle_GetCloseRangeFlankingStalkTime")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetCloseRangeFlankingStalkTime()
endFunction

; void CombatStyle::SetCloseRangeDuelingCircleMult(...) native
function CombatStyle_SetCloseRangeDuelingCircleMult() global
	int session = Costile2.GetSession("CombatStyle_SetCloseRangeDuelingCircleMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetCloseRangeDuelingCircleMult(cdcg_mult)
endFunction

; void CombatStyle::SetCloseRangeDuelingFallbackMult(...) native
function CombatStyle_SetCloseRangeDuelingFallbackMult() global
	int session = Costile2.GetSession("CombatStyle_SetCloseRangeDuelingFallbackMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetCloseRangeDuelingFallbackMult(cdcg_mult)
endFunction

; void CombatStyle::SetCloseRangeFlankingFlankDistance(...) native
function CombatStyle_SetCloseRangeFlankingFlankDistance() global
	int session = Costile2.GetSession("CombatStyle_SetCloseRangeFlankingFlankDistance")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetCloseRangeFlankingFlankDistance(cdcg_mult)
endFunction

; void CombatStyle::SetCloseRangeFlankingStalkTime(...) native
function CombatStyle_SetCloseRangeFlankingStalkTime() global
	int session = Costile2.GetSession("CombatStyle_SetCloseRangeFlankingStalkTime")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetCloseRangeFlankingStalkTime(cdcg_mult)
endFunction

; float CombatStyle::GetLongRangeStrafeMult(...) native
float function CombatStyle_GetLongRangeStrafeMult() global
	int session = Costile2.GetSession("CombatStyle_GetLongRangeStrafeMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetLongRangeStrafeMult()
endFunction

; void CombatStyle::SetLongRangeStrafeMult(...) native
function CombatStyle_SetLongRangeStrafeMult() global
	int session = Costile2.GetSession("CombatStyle_SetLongRangeStrafeMult")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetLongRangeStrafeMult(cdcg_mult)
endFunction

; float CombatStyle::GetFlightHoverChance(...) native
float function CombatStyle_GetFlightHoverChance() global
	int session = Costile2.GetSession("CombatStyle_GetFlightHoverChance")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetFlightHoverChance()
endFunction

; float CombatStyle::GetFlightDiveBombChance(...) native
float function CombatStyle_GetFlightDiveBombChance() global
	int session = Costile2.GetSession("CombatStyle_GetFlightDiveBombChance")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetFlightDiveBombChance()
endFunction

; float CombatStyle::GetFlightFlyingAttackChance(...) native
float function CombatStyle_GetFlightFlyingAttackChance() global
	int session = Costile2.GetSession("CombatStyle_GetFlightFlyingAttackChance")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	return this.GetFlightFlyingAttackChance()
endFunction

; void CombatStyle::SetFlightHoverChance(...) native
function CombatStyle_SetFlightHoverChance() global
	int session = Costile2.GetSession("CombatStyle_SetFlightHoverChance")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_chance = Costile2.GetFloat(session, 1)
	this.SetFlightHoverChance(cdcg_chance)
endFunction

; void CombatStyle::SetFlightDiveBombChance(...) native
function CombatStyle_SetFlightDiveBombChance() global
	int session = Costile2.GetSession("CombatStyle_SetFlightDiveBombChance")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_chance = Costile2.GetFloat(session, 1)
	this.SetFlightDiveBombChance(cdcg_chance)
endFunction

; void CombatStyle::SetFlightFlyingAttackChance(...) native
function CombatStyle_SetFlightFlyingAttackChance() global
	int session = Costile2.GetSession("CombatStyle_SetFlightFlyingAttackChance")
	CombatStyle this = Costile2.GetTESForm(session, 0) as CombatStyle
	float cdcg_mult = Costile2.GetFloat(session, 1)
	this.SetFlightFlyingAttackChance(cdcg_mult)
endFunction

