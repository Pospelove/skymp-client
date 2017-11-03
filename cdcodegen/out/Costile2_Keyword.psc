Scriptname Costile2_Keyword Hidden

;
;
;
; script Keyword.psc
;
;
;

; void Keyword::SendStoryEvent(...) native
function Keyword_SendStoryEvent() global
	int session = Costile2.GetSession("Keyword_SendStoryEvent")
	Keyword this = Costile2.GetTESForm(session, 0) as Keyword
	location cdcg_akLoc = Costile2.GetTESForm(session, 1) as location
	objectreference cdcg_akRef1 = Costile2.GetTESForm(session, 2) as objectreference
	objectreference cdcg_akRef2 = Costile2.GetTESForm(session, 3) as objectreference
	int cdcg_aiValue1 = Costile2.GetInt(session, 4)
	int cdcg_aiValue2 = Costile2.GetInt(session, 5)
	this.SendStoryEvent(cdcg_akLoc, cdcg_akRef1, cdcg_akRef2, cdcg_aiValue1, cdcg_aiValue2)
endFunction

; bool Keyword::SendStoryEventAndWait(...) native
bool function Keyword_SendStoryEventAndWait() global
	int session = Costile2.GetSession("Keyword_SendStoryEventAndWait")
	Keyword this = Costile2.GetTESForm(session, 0) as Keyword
	location cdcg_akLoc = Costile2.GetTESForm(session, 1) as location
	objectreference cdcg_akRef1 = Costile2.GetTESForm(session, 2) as objectreference
	objectreference cdcg_akRef2 = Costile2.GetTESForm(session, 3) as objectreference
	int cdcg_aiValue1 = Costile2.GetInt(session, 4)
	int cdcg_aiValue2 = Costile2.GetInt(session, 5)
	return this.SendStoryEventAndWait(cdcg_akLoc, cdcg_akRef1, cdcg_akRef2, cdcg_aiValue1, cdcg_aiValue2)
endFunction

; keyword Keyword::GetKeyword(...) native global
keyword function Keyword_GetKeyword() global
	int session = Costile2.GetSession("Keyword_GetKeyword")
	string cdcg_key = Costile2.GetString(session, 0)
	return Keyword.GetKeyword(cdcg_key)
endFunction

; string Keyword::GetString(...) native
string function Keyword_GetString() global
	int session = Costile2.GetSession("Keyword_GetString")
	Keyword this = Costile2.GetTESForm(session, 0) as Keyword
	return this.GetString()
endFunction

