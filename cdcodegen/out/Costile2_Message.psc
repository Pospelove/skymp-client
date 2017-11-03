Scriptname Costile2_Message Hidden

;
;
;
; script Message.psc
;
;
;

; int Message::Show(...) native
int function Message_Show() global
	int session = Costile2.GetSession("Message_Show")
	Message this = Costile2.GetTESForm(session, 0) as Message
	float cdcg_afArg1 = Costile2.GetFloat(session, 1)
	float cdcg_afArg2 = Costile2.GetFloat(session, 2)
	float cdcg_afArg3 = Costile2.GetFloat(session, 3)
	float cdcg_afArg4 = Costile2.GetFloat(session, 4)
	float cdcg_afArg5 = Costile2.GetFloat(session, 5)
	float cdcg_afArg6 = Costile2.GetFloat(session, 6)
	float cdcg_afArg7 = Costile2.GetFloat(session, 7)
	float cdcg_afArg8 = Costile2.GetFloat(session, 8)
	float cdcg_afArg9 = Costile2.GetFloat(session, 9)
	return this.Show(cdcg_afArg1, cdcg_afArg2, cdcg_afArg3, cdcg_afArg4, cdcg_afArg5, cdcg_afArg6, cdcg_afArg7, cdcg_afArg8, cdcg_afArg9)
endFunction

; void Message::ShowAsHelpMessage(...) native
function Message_ShowAsHelpMessage() global
	int session = Costile2.GetSession("Message_ShowAsHelpMessage")
	Message this = Costile2.GetTESForm(session, 0) as Message
	string cdcg_asEvent = Costile2.GetString(session, 1)
	float cdcg_afDuration = Costile2.GetFloat(session, 2)
	float cdcg_afInterval = Costile2.GetFloat(session, 3)
	int cdcg_aiMaxTimes = Costile2.GetInt(session, 4)
	this.ShowAsHelpMessage(cdcg_asEvent, cdcg_afDuration, cdcg_afInterval, cdcg_aiMaxTimes)
endFunction

; void Message::ResetHelpMessage(...) native global
function Message_ResetHelpMessage() global
	int session = Costile2.GetSession("Message_ResetHelpMessage")
	string cdcg_asEvent = Costile2.GetString(session, 0)
	Message.ResetHelpMessage(cdcg_asEvent)
endFunction

