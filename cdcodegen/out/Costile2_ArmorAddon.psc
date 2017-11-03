Scriptname Costile2_ArmorAddon Hidden

;
;
;
; script ArmorAddon.psc
;
;
;

; string ArmorAddon::GetModelPath(...) native
string function ArmorAddon_GetModelPath() global
	int session = Costile2.GetSession("ArmorAddon_GetModelPath")
	ArmorAddon this = Costile2.GetTESForm(session, 0) as ArmorAddon
	bool cdcg_firstPerson = Costile2.GetBool(session, 1)
	bool cdcg_female = Costile2.GetBool(session, 2)
	return this.GetModelPath(cdcg_firstPerson, cdcg_female)
endFunction

; void ArmorAddon::SetModelPath(...) native
function ArmorAddon_SetModelPath() global
	int session = Costile2.GetSession("ArmorAddon_SetModelPath")
	ArmorAddon this = Costile2.GetTESForm(session, 0) as ArmorAddon
	string cdcg_path = Costile2.GetString(session, 1)
	bool cdcg_firstPerson = Costile2.GetBool(session, 2)
	bool cdcg_female = Costile2.GetBool(session, 3)
	this.SetModelPath(cdcg_path, cdcg_firstPerson, cdcg_female)
endFunction

; int ArmorAddon::GetModelNumTextureSets(...) native
int function ArmorAddon_GetModelNumTextureSets() global
	int session = Costile2.GetSession("ArmorAddon_GetModelNumTextureSets")
	ArmorAddon this = Costile2.GetTESForm(session, 0) as ArmorAddon
	bool cdcg_first = Costile2.GetBool(session, 1)
	bool cdcg_female = Costile2.GetBool(session, 2)
	return this.GetModelNumTextureSets(cdcg_first, cdcg_female)
endFunction

; textureset ArmorAddon::GetModelNthTextureSet(...) native
textureset function ArmorAddon_GetModelNthTextureSet() global
	int session = Costile2.GetSession("ArmorAddon_GetModelNthTextureSet")
	ArmorAddon this = Costile2.GetTESForm(session, 0) as ArmorAddon
	int cdcg_n = Costile2.GetInt(session, 1)
	bool cdcg_first = Costile2.GetBool(session, 2)
	bool cdcg_female = Costile2.GetBool(session, 3)
	return this.GetModelNthTextureSet(cdcg_n, cdcg_first, cdcg_female)
endFunction

; void ArmorAddon::SetModelNthTextureSet(...) native
function ArmorAddon_SetModelNthTextureSet() global
	int session = Costile2.GetSession("ArmorAddon_SetModelNthTextureSet")
	ArmorAddon this = Costile2.GetTESForm(session, 0) as ArmorAddon
	textureset cdcg_texture = Costile2.GetTESForm(session, 1) as textureset
	int cdcg_n = Costile2.GetInt(session, 2)
	bool cdcg_first = Costile2.GetBool(session, 3)
	bool cdcg_female = Costile2.GetBool(session, 4)
	this.SetModelNthTextureSet(cdcg_texture, cdcg_n, cdcg_first, cdcg_female)
endFunction

; int ArmorAddon::GetNumAdditionalRaces(...) native
int function ArmorAddon_GetNumAdditionalRaces() global
	int session = Costile2.GetSession("ArmorAddon_GetNumAdditionalRaces")
	ArmorAddon this = Costile2.GetTESForm(session, 0) as ArmorAddon
	return this.GetNumAdditionalRaces()
endFunction

; race ArmorAddon::GetNthAdditionalRace(...) native
race function ArmorAddon_GetNthAdditionalRace() global
	int session = Costile2.GetSession("ArmorAddon_GetNthAdditionalRace")
	ArmorAddon this = Costile2.GetTESForm(session, 0) as ArmorAddon
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthAdditionalRace(cdcg_n)
endFunction

; int ArmorAddon::GetSlotMask(...) native
int function ArmorAddon_GetSlotMask() global
	int session = Costile2.GetSession("ArmorAddon_GetSlotMask")
	ArmorAddon this = Costile2.GetTESForm(session, 0) as ArmorAddon
	return this.GetSlotMask()
endFunction

; void ArmorAddon::SetSlotMask(...) native
function ArmorAddon_SetSlotMask() global
	int session = Costile2.GetSession("ArmorAddon_SetSlotMask")
	ArmorAddon this = Costile2.GetTESForm(session, 0) as ArmorAddon
	int cdcg_slotMask = Costile2.GetInt(session, 1)
	this.SetSlotMask(cdcg_slotMask)
endFunction

; int ArmorAddon::AddSlotToMask(...) native
int function ArmorAddon_AddSlotToMask() global
	int session = Costile2.GetSession("ArmorAddon_AddSlotToMask")
	ArmorAddon this = Costile2.GetTESForm(session, 0) as ArmorAddon
	int cdcg_slotMask = Costile2.GetInt(session, 1)
	return this.AddSlotToMask(cdcg_slotMask)
endFunction

; int ArmorAddon::RemoveSlotFromMask(...) native
int function ArmorAddon_RemoveSlotFromMask() global
	int session = Costile2.GetSession("ArmorAddon_RemoveSlotFromMask")
	ArmorAddon this = Costile2.GetTESForm(session, 0) as ArmorAddon
	int cdcg_slotMask = Costile2.GetInt(session, 1)
	return this.RemoveSlotFromMask(cdcg_slotMask)
endFunction

; int ArmorAddon::GetMaskForSlot(...) global
int function ArmorAddon_GetMaskForSlot() global
	int session = Costile2.GetSession("ArmorAddon_GetMaskForSlot")
	int cdcg_slot = Costile2.GetInt(session, 0)
	return ArmorAddon.GetMaskForSlot(cdcg_slot)
endFunction

