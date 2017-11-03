Scriptname Costile2_Apparatus Hidden

;
;
;
; script Apparatus.psc
;
;
;

; int Apparatus::GetQuality(...) native
int function Apparatus_GetQuality() global
	int session = Costile2.GetSession("Apparatus_GetQuality")
	Apparatus this = Costile2.GetTESForm(session, 0) as Apparatus
	return this.GetQuality()
endFunction

; void Apparatus::SetQuality(...) native
function Apparatus_SetQuality() global
	int session = Costile2.GetSession("Apparatus_SetQuality")
	Apparatus this = Costile2.GetTESForm(session, 0) as Apparatus
	int cdcg_quality = Costile2.GetInt(session, 1)
	this.SetQuality(cdcg_quality)
endFunction

