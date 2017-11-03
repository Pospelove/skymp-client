Scriptname Costile2_ColorForm Hidden

;
;
;
; script ColorForm.psc
;
;
;

; int ColorForm::GetColor(...) native
int function ColorForm_GetColor() global
	int session = Costile2.GetSession("ColorForm_GetColor")
	ColorForm this = Costile2.GetTESForm(session, 0) as ColorForm
	return this.GetColor()
endFunction

; void ColorForm::SetColor(...) native
function ColorForm_SetColor() global
	int session = Costile2.GetSession("ColorForm_SetColor")
	ColorForm this = Costile2.GetTESForm(session, 0) as ColorForm
	int cdcg_color = Costile2.GetInt(session, 1)
	this.SetColor(cdcg_color)
endFunction

; int ColorForm::GetRed(...)
int function ColorForm_GetRed() global
	int session = Costile2.GetSession("ColorForm_GetRed")
	ColorForm this = Costile2.GetTESForm(session, 0) as ColorForm
	return this.GetRed()
endFunction

; int ColorForm::GetGreen(...)
int function ColorForm_GetGreen() global
	int session = Costile2.GetSession("ColorForm_GetGreen")
	ColorForm this = Costile2.GetTESForm(session, 0) as ColorForm
	return this.GetGreen()
endFunction

; int ColorForm::GetBlue(...)
int function ColorForm_GetBlue() global
	int session = Costile2.GetSession("ColorForm_GetBlue")
	ColorForm this = Costile2.GetTESForm(session, 0) as ColorForm
	return this.GetBlue()
endFunction

; float ColorForm::GetHue(...)
float function ColorForm_GetHue() global
	int session = Costile2.GetSession("ColorForm_GetHue")
	ColorForm this = Costile2.GetTESForm(session, 0) as ColorForm
	return this.GetHue()
endFunction

; float ColorForm::GetSaturation(...)
float function ColorForm_GetSaturation() global
	int session = Costile2.GetSession("ColorForm_GetSaturation")
	ColorForm this = Costile2.GetTESForm(session, 0) as ColorForm
	return this.GetSaturation()
endFunction

; float ColorForm::GetValue(...)
float function ColorForm_GetValue() global
	int session = Costile2.GetSession("ColorForm_GetValue")
	ColorForm this = Costile2.GetTESForm(session, 0) as ColorForm
	return this.GetValue()
endFunction

