Scriptname Costile2_TreeObject Hidden

;
;
;
; script TreeObject.psc
;
;
;

; sounddescriptor TreeObject::GetHarvestSound(...) native
sounddescriptor function TreeObject_GetHarvestSound() global
	int session = Costile2.GetSession("TreeObject_GetHarvestSound")
	TreeObject this = Costile2.GetTESForm(session, 0) as TreeObject
	return this.GetHarvestSound()
endFunction

; void TreeObject::SetHarvestSound(...) native
function TreeObject_SetHarvestSound() global
	int session = Costile2.GetSession("TreeObject_SetHarvestSound")
	TreeObject this = Costile2.GetTESForm(session, 0) as TreeObject
	sounddescriptor cdcg_akSoundDescriptor = Costile2.GetTESForm(session, 1) as sounddescriptor
	this.SetHarvestSound(cdcg_akSoundDescriptor)
endFunction

; form TreeObject::GetIngredient(...) native
form function TreeObject_GetIngredient() global
	int session = Costile2.GetSession("TreeObject_GetIngredient")
	TreeObject this = Costile2.GetTESForm(session, 0) as TreeObject
	return this.GetIngredient()
endFunction

; void TreeObject::SetIngredient(...) native
function TreeObject_SetIngredient() global
	int session = Costile2.GetSession("TreeObject_SetIngredient")
	TreeObject this = Costile2.GetTESForm(session, 0) as TreeObject
	form cdcg_akIngredient = Costile2.GetTESForm(session, 1) as form
	this.SetIngredient(cdcg_akIngredient)
endFunction

