Scriptname Costile2_Flora Hidden

;
;
;
; script Flora.psc
;
;
;

; sounddescriptor Flora::GetHarvestSound(...) native
sounddescriptor function Flora_GetHarvestSound() global
	int session = Costile2.GetSession("Flora_GetHarvestSound")
	Flora this = Costile2.GetTESForm(session, 0) as Flora
	return this.GetHarvestSound()
endFunction

; void Flora::SetHarvestSound(...) native
function Flora_SetHarvestSound() global
	int session = Costile2.GetSession("Flora_SetHarvestSound")
	Flora this = Costile2.GetTESForm(session, 0) as Flora
	sounddescriptor cdcg_akSoundDescriptor = Costile2.GetTESForm(session, 1) as sounddescriptor
	this.SetHarvestSound(cdcg_akSoundDescriptor)
endFunction

; form Flora::GetIngredient(...) native
form function Flora_GetIngredient() global
	int session = Costile2.GetSession("Flora_GetIngredient")
	Flora this = Costile2.GetTESForm(session, 0) as Flora
	return this.GetIngredient()
endFunction

; void Flora::SetIngredient(...) native
function Flora_SetIngredient() global
	int session = Costile2.GetSession("Flora_SetIngredient")
	Flora this = Costile2.GetTESForm(session, 0) as Flora
	form cdcg_akIngredient = Costile2.GetTESForm(session, 1) as form
	this.SetIngredient(cdcg_akIngredient)
endFunction

