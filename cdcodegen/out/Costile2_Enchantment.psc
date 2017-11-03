Scriptname Costile2_Enchantment Hidden

;
;
;
; script Enchantment.psc
;
;
;

; bool Enchantment::IsHostile(...) native
bool function Enchantment_IsHostile() global
	int session = Costile2.GetSession("Enchantment_IsHostile")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	return this.IsHostile()
endFunction

; int Enchantment::GetNumEffects(...) native
int function Enchantment_GetNumEffects() global
	int session = Costile2.GetSession("Enchantment_GetNumEffects")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	return this.GetNumEffects()
endFunction

; float Enchantment::GetNthEffectMagnitude(...) native
float function Enchantment_GetNthEffectMagnitude() global
	int session = Costile2.GetSession("Enchantment_GetNthEffectMagnitude")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectMagnitude(cdcg_index)
endFunction

; int Enchantment::GetNthEffectArea(...) native
int function Enchantment_GetNthEffectArea() global
	int session = Costile2.GetSession("Enchantment_GetNthEffectArea")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectArea(cdcg_index)
endFunction

; int Enchantment::GetNthEffectDuration(...) native
int function Enchantment_GetNthEffectDuration() global
	int session = Costile2.GetSession("Enchantment_GetNthEffectDuration")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectDuration(cdcg_index)
endFunction

; magiceffect Enchantment::GetNthEffectMagicEffect(...) native
magiceffect function Enchantment_GetNthEffectMagicEffect() global
	int session = Costile2.GetSession("Enchantment_GetNthEffectMagicEffect")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectMagicEffect(cdcg_index)
endFunction

; int Enchantment::GetCostliestEffectIndex(...) native
int function Enchantment_GetCostliestEffectIndex() global
	int session = Costile2.GetSession("Enchantment_GetCostliestEffectIndex")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	return this.GetCostliestEffectIndex()
endFunction

; void Enchantment::SetNthEffectMagnitude(...) native
function Enchantment_SetNthEffectMagnitude() global
	int session = Costile2.GetSession("Enchantment_SetNthEffectMagnitude")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	int cdcg_index = Costile2.GetInt(session, 1)
	float cdcg_value = Costile2.GetFloat(session, 2)
	this.SetNthEffectMagnitude(cdcg_index, cdcg_value)
endFunction

; void Enchantment::SetNthEffectArea(...) native
function Enchantment_SetNthEffectArea() global
	int session = Costile2.GetSession("Enchantment_SetNthEffectArea")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	int cdcg_index = Costile2.GetInt(session, 1)
	int cdcg_value = Costile2.GetInt(session, 2)
	this.SetNthEffectArea(cdcg_index, cdcg_value)
endFunction

; void Enchantment::SetNthEffectDuration(...) native
function Enchantment_SetNthEffectDuration() global
	int session = Costile2.GetSession("Enchantment_SetNthEffectDuration")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	int cdcg_index = Costile2.GetInt(session, 1)
	int cdcg_value = Costile2.GetInt(session, 2)
	this.SetNthEffectDuration(cdcg_index, cdcg_value)
endFunction

; enchantment Enchantment::GetBaseEnchantment(...) native
enchantment function Enchantment_GetBaseEnchantment() global
	int session = Costile2.GetSession("Enchantment_GetBaseEnchantment")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	return this.GetBaseEnchantment()
endFunction

; formlist Enchantment::GetKeywordRestrictions(...) native
formlist function Enchantment_GetKeywordRestrictions() global
	int session = Costile2.GetSession("Enchantment_GetKeywordRestrictions")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	return this.GetKeywordRestrictions()
endFunction

; void Enchantment::SetKeywordRestrictions(...) native
function Enchantment_SetKeywordRestrictions() global
	int session = Costile2.GetSession("Enchantment_SetKeywordRestrictions")
	Enchantment this = Costile2.GetTESForm(session, 0) as Enchantment
	formlist cdcg_newKeywordList = Costile2.GetTESForm(session, 1) as formlist
	this.SetKeywordRestrictions(cdcg_newKeywordList)
endFunction

