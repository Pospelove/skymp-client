Scriptname Costile2_Utility Hidden

;
;
;
; script Utility.psc
;
;
;

; string Utility::GameTimeToString(...) native global
string function Utility_GameTimeToString() global
	int session = Costile2.GetSession("Utility_GameTimeToString")
	float cdcg_afGameTime = Costile2.GetFloat(session, 0)
	return Utility.GameTimeToString(cdcg_afGameTime)
endFunction

; float Utility::GetCurrentGameTime(...) native global
float function Utility_GetCurrentGameTime() global
	int session = Costile2.GetSession("Utility_GetCurrentGameTime")
	return Utility.GetCurrentGameTime()
endFunction

; float Utility::GetCurrentRealTime(...) native global
float function Utility_GetCurrentRealTime() global
	int session = Costile2.GetSession("Utility_GetCurrentRealTime")
	return Utility.GetCurrentRealTime()
endFunction

; bool Utility::IsInMenuMode(...) native global
bool function Utility_IsInMenuMode() global
	int session = Costile2.GetSession("Utility_IsInMenuMode")
	return Utility.IsInMenuMode()
endFunction

; int Utility::RandomInt(...) native global
int function Utility_RandomInt() global
	int session = Costile2.GetSession("Utility_RandomInt")
	int cdcg_aiMin = Costile2.GetInt(session, 0)
	int cdcg_aiMax = Costile2.GetInt(session, 1)
	return Utility.RandomInt(cdcg_aiMin, cdcg_aiMax)
endFunction

; float Utility::RandomFloat(...) native global
float function Utility_RandomFloat() global
	int session = Costile2.GetSession("Utility_RandomFloat")
	float cdcg_afMin = Costile2.GetFloat(session, 0)
	float cdcg_afMax = Costile2.GetFloat(session, 1)
	return Utility.RandomFloat(cdcg_afMin, cdcg_afMax)
endFunction

; void Utility::SetINIFloat(...) native global
function Utility_SetINIFloat() global
	int session = Costile2.GetSession("Utility_SetINIFloat")
	string cdcg_ini = Costile2.GetString(session, 0)
	float cdcg_value = Costile2.GetFloat(session, 1)
	Utility.SetINIFloat(cdcg_ini, cdcg_value)
endFunction

; void Utility::SetINIInt(...) native global
function Utility_SetINIInt() global
	int session = Costile2.GetSession("Utility_SetINIInt")
	string cdcg_ini = Costile2.GetString(session, 0)
	int cdcg_value = Costile2.GetInt(session, 1)
	Utility.SetINIInt(cdcg_ini, cdcg_value)
endFunction

; void Utility::SetINIBool(...) native global
function Utility_SetINIBool() global
	int session = Costile2.GetSession("Utility_SetINIBool")
	string cdcg_ini = Costile2.GetString(session, 0)
	bool cdcg_value = Costile2.GetBool(session, 1)
	Utility.SetINIBool(cdcg_ini, cdcg_value)
endFunction

; void Utility::SetINIString(...) native global
function Utility_SetINIString() global
	int session = Costile2.GetSession("Utility_SetINIString")
	string cdcg_ini = Costile2.GetString(session, 0)
	string cdcg_value = Costile2.GetString(session, 1)
	Utility.SetINIString(cdcg_ini, cdcg_value)
endFunction

; void Utility::Wait(...) native global
function Utility_Wait() global
	int session = Costile2.GetSession("Utility_Wait")
	float cdcg_afSeconds = Costile2.GetFloat(session, 0)
	Utility.Wait(cdcg_afSeconds)
endFunction

; void Utility::WaitGameTime(...) native global
function Utility_WaitGameTime() global
	int session = Costile2.GetSession("Utility_WaitGameTime")
	float cdcg_afHours = Costile2.GetFloat(session, 0)
	Utility.WaitGameTime(cdcg_afHours)
endFunction

; void Utility::WaitMenuMode(...) native global
function Utility_WaitMenuMode() global
	int session = Costile2.GetSession("Utility_WaitMenuMode")
	float cdcg_afSeconds = Costile2.GetFloat(session, 0)
	Utility.WaitMenuMode(cdcg_afSeconds)
endFunction

; string Utility::CaptureFrameRate(...) native global
string function Utility_CaptureFrameRate() global
	int session = Costile2.GetSession("Utility_CaptureFrameRate")
	int cdcg_numFrames = Costile2.GetInt(session, 0)
	return Utility.CaptureFrameRate(cdcg_numFrames)
endFunction

; void Utility::StartFrameRateCapture(...) native global
function Utility_StartFrameRateCapture() global
	int session = Costile2.GetSession("Utility_StartFrameRateCapture")
	Utility.StartFrameRateCapture()
endFunction

; void Utility::EndFrameRateCapture(...) native global
function Utility_EndFrameRateCapture() global
	int session = Costile2.GetSession("Utility_EndFrameRateCapture")
	Utility.EndFrameRateCapture()
endFunction

; float Utility::GetAverageFrameRate(...) native global
float function Utility_GetAverageFrameRate() global
	int session = Costile2.GetSession("Utility_GetAverageFrameRate")
	return Utility.GetAverageFrameRate()
endFunction

; float Utility::GetMinFrameRate(...) native global
float function Utility_GetMinFrameRate() global
	int session = Costile2.GetSession("Utility_GetMinFrameRate")
	return Utility.GetMinFrameRate()
endFunction

; float Utility::GetMaxFrameRate(...) native global
float function Utility_GetMaxFrameRate() global
	int session = Costile2.GetSession("Utility_GetMaxFrameRate")
	return Utility.GetMaxFrameRate()
endFunction

; int Utility::GetCurrentMemory(...) native global
int function Utility_GetCurrentMemory() global
	int session = Costile2.GetSession("Utility_GetCurrentMemory")
	return Utility.GetCurrentMemory()
endFunction

; int Utility::GetBudgetCount(...) native global
int function Utility_GetBudgetCount() global
	int session = Costile2.GetSession("Utility_GetBudgetCount")
	return Utility.GetBudgetCount()
endFunction

; int Utility::GetCurrentBudget(...) native global
int function Utility_GetCurrentBudget() global
	int session = Costile2.GetSession("Utility_GetCurrentBudget")
	int cdcg_aiBudgetNumber = Costile2.GetInt(session, 0)
	return Utility.GetCurrentBudget(cdcg_aiBudgetNumber)
endFunction

; bool Utility::OverBudget(...) native global
bool function Utility_OverBudget() global
	int session = Costile2.GetSession("Utility_OverBudget")
	int cdcg_aiBudgetNumber = Costile2.GetInt(session, 0)
	return Utility.OverBudget(cdcg_aiBudgetNumber)
endFunction

; string Utility::GetBudgetName(...) native global
string function Utility_GetBudgetName() global
	int session = Costile2.GetSession("Utility_GetBudgetName")
	int cdcg_aiBudgetNumber = Costile2.GetInt(session, 0)
	return Utility.GetBudgetName(cdcg_aiBudgetNumber)
endFunction

; float Utility::GetINIFloat(...) native global
float function Utility_GetINIFloat() global
	int session = Costile2.GetSession("Utility_GetINIFloat")
	string cdcg_ini = Costile2.GetString(session, 0)
	return Utility.GetINIFloat(cdcg_ini)
endFunction

; int Utility::GetINIInt(...) native global
int function Utility_GetINIInt() global
	int session = Costile2.GetSession("Utility_GetINIInt")
	string cdcg_ini = Costile2.GetString(session, 0)
	return Utility.GetINIInt(cdcg_ini)
endFunction

; bool Utility::GetINIBool(...) native global
bool function Utility_GetINIBool() global
	int session = Costile2.GetSession("Utility_GetINIBool")
	string cdcg_ini = Costile2.GetString(session, 0)
	return Utility.GetINIBool(cdcg_ini)
endFunction

; string Utility::GetINIString(...) native global
string function Utility_GetINIString() global
	int session = Costile2.GetSession("Utility_GetINIString")
	string cdcg_ini = Costile2.GetString(session, 0)
	return Utility.GetINIString(cdcg_ini)
endFunction

