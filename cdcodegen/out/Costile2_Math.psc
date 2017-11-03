Scriptname Costile2_Math Hidden

;
;
;
; script Math.psc
;
;
;

; float Math::abs(...) native global
float function Math_abs() global
	int session = Costile2.GetSession("Math_abs")
	float cdcg_afValue = Costile2.GetFloat(session, 0)
	return Math.abs(cdcg_afValue)
endFunction

; float Math::acos(...) native global
float function Math_acos() global
	int session = Costile2.GetSession("Math_acos")
	float cdcg_afValue = Costile2.GetFloat(session, 0)
	return Math.acos(cdcg_afValue)
endFunction

; float Math::asin(...) native global
float function Math_asin() global
	int session = Costile2.GetSession("Math_asin")
	float cdcg_afValue = Costile2.GetFloat(session, 0)
	return Math.asin(cdcg_afValue)
endFunction

; float Math::atan(...) native global
float function Math_atan() global
	int session = Costile2.GetSession("Math_atan")
	float cdcg_afValue = Costile2.GetFloat(session, 0)
	return Math.atan(cdcg_afValue)
endFunction

; int Math::Ceiling(...) native global
int function Math_Ceiling() global
	int session = Costile2.GetSession("Math_Ceiling")
	float cdcg_afValue = Costile2.GetFloat(session, 0)
	return Math.Ceiling(cdcg_afValue)
endFunction

; float Math::cos(...) native global
float function Math_cos() global
	int session = Costile2.GetSession("Math_cos")
	float cdcg_afValue = Costile2.GetFloat(session, 0)
	return Math.cos(cdcg_afValue)
endFunction

; float Math::DegreesToRadians(...) native global
float function Math_DegreesToRadians() global
	int session = Costile2.GetSession("Math_DegreesToRadians")
	float cdcg_afDegrees = Costile2.GetFloat(session, 0)
	return Math.DegreesToRadians(cdcg_afDegrees)
endFunction

; int Math::Floor(...) native global
int function Math_Floor() global
	int session = Costile2.GetSession("Math_Floor")
	float cdcg_afValue = Costile2.GetFloat(session, 0)
	return Math.Floor(cdcg_afValue)
endFunction

; float Math::pow(...) native global
float function Math_pow() global
	int session = Costile2.GetSession("Math_pow")
	float cdcg_x = Costile2.GetFloat(session, 0)
	float cdcg_y = Costile2.GetFloat(session, 1)
	return Math.pow(cdcg_x, cdcg_y)
endFunction

; float Math::RadiansToDegrees(...) native global
float function Math_RadiansToDegrees() global
	int session = Costile2.GetSession("Math_RadiansToDegrees")
	float cdcg_afRadians = Costile2.GetFloat(session, 0)
	return Math.RadiansToDegrees(cdcg_afRadians)
endFunction

; float Math::sin(...) native global
float function Math_sin() global
	int session = Costile2.GetSession("Math_sin")
	float cdcg_afValue = Costile2.GetFloat(session, 0)
	return Math.sin(cdcg_afValue)
endFunction

; float Math::sqrt(...) native global
float function Math_sqrt() global
	int session = Costile2.GetSession("Math_sqrt")
	float cdcg_afValue = Costile2.GetFloat(session, 0)
	return Math.sqrt(cdcg_afValue)
endFunction

; float Math::tan(...) native global
float function Math_tan() global
	int session = Costile2.GetSession("Math_tan")
	float cdcg_afValue = Costile2.GetFloat(session, 0)
	return Math.tan(cdcg_afValue)
endFunction

; int Math::LeftShift(...) native global
int function Math_LeftShift() global
	int session = Costile2.GetSession("Math_LeftShift")
	int cdcg_value = Costile2.GetInt(session, 0)
	int cdcg_shiftBy = Costile2.GetInt(session, 1)
	return Math.LeftShift(cdcg_value, cdcg_shiftBy)
endFunction

; int Math::RightShift(...) native global
int function Math_RightShift() global
	int session = Costile2.GetSession("Math_RightShift")
	int cdcg_value = Costile2.GetInt(session, 0)
	int cdcg_shiftBy = Costile2.GetInt(session, 1)
	return Math.RightShift(cdcg_value, cdcg_shiftBy)
endFunction

; int Math::LogicalAnd(...) native global
int function Math_LogicalAnd() global
	int session = Costile2.GetSession("Math_LogicalAnd")
	int cdcg_arg1 = Costile2.GetInt(session, 0)
	int cdcg_arg2 = Costile2.GetInt(session, 1)
	return Math.LogicalAnd(cdcg_arg1, cdcg_arg2)
endFunction

; int Math::LogicalOr(...) native global
int function Math_LogicalOr() global
	int session = Costile2.GetSession("Math_LogicalOr")
	int cdcg_arg1 = Costile2.GetInt(session, 0)
	int cdcg_arg2 = Costile2.GetInt(session, 1)
	return Math.LogicalOr(cdcg_arg1, cdcg_arg2)
endFunction

; int Math::LogicalXor(...) native global
int function Math_LogicalXor() global
	int session = Costile2.GetSession("Math_LogicalXor")
	int cdcg_arg1 = Costile2.GetInt(session, 0)
	int cdcg_arg2 = Costile2.GetInt(session, 1)
	return Math.LogicalXor(cdcg_arg1, cdcg_arg2)
endFunction

; int Math::LogicalNot(...) native global
int function Math_LogicalNot() global
	int session = Costile2.GetSession("Math_LogicalNot")
	int cdcg_arg1 = Costile2.GetInt(session, 0)
	return Math.LogicalNot(cdcg_arg1)
endFunction

; float Math::Log(...) native global
float function Math_Log() global
	int session = Costile2.GetSession("Math_Log")
	float cdcg_arg1 = Costile2.GetFloat(session, 0)
	return Math.Log(cdcg_arg1)
endFunction

