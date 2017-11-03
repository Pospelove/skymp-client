Scriptname Costile2_Potion Hidden

;
;
;
; script Potion.psc
;
;
;

; bool Potion::IsHostile(...) native
bool function Potion_IsHostile() global
	int session = Costile2.GetSession("Potion_IsHostile")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	return this.IsHostile()
endFunction

; bool Potion::IsFood(...) native
bool function Potion_IsFood() global
	int session = Costile2.GetSession("Potion_IsFood")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	return this.IsFood()
endFunction

; bool Potion::IsPoison(...) native
bool function Potion_IsPoison() global
	int session = Costile2.GetSession("Potion_IsPoison")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	return this.IsPoison()
endFunction

; int Potion::GetNumEffects(...) native
int function Potion_GetNumEffects() global
	int session = Costile2.GetSession("Potion_GetNumEffects")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	return this.GetNumEffects()
endFunction

; float Potion::GetNthEffectMagnitude(...) native
float function Potion_GetNthEffectMagnitude() global
	int session = Costile2.GetSession("Potion_GetNthEffectMagnitude")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectMagnitude(cdcg_index)
endFunction

; int Potion::GetNthEffectArea(...) native
int function Potion_GetNthEffectArea() global
	int session = Costile2.GetSession("Potion_GetNthEffectArea")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectArea(cdcg_index)
endFunction

; int Potion::GetNthEffectDuration(...) native
int function Potion_GetNthEffectDuration() global
	int session = Costile2.GetSession("Potion_GetNthEffectDuration")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectDuration(cdcg_index)
endFunction

; magiceffect Potion::GetNthEffectMagicEffect(...) native
magiceffect function Potion_GetNthEffectMagicEffect() global
	int session = Costile2.GetSession("Potion_GetNthEffectMagicEffect")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectMagicEffect(cdcg_index)
endFunction

; int Potion::GetCostliestEffectIndex(...) native
int function Potion_GetCostliestEffectIndex() global
	int session = Costile2.GetSession("Potion_GetCostliestEffectIndex")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	return this.GetCostliestEffectIndex()
endFunction

; void Potion::SetNthEffectMagnitude(...) native
function Potion_SetNthEffectMagnitude() global
	int session = Costile2.GetSession("Potion_SetNthEffectMagnitude")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	int cdcg_index = Costile2.GetInt(session, 1)
	float cdcg_value = Costile2.GetFloat(session, 2)
	this.SetNthEffectMagnitude(cdcg_index, cdcg_value)
endFunction

; void Potion::SetNthEffectArea(...) native
function Potion_SetNthEffectArea() global
	int session = Costile2.GetSession("Potion_SetNthEffectArea")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	int cdcg_index = Costile2.GetInt(session, 1)
	int cdcg_value = Costile2.GetInt(session, 2)
	this.SetNthEffectArea(cdcg_index, cdcg_value)
endFunction

; void Potion::SetNthEffectDuration(...) native
function Potion_SetNthEffectDuration() global
	int session = Costile2.GetSession("Potion_SetNthEffectDuration")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	int cdcg_index = Costile2.GetInt(session, 1)
	int cdcg_value = Costile2.GetInt(session, 2)
	this.SetNthEffectDuration(cdcg_index, cdcg_value)
endFunction

; sounddescriptor Potion::GetUseSound(...) native
sounddescriptor function Potion_GetUseSound() global
	int session = Costile2.GetSession("Potion_GetUseSound")
	Potion this = Costile2.GetTESForm(session, 0) as Potion
	return this.GetUseSound()
endFunction

