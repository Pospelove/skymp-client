Scriptname Costile2_LeveledActor Hidden

;
;
;
; script LeveledActor.psc
;
;
;

; void LeveledActor::AddForm(...) native
function LeveledActor_AddForm() global
	int session = Costile2.GetSession("LeveledActor_AddForm")
	LeveledActor this = Costile2.GetTESForm(session, 0) as LeveledActor
	form cdcg_apForm = Costile2.GetTESForm(session, 1) as form
	int cdcg_aiLevel = Costile2.GetInt(session, 2)
	this.AddForm(cdcg_apForm, cdcg_aiLevel)
endFunction

; void LeveledActor::Revert(...) native
function LeveledActor_Revert() global
	int session = Costile2.GetSession("LeveledActor_Revert")
	LeveledActor this = Costile2.GetTESForm(session, 0) as LeveledActor
	this.Revert()
endFunction

; int LeveledActor::GetNumForms(...) native
int function LeveledActor_GetNumForms() global
	int session = Costile2.GetSession("LeveledActor_GetNumForms")
	LeveledActor this = Costile2.GetTESForm(session, 0) as LeveledActor
	return this.GetNumForms()
endFunction

; form LeveledActor::GetNthForm(...) native
form function LeveledActor_GetNthForm() global
	int session = Costile2.GetSession("LeveledActor_GetNthForm")
	LeveledActor this = Costile2.GetTESForm(session, 0) as LeveledActor
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthForm(cdcg_n)
endFunction

; int LeveledActor::GetNthLevel(...) native
int function LeveledActor_GetNthLevel() global
	int session = Costile2.GetSession("LeveledActor_GetNthLevel")
	LeveledActor this = Costile2.GetTESForm(session, 0) as LeveledActor
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthLevel(cdcg_n)
endFunction

; void LeveledActor::SetNthLevel(...) native
function LeveledActor_SetNthLevel() global
	int session = Costile2.GetSession("LeveledActor_SetNthLevel")
	LeveledActor this = Costile2.GetTESForm(session, 0) as LeveledActor
	int cdcg_n = Costile2.GetInt(session, 1)
	int cdcg_level = Costile2.GetInt(session, 2)
	this.SetNthLevel(cdcg_n, cdcg_level)
endFunction

; int LeveledActor::GetNthCount(...) native
int function LeveledActor_GetNthCount() global
	int session = Costile2.GetSession("LeveledActor_GetNthCount")
	LeveledActor this = Costile2.GetTESForm(session, 0) as LeveledActor
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthCount(cdcg_n)
endFunction

; void LeveledActor::SetNthCount(...) native
function LeveledActor_SetNthCount() global
	int session = Costile2.GetSession("LeveledActor_SetNthCount")
	LeveledActor this = Costile2.GetTESForm(session, 0) as LeveledActor
	int cdcg_n = Costile2.GetInt(session, 1)
	int cdcg_count = Costile2.GetInt(session, 2)
	this.SetNthCount(cdcg_n, cdcg_count)
endFunction

