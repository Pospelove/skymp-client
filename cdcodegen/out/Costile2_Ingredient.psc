Scriptname Costile2_Ingredient Hidden

;
;
;
; script Ingredient.psc
;
;
;

; bool Ingredient::IsHostile(...) native
bool function Ingredient_IsHostile() global
	int session = Costile2.GetSession("Ingredient_IsHostile")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	return this.IsHostile()
endFunction

; void Ingredient::LearnEffect(...) native
function Ingredient_LearnEffect() global
	int session = Costile2.GetSession("Ingredient_LearnEffect")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	int cdcg_aiIndex = Costile2.GetInt(session, 1)
	this.LearnEffect(cdcg_aiIndex)
endFunction

; int Ingredient::LearnNextEffect(...) native
int function Ingredient_LearnNextEffect() global
	int session = Costile2.GetSession("Ingredient_LearnNextEffect")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	return this.LearnNextEffect()
endFunction

; void Ingredient::LearnAllEffects(...) native
function Ingredient_LearnAllEffects() global
	int session = Costile2.GetSession("Ingredient_LearnAllEffects")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	this.LearnAllEffects()
endFunction

; int Ingredient::GetNumEffects(...) native
int function Ingredient_GetNumEffects() global
	int session = Costile2.GetSession("Ingredient_GetNumEffects")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	return this.GetNumEffects()
endFunction

; float Ingredient::GetNthEffectMagnitude(...) native
float function Ingredient_GetNthEffectMagnitude() global
	int session = Costile2.GetSession("Ingredient_GetNthEffectMagnitude")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectMagnitude(cdcg_index)
endFunction

; int Ingredient::GetNthEffectArea(...) native
int function Ingredient_GetNthEffectArea() global
	int session = Costile2.GetSession("Ingredient_GetNthEffectArea")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectArea(cdcg_index)
endFunction

; int Ingredient::GetNthEffectDuration(...) native
int function Ingredient_GetNthEffectDuration() global
	int session = Costile2.GetSession("Ingredient_GetNthEffectDuration")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectDuration(cdcg_index)
endFunction

; magiceffect Ingredient::GetNthEffectMagicEffect(...) native
magiceffect function Ingredient_GetNthEffectMagicEffect() global
	int session = Costile2.GetSession("Ingredient_GetNthEffectMagicEffect")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectMagicEffect(cdcg_index)
endFunction

; int Ingredient::GetCostliestEffectIndex(...) native
int function Ingredient_GetCostliestEffectIndex() global
	int session = Costile2.GetSession("Ingredient_GetCostliestEffectIndex")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	return this.GetCostliestEffectIndex()
endFunction

; void Ingredient::SetNthEffectMagnitude(...) native
function Ingredient_SetNthEffectMagnitude() global
	int session = Costile2.GetSession("Ingredient_SetNthEffectMagnitude")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	int cdcg_index = Costile2.GetInt(session, 1)
	float cdcg_value = Costile2.GetFloat(session, 2)
	this.SetNthEffectMagnitude(cdcg_index, cdcg_value)
endFunction

; void Ingredient::SetNthEffectArea(...) native
function Ingredient_SetNthEffectArea() global
	int session = Costile2.GetSession("Ingredient_SetNthEffectArea")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	int cdcg_index = Costile2.GetInt(session, 1)
	int cdcg_value = Costile2.GetInt(session, 2)
	this.SetNthEffectArea(cdcg_index, cdcg_value)
endFunction

; void Ingredient::SetNthEffectDuration(...) native
function Ingredient_SetNthEffectDuration() global
	int session = Costile2.GetSession("Ingredient_SetNthEffectDuration")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	int cdcg_index = Costile2.GetInt(session, 1)
	int cdcg_value = Costile2.GetInt(session, 2)
	this.SetNthEffectDuration(cdcg_index, cdcg_value)
endFunction

; bool Ingredient::GetIsNthEffectKnown(...) native
bool function Ingredient_GetIsNthEffectKnown() global
	int session = Costile2.GetSession("Ingredient_GetIsNthEffectKnown")
	Ingredient this = Costile2.GetTESForm(session, 0) as Ingredient
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetIsNthEffectKnown(cdcg_index)
endFunction

