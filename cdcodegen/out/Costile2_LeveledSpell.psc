Scriptname Costile2_LeveledSpell Hidden

;
;
;
; script LeveledSpell.psc
;
;
;

; void LeveledSpell::AddForm(...) native
function LeveledSpell_AddForm() global
	int session = Costile2.GetSession("LeveledSpell_AddForm")
	LeveledSpell this = Costile2.GetTESForm(session, 0) as LeveledSpell
	form cdcg_apForm = Costile2.GetTESForm(session, 1) as form
	int cdcg_aiLevel = Costile2.GetInt(session, 2)
	this.AddForm(cdcg_apForm, cdcg_aiLevel)
endFunction

; void LeveledSpell::Revert(...) native
function LeveledSpell_Revert() global
	int session = Costile2.GetSession("LeveledSpell_Revert")
	LeveledSpell this = Costile2.GetTESForm(session, 0) as LeveledSpell
	this.Revert()
endFunction

; int LeveledSpell::GetChanceNone(...) native
int function LeveledSpell_GetChanceNone() global
	int session = Costile2.GetSession("LeveledSpell_GetChanceNone")
	LeveledSpell this = Costile2.GetTESForm(session, 0) as LeveledSpell
	return this.GetChanceNone()
endFunction

; void LeveledSpell::SetChanceNone(...) native
function LeveledSpell_SetChanceNone() global
	int session = Costile2.GetSession("LeveledSpell_SetChanceNone")
	LeveledSpell this = Costile2.GetTESForm(session, 0) as LeveledSpell
	int cdcg_chance = Costile2.GetInt(session, 1)
	this.SetChanceNone(cdcg_chance)
endFunction

; int LeveledSpell::GetNumForms(...) native
int function LeveledSpell_GetNumForms() global
	int session = Costile2.GetSession("LeveledSpell_GetNumForms")
	LeveledSpell this = Costile2.GetTESForm(session, 0) as LeveledSpell
	return this.GetNumForms()
endFunction

; form LeveledSpell::GetNthForm(...) native
form function LeveledSpell_GetNthForm() global
	int session = Costile2.GetSession("LeveledSpell_GetNthForm")
	LeveledSpell this = Costile2.GetTESForm(session, 0) as LeveledSpell
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthForm(cdcg_n)
endFunction

; int LeveledSpell::GetNthLevel(...) native
int function LeveledSpell_GetNthLevel() global
	int session = Costile2.GetSession("LeveledSpell_GetNthLevel")
	LeveledSpell this = Costile2.GetTESForm(session, 0) as LeveledSpell
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthLevel(cdcg_n)
endFunction

; void LeveledSpell::SetNthLevel(...) native
function LeveledSpell_SetNthLevel() global
	int session = Costile2.GetSession("LeveledSpell_SetNthLevel")
	LeveledSpell this = Costile2.GetTESForm(session, 0) as LeveledSpell
	int cdcg_n = Costile2.GetInt(session, 1)
	int cdcg_level = Costile2.GetInt(session, 2)
	this.SetNthLevel(cdcg_n, cdcg_level)
endFunction

