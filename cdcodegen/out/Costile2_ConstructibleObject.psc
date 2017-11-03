Scriptname Costile2_ConstructibleObject Hidden

;
;
;
; script ConstructibleObject.psc
;
;
;

; form ConstructibleObject::GetResult(...) native
form function ConstructibleObject_GetResult() global
	int session = Costile2.GetSession("ConstructibleObject_GetResult")
	ConstructibleObject this = Costile2.GetTESForm(session, 0) as ConstructibleObject
	return this.GetResult()
endFunction

; void ConstructibleObject::SetResult(...) native
function ConstructibleObject_SetResult() global
	int session = Costile2.GetSession("ConstructibleObject_SetResult")
	ConstructibleObject this = Costile2.GetTESForm(session, 0) as ConstructibleObject
	form cdcg_result = Costile2.GetTESForm(session, 1) as form
	this.SetResult(cdcg_result)
endFunction

; int ConstructibleObject::GetResultQuantity(...) native
int function ConstructibleObject_GetResultQuantity() global
	int session = Costile2.GetSession("ConstructibleObject_GetResultQuantity")
	ConstructibleObject this = Costile2.GetTESForm(session, 0) as ConstructibleObject
	return this.GetResultQuantity()
endFunction

; void ConstructibleObject::SetResultQuantity(...) native
function ConstructibleObject_SetResultQuantity() global
	int session = Costile2.GetSession("ConstructibleObject_SetResultQuantity")
	ConstructibleObject this = Costile2.GetTESForm(session, 0) as ConstructibleObject
	int cdcg_quantity = Costile2.GetInt(session, 1)
	this.SetResultQuantity(cdcg_quantity)
endFunction

; int ConstructibleObject::GetNumIngredients(...) native
int function ConstructibleObject_GetNumIngredients() global
	int session = Costile2.GetSession("ConstructibleObject_GetNumIngredients")
	ConstructibleObject this = Costile2.GetTESForm(session, 0) as ConstructibleObject
	return this.GetNumIngredients()
endFunction

; form ConstructibleObject::GetNthIngredient(...) native
form function ConstructibleObject_GetNthIngredient() global
	int session = Costile2.GetSession("ConstructibleObject_GetNthIngredient")
	ConstructibleObject this = Costile2.GetTESForm(session, 0) as ConstructibleObject
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthIngredient(cdcg_n)
endFunction

; void ConstructibleObject::SetNthIngredient(...) native
function ConstructibleObject_SetNthIngredient() global
	int session = Costile2.GetSession("ConstructibleObject_SetNthIngredient")
	ConstructibleObject this = Costile2.GetTESForm(session, 0) as ConstructibleObject
	form cdcg_required = Costile2.GetTESForm(session, 1) as form
	int cdcg_n = Costile2.GetInt(session, 2)
	this.SetNthIngredient(cdcg_required, cdcg_n)
endFunction

; int ConstructibleObject::GetNthIngredientQuantity(...) native
int function ConstructibleObject_GetNthIngredientQuantity() global
	int session = Costile2.GetSession("ConstructibleObject_GetNthIngredientQuantity")
	ConstructibleObject this = Costile2.GetTESForm(session, 0) as ConstructibleObject
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthIngredientQuantity(cdcg_n)
endFunction

; void ConstructibleObject::SetNthIngredientQuantity(...) native
function ConstructibleObject_SetNthIngredientQuantity() global
	int session = Costile2.GetSession("ConstructibleObject_SetNthIngredientQuantity")
	ConstructibleObject this = Costile2.GetTESForm(session, 0) as ConstructibleObject
	int cdcg_value = Costile2.GetInt(session, 1)
	int cdcg_n = Costile2.GetInt(session, 2)
	this.SetNthIngredientQuantity(cdcg_value, cdcg_n)
endFunction

; keyword ConstructibleObject::GetWorkbenchKeyword(...) native
keyword function ConstructibleObject_GetWorkbenchKeyword() global
	int session = Costile2.GetSession("ConstructibleObject_GetWorkbenchKeyword")
	ConstructibleObject this = Costile2.GetTESForm(session, 0) as ConstructibleObject
	return this.GetWorkbenchKeyword()
endFunction

; void ConstructibleObject::SetWorkbenchKeyword(...) native
function ConstructibleObject_SetWorkbenchKeyword() global
	int session = Costile2.GetSession("ConstructibleObject_SetWorkbenchKeyword")
	ConstructibleObject this = Costile2.GetTESForm(session, 0) as ConstructibleObject
	keyword cdcg_aKeyword = Costile2.GetTESForm(session, 1) as keyword
	this.SetWorkbenchKeyword(cdcg_aKeyword)
endFunction

