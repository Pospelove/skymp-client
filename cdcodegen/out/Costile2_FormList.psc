Scriptname Costile2_FormList Hidden

;
;
;
; script FormList.psc
;
;
;

; void FormList::AddForm(...) native
function FormList_AddForm() global
	int session = Costile2.GetSession("FormList_AddForm")
	FormList this = Costile2.GetTESForm(session, 0) as FormList
	form cdcg_apForm = Costile2.GetTESForm(session, 1) as form
	this.AddForm(cdcg_apForm)
endFunction

; int FormList::Find(...) native
int function FormList_Find() global
	int session = Costile2.GetSession("FormList_Find")
	FormList this = Costile2.GetTESForm(session, 0) as FormList
	form cdcg_apForm = Costile2.GetTESForm(session, 1) as form
	return this.Find(cdcg_apForm)
endFunction

; int FormList::GetSize(...) native
int function FormList_GetSize() global
	int session = Costile2.GetSession("FormList_GetSize")
	FormList this = Costile2.GetTESForm(session, 0) as FormList
	return this.GetSize()
endFunction

; form FormList::GetAt(...) native
form function FormList_GetAt() global
	int session = Costile2.GetSession("FormList_GetAt")
	FormList this = Costile2.GetTESForm(session, 0) as FormList
	int cdcg_aiIndex = Costile2.GetInt(session, 1)
	return this.GetAt(cdcg_aiIndex)
endFunction

; bool FormList::HasForm(...) native
bool function FormList_HasForm() global
	int session = Costile2.GetSession("FormList_HasForm")
	FormList this = Costile2.GetTESForm(session, 0) as FormList
	form cdcg_akForm = Costile2.GetTESForm(session, 1) as form
	return this.HasForm(cdcg_akForm)
endFunction

; void FormList::RemoveAddedForm(...) native
function FormList_RemoveAddedForm() global
	int session = Costile2.GetSession("FormList_RemoveAddedForm")
	FormList this = Costile2.GetTESForm(session, 0) as FormList
	form cdcg_apForm = Costile2.GetTESForm(session, 1) as form
	this.RemoveAddedForm(cdcg_apForm)
endFunction

; void FormList::Revert(...) native
function FormList_Revert() global
	int session = Costile2.GetSession("FormList_Revert")
	FormList this = Costile2.GetTESForm(session, 0) as FormList
	this.Revert()
endFunction

