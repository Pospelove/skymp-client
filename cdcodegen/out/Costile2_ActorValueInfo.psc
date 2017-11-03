Scriptname Costile2_ActorValueInfo Hidden

;
;
;
; script ActorValueInfo.psc
;
;
;

; actorvalueinfo ActorValueInfo::GetActorValueInfoByName(...) native global
actorvalueinfo function ActorValueInfo_GetActorValueInfoByName() global
	int session = Costile2.GetSession("ActorValueInfo_GetActorValueInfoByName")
	string cdcg_avName = Costile2.GetString(session, 0)
	return ActorValueInfo.GetActorValueInfoByName(cdcg_avName)
endFunction

; actorvalueinfo ActorValueInfo::GetAVIByName(...) global
actorvalueinfo function ActorValueInfo_GetAVIByName() global
	int session = Costile2.GetSession("ActorValueInfo_GetAVIByName")
	string cdcg_avName = Costile2.GetString(session, 0)
	return ActorValueInfo.GetAVIByName(cdcg_avName)
endFunction

; actorvalueinfo ActorValueInfo::GetActorValueInfoByID(...) native global
actorvalueinfo function ActorValueInfo_GetActorValueInfoByID() global
	int session = Costile2.GetSession("ActorValueInfo_GetActorValueInfoByID")
	int cdcg_id = Costile2.GetInt(session, 0)
	return ActorValueInfo.GetActorValueInfoByID(cdcg_id)
endFunction

; actorvalueinfo ActorValueInfo::GetAVIByID(...) global
actorvalueinfo function ActorValueInfo_GetAVIByID() global
	int session = Costile2.GetSession("ActorValueInfo_GetAVIByID")
	int cdcg_id = Costile2.GetInt(session, 0)
	return ActorValueInfo.GetAVIByID(cdcg_id)
endFunction

; bool ActorValueInfo::IsSkill(...) native
bool function ActorValueInfo_IsSkill() global
	int session = Costile2.GetSession("ActorValueInfo_IsSkill")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	return this.IsSkill()
endFunction

; float ActorValueInfo::GetSkillUseMult(...) native
float function ActorValueInfo_GetSkillUseMult() global
	int session = Costile2.GetSession("ActorValueInfo_GetSkillUseMult")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	return this.GetSkillUseMult()
endFunction

; void ActorValueInfo::SetSkillUseMult(...) native
function ActorValueInfo_SetSkillUseMult() global
	int session = Costile2.GetSession("ActorValueInfo_SetSkillUseMult")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	float cdcg_value = Costile2.GetFloat(session, 1)
	this.SetSkillUseMult(cdcg_value)
endFunction

; float ActorValueInfo::GetSkillOffsetMult(...) native
float function ActorValueInfo_GetSkillOffsetMult() global
	int session = Costile2.GetSession("ActorValueInfo_GetSkillOffsetMult")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	return this.GetSkillOffsetMult()
endFunction

; void ActorValueInfo::SetSkillOffsetMult(...) native
function ActorValueInfo_SetSkillOffsetMult() global
	int session = Costile2.GetSession("ActorValueInfo_SetSkillOffsetMult")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	float cdcg_value = Costile2.GetFloat(session, 1)
	this.SetSkillOffsetMult(cdcg_value)
endFunction

; float ActorValueInfo::GetSkillImproveMult(...) native
float function ActorValueInfo_GetSkillImproveMult() global
	int session = Costile2.GetSession("ActorValueInfo_GetSkillImproveMult")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	return this.GetSkillImproveMult()
endFunction

; void ActorValueInfo::SetSkillImproveMult(...) native
function ActorValueInfo_SetSkillImproveMult() global
	int session = Costile2.GetSession("ActorValueInfo_SetSkillImproveMult")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	float cdcg_value = Costile2.GetFloat(session, 1)
	this.SetSkillImproveMult(cdcg_value)
endFunction

; float ActorValueInfo::GetSkillImproveOffset(...) native
float function ActorValueInfo_GetSkillImproveOffset() global
	int session = Costile2.GetSession("ActorValueInfo_GetSkillImproveOffset")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	return this.GetSkillImproveOffset()
endFunction

; void ActorValueInfo::SetSkillImproveOffset(...) native
function ActorValueInfo_SetSkillImproveOffset() global
	int session = Costile2.GetSession("ActorValueInfo_SetSkillImproveOffset")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	float cdcg_value = Costile2.GetFloat(session, 1)
	this.SetSkillImproveOffset(cdcg_value)
endFunction

; float ActorValueInfo::GetSkillExperience(...) native
float function ActorValueInfo_GetSkillExperience() global
	int session = Costile2.GetSession("ActorValueInfo_GetSkillExperience")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	return this.GetSkillExperience()
endFunction

; void ActorValueInfo::SetSkillExperience(...) native
function ActorValueInfo_SetSkillExperience() global
	int session = Costile2.GetSession("ActorValueInfo_SetSkillExperience")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	float cdcg_exp = Costile2.GetFloat(session, 1)
	this.SetSkillExperience(cdcg_exp)
endFunction

; void ActorValueInfo::AddSkillExperience(...) native
function ActorValueInfo_AddSkillExperience() global
	int session = Costile2.GetSession("ActorValueInfo_AddSkillExperience")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	float cdcg_exp = Costile2.GetFloat(session, 1)
	this.AddSkillExperience(cdcg_exp)
endFunction

; float ActorValueInfo::GetExperienceForLevel(...) native
float function ActorValueInfo_GetExperienceForLevel() global
	int session = Costile2.GetSession("ActorValueInfo_GetExperienceForLevel")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	int cdcg_currentLevel = Costile2.GetInt(session, 1)
	return this.GetExperienceForLevel(cdcg_currentLevel)
endFunction

; int ActorValueInfo::GetSkillLegendaryLevel(...) native
int function ActorValueInfo_GetSkillLegendaryLevel() global
	int session = Costile2.GetSession("ActorValueInfo_GetSkillLegendaryLevel")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	return this.GetSkillLegendaryLevel()
endFunction

; void ActorValueInfo::SetSkillLegendaryLevel(...) native
function ActorValueInfo_SetSkillLegendaryLevel() global
	int session = Costile2.GetSession("ActorValueInfo_SetSkillLegendaryLevel")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	int cdcg_level = Costile2.GetInt(session, 1)
	this.SetSkillLegendaryLevel(cdcg_level)
endFunction

; void ActorValueInfo::GetPerkTree(...) native
function ActorValueInfo_GetPerkTree() global
	int session = Costile2.GetSession("ActorValueInfo_GetPerkTree")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	formlist cdcg_list = Costile2.GetTESForm(session, 1) as formlist
	actor cdcg_akActor = Costile2.GetTESForm(session, 2) as actor
	bool cdcg_unowned = Costile2.GetBool(session, 3)
	bool cdcg_allRanks = Costile2.GetBool(session, 4)
	this.GetPerkTree(cdcg_list, cdcg_akActor, cdcg_unowned, cdcg_allRanks)
endFunction

; float ActorValueInfo::GetCurrentValue(...) native
float function ActorValueInfo_GetCurrentValue() global
	int session = Costile2.GetSession("ActorValueInfo_GetCurrentValue")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	actor cdcg_akActor = Costile2.GetTESForm(session, 1) as actor
	return this.GetCurrentValue(cdcg_akActor)
endFunction

; float ActorValueInfo::GetBaseValue(...) native
float function ActorValueInfo_GetBaseValue() global
	int session = Costile2.GetSession("ActorValueInfo_GetBaseValue")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	actor cdcg_akActor = Costile2.GetTESForm(session, 1) as actor
	return this.GetBaseValue(cdcg_akActor)
endFunction

; float ActorValueInfo::GetMaximumValue(...) native
float function ActorValueInfo_GetMaximumValue() global
	int session = Costile2.GetSession("ActorValueInfo_GetMaximumValue")
	ActorValueInfo this = Costile2.GetTESForm(session, 0) as ActorValueInfo
	actor cdcg_akActor = Costile2.GetTESForm(session, 1) as actor
	return this.GetMaximumValue(cdcg_akActor)
endFunction

