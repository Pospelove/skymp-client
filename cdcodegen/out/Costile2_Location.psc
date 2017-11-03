Scriptname Costile2_Location Hidden

;
;
;
; script Location.psc
;
;
;

; float Location::GetKeywordData(...) native
float function Location_GetKeywordData() global
	int session = Costile2.GetSession("Location_GetKeywordData")
	Location this = Costile2.GetTESForm(session, 0) as Location
	keyword cdcg_akKeyword = Costile2.GetTESForm(session, 1) as keyword
	return this.GetKeywordData(cdcg_akKeyword)
endFunction

; int Location::GetRefTypeAliveCount(...) native
int function Location_GetRefTypeAliveCount() global
	int session = Costile2.GetSession("Location_GetRefTypeAliveCount")
	Location this = Costile2.GetTESForm(session, 0) as Location
	locationreftype cdcg_akRefType = Costile2.GetTESForm(session, 1) as locationreftype
	return this.GetRefTypeAliveCount(cdcg_akRefType)
endFunction

; int Location::GetRefTypeDeadCount(...) native
int function Location_GetRefTypeDeadCount() global
	int session = Costile2.GetSession("Location_GetRefTypeDeadCount")
	Location this = Costile2.GetTESForm(session, 0) as Location
	locationreftype cdcg_akRefType = Costile2.GetTESForm(session, 1) as locationreftype
	return this.GetRefTypeDeadCount(cdcg_akRefType)
endFunction

; bool Location::HasCommonParent(...) native
bool function Location_HasCommonParent() global
	int session = Costile2.GetSession("Location_HasCommonParent")
	Location this = Costile2.GetTESForm(session, 0) as Location
	location cdcg_akOther = Costile2.GetTESForm(session, 1) as location
	keyword cdcg_akFilter = Costile2.GetTESForm(session, 2) as keyword
	return this.HasCommonParent(cdcg_akOther, cdcg_akFilter)
endFunction

; bool Location::HasRefType(...) native
bool function Location_HasRefType() global
	int session = Costile2.GetSession("Location_HasRefType")
	Location this = Costile2.GetTESForm(session, 0) as Location
	locationreftype cdcg_akRefType = Costile2.GetTESForm(session, 1) as locationreftype
	return this.HasRefType(cdcg_akRefType)
endFunction

; bool Location::IsCleared(...) native
bool function Location_IsCleared() global
	int session = Costile2.GetSession("Location_IsCleared")
	Location this = Costile2.GetTESForm(session, 0) as Location
	return this.IsCleared()
endFunction

; bool Location::IsChild(...) native
bool function Location_IsChild() global
	int session = Costile2.GetSession("Location_IsChild")
	Location this = Costile2.GetTESForm(session, 0) as Location
	location cdcg_akOther = Costile2.GetTESForm(session, 1) as location
	return this.IsChild(cdcg_akOther)
endFunction

; bool Location::IsLoaded(...) native
bool function Location_IsLoaded() global
	int session = Costile2.GetSession("Location_IsLoaded")
	Location this = Costile2.GetTESForm(session, 0) as Location
	return this.IsLoaded()
endFunction

; bool Location::IsSameLocation(...)
bool function Location_IsSameLocation() global
	int session = Costile2.GetSession("Location_IsSameLocation")
	Location this = Costile2.GetTESForm(session, 0) as Location
	location cdcg_akOtherLocation = Costile2.GetTESForm(session, 1) as location
	keyword cdcg_akKeyword = Costile2.GetTESForm(session, 2) as keyword
	return this.IsSameLocation(cdcg_akOtherLocation, cdcg_akKeyword)
endFunction

; void Location::SetKeywordData(...) native
function Location_SetKeywordData() global
	int session = Costile2.GetSession("Location_SetKeywordData")
	Location this = Costile2.GetTESForm(session, 0) as Location
	keyword cdcg_akKeyword = Costile2.GetTESForm(session, 1) as keyword
	float cdcg_afData = Costile2.GetFloat(session, 2)
	this.SetKeywordData(cdcg_akKeyword, cdcg_afData)
endFunction

; void Location::SetCleared(...) native
function Location_SetCleared() global
	int session = Costile2.GetSession("Location_SetCleared")
	Location this = Costile2.GetTESForm(session, 0) as Location
	bool cdcg_abCleared = Costile2.GetBool(session, 1)
	this.SetCleared(cdcg_abCleared)
endFunction

