Scriptname Costile2_Outfit Hidden

;
;
;
; script Outfit.psc
;
;
;

; int Outfit::GetNumParts(...) native
int function Outfit_GetNumParts() global
	int session = Costile2.GetSession("Outfit_GetNumParts")
	Outfit this = Costile2.GetTESForm(session, 0) as Outfit
	return this.GetNumParts()
endFunction

; form Outfit::GetNthPart(...) native
form function Outfit_GetNthPart() global
	int session = Costile2.GetSession("Outfit_GetNthPart")
	Outfit this = Costile2.GetTESForm(session, 0) as Outfit
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthPart(cdcg_n)
endFunction

