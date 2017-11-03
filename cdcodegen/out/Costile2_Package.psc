Scriptname Costile2_Package Hidden

;
;
;
; script Package.psc
;
;
;

; quest Package::GetOwningQuest(...) native
quest function Package_GetOwningQuest() global
	int session = Costile2.GetSession("Package_GetOwningQuest")
	Package this = Costile2.GetTESForm(session, 0) as Package
	return this.GetOwningQuest()
endFunction

; package Package::GetTemplate(...) native
package function Package_GetTemplate() global
	int session = Costile2.GetSession("Package_GetTemplate")
	Package this = Costile2.GetTESForm(session, 0) as Package
	return this.GetTemplate()
endFunction

