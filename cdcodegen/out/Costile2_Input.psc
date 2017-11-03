Scriptname Costile2_Input Hidden

;
;
;
; script Input.psc
;
;
;

; bool Input::IsKeyPressed(...) native global
bool function Input_IsKeyPressed() global
	int session = Costile2.GetSession("Input_IsKeyPressed")
	int cdcg_dxKeycode = Costile2.GetInt(session, 0)
	return Input.IsKeyPressed(cdcg_dxKeycode)
endFunction

; void Input::TapKey(...) native global
function Input_TapKey() global
	int session = Costile2.GetSession("Input_TapKey")
	int cdcg_dxKeycode = Costile2.GetInt(session, 0)
	Input.TapKey(cdcg_dxKeycode)
endFunction

; void Input::HoldKey(...) native global
function Input_HoldKey() global
	int session = Costile2.GetSession("Input_HoldKey")
	int cdcg_dxKeycode = Costile2.GetInt(session, 0)
	Input.HoldKey(cdcg_dxKeycode)
endFunction

; void Input::ReleaseKey(...) native global
function Input_ReleaseKey() global
	int session = Costile2.GetSession("Input_ReleaseKey")
	int cdcg_dxKeycode = Costile2.GetInt(session, 0)
	Input.ReleaseKey(cdcg_dxKeycode)
endFunction

; int Input::GetNumKeysPressed(...) native global
int function Input_GetNumKeysPressed() global
	int session = Costile2.GetSession("Input_GetNumKeysPressed")
	return Input.GetNumKeysPressed()
endFunction

; int Input::GetNthKeyPressed(...) native global
int function Input_GetNthKeyPressed() global
	int session = Costile2.GetSession("Input_GetNthKeyPressed")
	int cdcg_n = Costile2.GetInt(session, 0)
	return Input.GetNthKeyPressed(cdcg_n)
endFunction

; int Input::GetMappedKey(...) native global
int function Input_GetMappedKey() global
	int session = Costile2.GetSession("Input_GetMappedKey")
	string cdcg_control = Costile2.GetString(session, 0)
	int cdcg_deviceType = Costile2.GetInt(session, 1)
	return Input.GetMappedKey(cdcg_control, cdcg_deviceType)
endFunction

; string Input::GetMappedControl(...) native global
string function Input_GetMappedControl() global
	int session = Costile2.GetSession("Input_GetMappedControl")
	int cdcg_keycode = Costile2.GetInt(session, 0)
	return Input.GetMappedControl(cdcg_keycode)
endFunction

