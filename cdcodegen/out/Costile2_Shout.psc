Scriptname Costile2_Shout Hidden

;
;
;
; script Shout.psc
;
;
;

; wordofpower Shout::GetNthWordOfPower(...) native
wordofpower function Shout_GetNthWordOfPower() global
	int session = Costile2.GetSession("Shout_GetNthWordOfPower")
	Shout this = Costile2.GetTESForm(session, 0) as Shout
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthWordOfPower(cdcg_n)
endFunction

; spell Shout::GetNthSpell(...) native
spell function Shout_GetNthSpell() global
	int session = Costile2.GetSession("Shout_GetNthSpell")
	Shout this = Costile2.GetTESForm(session, 0) as Shout
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthSpell(cdcg_n)
endFunction

; float Shout::GetNthRecoveryTime(...) native
float function Shout_GetNthRecoveryTime() global
	int session = Costile2.GetSession("Shout_GetNthRecoveryTime")
	Shout this = Costile2.GetTESForm(session, 0) as Shout
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthRecoveryTime(cdcg_n)
endFunction

; void Shout::SetNthWordOfPower(...) native
function Shout_SetNthWordOfPower() global
	int session = Costile2.GetSession("Shout_SetNthWordOfPower")
	Shout this = Costile2.GetTESForm(session, 0) as Shout
	int cdcg_n = Costile2.GetInt(session, 1)
	wordofpower cdcg_aWoop = Costile2.GetTESForm(session, 2) as wordofpower
	this.SetNthWordOfPower(cdcg_n, cdcg_aWoop)
endFunction

; void Shout::SetNthSpell(...) native
function Shout_SetNthSpell() global
	int session = Costile2.GetSession("Shout_SetNthSpell")
	Shout this = Costile2.GetTESForm(session, 0) as Shout
	int cdcg_n = Costile2.GetInt(session, 1)
	spell cdcg_aSpell = Costile2.GetTESForm(session, 2) as spell
	this.SetNthSpell(cdcg_n, cdcg_aSpell)
endFunction

; void Shout::SetNthRecoveryTime(...) native
function Shout_SetNthRecoveryTime() global
	int session = Costile2.GetSession("Shout_SetNthRecoveryTime")
	Shout this = Costile2.GetTESForm(session, 0) as Shout
	int cdcg_n = Costile2.GetInt(session, 1)
	float cdcg_time = Costile2.GetFloat(session, 2)
	this.SetNthRecoveryTime(cdcg_n, cdcg_time)
endFunction

