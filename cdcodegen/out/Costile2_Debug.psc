Scriptname Costile2_Debug Hidden

;
;
;
; script Debug.psc
;
;
;

; string Debug::GetPlatformName(...) native global
string function Debug_GetPlatformName() global
	int session = Costile2.GetSession("Debug_GetPlatformName")
	return Debug.GetPlatformName()
endFunction

; string Debug::GetVersionNumber(...) native global
string function Debug_GetVersionNumber() global
	int session = Costile2.GetSession("Debug_GetVersionNumber")
	return Debug.GetVersionNumber()
endFunction

; void Debug::MessageBox(...) native global
function Debug_MessageBox() global
	int session = Costile2.GetSession("Debug_MessageBox")
	string cdcg_asMessageBoxText = Costile2.GetString(session, 0)
	Debug.MessageBox(cdcg_asMessageBoxText)
endFunction

; void Debug::Notification(...) native global
function Debug_Notification() global
	int session = Costile2.GetSession("Debug_Notification")
	string cdcg_asNotificationText = Costile2.GetString(session, 0)
	Debug.Notification(cdcg_asNotificationText)
endFunction

; void Debug::QuitGame(...) native global
function Debug_QuitGame() global
	int session = Costile2.GetSession("Debug_QuitGame")
	Debug.QuitGame()
endFunction

; void Debug::SetFootIK(...) native global
function Debug_SetFootIK() global
	int session = Costile2.GetSession("Debug_SetFootIK")
	bool cdcg_abFootIK = Costile2.GetBool(session, 0)
	Debug.SetFootIK(cdcg_abFootIK)
endFunction

; void Debug::SetGodMode(...) native global
function Debug_SetGodMode() global
	int session = Costile2.GetSession("Debug_SetGodMode")
	bool cdcg_abGodMode = Costile2.GetBool(session, 0)
	Debug.SetGodMode(cdcg_abGodMode)
endFunction

; void Debug::SendAnimationEvent(...) native global
function Debug_SendAnimationEvent() global
	int session = Costile2.GetSession("Debug_SendAnimationEvent")
	objectreference cdcg_arRef = Costile2.GetTESForm(session, 0) as objectreference
	string cdcg_asEventName = Costile2.GetString(session, 1)
	Debug.SendAnimationEvent(cdcg_arRef, cdcg_asEventName)
endFunction

; void Debug::TakeScreenshot(...) native global
function Debug_TakeScreenshot() global
	int session = Costile2.GetSession("Debug_TakeScreenshot")
	string cdcg_asFilename = Costile2.GetString(session, 0)
	Debug.TakeScreenshot(cdcg_asFilename)
endFunction

; void Debug::ToggleAI(...) native global
function Debug_ToggleAI() global
	int session = Costile2.GetSession("Debug_ToggleAI")
	Debug.ToggleAI()
endFunction

; void Debug::ToggleCollisions(...) native global
function Debug_ToggleCollisions() global
	int session = Costile2.GetSession("Debug_ToggleCollisions")
	Debug.ToggleCollisions()
endFunction

; void Debug::ToggleMenus(...) native global
function Debug_ToggleMenus() global
	int session = Costile2.GetSession("Debug_ToggleMenus")
	Debug.ToggleMenus()
endFunction

