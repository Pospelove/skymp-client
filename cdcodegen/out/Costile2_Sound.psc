Scriptname Costile2_Sound Hidden

;
;
;
; script Sound.psc
;
;
;

; int Sound::Play(...) native
int function Sound_Play() global
	int session = Costile2.GetSession("Sound_Play")
	Sound this = Costile2.GetTESForm(session, 0) as Sound
	objectreference cdcg_akSource = Costile2.GetTESForm(session, 1) as objectreference
	return this.Play(cdcg_akSource)
endFunction

; bool Sound::PlayAndWait(...) native
bool function Sound_PlayAndWait() global
	int session = Costile2.GetSession("Sound_PlayAndWait")
	Sound this = Costile2.GetTESForm(session, 0) as Sound
	objectreference cdcg_akSource = Costile2.GetTESForm(session, 1) as objectreference
	return this.PlayAndWait(cdcg_akSource)
endFunction

; void Sound::StopInstance(...) native global
function Sound_StopInstance() global
	int session = Costile2.GetSession("Sound_StopInstance")
	int cdcg_aiPlaybackInstance = Costile2.GetInt(session, 0)
	Sound.StopInstance(cdcg_aiPlaybackInstance)
endFunction

; void Sound::SetInstanceVolume(...) native global
function Sound_SetInstanceVolume() global
	int session = Costile2.GetSession("Sound_SetInstanceVolume")
	int cdcg_aiPlaybackInstance = Costile2.GetInt(session, 0)
	float cdcg_afVolume = Costile2.GetFloat(session, 1)
	Sound.SetInstanceVolume(cdcg_aiPlaybackInstance, cdcg_afVolume)
endFunction

; sounddescriptor Sound::GetDescriptor(...) native
sounddescriptor function Sound_GetDescriptor() global
	int session = Costile2.GetSession("Sound_GetDescriptor")
	Sound this = Costile2.GetTESForm(session, 0) as Sound
	return this.GetDescriptor()
endFunction

