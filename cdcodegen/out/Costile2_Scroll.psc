Scriptname Costile2_Scroll Hidden

;
;
;
; script Scroll.psc
;
;
;

; void Scroll::Cast(...) native
function Scroll_Cast() global
	int session = Costile2.GetSession("Scroll_Cast")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	objectreference cdcg_akSource = Costile2.GetTESForm(session, 1) as objectreference
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 2) as objectreference
	this.Cast(cdcg_akSource, cdcg_akTarget)
endFunction

; float Scroll::GetCastTime(...) native
float function Scroll_GetCastTime() global
	int session = Costile2.GetSession("Scroll_GetCastTime")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	return this.GetCastTime()
endFunction

; perk Scroll::GetPerk(...) native
perk function Scroll_GetPerk() global
	int session = Costile2.GetSession("Scroll_GetPerk")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	return this.GetPerk()
endFunction

; int Scroll::GetNumEffects(...) native
int function Scroll_GetNumEffects() global
	int session = Costile2.GetSession("Scroll_GetNumEffects")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	return this.GetNumEffects()
endFunction

; float Scroll::GetNthEffectMagnitude(...) native
float function Scroll_GetNthEffectMagnitude() global
	int session = Costile2.GetSession("Scroll_GetNthEffectMagnitude")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectMagnitude(cdcg_index)
endFunction

; int Scroll::GetNthEffectArea(...) native
int function Scroll_GetNthEffectArea() global
	int session = Costile2.GetSession("Scroll_GetNthEffectArea")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectArea(cdcg_index)
endFunction

; int Scroll::GetNthEffectDuration(...) native
int function Scroll_GetNthEffectDuration() global
	int session = Costile2.GetSession("Scroll_GetNthEffectDuration")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectDuration(cdcg_index)
endFunction

; magiceffect Scroll::GetNthEffectMagicEffect(...) native
magiceffect function Scroll_GetNthEffectMagicEffect() global
	int session = Costile2.GetSession("Scroll_GetNthEffectMagicEffect")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectMagicEffect(cdcg_index)
endFunction

; int Scroll::GetCostliestEffectIndex(...) native
int function Scroll_GetCostliestEffectIndex() global
	int session = Costile2.GetSession("Scroll_GetCostliestEffectIndex")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	return this.GetCostliestEffectIndex()
endFunction

; void Scroll::SetNthEffectMagnitude(...) native
function Scroll_SetNthEffectMagnitude() global
	int session = Costile2.GetSession("Scroll_SetNthEffectMagnitude")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	int cdcg_index = Costile2.GetInt(session, 1)
	float cdcg_value = Costile2.GetFloat(session, 2)
	this.SetNthEffectMagnitude(cdcg_index, cdcg_value)
endFunction

; void Scroll::SetNthEffectArea(...) native
function Scroll_SetNthEffectArea() global
	int session = Costile2.GetSession("Scroll_SetNthEffectArea")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	int cdcg_index = Costile2.GetInt(session, 1)
	int cdcg_value = Costile2.GetInt(session, 2)
	this.SetNthEffectArea(cdcg_index, cdcg_value)
endFunction

; void Scroll::SetNthEffectDuration(...) native
function Scroll_SetNthEffectDuration() global
	int session = Costile2.GetSession("Scroll_SetNthEffectDuration")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	int cdcg_index = Costile2.GetInt(session, 1)
	int cdcg_value = Costile2.GetInt(session, 2)
	this.SetNthEffectDuration(cdcg_index, cdcg_value)
endFunction

; equipslot Scroll::GetEquipType(...) native
equipslot function Scroll_GetEquipType() global
	int session = Costile2.GetSession("Scroll_GetEquipType")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	return this.GetEquipType()
endFunction

; void Scroll::SetEquipType(...) native
function Scroll_SetEquipType() global
	int session = Costile2.GetSession("Scroll_SetEquipType")
	Scroll this = Costile2.GetTESForm(session, 0) as Scroll
	equipslot cdcg_type = Costile2.GetTESForm(session, 1) as equipslot
	this.SetEquipType(cdcg_type)
endFunction

