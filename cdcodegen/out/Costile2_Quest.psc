Scriptname Costile2_Quest Hidden

;
;
;
; script Quest.psc
;
;
;

; bool Quest::ModObjectiveGlobal(...)
bool function Quest_ModObjectiveGlobal() global
	int session = Costile2.GetSession("Quest_ModObjectiveGlobal")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	float cdcg_afModValue = Costile2.GetFloat(session, 1)
	globalvariable cdcg_aModGlobal = Costile2.GetTESForm(session, 2) as globalvariable
	int cdcg_aiObjectiveID = Costile2.GetInt(session, 3)
	float cdcg_afTargetValue = Costile2.GetFloat(session, 4)
	bool cdcg_abCountingUp = Costile2.GetBool(session, 5)
	bool cdcg_abCompleteObjective = Costile2.GetBool(session, 6)
	bool cdcg_abRedisplayObjective = Costile2.GetBool(session, 7)
	return this.ModObjectiveGlobal(cdcg_afModValue, cdcg_aModGlobal, cdcg_aiObjectiveID, cdcg_afTargetValue, cdcg_abCountingUp, cdcg_abCompleteObjective, cdcg_abRedisplayObjective)
endFunction

; void Quest::CompleteAllObjectives(...) native
function Quest_CompleteAllObjectives() global
	int session = Costile2.GetSession("Quest_CompleteAllObjectives")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	this.CompleteAllObjectives()
endFunction

; void Quest::CompleteQuest(...) native
function Quest_CompleteQuest() global
	int session = Costile2.GetSession("Quest_CompleteQuest")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	this.CompleteQuest()
endFunction

; void Quest::FailAllObjectives(...) native
function Quest_FailAllObjectives() global
	int session = Costile2.GetSession("Quest_FailAllObjectives")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	this.FailAllObjectives()
endFunction

; alias Quest::GetAlias(...) native
alias function Quest_GetAlias() global
	int session = Costile2.GetSession("Quest_GetAlias")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_aiAliasID = Costile2.GetInt(session, 1)
	return this.GetAlias(cdcg_aiAliasID)
endFunction

; int Quest::GetCurrentStageID(...) native
int function Quest_GetCurrentStageID() global
	int session = Costile2.GetSession("Quest_GetCurrentStageID")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	return this.GetCurrentStageID()
endFunction

; int Quest::GetStage(...)
int function Quest_GetStage() global
	int session = Costile2.GetSession("Quest_GetStage")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	return this.GetStage()
endFunction

; bool Quest::GetStageDone(...)
bool function Quest_GetStageDone() global
	int session = Costile2.GetSession("Quest_GetStageDone")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_aiStage = Costile2.GetInt(session, 1)
	return this.GetStageDone(cdcg_aiStage)
endFunction

; bool Quest::IsActive(...) native
bool function Quest_IsActive() global
	int session = Costile2.GetSession("Quest_IsActive")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	return this.IsActive()
endFunction

; bool Quest::IsCompleted(...) native
bool function Quest_IsCompleted() global
	int session = Costile2.GetSession("Quest_IsCompleted")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	return this.IsCompleted()
endFunction

; bool Quest::IsObjectiveCompleted(...) native
bool function Quest_IsObjectiveCompleted() global
	int session = Costile2.GetSession("Quest_IsObjectiveCompleted")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_aiObjective = Costile2.GetInt(session, 1)
	return this.IsObjectiveCompleted(cdcg_aiObjective)
endFunction

; bool Quest::IsObjectiveDisplayed(...) native
bool function Quest_IsObjectiveDisplayed() global
	int session = Costile2.GetSession("Quest_IsObjectiveDisplayed")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_aiObjective = Costile2.GetInt(session, 1)
	return this.IsObjectiveDisplayed(cdcg_aiObjective)
endFunction

; bool Quest::IsObjectiveFailed(...) native
bool function Quest_IsObjectiveFailed() global
	int session = Costile2.GetSession("Quest_IsObjectiveFailed")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_aiObjective = Costile2.GetInt(session, 1)
	return this.IsObjectiveFailed(cdcg_aiObjective)
endFunction

; bool Quest::IsRunning(...) native
bool function Quest_IsRunning() global
	int session = Costile2.GetSession("Quest_IsRunning")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	return this.IsRunning()
endFunction

; bool Quest::IsStageDone(...) native
bool function Quest_IsStageDone() global
	int session = Costile2.GetSession("Quest_IsStageDone")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_aiStage = Costile2.GetInt(session, 1)
	return this.IsStageDone(cdcg_aiStage)
endFunction

; bool Quest::IsStarting(...) native
bool function Quest_IsStarting() global
	int session = Costile2.GetSession("Quest_IsStarting")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	return this.IsStarting()
endFunction

; bool Quest::IsStopping(...) native
bool function Quest_IsStopping() global
	int session = Costile2.GetSession("Quest_IsStopping")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	return this.IsStopping()
endFunction

; bool Quest::IsStopped(...) native
bool function Quest_IsStopped() global
	int session = Costile2.GetSession("Quest_IsStopped")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	return this.IsStopped()
endFunction

; void Quest::Reset(...) native
function Quest_Reset() global
	int session = Costile2.GetSession("Quest_Reset")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	this.Reset()
endFunction

; void Quest::SetActive(...) native
function Quest_SetActive() global
	int session = Costile2.GetSession("Quest_SetActive")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	bool cdcg_abActive = Costile2.GetBool(session, 1)
	this.SetActive(cdcg_abActive)
endFunction

; bool Quest::SetCurrentStageID(...) native
bool function Quest_SetCurrentStageID() global
	int session = Costile2.GetSession("Quest_SetCurrentStageID")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_aiStageID = Costile2.GetInt(session, 1)
	return this.SetCurrentStageID(cdcg_aiStageID)
endFunction

; void Quest::SetObjectiveCompleted(...) native
function Quest_SetObjectiveCompleted() global
	int session = Costile2.GetSession("Quest_SetObjectiveCompleted")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_aiObjective = Costile2.GetInt(session, 1)
	bool cdcg_abCompleted = Costile2.GetBool(session, 2)
	this.SetObjectiveCompleted(cdcg_aiObjective, cdcg_abCompleted)
endFunction

; void Quest::SetObjectiveDisplayed(...) native
function Quest_SetObjectiveDisplayed() global
	int session = Costile2.GetSession("Quest_SetObjectiveDisplayed")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_aiObjective = Costile2.GetInt(session, 1)
	bool cdcg_abDisplayed = Costile2.GetBool(session, 2)
	bool cdcg_abForce = Costile2.GetBool(session, 3)
	this.SetObjectiveDisplayed(cdcg_aiObjective, cdcg_abDisplayed, cdcg_abForce)
endFunction

; void Quest::SetObjectiveFailed(...) native
function Quest_SetObjectiveFailed() global
	int session = Costile2.GetSession("Quest_SetObjectiveFailed")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_aiObjective = Costile2.GetInt(session, 1)
	bool cdcg_abFailed = Costile2.GetBool(session, 2)
	this.SetObjectiveFailed(cdcg_aiObjective, cdcg_abFailed)
endFunction

; bool Quest::SetStage(...)
bool function Quest_SetStage() global
	int session = Costile2.GetSession("Quest_SetStage")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_aiStage = Costile2.GetInt(session, 1)
	return this.SetStage(cdcg_aiStage)
endFunction

; bool Quest::Start(...) native
bool function Quest_Start() global
	int session = Costile2.GetSession("Quest_Start")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	return this.Start()
endFunction

; void Quest::Stop(...) native
function Quest_Stop() global
	int session = Costile2.GetSession("Quest_Stop")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	this.Stop()
endFunction

; bool Quest::UpdateCurrentInstanceGlobal(...) native
bool function Quest_UpdateCurrentInstanceGlobal() global
	int session = Costile2.GetSession("Quest_UpdateCurrentInstanceGlobal")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	globalvariable cdcg_aUpdateGlobal = Costile2.GetTESForm(session, 1) as globalvariable
	return this.UpdateCurrentInstanceGlobal(cdcg_aUpdateGlobal)
endFunction

; quest Quest::GetQuest(...) native global
quest function Quest_GetQuest() global
	int session = Costile2.GetSession("Quest_GetQuest")
	string cdcg_editorId = Costile2.GetString(session, 0)
	return Quest.GetQuest(cdcg_editorId)
endFunction

; string Quest::GetID(...) native
string function Quest_GetID() global
	int session = Costile2.GetSession("Quest_GetID")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	return this.GetID()
endFunction

; int Quest::GetPriority(...) native
int function Quest_GetPriority() global
	int session = Costile2.GetSession("Quest_GetPriority")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	return this.GetPriority()
endFunction

; int Quest::GetNumAliases(...) native
int function Quest_GetNumAliases() global
	int session = Costile2.GetSession("Quest_GetNumAliases")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	return this.GetNumAliases()
endFunction

; alias Quest::GetNthAlias(...) native
alias function Quest_GetNthAlias() global
	int session = Costile2.GetSession("Quest_GetNthAlias")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthAlias(cdcg_index)
endFunction

; alias Quest::GetAliasByName(...) native
alias function Quest_GetAliasByName() global
	int session = Costile2.GetSession("Quest_GetAliasByName")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	string cdcg_name = Costile2.GetString(session, 1)
	return this.GetAliasByName(cdcg_name)
endFunction

; alias Quest::GetAliasById(...) native
alias function Quest_GetAliasById() global
	int session = Costile2.GetSession("Quest_GetAliasById")
	Quest this = Costile2.GetTESForm(session, 0) as Quest
	int cdcg_aliasId = Costile2.GetInt(session, 1)
	return this.GetAliasById(cdcg_aliasId)
endFunction

