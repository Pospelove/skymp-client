Scriptname Costile2_Spell Hidden

;
;
;
; script Spell.psc
;
;
;

; void Spell::Cast(...) native
function Spell_Cast() global
	int session = Costile2.GetSession("Spell_Cast")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	objectreference cdcg_akSource = Costile2.GetTESForm(session, 1) as objectreference
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 2) as objectreference
	this.Cast(cdcg_akSource, cdcg_akTarget)
endFunction

; void Spell::RemoteCast(...) native
function Spell_RemoteCast() global
	int session = Costile2.GetSession("Spell_RemoteCast")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	objectreference cdcg_akSource = Costile2.GetTESForm(session, 1) as objectreference
	actor cdcg_akBlameActor = Costile2.GetTESForm(session, 2) as actor
	objectreference cdcg_akTarget = Costile2.GetTESForm(session, 3) as objectreference
	this.RemoteCast(cdcg_akSource, cdcg_akBlameActor, cdcg_akTarget)
endFunction

; bool Spell::IsHostile(...) native
bool function Spell_IsHostile() global
	int session = Costile2.GetSession("Spell_IsHostile")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	return this.IsHostile()
endFunction

; void Spell::Preload(...) native
function Spell_Preload() global
	int session = Costile2.GetSession("Spell_Preload")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	this.Preload()
endFunction

; void Spell::Unload(...) native
function Spell_Unload() global
	int session = Costile2.GetSession("Spell_Unload")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	this.Unload()
endFunction

; float Spell::GetCastTime(...) native
float function Spell_GetCastTime() global
	int session = Costile2.GetSession("Spell_GetCastTime")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	return this.GetCastTime()
endFunction

; perk Spell::GetPerk(...) native
perk function Spell_GetPerk() global
	int session = Costile2.GetSession("Spell_GetPerk")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	return this.GetPerk()
endFunction

; int Spell::GetNumEffects(...) native
int function Spell_GetNumEffects() global
	int session = Costile2.GetSession("Spell_GetNumEffects")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	return this.GetNumEffects()
endFunction

; float Spell::GetNthEffectMagnitude(...) native
float function Spell_GetNthEffectMagnitude() global
	int session = Costile2.GetSession("Spell_GetNthEffectMagnitude")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectMagnitude(cdcg_index)
endFunction

; int Spell::GetNthEffectArea(...) native
int function Spell_GetNthEffectArea() global
	int session = Costile2.GetSession("Spell_GetNthEffectArea")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectArea(cdcg_index)
endFunction

; int Spell::GetNthEffectDuration(...) native
int function Spell_GetNthEffectDuration() global
	int session = Costile2.GetSession("Spell_GetNthEffectDuration")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectDuration(cdcg_index)
endFunction

; magiceffect Spell::GetNthEffectMagicEffect(...) native
magiceffect function Spell_GetNthEffectMagicEffect() global
	int session = Costile2.GetSession("Spell_GetNthEffectMagicEffect")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetNthEffectMagicEffect(cdcg_index)
endFunction

; int Spell::GetCostliestEffectIndex(...) native
int function Spell_GetCostliestEffectIndex() global
	int session = Costile2.GetSession("Spell_GetCostliestEffectIndex")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	return this.GetCostliestEffectIndex()
endFunction

; int Spell::GetMagickaCost(...) native
int function Spell_GetMagickaCost() global
	int session = Costile2.GetSession("Spell_GetMagickaCost")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	return this.GetMagickaCost()
endFunction

; int Spell::GetEffectiveMagickaCost(...) native
int function Spell_GetEffectiveMagickaCost() global
	int session = Costile2.GetSession("Spell_GetEffectiveMagickaCost")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	actor cdcg_caster = Costile2.GetTESForm(session, 1) as actor
	return this.GetEffectiveMagickaCost(cdcg_caster)
endFunction

; void Spell::SetNthEffectMagnitude(...) native
function Spell_SetNthEffectMagnitude() global
	int session = Costile2.GetSession("Spell_SetNthEffectMagnitude")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	int cdcg_index = Costile2.GetInt(session, 1)
	float cdcg_value = Costile2.GetFloat(session, 2)
	this.SetNthEffectMagnitude(cdcg_index, cdcg_value)
endFunction

; void Spell::SetNthEffectArea(...) native
function Spell_SetNthEffectArea() global
	int session = Costile2.GetSession("Spell_SetNthEffectArea")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	int cdcg_index = Costile2.GetInt(session, 1)
	int cdcg_value = Costile2.GetInt(session, 2)
	this.SetNthEffectArea(cdcg_index, cdcg_value)
endFunction

; void Spell::SetNthEffectDuration(...) native
function Spell_SetNthEffectDuration() global
	int session = Costile2.GetSession("Spell_SetNthEffectDuration")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	int cdcg_index = Costile2.GetInt(session, 1)
	int cdcg_value = Costile2.GetInt(session, 2)
	this.SetNthEffectDuration(cdcg_index, cdcg_value)
endFunction

; equipslot Spell::GetEquipType(...) native
equipslot function Spell_GetEquipType() global
	int session = Costile2.GetSession("Spell_GetEquipType")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	return this.GetEquipType()
endFunction

; void Spell::SetEquipType(...) native
function Spell_SetEquipType() global
	int session = Costile2.GetSession("Spell_SetEquipType")
	Spell this = Costile2.GetTESForm(session, 0) as Spell
	equipslot cdcg_type = Costile2.GetTESForm(session, 1) as equipslot
	this.SetEquipType(cdcg_type)
endFunction

