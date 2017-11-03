Scriptname Costile2_ActorBase Hidden

;
;
;
; script ActorBase.psc
;
;
;

; class ActorBase::GetClass(...) native
class function ActorBase_GetClass() global
	int session = Costile2.GetSession("ActorBase_GetClass")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetClass()
endFunction

; int ActorBase::GetDeadCount(...) native
int function ActorBase_GetDeadCount() global
	int session = Costile2.GetSession("ActorBase_GetDeadCount")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetDeadCount()
endFunction

; formlist ActorBase::GetGiftFilter(...) native
formlist function ActorBase_GetGiftFilter() global
	int session = Costile2.GetSession("ActorBase_GetGiftFilter")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetGiftFilter()
endFunction

; race ActorBase::GetRace(...) native
race function ActorBase_GetRace() global
	int session = Costile2.GetSession("ActorBase_GetRace")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetRace()
endFunction

; int ActorBase::GetSex(...) native
int function ActorBase_GetSex() global
	int session = Costile2.GetSession("ActorBase_GetSex")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetSex()
endFunction

; bool ActorBase::IsEssential(...) native
bool function ActorBase_IsEssential() global
	int session = Costile2.GetSession("ActorBase_IsEssential")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.IsEssential()
endFunction

; bool ActorBase::IsInvulnerable(...) native
bool function ActorBase_IsInvulnerable() global
	int session = Costile2.GetSession("ActorBase_IsInvulnerable")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.IsInvulnerable()
endFunction

; bool ActorBase::IsProtected(...) native
bool function ActorBase_IsProtected() global
	int session = Costile2.GetSession("ActorBase_IsProtected")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.IsProtected()
endFunction

; bool ActorBase::IsUnique(...) native
bool function ActorBase_IsUnique() global
	int session = Costile2.GetSession("ActorBase_IsUnique")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.IsUnique()
endFunction

; void ActorBase::SetEssential(...) native
function ActorBase_SetEssential() global
	int session = Costile2.GetSession("ActorBase_SetEssential")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	bool cdcg_abEssential = Costile2.GetBool(session, 1)
	this.SetEssential(cdcg_abEssential)
endFunction

; void ActorBase::SetInvulnerable(...) native
function ActorBase_SetInvulnerable() global
	int session = Costile2.GetSession("ActorBase_SetInvulnerable")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	bool cdcg_abInvulnerable = Costile2.GetBool(session, 1)
	this.SetInvulnerable(cdcg_abInvulnerable)
endFunction

; void ActorBase::SetProtected(...) native
function ActorBase_SetProtected() global
	int session = Costile2.GetSession("ActorBase_SetProtected")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	bool cdcg_abProtected = Costile2.GetBool(session, 1)
	this.SetProtected(cdcg_abProtected)
endFunction

; void ActorBase::SetOutfit(...) native
function ActorBase_SetOutfit() global
	int session = Costile2.GetSession("ActorBase_SetOutfit")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	outfit cdcg_akOutfit = Costile2.GetTESForm(session, 1) as outfit
	bool cdcg_abSleepOutfit = Costile2.GetBool(session, 2)
	this.SetOutfit(cdcg_akOutfit, cdcg_abSleepOutfit)
endFunction

; combatstyle ActorBase::GetCombatStyle(...) native
combatstyle function ActorBase_GetCombatStyle() global
	int session = Costile2.GetSession("ActorBase_GetCombatStyle")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetCombatStyle()
endFunction

; void ActorBase::SetCombatStyle(...) native
function ActorBase_SetCombatStyle() global
	int session = Costile2.GetSession("ActorBase_SetCombatStyle")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	combatstyle cdcg_cs = Costile2.GetTESForm(session, 1) as combatstyle
	this.SetCombatStyle(cdcg_cs)
endFunction

; outfit ActorBase::GetOutfit(...) native
outfit function ActorBase_GetOutfit() global
	int session = Costile2.GetSession("ActorBase_GetOutfit")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	bool cdcg_bSleepOutfit = Costile2.GetBool(session, 1)
	return this.GetOutfit(cdcg_bSleepOutfit)
endFunction

; void ActorBase::SetClass(...) native
function ActorBase_SetClass() global
	int session = Costile2.GetSession("ActorBase_SetClass")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	class cdcg_c = Costile2.GetTESForm(session, 1) as class
	this.SetClass(cdcg_c)
endFunction

; float ActorBase::GetHeight(...) native
float function ActorBase_GetHeight() global
	int session = Costile2.GetSession("ActorBase_GetHeight")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetHeight()
endFunction

; void ActorBase::SetHeight(...) native
function ActorBase_SetHeight() global
	int session = Costile2.GetSession("ActorBase_SetHeight")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	float cdcg_height = Costile2.GetFloat(session, 1)
	this.SetHeight(cdcg_height)
endFunction

; float ActorBase::GetWeight(...) native
float function ActorBase_GetWeight() global
	int session = Costile2.GetSession("ActorBase_GetWeight")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetWeight()
endFunction

; void ActorBase::SetWeight(...) native
function ActorBase_SetWeight() global
	int session = Costile2.GetSession("ActorBase_SetWeight")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	float cdcg_weight = Costile2.GetFloat(session, 1)
	this.SetWeight(cdcg_weight)
endFunction

; int ActorBase::GetNumHeadParts(...) native
int function ActorBase_GetNumHeadParts() global
	int session = Costile2.GetSession("ActorBase_GetNumHeadParts")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetNumHeadParts()
endFunction

; headpart ActorBase::GetNthHeadPart(...) native
headpart function ActorBase_GetNthHeadPart() global
	int session = Costile2.GetSession("ActorBase_GetNthHeadPart")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	int cdcg_slotPart = Costile2.GetInt(session, 1)
	return this.GetNthHeadPart(cdcg_slotPart)
endFunction

; void ActorBase::SetNthHeadPart(...) native
function ActorBase_SetNthHeadPart() global
	int session = Costile2.GetSession("ActorBase_SetNthHeadPart")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	headpart cdcg_headPart = Costile2.GetTESForm(session, 1) as headpart
	int cdcg_slotPart = Costile2.GetInt(session, 2)
	this.SetNthHeadPart(cdcg_headPart, cdcg_slotPart)
endFunction

; int ActorBase::GetIndexOfHeadPartByType(...) native
int function ActorBase_GetIndexOfHeadPartByType() global
	int session = Costile2.GetSession("ActorBase_GetIndexOfHeadPartByType")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	int cdcg_type = Costile2.GetInt(session, 1)
	return this.GetIndexOfHeadPartByType(cdcg_type)
endFunction

; int ActorBase::GetNumOverlayHeadParts(...) native
int function ActorBase_GetNumOverlayHeadParts() global
	int session = Costile2.GetSession("ActorBase_GetNumOverlayHeadParts")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetNumOverlayHeadParts()
endFunction

; headpart ActorBase::GetNthOverlayHeadPart(...) native
headpart function ActorBase_GetNthOverlayHeadPart() global
	int session = Costile2.GetSession("ActorBase_GetNthOverlayHeadPart")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	int cdcg_slotPart = Costile2.GetInt(session, 1)
	return this.GetNthOverlayHeadPart(cdcg_slotPart)
endFunction

; int ActorBase::GetIndexOfOverlayHeadPartByType(...) native
int function ActorBase_GetIndexOfOverlayHeadPartByType() global
	int session = Costile2.GetSession("ActorBase_GetIndexOfOverlayHeadPartByType")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	int cdcg_type = Costile2.GetInt(session, 1)
	return this.GetIndexOfOverlayHeadPartByType(cdcg_type)
endFunction

; float ActorBase::GetFaceMorph(...) native
float function ActorBase_GetFaceMorph() global
	int session = Costile2.GetSession("ActorBase_GetFaceMorph")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetFaceMorph(cdcg_index)
endFunction

; void ActorBase::SetFaceMorph(...) native
function ActorBase_SetFaceMorph() global
	int session = Costile2.GetSession("ActorBase_SetFaceMorph")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	float cdcg_value = Costile2.GetFloat(session, 1)
	int cdcg_index = Costile2.GetInt(session, 2)
	this.SetFaceMorph(cdcg_value, cdcg_index)
endFunction

; int ActorBase::GetFacePreset(...) native
int function ActorBase_GetFacePreset() global
	int session = Costile2.GetSession("ActorBase_GetFacePreset")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	int cdcg_index = Costile2.GetInt(session, 1)
	return this.GetFacePreset(cdcg_index)
endFunction

; void ActorBase::SetFacePreset(...) native
function ActorBase_SetFacePreset() global
	int session = Costile2.GetSession("ActorBase_SetFacePreset")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	int cdcg_value = Costile2.GetInt(session, 1)
	int cdcg_index = Costile2.GetInt(session, 2)
	this.SetFacePreset(cdcg_value, cdcg_index)
endFunction

; colorform ActorBase::GetHairColor(...) native
colorform function ActorBase_GetHairColor() global
	int session = Costile2.GetSession("ActorBase_GetHairColor")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetHairColor()
endFunction

; void ActorBase::SetHairColor(...) native
function ActorBase_SetHairColor() global
	int session = Costile2.GetSession("ActorBase_SetHairColor")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	colorform cdcg_color = Costile2.GetTESForm(session, 1) as colorform
	this.SetHairColor(cdcg_color)
endFunction

; int ActorBase::GetSpellCount(...) native
int function ActorBase_GetSpellCount() global
	int session = Costile2.GetSession("ActorBase_GetSpellCount")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetSpellCount()
endFunction

; spell ActorBase::GetNthSpell(...) native
spell function ActorBase_GetNthSpell() global
	int session = Costile2.GetSession("ActorBase_GetNthSpell")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthSpell(cdcg_n)
endFunction

; textureset ActorBase::GetFaceTextureSet(...) native
textureset function ActorBase_GetFaceTextureSet() global
	int session = Costile2.GetSession("ActorBase_GetFaceTextureSet")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetFaceTextureSet()
endFunction

; void ActorBase::SetFaceTextureSet(...) native
function ActorBase_SetFaceTextureSet() global
	int session = Costile2.GetSession("ActorBase_SetFaceTextureSet")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	textureset cdcg_textures = Costile2.GetTESForm(session, 1) as textureset
	this.SetFaceTextureSet(cdcg_textures)
endFunction

; voicetype ActorBase::GetVoiceType(...) native
voicetype function ActorBase_GetVoiceType() global
	int session = Costile2.GetSession("ActorBase_GetVoiceType")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetVoiceType()
endFunction

; void ActorBase::SetVoiceType(...) native
function ActorBase_SetVoiceType() global
	int session = Costile2.GetSession("ActorBase_SetVoiceType")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	voicetype cdcg_nVoice = Costile2.GetTESForm(session, 1) as voicetype
	this.SetVoiceType(cdcg_nVoice)
endFunction

; armor ActorBase::GetSkin(...) native
armor function ActorBase_GetSkin() global
	int session = Costile2.GetSession("ActorBase_GetSkin")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetSkin()
endFunction

; void ActorBase::SetSkin(...) native
function ActorBase_SetSkin() global
	int session = Costile2.GetSession("ActorBase_SetSkin")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	armor cdcg_skin = Costile2.GetTESForm(session, 1) as armor
	this.SetSkin(cdcg_skin)
endFunction

; armor ActorBase::GetSkinFar(...) native
armor function ActorBase_GetSkinFar() global
	int session = Costile2.GetSession("ActorBase_GetSkinFar")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetSkinFar()
endFunction

; void ActorBase::SetSkinFar(...) native
function ActorBase_SetSkinFar() global
	int session = Costile2.GetSession("ActorBase_SetSkinFar")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	armor cdcg_skin = Costile2.GetTESForm(session, 1) as armor
	this.SetSkinFar(cdcg_skin)
endFunction

; actorbase ActorBase::GetTemplate(...) native
actorbase function ActorBase_GetTemplate() global
	int session = Costile2.GetSession("ActorBase_GetTemplate")
	ActorBase this = Costile2.GetTESForm(session, 0) as ActorBase
	return this.GetTemplate()
endFunction

