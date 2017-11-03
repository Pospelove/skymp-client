Scriptname Costile2_GlobalVariable Hidden

;
;
;
; script GlobalVariable.psc
;
;
;

; float GlobalVariable::GetValue(...) native
float function GlobalVariable_GetValue() global
	int session = Costile2.GetSession("GlobalVariable_GetValue")
	GlobalVariable this = Costile2.GetTESForm(session, 0) as GlobalVariable
	return this.GetValue()
endFunction

; void GlobalVariable::SetValue(...) native
function GlobalVariable_SetValue() global
	int session = Costile2.GetSession("GlobalVariable_SetValue")
	GlobalVariable this = Costile2.GetTESForm(session, 0) as GlobalVariable
	float cdcg_afNewValue = Costile2.GetFloat(session, 1)
	this.SetValue(cdcg_afNewValue)
endFunction

; int GlobalVariable::GetValueInt(...)
int function GlobalVariable_GetValueInt() global
	int session = Costile2.GetSession("GlobalVariable_GetValueInt")
	GlobalVariable this = Costile2.GetTESForm(session, 0) as GlobalVariable
	return this.GetValueInt()
endFunction

; void GlobalVariable::SetValueInt(...)
function GlobalVariable_SetValueInt() global
	int session = Costile2.GetSession("GlobalVariable_SetValueInt")
	GlobalVariable this = Costile2.GetTESForm(session, 0) as GlobalVariable
	int cdcg_aiNewValue = Costile2.GetInt(session, 1)
	this.SetValueInt(cdcg_aiNewValue)
endFunction

; float GlobalVariable::Mod(...)
float function GlobalVariable_Mod() global
	int session = Costile2.GetSession("GlobalVariable_Mod")
	GlobalVariable this = Costile2.GetTESForm(session, 0) as GlobalVariable
	float cdcg_afHowMuch = Costile2.GetFloat(session, 1)
	return this.Mod(cdcg_afHowMuch)
endFunction

