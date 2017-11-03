Scriptname Costile2_LeveledItem Hidden

;
;
;
; script LeveledItem.psc
;
;
;

; void LeveledItem::AddForm(...) native
function LeveledItem_AddForm() global
	int session = Costile2.GetSession("LeveledItem_AddForm")
	LeveledItem this = Costile2.GetTESForm(session, 0) as LeveledItem
	form cdcg_apForm = Costile2.GetTESForm(session, 1) as form
	int cdcg_aiLevel = Costile2.GetInt(session, 2)
	int cdcg_aiCount = Costile2.GetInt(session, 3)
	this.AddForm(cdcg_apForm, cdcg_aiLevel, cdcg_aiCount)
endFunction

; void LeveledItem::Revert(...) native
function LeveledItem_Revert() global
	int session = Costile2.GetSession("LeveledItem_Revert")
	LeveledItem this = Costile2.GetTESForm(session, 0) as LeveledItem
	this.Revert()
endFunction

; int LeveledItem::GetChanceNone(...) native
int function LeveledItem_GetChanceNone() global
	int session = Costile2.GetSession("LeveledItem_GetChanceNone")
	LeveledItem this = Costile2.GetTESForm(session, 0) as LeveledItem
	return this.GetChanceNone()
endFunction

; void LeveledItem::SetChanceNone(...) native
function LeveledItem_SetChanceNone() global
	int session = Costile2.GetSession("LeveledItem_SetChanceNone")
	LeveledItem this = Costile2.GetTESForm(session, 0) as LeveledItem
	int cdcg_chance = Costile2.GetInt(session, 1)
	this.SetChanceNone(cdcg_chance)
endFunction

; globalvariable LeveledItem::GetChanceGlobal(...) native
globalvariable function LeveledItem_GetChanceGlobal() global
	int session = Costile2.GetSession("LeveledItem_GetChanceGlobal")
	LeveledItem this = Costile2.GetTESForm(session, 0) as LeveledItem
	return this.GetChanceGlobal()
endFunction

; void LeveledItem::SetChanceGlobal(...) native
function LeveledItem_SetChanceGlobal() global
	int session = Costile2.GetSession("LeveledItem_SetChanceGlobal")
	LeveledItem this = Costile2.GetTESForm(session, 0) as LeveledItem
	globalvariable cdcg_glob = Costile2.GetTESForm(session, 1) as globalvariable
	this.SetChanceGlobal(cdcg_glob)
endFunction

; int LeveledItem::GetNumForms(...) native
int function LeveledItem_GetNumForms() global
	int session = Costile2.GetSession("LeveledItem_GetNumForms")
	LeveledItem this = Costile2.GetTESForm(session, 0) as LeveledItem
	return this.GetNumForms()
endFunction

; form LeveledItem::GetNthForm(...) native
form function LeveledItem_GetNthForm() global
	int session = Costile2.GetSession("LeveledItem_GetNthForm")
	LeveledItem this = Costile2.GetTESForm(session, 0) as LeveledItem
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthForm(cdcg_n)
endFunction

; int LeveledItem::GetNthLevel(...) native
int function LeveledItem_GetNthLevel() global
	int session = Costile2.GetSession("LeveledItem_GetNthLevel")
	LeveledItem this = Costile2.GetTESForm(session, 0) as LeveledItem
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthLevel(cdcg_n)
endFunction

; void LeveledItem::SetNthLevel(...) native
function LeveledItem_SetNthLevel() global
	int session = Costile2.GetSession("LeveledItem_SetNthLevel")
	LeveledItem this = Costile2.GetTESForm(session, 0) as LeveledItem
	int cdcg_n = Costile2.GetInt(session, 1)
	int cdcg_level = Costile2.GetInt(session, 2)
	this.SetNthLevel(cdcg_n, cdcg_level)
endFunction

; int LeveledItem::GetNthCount(...) native
int function LeveledItem_GetNthCount() global
	int session = Costile2.GetSession("LeveledItem_GetNthCount")
	LeveledItem this = Costile2.GetTESForm(session, 0) as LeveledItem
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthCount(cdcg_n)
endFunction

; void LeveledItem::SetNthCount(...) native
function LeveledItem_SetNthCount() global
	int session = Costile2.GetSession("LeveledItem_SetNthCount")
	LeveledItem this = Costile2.GetTESForm(session, 0) as LeveledItem
	int cdcg_n = Costile2.GetInt(session, 1)
	int cdcg_count = Costile2.GetInt(session, 2)
	this.SetNthCount(cdcg_n, cdcg_count)
endFunction

