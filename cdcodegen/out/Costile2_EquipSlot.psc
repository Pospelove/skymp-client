Scriptname Costile2_EquipSlot Hidden

;
;
;
; script EquipSlot.psc
;
;
;

; int EquipSlot::GetNumParents(...) native
int function EquipSlot_GetNumParents() global
	int session = Costile2.GetSession("EquipSlot_GetNumParents")
	EquipSlot this = Costile2.GetTESForm(session, 0) as EquipSlot
	return this.GetNumParents()
endFunction

; equipslot EquipSlot::GetNthParent(...) native
equipslot function EquipSlot_GetNthParent() global
	int session = Costile2.GetSession("EquipSlot_GetNthParent")
	EquipSlot this = Costile2.GetTESForm(session, 0) as EquipSlot
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthParent(cdcg_n)
endFunction

