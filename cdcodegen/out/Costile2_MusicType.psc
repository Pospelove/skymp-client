Scriptname Costile2_MusicType Hidden

;
;
;
; script MusicType.psc
;
;
;

; void MusicType::Add(...) native
function MusicType_Add() global
	int session = Costile2.GetSession("MusicType_Add")
	MusicType this = Costile2.GetTESForm(session, 0) as MusicType
	this.Add()
endFunction

; void MusicType::Remove(...) native
function MusicType_Remove() global
	int session = Costile2.GetSession("MusicType_Remove")
	MusicType this = Costile2.GetTESForm(session, 0) as MusicType
	this.Remove()
endFunction

