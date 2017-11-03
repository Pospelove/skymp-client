Scriptname Costile2_Perk Hidden

;
;
;
; script Perk.psc
;
;
;

; perk Perk::GetNextPerk(...) native
perk function Perk_GetNextPerk() global
	int session = Costile2.GetSession("Perk_GetNextPerk")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	return this.GetNextPerk()
endFunction

; int Perk::GetNumEntries(...) native
int function Perk_GetNumEntries() global
	int session = Costile2.GetSession("Perk_GetNumEntries")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	return this.GetNumEntries()
endFunction

; int Perk::GetNthEntryRank(...) native
int function Perk_GetNthEntryRank() global
	int session = Costile2.GetSession("Perk_GetNthEntryRank")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthEntryRank(cdcg_n)
endFunction

; bool Perk::SetNthEntryRank(...) native
bool function Perk_SetNthEntryRank() global
	int session = Costile2.GetSession("Perk_SetNthEntryRank")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	int cdcg_rank = Costile2.GetInt(session, 2)
	return this.SetNthEntryRank(cdcg_n, cdcg_rank)
endFunction

; int Perk::GetNthEntryPriority(...) native
int function Perk_GetNthEntryPriority() global
	int session = Costile2.GetSession("Perk_GetNthEntryPriority")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthEntryPriority(cdcg_n)
endFunction

; bool Perk::SetNthEntryPriority(...) native
bool function Perk_SetNthEntryPriority() global
	int session = Costile2.GetSession("Perk_SetNthEntryPriority")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	int cdcg_priority = Costile2.GetInt(session, 2)
	return this.SetNthEntryPriority(cdcg_n, cdcg_priority)
endFunction

; quest Perk::GetNthEntryQuest(...) native
quest function Perk_GetNthEntryQuest() global
	int session = Costile2.GetSession("Perk_GetNthEntryQuest")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthEntryQuest(cdcg_n)
endFunction

; bool Perk::SetNthEntryQuest(...) native
bool function Perk_SetNthEntryQuest() global
	int session = Costile2.GetSession("Perk_SetNthEntryQuest")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	quest cdcg_newQuest = Costile2.GetTESForm(session, 2) as quest
	return this.SetNthEntryQuest(cdcg_n, cdcg_newQuest)
endFunction

; int Perk::GetNthEntryStage(...) native
int function Perk_GetNthEntryStage() global
	int session = Costile2.GetSession("Perk_GetNthEntryStage")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthEntryStage(cdcg_n)
endFunction

; bool Perk::SetNthEntryStage(...) native
bool function Perk_SetNthEntryStage() global
	int session = Costile2.GetSession("Perk_SetNthEntryStage")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	int cdcg_stage = Costile2.GetInt(session, 2)
	return this.SetNthEntryStage(cdcg_n, cdcg_stage)
endFunction

; spell Perk::GetNthEntrySpell(...) native
spell function Perk_GetNthEntrySpell() global
	int session = Costile2.GetSession("Perk_GetNthEntrySpell")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthEntrySpell(cdcg_n)
endFunction

; bool Perk::SetNthEntrySpell(...) native
bool function Perk_SetNthEntrySpell() global
	int session = Costile2.GetSession("Perk_SetNthEntrySpell")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	spell cdcg_newSpell = Costile2.GetTESForm(session, 2) as spell
	return this.SetNthEntrySpell(cdcg_n, cdcg_newSpell)
endFunction

; leveleditem Perk::GetNthEntryLeveledList(...) native
leveleditem function Perk_GetNthEntryLeveledList() global
	int session = Costile2.GetSession("Perk_GetNthEntryLeveledList")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthEntryLeveledList(cdcg_n)
endFunction

; bool Perk::SetNthEntryLeveledList(...) native
bool function Perk_SetNthEntryLeveledList() global
	int session = Costile2.GetSession("Perk_SetNthEntryLeveledList")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	leveleditem cdcg_lList = Costile2.GetTESForm(session, 2) as leveleditem
	return this.SetNthEntryLeveledList(cdcg_n, cdcg_lList)
endFunction

; string Perk::GetNthEntryText(...) native
string function Perk_GetNthEntryText() global
	int session = Costile2.GetSession("Perk_GetNthEntryText")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthEntryText(cdcg_n)
endFunction

; bool Perk::SetNthEntryText(...) native
bool function Perk_SetNthEntryText() global
	int session = Costile2.GetSession("Perk_SetNthEntryText")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	string cdcg_newText = Costile2.GetString(session, 2)
	return this.SetNthEntryText(cdcg_n, cdcg_newText)
endFunction

; float Perk::GetNthEntryValue(...) native
float function Perk_GetNthEntryValue() global
	int session = Costile2.GetSession("Perk_GetNthEntryValue")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	int cdcg_i = Costile2.GetInt(session, 2)
	return this.GetNthEntryValue(cdcg_n, cdcg_i)
endFunction

; bool Perk::SetNthEntryValue(...) native
bool function Perk_SetNthEntryValue() global
	int session = Costile2.GetSession("Perk_SetNthEntryValue")
	Perk this = Costile2.GetTESForm(session, 0) as Perk
	int cdcg_n = Costile2.GetInt(session, 1)
	int cdcg_i = Costile2.GetInt(session, 2)
	float cdcg_value = Costile2.GetFloat(session, 3)
	return this.SetNthEntryValue(cdcg_n, cdcg_i, cdcg_value)
endFunction

