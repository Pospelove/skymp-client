Scriptname Costile2_Form Hidden

;
;
;
; script Form.psc
;
;
;

; int Form::GetFormID(...) native
int function Form_GetFormID() global
	int session = Costile2.GetSession("Form_GetFormID")
	Form this = Costile2.GetTESForm(session, 0) as Form
	return this.GetFormID()
endFunction

; int Form::GetGoldValue(...) native
int function Form_GetGoldValue() global
	int session = Costile2.GetSession("Form_GetGoldValue")
	Form this = Costile2.GetTESForm(session, 0) as Form
	return this.GetGoldValue()
endFunction

; bool Form::HasKeyword(...) native
bool function Form_HasKeyword() global
	int session = Costile2.GetSession("Form_HasKeyword")
	Form this = Costile2.GetTESForm(session, 0) as Form
	keyword cdcg_akKeyword = Costile2.GetTESForm(session, 1) as keyword
	return this.HasKeyword(cdcg_akKeyword)
endFunction

; bool Form::PlayerKnows(...) native
bool function Form_PlayerKnows() global
	int session = Costile2.GetSession("Form_PlayerKnows")
	Form this = Costile2.GetTESForm(session, 0) as Form
	return this.PlayerKnows()
endFunction

; bool Form::RegisterForAnimationEvent(...) native
bool function Form_RegisterForAnimationEvent() global
	int session = Costile2.GetSession("Form_RegisterForAnimationEvent")
	Form this = Costile2.GetTESForm(session, 0) as Form
	objectreference cdcg_akSender = Costile2.GetTESForm(session, 1) as objectreference
	string cdcg_asEventName = Costile2.GetString(session, 2)
	return this.RegisterForAnimationEvent(cdcg_akSender, cdcg_asEventName)
endFunction

; void Form::RegisterForLOS(...) native
function Form_RegisterForLOS() global
	int session = Costile2.GetSession("Form_RegisterForLOS")
	Form this = Costile2.GetTESForm(session, 0) as Form
	actor cdcg_akViewer = Costile2.GetTESForm(session, 1) as actor
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 2) as objectreference
	this.RegisterForLOS(cdcg_akViewer, cdcg_akTarget)
endFunction

; void Form::RegisterForSingleLOSGain(...) native
function Form_RegisterForSingleLOSGain() global
	int session = Costile2.GetSession("Form_RegisterForSingleLOSGain")
	Form this = Costile2.GetTESForm(session, 0) as Form
	actor cdcg_akViewer = Costile2.GetTESForm(session, 1) as actor
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 2) as objectreference
	this.RegisterForSingleLOSGain(cdcg_akViewer, cdcg_akTarget)
endFunction

; void Form::RegisterForSingleLOSLost(...) native
function Form_RegisterForSingleLOSLost() global
	int session = Costile2.GetSession("Form_RegisterForSingleLOSLost")
	Form this = Costile2.GetTESForm(session, 0) as Form
	actor cdcg_akViewer = Costile2.GetTESForm(session, 1) as actor
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 2) as objectreference
	this.RegisterForSingleLOSLost(cdcg_akViewer, cdcg_akTarget)
endFunction

; void Form::RegisterForSingleUpdate(...) native
function Form_RegisterForSingleUpdate() global
	int session = Costile2.GetSession("Form_RegisterForSingleUpdate")
	Form this = Costile2.GetTESForm(session, 0) as Form
	float cdcg_afInterval = Costile2.GetFloat(session, 1)
	this.RegisterForSingleUpdate(cdcg_afInterval)
endFunction

; void Form::RegisterForSleep(...) native
function Form_RegisterForSleep() global
	int session = Costile2.GetSession("Form_RegisterForSleep")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.RegisterForSleep()
endFunction

; void Form::RegisterForTrackedStatsEvent(...) native
function Form_RegisterForTrackedStatsEvent() global
	int session = Costile2.GetSession("Form_RegisterForTrackedStatsEvent")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.RegisterForTrackedStatsEvent()
endFunction

; void Form::RegisterForUpdate(...) native
function Form_RegisterForUpdate() global
	int session = Costile2.GetSession("Form_RegisterForUpdate")
	Form this = Costile2.GetTESForm(session, 0) as Form
	float cdcg_afInterval = Costile2.GetFloat(session, 1)
	this.RegisterForUpdate(cdcg_afInterval)
endFunction

; void Form::RegisterForUpdateGameTime(...) native
function Form_RegisterForUpdateGameTime() global
	int session = Costile2.GetSession("Form_RegisterForUpdateGameTime")
	Form this = Costile2.GetTESForm(session, 0) as Form
	float cdcg_afInterval = Costile2.GetFloat(session, 1)
	this.RegisterForUpdateGameTime(cdcg_afInterval)
endFunction

; void Form::RegisterForSingleUpdateGameTime(...) native
function Form_RegisterForSingleUpdateGameTime() global
	int session = Costile2.GetSession("Form_RegisterForSingleUpdateGameTime")
	Form this = Costile2.GetTESForm(session, 0) as Form
	float cdcg_afInterval = Costile2.GetFloat(session, 1)
	this.RegisterForSingleUpdateGameTime(cdcg_afInterval)
endFunction

; void Form::StartObjectProfiling(...) native
function Form_StartObjectProfiling() global
	int session = Costile2.GetSession("Form_StartObjectProfiling")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.StartObjectProfiling()
endFunction

; void Form::StopObjectProfiling(...) native
function Form_StopObjectProfiling() global
	int session = Costile2.GetSession("Form_StopObjectProfiling")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.StopObjectProfiling()
endFunction

; void Form::UnregisterForAnimationEvent(...) native
function Form_UnregisterForAnimationEvent() global
	int session = Costile2.GetSession("Form_UnregisterForAnimationEvent")
	Form this = Costile2.GetTESForm(session, 0) as Form
	objectreference cdcg_akSender = Costile2.GetTESForm(session, 1) as objectreference
	string cdcg_asEventName = Costile2.GetString(session, 2)
	this.UnregisterForAnimationEvent(cdcg_akSender, cdcg_asEventName)
endFunction

; void Form::UnregisterForLOS(...) native
function Form_UnregisterForLOS() global
	int session = Costile2.GetSession("Form_UnregisterForLOS")
	Form this = Costile2.GetTESForm(session, 0) as Form
	actor cdcg_akViewer = Costile2.GetTESForm(session, 1) as actor
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 2) as objectreference
	this.UnregisterForLOS(cdcg_akViewer, cdcg_akTarget)
endFunction

; void Form::UnregisterForSleep(...) native
function Form_UnregisterForSleep() global
	int session = Costile2.GetSession("Form_UnregisterForSleep")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.UnregisterForSleep()
endFunction

; void Form::UnregisterForTrackedStatsEvent(...) native
function Form_UnregisterForTrackedStatsEvent() global
	int session = Costile2.GetSession("Form_UnregisterForTrackedStatsEvent")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.UnregisterForTrackedStatsEvent()
endFunction

; void Form::UnregisterForUpdate(...) native
function Form_UnregisterForUpdate() global
	int session = Costile2.GetSession("Form_UnregisterForUpdate")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.UnregisterForUpdate()
endFunction

; void Form::UnregisterForUpdateGameTime(...) native
function Form_UnregisterForUpdateGameTime() global
	int session = Costile2.GetSession("Form_UnregisterForUpdateGameTime")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.UnregisterForUpdateGameTime()
endFunction

; int Form::GetType(...) native
int function Form_GetType() global
	int session = Costile2.GetSession("Form_GetType")
	Form this = Costile2.GetTESForm(session, 0) as Form
	return this.GetType()
endFunction

; string Form::GetName(...) native
string function Form_GetName() global
	int session = Costile2.GetSession("Form_GetName")
	Form this = Costile2.GetTESForm(session, 0) as Form
	return this.GetName()
endFunction

; void Form::SetName(...) native
function Form_SetName() global
	int session = Costile2.GetSession("Form_SetName")
	Form this = Costile2.GetTESForm(session, 0) as Form
	string cdcg_name = Costile2.GetString(session, 1)
	this.SetName(cdcg_name)
endFunction

; float Form::GetWeight(...) native
float function Form_GetWeight() global
	int session = Costile2.GetSession("Form_GetWeight")
	Form this = Costile2.GetTESForm(session, 0) as Form
	return this.GetWeight()
endFunction

; void Form::SetWeight(...) native
function Form_SetWeight() global
	int session = Costile2.GetSession("Form_SetWeight")
	Form this = Costile2.GetTESForm(session, 0) as Form
	float cdcg_weight = Costile2.GetFloat(session, 1)
	this.SetWeight(cdcg_weight)
endFunction

; void Form::SetGoldValue(...) native
function Form_SetGoldValue() global
	int session = Costile2.GetSession("Form_SetGoldValue")
	Form this = Costile2.GetTESForm(session, 0) as Form
	int cdcg_value = Costile2.GetInt(session, 1)
	this.SetGoldValue(cdcg_value)
endFunction

; int Form::GetNumKeywords(...) native
int function Form_GetNumKeywords() global
	int session = Costile2.GetSession("Form_GetNumKeywords")
	Form this = Costile2.GetTESForm(session, 0) as Form
	return this.GetNumKeywords()
endFunction

; keyword Form::GetNthKeyword(...) native
keyword function Form_GetNthKeyword() global
	int session = Costile2.GetSession("Form_GetNthKeyword")
	Form this = Costile2.GetTESForm(session, 0) as Form
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthKeyword(cdcg_index)
endFunction

; bool Form::HasKeywordString(...)
bool function Form_HasKeywordString() global
	int session = Costile2.GetSession("Form_HasKeywordString")
	Form this = Costile2.GetTESForm(session, 0) as Form
	string cdcg_s = Costile2.GetString(session, 1)
	return this.HasKeywordString(cdcg_s)
endFunction

; void Form::SetPlayerKnows(...) native
function Form_SetPlayerKnows() global
	int session = Costile2.GetSession("Form_SetPlayerKnows")
	Form this = Costile2.GetTESForm(session, 0) as Form
	bool cdcg_knows = Costile2.GetBool(session, 1)
	this.SetPlayerKnows(cdcg_knows)
endFunction

; void Form::RegisterForKey(...) native
function Form_RegisterForKey() global
	int session = Costile2.GetSession("Form_RegisterForKey")
	Form this = Costile2.GetTESForm(session, 0) as Form
	int cdcg_keyCode = Costile2.GetInt(session, 1)
	this.RegisterForKey(cdcg_keyCode)
endFunction

; void Form::UnregisterForKey(...) native
function Form_UnregisterForKey() global
	int session = Costile2.GetSession("Form_UnregisterForKey")
	Form this = Costile2.GetTESForm(session, 0) as Form
	int cdcg_keyCode = Costile2.GetInt(session, 1)
	this.UnregisterForKey(cdcg_keyCode)
endFunction

; void Form::UnregisterForAllKeys(...) native
function Form_UnregisterForAllKeys() global
	int session = Costile2.GetSession("Form_UnregisterForAllKeys")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.UnregisterForAllKeys()
endFunction

; void Form::RegisterForControl(...) native
function Form_RegisterForControl() global
	int session = Costile2.GetSession("Form_RegisterForControl")
	Form this = Costile2.GetTESForm(session, 0) as Form
	string cdcg_control = Costile2.GetString(session, 1)
	this.RegisterForControl(cdcg_control)
endFunction

; void Form::UnregisterForControl(...) native
function Form_UnregisterForControl() global
	int session = Costile2.GetSession("Form_UnregisterForControl")
	Form this = Costile2.GetTESForm(session, 0) as Form
	string cdcg_control = Costile2.GetString(session, 1)
	this.UnregisterForControl(cdcg_control)
endFunction

; void Form::UnregisterForAllControls(...) native
function Form_UnregisterForAllControls() global
	int session = Costile2.GetSession("Form_UnregisterForAllControls")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.UnregisterForAllControls()
endFunction

; void Form::RegisterForMenu(...) native
function Form_RegisterForMenu() global
	int session = Costile2.GetSession("Form_RegisterForMenu")
	Form this = Costile2.GetTESForm(session, 0) as Form
	string cdcg_menuName = Costile2.GetString(session, 1)
	this.RegisterForMenu(cdcg_menuName)
endFunction

; void Form::UnregisterForMenu(...) native
function Form_UnregisterForMenu() global
	int session = Costile2.GetSession("Form_UnregisterForMenu")
	Form this = Costile2.GetTESForm(session, 0) as Form
	string cdcg_menuName = Costile2.GetString(session, 1)
	this.UnregisterForMenu(cdcg_menuName)
endFunction

; void Form::UnregisterForAllMenus(...) native
function Form_UnregisterForAllMenus() global
	int session = Costile2.GetSession("Form_UnregisterForAllMenus")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.UnregisterForAllMenus()
endFunction

; void Form::RegisterForModEvent(...) native
function Form_RegisterForModEvent() global
	int session = Costile2.GetSession("Form_RegisterForModEvent")
	Form this = Costile2.GetTESForm(session, 0) as Form
	string cdcg_eventName = Costile2.GetString(session, 1)
	string cdcg_callbackName = Costile2.GetString(session, 2)
	this.RegisterForModEvent(cdcg_eventName, cdcg_callbackName)
endFunction

; void Form::UnregisterForModEvent(...) native
function Form_UnregisterForModEvent() global
	int session = Costile2.GetSession("Form_UnregisterForModEvent")
	Form this = Costile2.GetTESForm(session, 0) as Form
	string cdcg_eventName = Costile2.GetString(session, 1)
	this.UnregisterForModEvent(cdcg_eventName)
endFunction

; void Form::UnregisterForAllModEvents(...) native
function Form_UnregisterForAllModEvents() global
	int session = Costile2.GetSession("Form_UnregisterForAllModEvents")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.UnregisterForAllModEvents()
endFunction

; void Form::SendModEvent(...) native
function Form_SendModEvent() global
	int session = Costile2.GetSession("Form_SendModEvent")
	Form this = Costile2.GetTESForm(session, 0) as Form
	string cdcg_eventName = Costile2.GetString(session, 1)
	string cdcg_strArg = Costile2.GetString(session, 2)
	float cdcg_numArg = Costile2.GetFloat(session, 3)
	this.SendModEvent(cdcg_eventName, cdcg_strArg, cdcg_numArg)
endFunction

; void Form::RegisterForCameraState(...) native
function Form_RegisterForCameraState() global
	int session = Costile2.GetSession("Form_RegisterForCameraState")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.RegisterForCameraState()
endFunction

; void Form::UnregisterForCameraState(...) native
function Form_UnregisterForCameraState() global
	int session = Costile2.GetSession("Form_UnregisterForCameraState")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.UnregisterForCameraState()
endFunction

; void Form::RegisterForCrosshairRef(...) native
function Form_RegisterForCrosshairRef() global
	int session = Costile2.GetSession("Form_RegisterForCrosshairRef")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.RegisterForCrosshairRef()
endFunction

; void Form::UnregisterForCrosshairRef(...) native
function Form_UnregisterForCrosshairRef() global
	int session = Costile2.GetSession("Form_UnregisterForCrosshairRef")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.UnregisterForCrosshairRef()
endFunction

; void Form::RegisterForActorAction(...) native
function Form_RegisterForActorAction() global
	int session = Costile2.GetSession("Form_RegisterForActorAction")
	Form this = Costile2.GetTESForm(session, 0) as Form
	int cdcg_actionType = Costile2.GetInt(session, 1)
	this.RegisterForActorAction(cdcg_actionType)
endFunction

; void Form::UnregisterForActorAction(...) native
function Form_UnregisterForActorAction() global
	int session = Costile2.GetSession("Form_UnregisterForActorAction")
	Form this = Costile2.GetTESForm(session, 0) as Form
	int cdcg_actionType = Costile2.GetInt(session, 1)
	this.UnregisterForActorAction(cdcg_actionType)
endFunction

; void Form::RegisterForNiNodeUpdate(...) native
function Form_RegisterForNiNodeUpdate() global
	int session = Costile2.GetSession("Form_RegisterForNiNodeUpdate")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.RegisterForNiNodeUpdate()
endFunction

; void Form::UnregisterForNiNodeUpdate(...) native
function Form_UnregisterForNiNodeUpdate() global
	int session = Costile2.GetSession("Form_UnregisterForNiNodeUpdate")
	Form this = Costile2.GetTESForm(session, 0) as Form
	this.UnregisterForNiNodeUpdate()
endFunction

; form Form::TempClone(...) native
form function Form_TempClone() global
	int session = Costile2.GetSession("Form_TempClone")
	Form this = Costile2.GetTESForm(session, 0) as Form
	return this.TempClone()
endFunction

; bool Form::HasWorldModel(...) native
bool function Form_HasWorldModel() global
	int session = Costile2.GetSession("Form_HasWorldModel")
	Form this = Costile2.GetTESForm(session, 0) as Form
	return this.HasWorldModel()
endFunction

; string Form::GetWorldModelPath(...) native
string function Form_GetWorldModelPath() global
	int session = Costile2.GetSession("Form_GetWorldModelPath")
	Form this = Costile2.GetTESForm(session, 0) as Form
	return this.GetWorldModelPath()
endFunction

; void Form::SetWorldModelPath(...) native
function Form_SetWorldModelPath() global
	int session = Costile2.GetSession("Form_SetWorldModelPath")
	Form this = Costile2.GetTESForm(session, 0) as Form
	string cdcg_path = Costile2.GetString(session, 1)
	this.SetWorldModelPath(cdcg_path)
endFunction

; int Form::GetWorldModelNumTextureSets(...) native
int function Form_GetWorldModelNumTextureSets() global
	int session = Costile2.GetSession("Form_GetWorldModelNumTextureSets")
	Form this = Costile2.GetTESForm(session, 0) as Form
	return this.GetWorldModelNumTextureSets()
endFunction

; textureset Form::GetWorldModelNthTextureSet(...) native
textureset function Form_GetWorldModelNthTextureSet() global
	int session = Costile2.GetSession("Form_GetWorldModelNthTextureSet")
	Form this = Costile2.GetTESForm(session, 0) as Form
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetWorldModelNthTextureSet(cdcg_n)
endFunction

; void Form::SetWorldModelNthTextureSet(...) native
function Form_SetWorldModelNthTextureSet() global
	int session = Costile2.GetSession("Form_SetWorldModelNthTextureSet")
	Form this = Costile2.GetTESForm(session, 0) as Form
	textureset cdcg_nSet = Costile2.GetTESForm(session, 1) as textureset
	int cdcg_n = Costile2.GetInt(session, 2)
	this.SetWorldModelNthTextureSet(cdcg_nSet, cdcg_n)
endFunction

; bool Form::IsPlayable(...) native
bool function Form_IsPlayable() global
	int session = Costile2.GetSession("Form_IsPlayable")
	Form this = Costile2.GetTESForm(session, 0) as Form
	return this.IsPlayable()
endFunction

