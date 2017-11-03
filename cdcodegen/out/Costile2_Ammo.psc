Scriptname Costile2_Ammo Hidden

;
;
;
; script Ammo.psc
;
;
;

; bool Ammo::IsBolt(...) native
bool function Ammo_IsBolt() global
	int session = Costile2.GetSession("Ammo_IsBolt")
	Ammo this = Costile2.GetTESForm(session, 0) as Ammo
	return this.IsBolt()
endFunction

; projectile Ammo::GetProjectile(...) native
projectile function Ammo_GetProjectile() global
	int session = Costile2.GetSession("Ammo_GetProjectile")
	Ammo this = Costile2.GetTESForm(session, 0) as Ammo
	return this.GetProjectile()
endFunction

; float Ammo::GetDamage(...) native
float function Ammo_GetDamage() global
	int session = Costile2.GetSession("Ammo_GetDamage")
	Ammo this = Costile2.GetTESForm(session, 0) as Ammo
	return this.GetDamage()
endFunction

