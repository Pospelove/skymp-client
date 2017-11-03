Scriptname Costile2_ImageSpaceModifier Hidden

;
;
;
; script ImageSpaceModifier.psc
;
;
;

; void ImageSpaceModifier::Apply(...) native
function ImageSpaceModifier_Apply() global
	int session = Costile2.GetSession("ImageSpaceModifier_Apply")
	ImageSpaceModifier this = Costile2.GetTESForm(session, 0) as ImageSpaceModifier
	float cdcg_afStrength = Costile2.GetFloat(session, 1)
	this.Apply(cdcg_afStrength)
endFunction

; void ImageSpaceModifier::ApplyCrossFade(...) native
function ImageSpaceModifier_ApplyCrossFade() global
	int session = Costile2.GetSession("ImageSpaceModifier_ApplyCrossFade")
	ImageSpaceModifier this = Costile2.GetTESForm(session, 0) as ImageSpaceModifier
	float cdcg_afFadeDuration = Costile2.GetFloat(session, 1)
	this.ApplyCrossFade(cdcg_afFadeDuration)
endFunction

; void ImageSpaceModifier::PopTo(...) native
function ImageSpaceModifier_PopTo() global
	int session = Costile2.GetSession("ImageSpaceModifier_PopTo")
	ImageSpaceModifier this = Costile2.GetTESForm(session, 0) as ImageSpaceModifier
	imagespacemodifier cdcg_akNewModifier = Costile2.GetTESForm(session, 1) as imagespacemodifier
	float cdcg_afStrength = Costile2.GetFloat(session, 2)
	this.PopTo(cdcg_akNewModifier, cdcg_afStrength)
endFunction

; void ImageSpaceModifier::Remove(...) native
function ImageSpaceModifier_Remove() global
	int session = Costile2.GetSession("ImageSpaceModifier_Remove")
	ImageSpaceModifier this = Costile2.GetTESForm(session, 0) as ImageSpaceModifier
	this.Remove()
endFunction

; void ImageSpaceModifier::RemoveCrossFade(...) native global
function ImageSpaceModifier_RemoveCrossFade() global
	int session = Costile2.GetSession("ImageSpaceModifier_RemoveCrossFade")
	float cdcg_afFadeDuration = Costile2.GetFloat(session, 0)
	ImageSpaceModifier.RemoveCrossFade(cdcg_afFadeDuration)
endFunction

