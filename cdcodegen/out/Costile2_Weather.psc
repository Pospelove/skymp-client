Scriptname Costile2_Weather Hidden

;
;
;
; script Weather.psc
;
;
;

; void Weather::ReleaseOverride(...) native global
function Weather_ReleaseOverride() global
	int session = Costile2.GetSession("Weather_ReleaseOverride")
	Weather.ReleaseOverride()
endFunction

; void Weather::ForceActive(...) native
function Weather_ForceActive() global
	int session = Costile2.GetSession("Weather_ForceActive")
	Weather this = Costile2.GetTESForm(session, 0) as Weather
	bool cdcg_abOverride = Costile2.GetBool(session, 1)
	this.ForceActive(cdcg_abOverride)
endFunction

; void Weather::SetActive(...) native
function Weather_SetActive() global
	int session = Costile2.GetSession("Weather_SetActive")
	Weather this = Costile2.GetTESForm(session, 0) as Weather
	bool cdcg_abOverride = Costile2.GetBool(session, 1)
	bool cdcg_abAccelerate = Costile2.GetBool(session, 2)
	this.SetActive(cdcg_abOverride, cdcg_abAccelerate)
endFunction

; weather Weather::FindWeather(...) native global
weather function Weather_FindWeather() global
	int session = Costile2.GetSession("Weather_FindWeather")
	int cdcg_auiType = Costile2.GetInt(session, 0)
	return Weather.FindWeather(cdcg_auiType)
endFunction

; int Weather::GetClassification(...) native
int function Weather_GetClassification() global
	int session = Costile2.GetSession("Weather_GetClassification")
	Weather this = Costile2.GetTESForm(session, 0) as Weather
	return this.GetClassification()
endFunction

; weather Weather::GetCurrentWeather(...) native global
weather function Weather_GetCurrentWeather() global
	int session = Costile2.GetSession("Weather_GetCurrentWeather")
	return Weather.GetCurrentWeather()
endFunction

; weather Weather::GetOutgoingWeather(...) native global
weather function Weather_GetOutgoingWeather() global
	int session = Costile2.GetSession("Weather_GetOutgoingWeather")
	return Weather.GetOutgoingWeather()
endFunction

; float Weather::GetCurrentWeatherTransition(...) native global
float function Weather_GetCurrentWeatherTransition() global
	int session = Costile2.GetSession("Weather_GetCurrentWeatherTransition")
	return Weather.GetCurrentWeatherTransition()
endFunction

; int Weather::GetSkyMode(...) native global
int function Weather_GetSkyMode() global
	int session = Costile2.GetSession("Weather_GetSkyMode")
	return Weather.GetSkyMode()
endFunction

; float Weather::GetSunGlare(...) native
float function Weather_GetSunGlare() global
	int session = Costile2.GetSession("Weather_GetSunGlare")
	Weather this = Costile2.GetTESForm(session, 0) as Weather
	return this.GetSunGlare()
endFunction

; float Weather::GetSunDamage(...) native
float function Weather_GetSunDamage() global
	int session = Costile2.GetSession("Weather_GetSunDamage")
	Weather this = Costile2.GetTESForm(session, 0) as Weather
	return this.GetSunDamage()
endFunction

; float Weather::GetWindDirection(...) native
float function Weather_GetWindDirection() global
	int session = Costile2.GetSession("Weather_GetWindDirection")
	Weather this = Costile2.GetTESForm(session, 0) as Weather
	return this.GetWindDirection()
endFunction

; float Weather::GetWindDirectionRange(...) native
float function Weather_GetWindDirectionRange() global
	int session = Costile2.GetSession("Weather_GetWindDirectionRange")
	Weather this = Costile2.GetTESForm(session, 0) as Weather
	return this.GetWindDirectionRange()
endFunction

; float Weather::GetFogDistance(...) native
float function Weather_GetFogDistance() global
	int session = Costile2.GetSession("Weather_GetFogDistance")
	Weather this = Costile2.GetTESForm(session, 0) as Weather
	bool cdcg_day = Costile2.GetBool(session, 1)
	int cdcg_type = Costile2.GetInt(session, 2)
	return this.GetFogDistance(cdcg_day, cdcg_type)
endFunction

