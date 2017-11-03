Scriptname Costile2_Book Hidden

;
;
;
; script Book.psc
;
;
;

; spell Book::GetSpell(...) native
spell function Book_GetSpell() global
	int session = Costile2.GetSession("Book_GetSpell")
	Book this = Costile2.GetTESForm(session, 0) as Book
	return this.GetSpell()
endFunction

; int Book::GetSkill(...) native
int function Book_GetSkill() global
	int session = Costile2.GetSession("Book_GetSkill")
	Book this = Costile2.GetTESForm(session, 0) as Book
	return this.GetSkill()
endFunction

; bool Book::IsRead(...) native
bool function Book_IsRead() global
	int session = Costile2.GetSession("Book_IsRead")
	Book this = Costile2.GetTESForm(session, 0) as Book
	return this.IsRead()
endFunction

; bool Book::IsTakeable(...) native
bool function Book_IsTakeable() global
	int session = Costile2.GetSession("Book_IsTakeable")
	Book this = Costile2.GetTESForm(session, 0) as Book
	return this.IsTakeable()
endFunction

