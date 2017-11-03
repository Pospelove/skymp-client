Scriptname Costile2_Cell Hidden

;
;
;
; script Cell.psc
;
;
;

; actorbase Cell::GetActorOwner(...) native
actorbase function Cell_GetActorOwner() global
	int session = Costile2.GetSession("Cell_GetActorOwner")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	return this.GetActorOwner()
endFunction

; faction Cell::GetFactionOwner(...) native
faction function Cell_GetFactionOwner() global
	int session = Costile2.GetSession("Cell_GetFactionOwner")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	return this.GetFactionOwner()
endFunction

; bool Cell::IsAttached(...) native
bool function Cell_IsAttached() global
	int session = Costile2.GetSession("Cell_IsAttached")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	return this.IsAttached()
endFunction

; bool Cell::IsInterior(...) native
bool function Cell_IsInterior() global
	int session = Costile2.GetSession("Cell_IsInterior")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	return this.IsInterior()
endFunction

; void Cell::Reset(...) native
function Cell_Reset() global
	int session = Costile2.GetSession("Cell_Reset")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	this.Reset()
endFunction

; void Cell::SetActorOwner(...) native
function Cell_SetActorOwner() global
	int session = Costile2.GetSession("Cell_SetActorOwner")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	actorbase cdcg_akActor = Costile2.GetTESForm(session, 1) as actorbase
	this.SetActorOwner(cdcg_akActor)
endFunction

; void Cell::SetFactionOwner(...) native
function Cell_SetFactionOwner() global
	int session = Costile2.GetSession("Cell_SetFactionOwner")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	faction cdcg_akFaction = Costile2.GetTESForm(session, 1) as faction
	this.SetFactionOwner(cdcg_akFaction)
endFunction

; void Cell::SetFogColor(...) native
function Cell_SetFogColor() global
	int session = Costile2.GetSession("Cell_SetFogColor")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	int cdcg_aiNearRed = Costile2.GetInt(session, 1)
	int cdcg_aiNearGreen = Costile2.GetInt(session, 2)
	int cdcg_aiNearBlue = Costile2.GetInt(session, 3)
	int cdcg_aiFarRed = Costile2.GetInt(session, 4)
	int cdcg_aiFarGreen = Costile2.GetInt(session, 5)
	int cdcg_aiFarBlue = Costile2.GetInt(session, 6)
	this.SetFogColor(cdcg_aiNearRed, cdcg_aiNearGreen, cdcg_aiNearBlue, cdcg_aiFarRed, cdcg_aiFarGreen, cdcg_aiFarBlue)
endFunction

; void Cell::SetFogPlanes(...) native
function Cell_SetFogPlanes() global
	int session = Costile2.GetSession("Cell_SetFogPlanes")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	float cdcg_afNear = Costile2.GetFloat(session, 1)
	float cdcg_afFar = Costile2.GetFloat(session, 2)
	this.SetFogPlanes(cdcg_afNear, cdcg_afFar)
endFunction

; void Cell::SetFogPower(...) native
function Cell_SetFogPower() global
	int session = Costile2.GetSession("Cell_SetFogPower")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	float cdcg_afPower = Costile2.GetFloat(session, 1)
	this.SetFogPower(cdcg_afPower)
endFunction

; void Cell::SetPublic(...) native
function Cell_SetPublic() global
	int session = Costile2.GetSession("Cell_SetPublic")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	bool cdcg_abPublic = Costile2.GetBool(session, 1)
	this.SetPublic(cdcg_abPublic)
endFunction

; int Cell::GetNumRefs(...) native
int function Cell_GetNumRefs() global
	int session = Costile2.GetSession("Cell_GetNumRefs")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	int cdcg_formTypeFilter = Costile2.GetInt(session, 1)
	return this.GetNumRefs(cdcg_formTypeFilter)
endFunction

; objectreference Cell::GetNthRef(...) native
objectreference function Cell_GetNthRef() global
	int session = Costile2.GetSession("Cell_GetNthRef")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	int cdcg_n = Costile2.GetInt(session, 1)
	int cdcg_formTypeFilter = Costile2.GetInt(session, 2)
	return this.GetNthRef(cdcg_n, cdcg_formTypeFilter)
endFunction

; float Cell::GetWaterLevel(...) native
float function Cell_GetWaterLevel() global
	int session = Costile2.GetSession("Cell_GetWaterLevel")
	Cell this = Costile2.GetTESForm(session, 0) as Cell
	return this.GetWaterLevel()
endFunction

