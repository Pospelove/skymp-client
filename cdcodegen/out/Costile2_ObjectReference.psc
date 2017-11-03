Scriptname Costile2_ObjectReference Hidden

;
;
;
; script ObjectReference.psc
;
;
;

; bool ObjectReference::rampRumble(...)
bool function ObjectReference_rampRumble() global
	int session = Costile2.GetSession("ObjectReference_rampRumble")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	float cdcg_power = Costile2.GetFloat(session, 1)
	float cdcg_duration = Costile2.GetFloat(session, 2)
	float cdcg_falloff = Costile2.GetFloat(session, 3)
	return this.rampRumble(cdcg_power, cdcg_duration, cdcg_falloff)
endFunction

; bool ObjectReference::IsNearPlayer(...)
bool function ObjectReference_IsNearPlayer() global
	int session = Costile2.GetSession("ObjectReference_IsNearPlayer")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsNearPlayer()
endFunction

; bool ObjectReference::IsInInterior(...)
bool function ObjectReference_IsInInterior() global
	int session = Costile2.GetSession("ObjectReference_IsInInterior")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsInInterior()
endFunction

; bool ObjectReference::MoveToIfUnloaded(...)
bool function ObjectReference_MoveToIfUnloaded() global
	int session = Costile2.GetSession("ObjectReference_MoveToIfUnloaded")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 1) as objectreference
	float cdcg_afXOffset = Costile2.GetFloat(session, 2)
	float cdcg_afYOffset = Costile2.GetFloat(session, 3)
	float cdcg_afZOffset = Costile2.GetFloat(session, 4)
	return this.MoveToIfUnloaded(cdcg_akTarget, cdcg_afXOffset, cdcg_afYOffset, cdcg_afZOffset)
endFunction

; void ObjectReference::MoveToWhenUnloaded(...)
function ObjectReference_MoveToWhenUnloaded() global
	int session = Costile2.GetSession("ObjectReference_MoveToWhenUnloaded")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 1) as objectreference
	float cdcg_afXOffset = Costile2.GetFloat(session, 2)
	float cdcg_afYOffset = Costile2.GetFloat(session, 3)
	float cdcg_afZOffset = Costile2.GetFloat(session, 4)
	this.MoveToWhenUnloaded(cdcg_akTarget, cdcg_afXOffset, cdcg_afYOffset, cdcg_afZOffset)
endFunction

; void ObjectReference::DeleteWhenAble(...)
function ObjectReference_DeleteWhenAble() global
	int session = Costile2.GetSession("ObjectReference_DeleteWhenAble")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	this.DeleteWhenAble()
endFunction

; void ObjectReference::AddKeyIfNeeded(...)
function ObjectReference_AddKeyIfNeeded() global
	int session = Costile2.GetSession("ObjectReference_AddKeyIfNeeded")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_ObjectWithNeededKey = Costile2.GetTESForm(session, 1) as objectreference
	this.AddKeyIfNeeded(cdcg_ObjectWithNeededKey)
endFunction

; bool ObjectReference::Activate(...) native
bool function ObjectReference_Activate() global
	int session = Costile2.GetSession("ObjectReference_Activate")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akActivator = Costile2.GetTESForm(session, 1) as objectreference
	bool cdcg_abDefaultProcessingOnly = Costile2.GetBool(session, 2)
	return this.Activate(cdcg_akActivator, cdcg_abDefaultProcessingOnly)
endFunction

; bool ObjectReference::AddDependentAnimatedObjectReference(...) native
bool function ObjectReference_AddDependentAnimatedObjectReference() global
	int session = Costile2.GetSession("ObjectReference_AddDependentAnimatedObjectReference")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akDependent = Costile2.GetTESForm(session, 1) as objectreference
	return this.AddDependentAnimatedObjectReference(cdcg_akDependent)
endFunction

; void ObjectReference::AddInventoryEventFilter(...) native
function ObjectReference_AddInventoryEventFilter() global
	int session = Costile2.GetSession("ObjectReference_AddInventoryEventFilter")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	form cdcg_akFilter = Costile2.GetTESForm(session, 1) as form
	this.AddInventoryEventFilter(cdcg_akFilter)
endFunction

; void ObjectReference::AddItem(...) native
function ObjectReference_AddItem() global
	int session = Costile2.GetSession("ObjectReference_AddItem")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	form cdcg_akItemToAdd = Costile2.GetTESForm(session, 1) as form
	int cdcg_aiCount = Costile2.GetInt(session, 2)
	bool cdcg_abSilent = Costile2.GetBool(session, 3)
	this.AddItem(cdcg_akItemToAdd, cdcg_aiCount, cdcg_abSilent)
endFunction

; void ObjectReference::AddToMap(...) native
function ObjectReference_AddToMap() global
	int session = Costile2.GetSession("ObjectReference_AddToMap")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abAllowFastTravel = Costile2.GetBool(session, 1)
	this.AddToMap(cdcg_abAllowFastTravel)
endFunction

; void ObjectReference::ApplyHavokImpulse(...) native
function ObjectReference_ApplyHavokImpulse() global
	int session = Costile2.GetSession("ObjectReference_ApplyHavokImpulse")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	float cdcg_afX = Costile2.GetFloat(session, 1)
	float cdcg_afY = Costile2.GetFloat(session, 2)
	float cdcg_afZ = Costile2.GetFloat(session, 3)
	float cdcg_afMagnitude = Costile2.GetFloat(session, 4)
	this.ApplyHavokImpulse(cdcg_afX, cdcg_afY, cdcg_afZ, cdcg_afMagnitude)
endFunction

; void ObjectReference::BlockActivation(...) native
function ObjectReference_BlockActivation() global
	int session = Costile2.GetSession("ObjectReference_BlockActivation")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abBlocked = Costile2.GetBool(session, 1)
	this.BlockActivation(cdcg_abBlocked)
endFunction

; int ObjectReference::CalculateEncounterLevel(...) native
int function ObjectReference_CalculateEncounterLevel() global
	int session = Costile2.GetSession("ObjectReference_CalculateEncounterLevel")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	int cdcg_aiDifficulty = Costile2.GetInt(session, 1)
	return this.CalculateEncounterLevel(cdcg_aiDifficulty)
endFunction

; bool ObjectReference::CanFastTravelToMarker(...) native
bool function ObjectReference_CanFastTravelToMarker() global
	int session = Costile2.GetSession("ObjectReference_CanFastTravelToMarker")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.CanFastTravelToMarker()
endFunction

; void ObjectReference::ClearDestruction(...) native
function ObjectReference_ClearDestruction() global
	int session = Costile2.GetSession("ObjectReference_ClearDestruction")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	this.ClearDestruction()
endFunction

; void ObjectReference::CreateDetectionEvent(...) native
function ObjectReference_CreateDetectionEvent() global
	int session = Costile2.GetSession("ObjectReference_CreateDetectionEvent")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	actor cdcg_akOwner = Costile2.GetTESForm(session, 1) as actor
	int cdcg_aiSoundLevel = Costile2.GetInt(session, 2)
	this.CreateDetectionEvent(cdcg_akOwner, cdcg_aiSoundLevel)
endFunction

; void ObjectReference::DamageObject(...) native
function ObjectReference_DamageObject() global
	int session = Costile2.GetSession("ObjectReference_DamageObject")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	float cdcg_afDamage = Costile2.GetFloat(session, 1)
	this.DamageObject(cdcg_afDamage)
endFunction

; void ObjectReference::Delete(...) native
function ObjectReference_Delete() global
	int session = Costile2.GetSession("ObjectReference_Delete")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	this.Delete()
endFunction

; void ObjectReference::Disable(...) native
function ObjectReference_Disable() global
	int session = Costile2.GetSession("ObjectReference_Disable")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abFadeOut = Costile2.GetBool(session, 1)
	this.Disable(cdcg_abFadeOut)
endFunction

; void ObjectReference::DisableNoWait(...) native
function ObjectReference_DisableNoWait() global
	int session = Costile2.GetSession("ObjectReference_DisableNoWait")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abFadeOut = Costile2.GetBool(session, 1)
	this.DisableNoWait(cdcg_abFadeOut)
endFunction

; objectreference ObjectReference::DropObject(...) native
objectreference function ObjectReference_DropObject() global
	int session = Costile2.GetSession("ObjectReference_DropObject")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	form cdcg_akObject = Costile2.GetTESForm(session, 1) as form
	int cdcg_aiCount = Costile2.GetInt(session, 2)
	return this.DropObject(cdcg_akObject, cdcg_aiCount)
endFunction

; void ObjectReference::Enable(...) native
function ObjectReference_Enable() global
	int session = Costile2.GetSession("ObjectReference_Enable")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abFadeIn = Costile2.GetBool(session, 1)
	this.Enable(cdcg_abFadeIn)
endFunction

; void ObjectReference::EnableFastTravel(...) native
function ObjectReference_EnableFastTravel() global
	int session = Costile2.GetSession("ObjectReference_EnableFastTravel")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abEnable = Costile2.GetBool(session, 1)
	this.EnableFastTravel(cdcg_abEnable)
endFunction

; void ObjectReference::EnableNoWait(...) native
function ObjectReference_EnableNoWait() global
	int session = Costile2.GetSession("ObjectReference_EnableNoWait")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abFadeIn = Costile2.GetBool(session, 1)
	this.EnableNoWait(cdcg_abFadeIn)
endFunction

; void ObjectReference::ForceAddRagdollToWorld(...) native
function ObjectReference_ForceAddRagdollToWorld() global
	int session = Costile2.GetSession("ObjectReference_ForceAddRagdollToWorld")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	this.ForceAddRagdollToWorld()
endFunction

; void ObjectReference::ForceRemoveRagdollFromWorld(...) native
function ObjectReference_ForceRemoveRagdollFromWorld() global
	int session = Costile2.GetSession("ObjectReference_ForceRemoveRagdollFromWorld")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	this.ForceRemoveRagdollFromWorld()
endFunction

; actorbase ObjectReference::GetActorOwner(...) native
actorbase function ObjectReference_GetActorOwner() global
	int session = Costile2.GetSession("ObjectReference_GetActorOwner")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetActorOwner()
endFunction

; float ObjectReference::GetAngleX(...) native
float function ObjectReference_GetAngleX() global
	int session = Costile2.GetSession("ObjectReference_GetAngleX")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetAngleX()
endFunction

; float ObjectReference::GetAngleY(...) native
float function ObjectReference_GetAngleY() global
	int session = Costile2.GetSession("ObjectReference_GetAngleY")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetAngleY()
endFunction

; float ObjectReference::GetAngleZ(...) native
float function ObjectReference_GetAngleZ() global
	int session = Costile2.GetSession("ObjectReference_GetAngleZ")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetAngleZ()
endFunction

; bool ObjectReference::GetAnimationVariableBool(...) native
bool function ObjectReference_GetAnimationVariableBool() global
	int session = Costile2.GetSession("ObjectReference_GetAnimationVariableBool")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_arVariableName = Costile2.GetString(session, 1)
	return this.GetAnimationVariableBool(cdcg_arVariableName)
endFunction

; int ObjectReference::GetAnimationVariableInt(...) native
int function ObjectReference_GetAnimationVariableInt() global
	int session = Costile2.GetSession("ObjectReference_GetAnimationVariableInt")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_arVariableName = Costile2.GetString(session, 1)
	return this.GetAnimationVariableInt(cdcg_arVariableName)
endFunction

; float ObjectReference::GetAnimationVariableFloat(...) native
float function ObjectReference_GetAnimationVariableFloat() global
	int session = Costile2.GetSession("ObjectReference_GetAnimationVariableFloat")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_arVariableName = Costile2.GetString(session, 1)
	return this.GetAnimationVariableFloat(cdcg_arVariableName)
endFunction

; form ObjectReference::GetBaseObject(...) native
form function ObjectReference_GetBaseObject() global
	int session = Costile2.GetSession("ObjectReference_GetBaseObject")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetBaseObject()
endFunction

; int ObjectReference::GetCurrentDestructionStage(...) native
int function ObjectReference_GetCurrentDestructionStage() global
	int session = Costile2.GetSession("ObjectReference_GetCurrentDestructionStage")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetCurrentDestructionStage()
endFunction

; location ObjectReference::GetCurrentLocation(...) native
location function ObjectReference_GetCurrentLocation() global
	int session = Costile2.GetSession("ObjectReference_GetCurrentLocation")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetCurrentLocation()
endFunction

; scene ObjectReference::GetCurrentScene(...) native
scene function ObjectReference_GetCurrentScene() global
	int session = Costile2.GetSession("ObjectReference_GetCurrentScene")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetCurrentScene()
endFunction

; float ObjectReference::GetDistance(...) native
float function ObjectReference_GetDistance() global
	int session = Costile2.GetSession("ObjectReference_GetDistance")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akOther = Costile2.GetTESForm(session, 1) as objectreference
	return this.GetDistance(cdcg_akOther)
endFunction

; location ObjectReference::GetEditorLocation(...) native
location function ObjectReference_GetEditorLocation() global
	int session = Costile2.GetSession("ObjectReference_GetEditorLocation")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetEditorLocation()
endFunction

; faction ObjectReference::GetFactionOwner(...) native
faction function ObjectReference_GetFactionOwner() global
	int session = Costile2.GetSession("ObjectReference_GetFactionOwner")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetFactionOwner()
endFunction

; float ObjectReference::GetHeadingAngle(...) native
float function ObjectReference_GetHeadingAngle() global
	int session = Costile2.GetSession("ObjectReference_GetHeadingAngle")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akOther = Costile2.GetTESForm(session, 1) as objectreference
	return this.GetHeadingAngle(cdcg_akOther)
endFunction

; float ObjectReference::GetHeight(...) native
float function ObjectReference_GetHeight() global
	int session = Costile2.GetSession("ObjectReference_GetHeight")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetHeight()
endFunction

; int ObjectReference::GetItemCount(...) native
int function ObjectReference_GetItemCount() global
	int session = Costile2.GetSession("ObjectReference_GetItemCount")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	form cdcg_akItem = Costile2.GetTESForm(session, 1) as form
	return this.GetItemCount(cdcg_akItem)
endFunction

; float ObjectReference::GetItemHealthPercent(...) native
float function ObjectReference_GetItemHealthPercent() global
	int session = Costile2.GetSession("ObjectReference_GetItemHealthPercent")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetItemHealthPercent()
endFunction

; key ObjectReference::GetKey(...) native
key function ObjectReference_GetKey() global
	int session = Costile2.GetSession("ObjectReference_GetKey")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetKey()
endFunction

; float ObjectReference::GetLength(...) native
float function ObjectReference_GetLength() global
	int session = Costile2.GetSession("ObjectReference_GetLength")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetLength()
endFunction

; objectreference ObjectReference::GetLinkedRef(...) native
objectreference function ObjectReference_GetLinkedRef() global
	int session = Costile2.GetSession("ObjectReference_GetLinkedRef")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	keyword cdcg_apKeyword = Costile2.GetTESForm(session, 1) as keyword
	return this.GetLinkedRef(cdcg_apKeyword)
endFunction

; int ObjectReference::GetLockLevel(...) native
int function ObjectReference_GetLockLevel() global
	int session = Costile2.GetSession("ObjectReference_GetLockLevel")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetLockLevel()
endFunction

; int ObjectReference::countLinkedRefChain(...)
int function ObjectReference_countLinkedRefChain() global
	int session = Costile2.GetSession("ObjectReference_countLinkedRefChain")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	keyword cdcg_apKeyword = Costile2.GetTESForm(session, 1) as keyword
	int cdcg_maxExpectedLinkedRefs = Costile2.GetInt(session, 2)
	return this.countLinkedRefChain(cdcg_apKeyword, cdcg_maxExpectedLinkedRefs)
endFunction

; objectreference ObjectReference::GetNthLinkedRef(...) native
objectreference function ObjectReference_GetNthLinkedRef() global
	int session = Costile2.GetSession("ObjectReference_GetNthLinkedRef")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	int cdcg_aiLinkedRef = Costile2.GetInt(session, 1)
	return this.GetNthLinkedRef(cdcg_aiLinkedRef)
endFunction

; void ObjectReference::EnableLinkChain(...)
function ObjectReference_EnableLinkChain() global
	int session = Costile2.GetSession("ObjectReference_EnableLinkChain")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	keyword cdcg_apKeyword = Costile2.GetTESForm(session, 1) as keyword
	this.EnableLinkChain(cdcg_apKeyword)
endFunction

; void ObjectReference::DisableLinkChain(...)
function ObjectReference_DisableLinkChain() global
	int session = Costile2.GetSession("ObjectReference_DisableLinkChain")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	keyword cdcg_apKeyword = Costile2.GetTESForm(session, 1) as keyword
	bool cdcg_abFadeOut = Costile2.GetBool(session, 2)
	this.DisableLinkChain(cdcg_apKeyword, cdcg_abFadeOut)
endFunction

; float ObjectReference::GetMass(...) native
float function ObjectReference_GetMass() global
	int session = Costile2.GetSession("ObjectReference_GetMass")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetMass()
endFunction

; int ObjectReference::GetOpenState(...) native
int function ObjectReference_GetOpenState() global
	int session = Costile2.GetSession("ObjectReference_GetOpenState")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetOpenState()
endFunction

; cell ObjectReference::GetParentCell(...) native
cell function ObjectReference_GetParentCell() global
	int session = Costile2.GetSession("ObjectReference_GetParentCell")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetParentCell()
endFunction

; float ObjectReference::GetPositionX(...) native
float function ObjectReference_GetPositionX() global
	int session = Costile2.GetSession("ObjectReference_GetPositionX")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetPositionX()
endFunction

; float ObjectReference::GetPositionY(...) native
float function ObjectReference_GetPositionY() global
	int session = Costile2.GetSession("ObjectReference_GetPositionY")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetPositionY()
endFunction

; float ObjectReference::GetPositionZ(...) native
float function ObjectReference_GetPositionZ() global
	int session = Costile2.GetSession("ObjectReference_GetPositionZ")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetPositionZ()
endFunction

; float ObjectReference::GetScale(...) native
float function ObjectReference_GetScale() global
	int session = Costile2.GetSession("ObjectReference_GetScale")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetScale()
endFunction

; int ObjectReference::GetTriggerObjectCount(...) native
int function ObjectReference_GetTriggerObjectCount() global
	int session = Costile2.GetSession("ObjectReference_GetTriggerObjectCount")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetTriggerObjectCount()
endFunction

; voicetype ObjectReference::GetVoiceType(...) native
voicetype function ObjectReference_GetVoiceType() global
	int session = Costile2.GetSession("ObjectReference_GetVoiceType")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetVoiceType()
endFunction

; float ObjectReference::GetWidth(...) native
float function ObjectReference_GetWidth() global
	int session = Costile2.GetSession("ObjectReference_GetWidth")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetWidth()
endFunction

; worldspace ObjectReference::GetWorldSpace(...) native
worldspace function ObjectReference_GetWorldSpace() global
	int session = Costile2.GetSession("ObjectReference_GetWorldSpace")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetWorldSpace()
endFunction

; actor ObjectReference::GetSelfAsActor(...)
actor function ObjectReference_GetSelfAsActor() global
	int session = Costile2.GetSession("ObjectReference_GetSelfAsActor")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetSelfAsActor()
endFunction

; bool ObjectReference::HasEffectKeyword(...) native
bool function ObjectReference_HasEffectKeyword() global
	int session = Costile2.GetSession("ObjectReference_HasEffectKeyword")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	keyword cdcg_akKeyword = Costile2.GetTESForm(session, 1) as keyword
	return this.HasEffectKeyword(cdcg_akKeyword)
endFunction

; bool ObjectReference::HasNode(...) native
bool function ObjectReference_HasNode() global
	int session = Costile2.GetSession("ObjectReference_HasNode")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_asNodeName = Costile2.GetString(session, 1)
	return this.HasNode(cdcg_asNodeName)
endFunction

; bool ObjectReference::HasRefType(...) native
bool function ObjectReference_HasRefType() global
	int session = Costile2.GetSession("ObjectReference_HasRefType")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	locationreftype cdcg_akRefType = Costile2.GetTESForm(session, 1) as locationreftype
	return this.HasRefType(cdcg_akRefType)
endFunction

; void ObjectReference::IgnoreFriendlyHits(...) native
function ObjectReference_IgnoreFriendlyHits() global
	int session = Costile2.GetSession("ObjectReference_IgnoreFriendlyHits")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abIgnore = Costile2.GetBool(session, 1)
	this.IgnoreFriendlyHits(cdcg_abIgnore)
endFunction

; void ObjectReference::InterruptCast(...) native
function ObjectReference_InterruptCast() global
	int session = Costile2.GetSession("ObjectReference_InterruptCast")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	this.InterruptCast()
endFunction

; bool ObjectReference::IsActivateChild(...) native
bool function ObjectReference_IsActivateChild() global
	int session = Costile2.GetSession("ObjectReference_IsActivateChild")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akChild = Costile2.GetTESForm(session, 1) as objectreference
	return this.IsActivateChild(cdcg_akChild)
endFunction

; bool ObjectReference::IsActivationBlocked(...) native
bool function ObjectReference_IsActivationBlocked() global
	int session = Costile2.GetSession("ObjectReference_IsActivationBlocked")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsActivationBlocked()
endFunction

; bool ObjectReference::Is3DLoaded(...) native
bool function ObjectReference_Is3DLoaded() global
	int session = Costile2.GetSession("ObjectReference_Is3DLoaded")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.Is3DLoaded()
endFunction

; bool ObjectReference::IsDeleted(...) native
bool function ObjectReference_IsDeleted() global
	int session = Costile2.GetSession("ObjectReference_IsDeleted")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsDeleted()
endFunction

; bool ObjectReference::IsDisabled(...) native
bool function ObjectReference_IsDisabled() global
	int session = Costile2.GetSession("ObjectReference_IsDisabled")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsDisabled()
endFunction

; bool ObjectReference::IsEnabled(...)
bool function ObjectReference_IsEnabled() global
	int session = Costile2.GetSession("ObjectReference_IsEnabled")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsEnabled()
endFunction

; bool ObjectReference::IsFurnitureInUse(...) native
bool function ObjectReference_IsFurnitureInUse() global
	int session = Costile2.GetSession("ObjectReference_IsFurnitureInUse")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abIgnoreReserved = Costile2.GetBool(session, 1)
	return this.IsFurnitureInUse(cdcg_abIgnoreReserved)
endFunction

; bool ObjectReference::IsFurnitureMarkerInUse(...) native
bool function ObjectReference_IsFurnitureMarkerInUse() global
	int session = Costile2.GetSession("ObjectReference_IsFurnitureMarkerInUse")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	int cdcg_aiMarker = Costile2.GetInt(session, 1)
	bool cdcg_abIgnoreReserved = Costile2.GetBool(session, 2)
	return this.IsFurnitureMarkerInUse(cdcg_aiMarker, cdcg_abIgnoreReserved)
endFunction

; bool ObjectReference::IsIgnoringFriendlyHits(...) native
bool function ObjectReference_IsIgnoringFriendlyHits() global
	int session = Costile2.GetSession("ObjectReference_IsIgnoringFriendlyHits")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsIgnoringFriendlyHits()
endFunction

; bool ObjectReference::IsInDialogueWithPlayer(...) native
bool function ObjectReference_IsInDialogueWithPlayer() global
	int session = Costile2.GetSession("ObjectReference_IsInDialogueWithPlayer")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsInDialogueWithPlayer()
endFunction

; bool ObjectReference::IsLockBroken(...) native
bool function ObjectReference_IsLockBroken() global
	int session = Costile2.GetSession("ObjectReference_IsLockBroken")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsLockBroken()
endFunction

; bool ObjectReference::IsLocked(...) native
bool function ObjectReference_IsLocked() global
	int session = Costile2.GetSession("ObjectReference_IsLocked")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsLocked()
endFunction

; bool ObjectReference::IsMapMarkerVisible(...) native
bool function ObjectReference_IsMapMarkerVisible() global
	int session = Costile2.GetSession("ObjectReference_IsMapMarkerVisible")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsMapMarkerVisible()
endFunction

; void ObjectReference::KnockAreaEffect(...) native
function ObjectReference_KnockAreaEffect() global
	int session = Costile2.GetSession("ObjectReference_KnockAreaEffect")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	float cdcg_afMagnitude = Costile2.GetFloat(session, 1)
	float cdcg_afRadius = Costile2.GetFloat(session, 2)
	this.KnockAreaEffect(cdcg_afMagnitude, cdcg_afRadius)
endFunction

; void ObjectReference::Lock(...) native
function ObjectReference_Lock() global
	int session = Costile2.GetSession("ObjectReference_Lock")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abLock = Costile2.GetBool(session, 1)
	bool cdcg_abAsOwner = Costile2.GetBool(session, 2)
	this.Lock(cdcg_abLock, cdcg_abAsOwner)
endFunction

; void ObjectReference::MoveTo(...) native
function ObjectReference_MoveTo() global
	int session = Costile2.GetSession("ObjectReference_MoveTo")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 1) as objectreference
	float cdcg_afXOffset = Costile2.GetFloat(session, 2)
	float cdcg_afYOffset = Costile2.GetFloat(session, 3)
	float cdcg_afZOffset = Costile2.GetFloat(session, 4)
	bool cdcg_abMatchRotation = Costile2.GetBool(session, 5)
	this.MoveTo(cdcg_akTarget, cdcg_afXOffset, cdcg_afYOffset, cdcg_afZOffset, cdcg_abMatchRotation)
endFunction

; void ObjectReference::MoveToInteractionLocation(...) native
function ObjectReference_MoveToInteractionLocation() global
	int session = Costile2.GetSession("ObjectReference_MoveToInteractionLocation")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 1) as objectreference
	this.MoveToInteractionLocation(cdcg_akTarget)
endFunction

; void ObjectReference::MoveToMyEditorLocation(...) native
function ObjectReference_MoveToMyEditorLocation() global
	int session = Costile2.GetSession("ObjectReference_MoveToMyEditorLocation")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	this.MoveToMyEditorLocation()
endFunction

; void ObjectReference::MoveToNode(...) native
function ObjectReference_MoveToNode() global
	int session = Costile2.GetSession("ObjectReference_MoveToNode")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 1) as objectreference
	string cdcg_asNodeName = Costile2.GetString(session, 2)
	this.MoveToNode(cdcg_akTarget, cdcg_asNodeName)
endFunction

; objectreference ObjectReference::PlaceAtMe(...) native
objectreference function ObjectReference_PlaceAtMe() global
	int session = Costile2.GetSession("ObjectReference_PlaceAtMe")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	form cdcg_akFormToPlace = Costile2.GetTESForm(session, 1) as form
	int cdcg_aiCount = Costile2.GetInt(session, 2)
	bool cdcg_abForcePersist = Costile2.GetBool(session, 3)
	bool cdcg_abInitiallyDisabled = Costile2.GetBool(session, 4)
	return this.PlaceAtMe(cdcg_akFormToPlace, cdcg_aiCount, cdcg_abForcePersist, cdcg_abInitiallyDisabled)
endFunction

; actor ObjectReference::PlaceActorAtMe(...) native
actor function ObjectReference_PlaceActorAtMe() global
	int session = Costile2.GetSession("ObjectReference_PlaceActorAtMe")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	actorbase cdcg_akActorToPlace = Costile2.GetTESForm(session, 1) as actorbase
	int cdcg_aiLevelMod = Costile2.GetInt(session, 2)
	encounterzone cdcg_akZone = Costile2.GetTESForm(session, 3) as encounterzone
	return this.PlaceActorAtMe(cdcg_akActorToPlace, cdcg_aiLevelMod, cdcg_akZone)
endFunction

; bool ObjectReference::PlayAnimation(...) native
bool function ObjectReference_PlayAnimation() global
	int session = Costile2.GetSession("ObjectReference_PlayAnimation")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_asAnimation = Costile2.GetString(session, 1)
	return this.PlayAnimation(cdcg_asAnimation)
endFunction

; bool ObjectReference::PlayAnimationAndWait(...) native
bool function ObjectReference_PlayAnimationAndWait() global
	int session = Costile2.GetSession("ObjectReference_PlayAnimationAndWait")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_asAnimation = Costile2.GetString(session, 1)
	string cdcg_asEventName = Costile2.GetString(session, 2)
	return this.PlayAnimationAndWait(cdcg_asAnimation, cdcg_asEventName)
endFunction

; bool ObjectReference::PlayGamebryoAnimation(...) native
bool function ObjectReference_PlayGamebryoAnimation() global
	int session = Costile2.GetSession("ObjectReference_PlayGamebryoAnimation")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_asAnimation = Costile2.GetString(session, 1)
	bool cdcg_abStartOver = Costile2.GetBool(session, 2)
	float cdcg_afEaseInTime = Costile2.GetFloat(session, 3)
	return this.PlayGamebryoAnimation(cdcg_asAnimation, cdcg_abStartOver, cdcg_afEaseInTime)
endFunction

; bool ObjectReference::PlayImpactEffect(...) native
bool function ObjectReference_PlayImpactEffect() global
	int session = Costile2.GetSession("ObjectReference_PlayImpactEffect")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	impactdataset cdcg_akImpactEffect = Costile2.GetTESForm(session, 1) as impactdataset
	string cdcg_asNodeName = Costile2.GetString(session, 2)
	float cdcg_afPickDirX = Costile2.GetFloat(session, 3)
	float cdcg_afPickDirY = Costile2.GetFloat(session, 4)
	float cdcg_afPickDirZ = Costile2.GetFloat(session, 5)
	float cdcg_afPickLength = Costile2.GetFloat(session, 6)
	bool cdcg_abApplyNodeRotation = Costile2.GetBool(session, 7)
	bool cdcg_abUseNodeLocalRotation = Costile2.GetBool(session, 8)
	return this.PlayImpactEffect(cdcg_akImpactEffect, cdcg_asNodeName, cdcg_afPickDirX, cdcg_afPickDirY, cdcg_afPickDirZ, cdcg_afPickLength, cdcg_abApplyNodeRotation, cdcg_abUseNodeLocalRotation)
endFunction

; bool ObjectReference::PlaySyncedAnimationSS(...) native
bool function ObjectReference_PlaySyncedAnimationSS() global
	int session = Costile2.GetSession("ObjectReference_PlaySyncedAnimationSS")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_asAnimation1 = Costile2.GetString(session, 1)
	objectreference cdcg_akObj2 = Costile2.GetTESForm(session, 2) as objectreference
	string cdcg_asAnimation2 = Costile2.GetString(session, 3)
	return this.PlaySyncedAnimationSS(cdcg_asAnimation1, cdcg_akObj2, cdcg_asAnimation2)
endFunction

; bool ObjectReference::PlaySyncedAnimationAndWaitSS(...) native
bool function ObjectReference_PlaySyncedAnimationAndWaitSS() global
	int session = Costile2.GetSession("ObjectReference_PlaySyncedAnimationAndWaitSS")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_asAnimation1 = Costile2.GetString(session, 1)
	string cdcg_asEvent1 = Costile2.GetString(session, 2)
	objectreference cdcg_akObj2 = Costile2.GetTESForm(session, 3) as objectreference
	string cdcg_asAnimation2 = Costile2.GetString(session, 4)
	string cdcg_asEvent2 = Costile2.GetString(session, 5)
	return this.PlaySyncedAnimationAndWaitSS(cdcg_asAnimation1, cdcg_asEvent1, cdcg_akObj2, cdcg_asAnimation2, cdcg_asEvent2)
endFunction

; void ObjectReference::PlayTerrainEffect(...) native
function ObjectReference_PlayTerrainEffect() global
	int session = Costile2.GetSession("ObjectReference_PlayTerrainEffect")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_asEffectModelName = Costile2.GetString(session, 1)
	string cdcg_asAttachBoneName = Costile2.GetString(session, 2)
	this.PlayTerrainEffect(cdcg_asEffectModelName, cdcg_asAttachBoneName)
endFunction

; void ObjectReference::ProcessTrapHit(...) native
function ObjectReference_ProcessTrapHit() global
	int session = Costile2.GetSession("ObjectReference_ProcessTrapHit")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akTrap = Costile2.GetTESForm(session, 1) as objectreference
	float cdcg_afDamage = Costile2.GetFloat(session, 2)
	float cdcg_afPushback = Costile2.GetFloat(session, 3)
	float cdcg_afXVel = Costile2.GetFloat(session, 4)
	float cdcg_afYVel = Costile2.GetFloat(session, 5)
	float cdcg_afZVel = Costile2.GetFloat(session, 6)
	float cdcg_afXPos = Costile2.GetFloat(session, 7)
	float cdcg_afYPos = Costile2.GetFloat(session, 8)
	float cdcg_afZPos = Costile2.GetFloat(session, 9)
	int cdcg_aeMaterial = Costile2.GetInt(session, 10)
	float cdcg_afStagger = Costile2.GetFloat(session, 11)
	this.ProcessTrapHit(cdcg_akTrap, cdcg_afDamage, cdcg_afPushback, cdcg_afXVel, cdcg_afYVel, cdcg_afZVel, cdcg_afXPos, cdcg_afYPos, cdcg_afZPos, cdcg_aeMaterial, cdcg_afStagger)
endFunction

; void ObjectReference::PushActorAway(...) native
function ObjectReference_PushActorAway() global
	int session = Costile2.GetSession("ObjectReference_PushActorAway")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	actor cdcg_akActorToPush = Costile2.GetTESForm(session, 1) as actor
	float cdcg_aiKnockbackForce = Costile2.GetFloat(session, 2)
	this.PushActorAway(cdcg_akActorToPush, cdcg_aiKnockbackForce)
endFunction

; void ObjectReference::RemoveAllInventoryEventFilters(...) native
function ObjectReference_RemoveAllInventoryEventFilters() global
	int session = Costile2.GetSession("ObjectReference_RemoveAllInventoryEventFilters")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	this.RemoveAllInventoryEventFilters()
endFunction

; void ObjectReference::RemoveAllItems(...) native
function ObjectReference_RemoveAllItems() global
	int session = Costile2.GetSession("ObjectReference_RemoveAllItems")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akTransferTo = Costile2.GetTESForm(session, 1) as objectreference
	bool cdcg_abKeepOwnership = Costile2.GetBool(session, 2)
	bool cdcg_abRemoveQuestItems = Costile2.GetBool(session, 3)
	this.RemoveAllItems(cdcg_akTransferTo, cdcg_abKeepOwnership, cdcg_abRemoveQuestItems)
endFunction

; void ObjectReference::RemoveInventoryEventFilter(...) native
function ObjectReference_RemoveInventoryEventFilter() global
	int session = Costile2.GetSession("ObjectReference_RemoveInventoryEventFilter")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	form cdcg_akFilter = Costile2.GetTESForm(session, 1) as form
	this.RemoveInventoryEventFilter(cdcg_akFilter)
endFunction

; void ObjectReference::RemoveItem(...) native
function ObjectReference_RemoveItem() global
	int session = Costile2.GetSession("ObjectReference_RemoveItem")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	form cdcg_akItemToRemove = Costile2.GetTESForm(session, 1) as form
	int cdcg_aiCount = Costile2.GetInt(session, 2)
	bool cdcg_abSilent = Costile2.GetBool(session, 3)
	objectreference cdcg_akOtherContainer = Costile2.GetTESForm(session, 4) as objectreference
	this.RemoveItem(cdcg_akItemToRemove, cdcg_aiCount, cdcg_abSilent, cdcg_akOtherContainer)
endFunction

; bool ObjectReference::RemoveDependentAnimatedObjectReference(...) native
bool function ObjectReference_RemoveDependentAnimatedObjectReference() global
	int session = Costile2.GetSession("ObjectReference_RemoveDependentAnimatedObjectReference")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akDependent = Costile2.GetTESForm(session, 1) as objectreference
	return this.RemoveDependentAnimatedObjectReference(cdcg_akDependent)
endFunction

; void ObjectReference::Reset(...) native
function ObjectReference_Reset() global
	int session = Costile2.GetSession("ObjectReference_Reset")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 1) as objectreference
	this.Reset(cdcg_akTarget)
endFunction

; void ObjectReference::Say(...) native
function ObjectReference_Say() global
	int session = Costile2.GetSession("ObjectReference_Say")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	topic cdcg_akTopicToSay = Costile2.GetTESForm(session, 1) as topic
	actor cdcg_akActorToSpeakAs = Costile2.GetTESForm(session, 2) as actor
	bool cdcg_abSpeakInPlayersHead = Costile2.GetBool(session, 3)
	this.Say(cdcg_akTopicToSay, cdcg_akActorToSpeakAs, cdcg_abSpeakInPlayersHead)
endFunction

; void ObjectReference::SendStealAlarm(...) native
function ObjectReference_SendStealAlarm() global
	int session = Costile2.GetSession("ObjectReference_SendStealAlarm")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	actor cdcg_akThief = Costile2.GetTESForm(session, 1) as actor
	this.SendStealAlarm(cdcg_akThief)
endFunction

; void ObjectReference::SetActorCause(...) native
function ObjectReference_SetActorCause() global
	int session = Costile2.GetSession("ObjectReference_SetActorCause")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	actor cdcg_akActor = Costile2.GetTESForm(session, 1) as actor
	this.SetActorCause(cdcg_akActor)
endFunction

; void ObjectReference::SetActorOwner(...) native
function ObjectReference_SetActorOwner() global
	int session = Costile2.GetSession("ObjectReference_SetActorOwner")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	actorbase cdcg_akActorBase = Costile2.GetTESForm(session, 1) as actorbase
	this.SetActorOwner(cdcg_akActorBase)
endFunction

; void ObjectReference::SetAngle(...) native
function ObjectReference_SetAngle() global
	int session = Costile2.GetSession("ObjectReference_SetAngle")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	float cdcg_afXAngle = Costile2.GetFloat(session, 1)
	float cdcg_afYAngle = Costile2.GetFloat(session, 2)
	float cdcg_afZAngle = Costile2.GetFloat(session, 3)
	this.SetAngle(cdcg_afXAngle, cdcg_afYAngle, cdcg_afZAngle)
endFunction

; void ObjectReference::SetAnimationVariableBool(...) native
function ObjectReference_SetAnimationVariableBool() global
	int session = Costile2.GetSession("ObjectReference_SetAnimationVariableBool")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_arVariableName = Costile2.GetString(session, 1)
	bool cdcg_abNewValue = Costile2.GetBool(session, 2)
	this.SetAnimationVariableBool(cdcg_arVariableName, cdcg_abNewValue)
endFunction

; void ObjectReference::SetAnimationVariableInt(...) native
function ObjectReference_SetAnimationVariableInt() global
	int session = Costile2.GetSession("ObjectReference_SetAnimationVariableInt")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_arVariableName = Costile2.GetString(session, 1)
	int cdcg_aiNewValue = Costile2.GetInt(session, 2)
	this.SetAnimationVariableInt(cdcg_arVariableName, cdcg_aiNewValue)
endFunction

; void ObjectReference::SetAnimationVariableFloat(...) native
function ObjectReference_SetAnimationVariableFloat() global
	int session = Costile2.GetSession("ObjectReference_SetAnimationVariableFloat")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_arVariableName = Costile2.GetString(session, 1)
	float cdcg_afNewValue = Costile2.GetFloat(session, 2)
	this.SetAnimationVariableFloat(cdcg_arVariableName, cdcg_afNewValue)
endFunction

; void ObjectReference::SetDestroyed(...) native
function ObjectReference_SetDestroyed() global
	int session = Costile2.GetSession("ObjectReference_SetDestroyed")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abDestroyed = Costile2.GetBool(session, 1)
	this.SetDestroyed(cdcg_abDestroyed)
endFunction

; void ObjectReference::SetFactionOwner(...) native
function ObjectReference_SetFactionOwner() global
	int session = Costile2.GetSession("ObjectReference_SetFactionOwner")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	faction cdcg_akFaction = Costile2.GetTESForm(session, 1) as faction
	this.SetFactionOwner(cdcg_akFaction)
endFunction

; void ObjectReference::SetLockLevel(...) native
function ObjectReference_SetLockLevel() global
	int session = Costile2.GetSession("ObjectReference_SetLockLevel")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	int cdcg_aiLockLevel = Costile2.GetInt(session, 1)
	this.SetLockLevel(cdcg_aiLockLevel)
endFunction

; void ObjectReference::SetMotionType(...) native
function ObjectReference_SetMotionType() global
	int session = Costile2.GetSession("ObjectReference_SetMotionType")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	int cdcg_aeMotionType = Costile2.GetInt(session, 1)
	bool cdcg_abAllowActivate = Costile2.GetBool(session, 2)
	this.SetMotionType(cdcg_aeMotionType, cdcg_abAllowActivate)
endFunction

; void ObjectReference::SetNoFavorAllowed(...) native
function ObjectReference_SetNoFavorAllowed() global
	int session = Costile2.GetSession("ObjectReference_SetNoFavorAllowed")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abNoFavor = Costile2.GetBool(session, 1)
	this.SetNoFavorAllowed(cdcg_abNoFavor)
endFunction

; void ObjectReference::SetOpen(...) native
function ObjectReference_SetOpen() global
	int session = Costile2.GetSession("ObjectReference_SetOpen")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_abOpen = Costile2.GetBool(session, 1)
	this.SetOpen(cdcg_abOpen)
endFunction

; void ObjectReference::SetPosition(...) native
function ObjectReference_SetPosition() global
	int session = Costile2.GetSession("ObjectReference_SetPosition")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	float cdcg_afX = Costile2.GetFloat(session, 1)
	float cdcg_afY = Costile2.GetFloat(session, 2)
	float cdcg_afZ = Costile2.GetFloat(session, 3)
	this.SetPosition(cdcg_afX, cdcg_afY, cdcg_afZ)
endFunction

; void ObjectReference::SetScale(...) native
function ObjectReference_SetScale() global
	int session = Costile2.GetSession("ObjectReference_SetScale")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	float cdcg_afScale = Costile2.GetFloat(session, 1)
	this.SetScale(cdcg_afScale)
endFunction

; void ObjectReference::TranslateTo(...) native
function ObjectReference_TranslateTo() global
	int session = Costile2.GetSession("ObjectReference_TranslateTo")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	float cdcg_afX = Costile2.GetFloat(session, 1)
	float cdcg_afY = Costile2.GetFloat(session, 2)
	float cdcg_afZ = Costile2.GetFloat(session, 3)
	float cdcg_afXAngle = Costile2.GetFloat(session, 4)
	float cdcg_afYAngle = Costile2.GetFloat(session, 5)
	float cdcg_afZAngle = Costile2.GetFloat(session, 6)
	float cdcg_afSpeed = Costile2.GetFloat(session, 7)
	float cdcg_afMaxRotationSpeed = Costile2.GetFloat(session, 8)
	this.TranslateTo(cdcg_afX, cdcg_afY, cdcg_afZ, cdcg_afXAngle, cdcg_afYAngle, cdcg_afZAngle, cdcg_afSpeed, cdcg_afMaxRotationSpeed)
endFunction

; void ObjectReference::SplineTranslateTo(...) native
function ObjectReference_SplineTranslateTo() global
	int session = Costile2.GetSession("ObjectReference_SplineTranslateTo")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	float cdcg_afX = Costile2.GetFloat(session, 1)
	float cdcg_afY = Costile2.GetFloat(session, 2)
	float cdcg_afZ = Costile2.GetFloat(session, 3)
	float cdcg_afXAngle = Costile2.GetFloat(session, 4)
	float cdcg_afYAngle = Costile2.GetFloat(session, 5)
	float cdcg_afZAngle = Costile2.GetFloat(session, 6)
	float cdcg_afTangentMagnitude = Costile2.GetFloat(session, 7)
	float cdcg_afSpeed = Costile2.GetFloat(session, 8)
	float cdcg_afMaxRotationSpeed = Costile2.GetFloat(session, 9)
	this.SplineTranslateTo(cdcg_afX, cdcg_afY, cdcg_afZ, cdcg_afXAngle, cdcg_afYAngle, cdcg_afZAngle, cdcg_afTangentMagnitude, cdcg_afSpeed, cdcg_afMaxRotationSpeed)
endFunction

; void ObjectReference::SplineTranslateToRefNode(...) native
function ObjectReference_SplineTranslateToRefNode() global
	int session = Costile2.GetSession("ObjectReference_SplineTranslateToRefNode")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_arTarget = Costile2.GetTESForm(session, 1) as objectreference
	string cdcg_arNodeName = Costile2.GetString(session, 2)
	float cdcg_afTangentMagnitude = Costile2.GetFloat(session, 3)
	float cdcg_afSpeed = Costile2.GetFloat(session, 4)
	float cdcg_afMaxRotationSpeed = Costile2.GetFloat(session, 5)
	this.SplineTranslateToRefNode(cdcg_arTarget, cdcg_arNodeName, cdcg_afTangentMagnitude, cdcg_afSpeed, cdcg_afMaxRotationSpeed)
endFunction

; void ObjectReference::StopTranslation(...) native
function ObjectReference_StopTranslation() global
	int session = Costile2.GetSession("ObjectReference_StopTranslation")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	this.StopTranslation()
endFunction

; void ObjectReference::TranslateToRef(...)
function ObjectReference_TranslateToRef() global
	int session = Costile2.GetSession("ObjectReference_TranslateToRef")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_arTarget = Costile2.GetTESForm(session, 1) as objectreference
	float cdcg_afSpeed = Costile2.GetFloat(session, 2)
	float cdcg_afMaxRotationSpeed = Costile2.GetFloat(session, 3)
	this.TranslateToRef(cdcg_arTarget, cdcg_afSpeed, cdcg_afMaxRotationSpeed)
endFunction

; void ObjectReference::SplineTranslateToRef(...)
function ObjectReference_SplineTranslateToRef() global
	int session = Costile2.GetSession("ObjectReference_SplineTranslateToRef")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_arTarget = Costile2.GetTESForm(session, 1) as objectreference
	float cdcg_afTangentMagnitude = Costile2.GetFloat(session, 2)
	float cdcg_afSpeed = Costile2.GetFloat(session, 3)
	float cdcg_afMaxRotationSpeed = Costile2.GetFloat(session, 4)
	this.SplineTranslateToRef(cdcg_arTarget, cdcg_afTangentMagnitude, cdcg_afSpeed, cdcg_afMaxRotationSpeed)
endFunction

; void ObjectReference::TetherToHorse(...) native
function ObjectReference_TetherToHorse() global
	int session = Costile2.GetSession("ObjectReference_TetherToHorse")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	objectreference cdcg_akHorse = Costile2.GetTESForm(session, 1) as objectreference
	this.TetherToHorse(cdcg_akHorse)
endFunction

; bool ObjectReference::WaitForAnimationEvent(...) native
bool function ObjectReference_WaitForAnimationEvent() global
	int session = Costile2.GetSession("ObjectReference_WaitForAnimationEvent")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_asEventName = Costile2.GetString(session, 1)
	return this.WaitForAnimationEvent(cdcg_asEventName)
endFunction

; bool ObjectReference::IsInLocation(...)
bool function ObjectReference_IsInLocation() global
	int session = Costile2.GetSession("ObjectReference_IsInLocation")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	location cdcg_akLocation = Costile2.GetTESForm(session, 1) as location
	return this.IsInLocation(cdcg_akLocation)
endFunction

; int ObjectReference::GetNumItems(...) native
int function ObjectReference_GetNumItems() global
	int session = Costile2.GetSession("ObjectReference_GetNumItems")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetNumItems()
endFunction

; form ObjectReference::GetNthForm(...) native
form function ObjectReference_GetNthForm() global
	int session = Costile2.GetSession("ObjectReference_GetNthForm")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthForm(cdcg_index)
endFunction

; float ObjectReference::GetTotalItemWeight(...) native
float function ObjectReference_GetTotalItemWeight() global
	int session = Costile2.GetSession("ObjectReference_GetTotalItemWeight")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetTotalItemWeight()
endFunction

; float ObjectReference::GetTotalArmorWeight(...) native
float function ObjectReference_GetTotalArmorWeight() global
	int session = Costile2.GetSession("ObjectReference_GetTotalArmorWeight")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetTotalArmorWeight()
endFunction

; bool ObjectReference::IsHarvested(...) native
bool function ObjectReference_IsHarvested() global
	int session = Costile2.GetSession("ObjectReference_IsHarvested")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsHarvested()
endFunction

; void ObjectReference::SetHarvested(...) native
function ObjectReference_SetHarvested() global
	int session = Costile2.GetSession("ObjectReference_SetHarvested")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	bool cdcg_harvested = Costile2.GetBool(session, 1)
	this.SetHarvested(cdcg_harvested)
endFunction

; void ObjectReference::SetItemHealthPercent(...) native
function ObjectReference_SetItemHealthPercent() global
	int session = Costile2.GetSession("ObjectReference_SetItemHealthPercent")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	float cdcg_health = Costile2.GetFloat(session, 1)
	this.SetItemHealthPercent(cdcg_health)
endFunction

; void ObjectReference::SetItemMaxCharge(...) native
function ObjectReference_SetItemMaxCharge() global
	int session = Costile2.GetSession("ObjectReference_SetItemMaxCharge")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	float cdcg_maxCharge = Costile2.GetFloat(session, 1)
	this.SetItemMaxCharge(cdcg_maxCharge)
endFunction

; float ObjectReference::GetItemMaxCharge(...) native
float function ObjectReference_GetItemMaxCharge() global
	int session = Costile2.GetSession("ObjectReference_GetItemMaxCharge")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetItemMaxCharge()
endFunction

; float ObjectReference::GetItemCharge(...) native
float function ObjectReference_GetItemCharge() global
	int session = Costile2.GetSession("ObjectReference_GetItemCharge")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetItemCharge()
endFunction

; void ObjectReference::SetItemCharge(...) native
function ObjectReference_SetItemCharge() global
	int session = Costile2.GetSession("ObjectReference_SetItemCharge")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	float cdcg_charge = Costile2.GetFloat(session, 1)
	this.SetItemCharge(cdcg_charge)
endFunction

; void ObjectReference::ResetInventory(...) native
function ObjectReference_ResetInventory() global
	int session = Costile2.GetSession("ObjectReference_ResetInventory")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	this.ResetInventory()
endFunction

; bool ObjectReference::IsOffLimits(...) native
bool function ObjectReference_IsOffLimits() global
	int session = Costile2.GetSession("ObjectReference_IsOffLimits")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.IsOffLimits()
endFunction

; string ObjectReference::GetDisplayName(...) native
string function ObjectReference_GetDisplayName() global
	int session = Costile2.GetSession("ObjectReference_GetDisplayName")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetDisplayName()
endFunction

; bool ObjectReference::SetDisplayName(...) native
bool function ObjectReference_SetDisplayName() global
	int session = Costile2.GetSession("ObjectReference_SetDisplayName")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	string cdcg_name = Costile2.GetString(session, 1)
	bool cdcg_force = Costile2.GetBool(session, 2)
	return this.SetDisplayName(cdcg_name, cdcg_force)
endFunction

; objectreference ObjectReference::GetEnableParent(...) native
objectreference function ObjectReference_GetEnableParent() global
	int session = Costile2.GetSession("ObjectReference_GetEnableParent")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetEnableParent()
endFunction

; enchantment ObjectReference::GetEnchantment(...) native
enchantment function ObjectReference_GetEnchantment() global
	int session = Costile2.GetSession("ObjectReference_GetEnchantment")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetEnchantment()
endFunction

; void ObjectReference::SetEnchantment(...) native
function ObjectReference_SetEnchantment() global
	int session = Costile2.GetSession("ObjectReference_SetEnchantment")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	enchantment cdcg_source = Costile2.GetTESForm(session, 1) as enchantment
	float cdcg_maxCharge = Costile2.GetFloat(session, 2)
	this.SetEnchantment(cdcg_source, cdcg_maxCharge)
endFunction

; int ObjectReference::GetNumReferenceAliases(...) native
int function ObjectReference_GetNumReferenceAliases() global
	int session = Costile2.GetSession("ObjectReference_GetNumReferenceAliases")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetNumReferenceAliases()
endFunction

; referencealias ObjectReference::GetNthReferenceAlias(...) native
referencealias function ObjectReference_GetNthReferenceAlias() global
	int session = Costile2.GetSession("ObjectReference_GetNthReferenceAlias")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthReferenceAlias(cdcg_n)
endFunction

; potion ObjectReference::GetPoison(...) native
potion function ObjectReference_GetPoison() global
	int session = Costile2.GetSession("ObjectReference_GetPoison")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	return this.GetPoison()
endFunction

; void ObjectReference::GetAllForms(...) native
function ObjectReference_GetAllForms() global
	int session = Costile2.GetSession("ObjectReference_GetAllForms")
	ObjectReference this = Costile2.GetTESForm(session, 0) as ObjectReference
	formlist cdcg_toFill = Costile2.GetTESForm(session, 1) as formlist
	this.GetAllForms(cdcg_toFill)
endFunction

