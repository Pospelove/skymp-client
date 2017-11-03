Scriptname Costile2_Art Hidden

;
;
;
; script Art.psc
;
;
;

; string Art::GetModelPath(...) native
string function Art_GetModelPath() global
	int session = Costile2.GetSession("Art_GetModelPath")
	Art this = Costile2.GetTESForm(session, 0) as Art
	return this.GetModelPath()
endFunction

; void Art::SetModelPath(...) native
function Art_SetModelPath() global
	int session = Costile2.GetSession("Art_SetModelPath")
	Art this = Costile2.GetTESForm(session, 0) as Art
	string cdcg_path = Costile2.GetString(session, 1)
	this.SetModelPath(cdcg_path)
endFunction

