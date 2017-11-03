Scriptname Costile2_HeadPart Hidden

;
;
;
; script HeadPart.psc
;
;
;

; headpart HeadPart::GetHeadPart(...) native global
headpart function HeadPart_GetHeadPart() global
	int session = Costile2.GetSession("HeadPart_GetHeadPart")
	string cdcg_name = Costile2.GetString(session, 0)
	return HeadPart.GetHeadPart(cdcg_name)
endFunction

; int HeadPart::GetType(...) native
int function HeadPart_GetType() global
	int session = Costile2.GetSession("HeadPart_GetType")
	HeadPart this = Costile2.GetTESForm(session, 0) as HeadPart
	return this.GetType()
endFunction

; int HeadPart::GetNumExtraParts(...) native
int function HeadPart_GetNumExtraParts() global
	int session = Costile2.GetSession("HeadPart_GetNumExtraParts")
	HeadPart this = Costile2.GetTESForm(session, 0) as HeadPart
	return this.GetNumExtraParts()
endFunction

; headpart HeadPart::GetNthExtraPart(...) native
headpart function HeadPart_GetNthExtraPart() global
	int session = Costile2.GetSession("HeadPart_GetNthExtraPart")
	HeadPart this = Costile2.GetTESForm(session, 0) as HeadPart
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthExtraPart(cdcg_n)
endFunction

; bool HeadPart::HasExtraPart(...) native
bool function HeadPart_HasExtraPart() global
	int session = Costile2.GetSession("HeadPart_HasExtraPart")
	HeadPart this = Costile2.GetTESForm(session, 0) as HeadPart
	headpart cdcg_p = Costile2.GetTESForm(session, 1) as headpart
	return this.HasExtraPart(cdcg_p)
endFunction

; int HeadPart::GetIndexOfExtraPart(...) native
int function HeadPart_GetIndexOfExtraPart() global
	int session = Costile2.GetSession("HeadPart_GetIndexOfExtraPart")
	HeadPart this = Costile2.GetTESForm(session, 0) as HeadPart
	headpart cdcg_p = Costile2.GetTESForm(session, 1) as headpart
	return this.GetIndexOfExtraPart(cdcg_p)
endFunction

; formlist HeadPart::GetValidRaces(...) native
formlist function HeadPart_GetValidRaces() global
	int session = Costile2.GetSession("HeadPart_GetValidRaces")
	HeadPart this = Costile2.GetTESForm(session, 0) as HeadPart
	return this.GetValidRaces()
endFunction

; void HeadPart::SetValidRaces(...) native
function HeadPart_SetValidRaces() global
	int session = Costile2.GetSession("HeadPart_SetValidRaces")
	HeadPart this = Costile2.GetTESForm(session, 0) as HeadPart
	formlist cdcg_vRaces = Costile2.GetTESForm(session, 1) as formlist
	this.SetValidRaces(cdcg_vRaces)
endFunction

; bool HeadPart::IsExtraPart(...) native
bool function HeadPart_IsExtraPart() global
	int session = Costile2.GetSession("HeadPart_IsExtraPart")
	HeadPart this = Costile2.GetTESForm(session, 0) as HeadPart
	return this.IsExtraPart()
endFunction

