Scriptname Costile2_Topic Hidden

;
;
;
; script Topic.psc
;
;
;

; void Topic::Add(...) native
function Topic_Add() global
	int session = Costile2.GetSession("Topic_Add")
	Topic this = Costile2.GetTESForm(session, 0) as Topic
	this.Add()
endFunction

