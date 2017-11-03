Scriptname Costile2_SoulGem Hidden

;
;
;
; script SoulGem.psc
;
;
;

; int SoulGem::GetSoulSize(...) native
int function SoulGem_GetSoulSize() global
	int session = Costile2.GetSession("SoulGem_GetSoulSize")
	SoulGem this = Costile2.GetTESForm(session, 0) as SoulGem
	return this.GetSoulSize()
endFunction

; int SoulGem::GetGemSize(...) native
int function SoulGem_GetGemSize() global
	int session = Costile2.GetSession("SoulGem_GetGemSize")
	SoulGem this = Costile2.GetTESForm(session, 0) as SoulGem
	return this.GetGemSize()
endFunction

