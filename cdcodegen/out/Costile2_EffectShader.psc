Scriptname Costile2_EffectShader Hidden

;
;
;
; script EffectShader.psc
;
;
;

; void EffectShader::Play(...) native
function EffectShader_Play() global
	int session = Costile2.GetSession("EffectShader_Play")
	EffectShader this = Costile2.GetTESForm(session, 0) as EffectShader
	objectreference cdcg_akObject = Costile2.GetTESForm(session, 1) as objectreference
	float cdcg_afDuration = Costile2.GetFloat(session, 2)
	this.Play(cdcg_akObject, cdcg_afDuration)
endFunction

; void EffectShader::Stop(...) native
function EffectShader_Stop() global
	int session = Costile2.GetSession("EffectShader_Stop")
	EffectShader this = Costile2.GetTESForm(session, 0) as EffectShader
	objectreference cdcg_akObject = Costile2.GetTESForm(session, 1) as objectreference
	this.Stop(cdcg_akObject)
endFunction

