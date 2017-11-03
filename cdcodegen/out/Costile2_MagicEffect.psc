Scriptname Costile2_MagicEffect Hidden

;
;
;
; script MagicEffect.psc
;
;
;

; string MagicEffect::GetAssociatedSkill(...) native
string function MagicEffect_GetAssociatedSkill() global
	int session = Costile2.GetSession("MagicEffect_GetAssociatedSkill")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetAssociatedSkill()
endFunction

; void MagicEffect::SetAssociatedSkill(...) native
function MagicEffect_SetAssociatedSkill() global
	int session = Costile2.GetSession("MagicEffect_SetAssociatedSkill")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	string cdcg_skill = Costile2.GetString(session, 1)
	this.SetAssociatedSkill(cdcg_skill)
endFunction

; string MagicEffect::GetResistance(...) native
string function MagicEffect_GetResistance() global
	int session = Costile2.GetSession("MagicEffect_GetResistance")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetResistance()
endFunction

; void MagicEffect::SetResistance(...) native
function MagicEffect_SetResistance() global
	int session = Costile2.GetSession("MagicEffect_SetResistance")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	string cdcg_skill = Costile2.GetString(session, 1)
	this.SetResistance(cdcg_skill)
endFunction

; bool MagicEffect::IsEffectFlagSet(...) native
bool function MagicEffect_IsEffectFlagSet() global
	int session = Costile2.GetSession("MagicEffect_IsEffectFlagSet")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	int cdcg_flag = Costile2.GetInt(session, 1)
	return this.IsEffectFlagSet(cdcg_flag)
endFunction

; void MagicEffect::SetEffectFlag(...) native
function MagicEffect_SetEffectFlag() global
	int session = Costile2.GetSession("MagicEffect_SetEffectFlag")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	int cdcg_flag = Costile2.GetInt(session, 1)
	this.SetEffectFlag(cdcg_flag)
endFunction

; void MagicEffect::ClearEffectFlag(...) native
function MagicEffect_ClearEffectFlag() global
	int session = Costile2.GetSession("MagicEffect_ClearEffectFlag")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	int cdcg_flag = Costile2.GetInt(session, 1)
	this.ClearEffectFlag(cdcg_flag)
endFunction

; float MagicEffect::GetCastTime(...) native
float function MagicEffect_GetCastTime() global
	int session = Costile2.GetSession("MagicEffect_GetCastTime")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetCastTime()
endFunction

; void MagicEffect::SetCastTime(...) native
function MagicEffect_SetCastTime() global
	int session = Costile2.GetSession("MagicEffect_SetCastTime")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	float cdcg_castTime = Costile2.GetFloat(session, 1)
	this.SetCastTime(cdcg_castTime)
endFunction

; int MagicEffect::GetSkillLevel(...) native
int function MagicEffect_GetSkillLevel() global
	int session = Costile2.GetSession("MagicEffect_GetSkillLevel")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetSkillLevel()
endFunction

; void MagicEffect::SetSkillLevel(...) native
function MagicEffect_SetSkillLevel() global
	int session = Costile2.GetSession("MagicEffect_SetSkillLevel")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	int cdcg_level = Costile2.GetInt(session, 1)
	this.SetSkillLevel(cdcg_level)
endFunction

; int MagicEffect::GetArea(...) native
int function MagicEffect_GetArea() global
	int session = Costile2.GetSession("MagicEffect_GetArea")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetArea()
endFunction

; void MagicEffect::SetArea(...) native
function MagicEffect_SetArea() global
	int session = Costile2.GetSession("MagicEffect_SetArea")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	int cdcg_area = Costile2.GetInt(session, 1)
	this.SetArea(cdcg_area)
endFunction

; float MagicEffect::GetSkillUsageMult(...) native
float function MagicEffect_GetSkillUsageMult() global
	int session = Costile2.GetSession("MagicEffect_GetSkillUsageMult")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetSkillUsageMult()
endFunction

; void MagicEffect::SetSkillUsageMult(...) native
function MagicEffect_SetSkillUsageMult() global
	int session = Costile2.GetSession("MagicEffect_SetSkillUsageMult")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	float cdcg_usageMult = Costile2.GetFloat(session, 1)
	this.SetSkillUsageMult(cdcg_usageMult)
endFunction

; float MagicEffect::GetBaseCost(...) native
float function MagicEffect_GetBaseCost() global
	int session = Costile2.GetSession("MagicEffect_GetBaseCost")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetBaseCost()
endFunction

; void MagicEffect::SetBaseCost(...) native
function MagicEffect_SetBaseCost() global
	int session = Costile2.GetSession("MagicEffect_SetBaseCost")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	float cdcg_cost = Costile2.GetFloat(session, 1)
	this.SetBaseCost(cdcg_cost)
endFunction

; light MagicEffect::GetLight(...) native
light function MagicEffect_GetLight() global
	int session = Costile2.GetSession("MagicEffect_GetLight")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetLight()
endFunction

; void MagicEffect::SetLight(...) native
function MagicEffect_SetLight() global
	int session = Costile2.GetSession("MagicEffect_SetLight")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	light cdcg_obj = Costile2.GetTESForm(session, 1) as light
	this.SetLight(cdcg_obj)
endFunction

; effectshader MagicEffect::GetHitShader(...) native
effectshader function MagicEffect_GetHitShader() global
	int session = Costile2.GetSession("MagicEffect_GetHitShader")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetHitShader()
endFunction

; void MagicEffect::SetHitShader(...) native
function MagicEffect_SetHitShader() global
	int session = Costile2.GetSession("MagicEffect_SetHitShader")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	effectshader cdcg_obj = Costile2.GetTESForm(session, 1) as effectshader
	this.SetHitShader(cdcg_obj)
endFunction

; effectshader MagicEffect::GetEnchantShader(...) native
effectshader function MagicEffect_GetEnchantShader() global
	int session = Costile2.GetSession("MagicEffect_GetEnchantShader")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetEnchantShader()
endFunction

; void MagicEffect::SetEnchantShader(...) native
function MagicEffect_SetEnchantShader() global
	int session = Costile2.GetSession("MagicEffect_SetEnchantShader")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	effectshader cdcg_obj = Costile2.GetTESForm(session, 1) as effectshader
	this.SetEnchantShader(cdcg_obj)
endFunction

; projectile MagicEffect::GetProjectile(...) native
projectile function MagicEffect_GetProjectile() global
	int session = Costile2.GetSession("MagicEffect_GetProjectile")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetProjectile()
endFunction

; void MagicEffect::SetProjectile(...) native
function MagicEffect_SetProjectile() global
	int session = Costile2.GetSession("MagicEffect_SetProjectile")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	projectile cdcg_obj = Costile2.GetTESForm(session, 1) as projectile
	this.SetProjectile(cdcg_obj)
endFunction

; explosion MagicEffect::GetExplosion(...) native
explosion function MagicEffect_GetExplosion() global
	int session = Costile2.GetSession("MagicEffect_GetExplosion")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetExplosion()
endFunction

; void MagicEffect::SetExplosion(...) native
function MagicEffect_SetExplosion() global
	int session = Costile2.GetSession("MagicEffect_SetExplosion")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	explosion cdcg_obj = Costile2.GetTESForm(session, 1) as explosion
	this.SetExplosion(cdcg_obj)
endFunction

; art MagicEffect::GetCastingArt(...) native
art function MagicEffect_GetCastingArt() global
	int session = Costile2.GetSession("MagicEffect_GetCastingArt")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetCastingArt()
endFunction

; void MagicEffect::SetCastingArt(...) native
function MagicEffect_SetCastingArt() global
	int session = Costile2.GetSession("MagicEffect_SetCastingArt")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	art cdcg_obj = Costile2.GetTESForm(session, 1) as art
	this.SetCastingArt(cdcg_obj)
endFunction

; art MagicEffect::GetHitEffectArt(...) native
art function MagicEffect_GetHitEffectArt() global
	int session = Costile2.GetSession("MagicEffect_GetHitEffectArt")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetHitEffectArt()
endFunction

; void MagicEffect::SetHitEffectArt(...) native
function MagicEffect_SetHitEffectArt() global
	int session = Costile2.GetSession("MagicEffect_SetHitEffectArt")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	art cdcg_obj = Costile2.GetTESForm(session, 1) as art
	this.SetHitEffectArt(cdcg_obj)
endFunction

; art MagicEffect::GetEnchantArt(...) native
art function MagicEffect_GetEnchantArt() global
	int session = Costile2.GetSession("MagicEffect_GetEnchantArt")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetEnchantArt()
endFunction

; void MagicEffect::SetEnchantArt(...) native
function MagicEffect_SetEnchantArt() global
	int session = Costile2.GetSession("MagicEffect_SetEnchantArt")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	art cdcg_obj = Costile2.GetTESForm(session, 1) as art
	this.SetEnchantArt(cdcg_obj)
endFunction

; impactdataset MagicEffect::GetImpactDataSet(...) native
impactdataset function MagicEffect_GetImpactDataSet() global
	int session = Costile2.GetSession("MagicEffect_GetImpactDataSet")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetImpactDataSet()
endFunction

; void MagicEffect::SetImpactDataSet(...) native
function MagicEffect_SetImpactDataSet() global
	int session = Costile2.GetSession("MagicEffect_SetImpactDataSet")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	impactdataset cdcg_obj = Costile2.GetTESForm(session, 1) as impactdataset
	this.SetImpactDataSet(cdcg_obj)
endFunction

; spell MagicEffect::GetEquipAbility(...) native
spell function MagicEffect_GetEquipAbility() global
	int session = Costile2.GetSession("MagicEffect_GetEquipAbility")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetEquipAbility()
endFunction

; void MagicEffect::SetEquipAbility(...) native
function MagicEffect_SetEquipAbility() global
	int session = Costile2.GetSession("MagicEffect_SetEquipAbility")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	spell cdcg_obj = Costile2.GetTESForm(session, 1) as spell
	this.SetEquipAbility(cdcg_obj)
endFunction

; imagespacemodifier MagicEffect::GetImageSpaceMod(...) native
imagespacemodifier function MagicEffect_GetImageSpaceMod() global
	int session = Costile2.GetSession("MagicEffect_GetImageSpaceMod")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetImageSpaceMod()
endFunction

; void MagicEffect::SetImageSpaceMod(...) native
function MagicEffect_SetImageSpaceMod() global
	int session = Costile2.GetSession("MagicEffect_SetImageSpaceMod")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	imagespacemodifier cdcg_obj = Costile2.GetTESForm(session, 1) as imagespacemodifier
	this.SetImageSpaceMod(cdcg_obj)
endFunction

; perk MagicEffect::GetPerk(...) native
perk function MagicEffect_GetPerk() global
	int session = Costile2.GetSession("MagicEffect_GetPerk")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetPerk()
endFunction

; void MagicEffect::SetPerk(...) native
function MagicEffect_SetPerk() global
	int session = Costile2.GetSession("MagicEffect_SetPerk")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	perk cdcg_obj = Costile2.GetTESForm(session, 1) as perk
	this.SetPerk(cdcg_obj)
endFunction

; int MagicEffect::GetCastingType(...) native
int function MagicEffect_GetCastingType() global
	int session = Costile2.GetSession("MagicEffect_GetCastingType")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetCastingType()
endFunction

; int MagicEffect::GetDeliveryType(...) native
int function MagicEffect_GetDeliveryType() global
	int session = Costile2.GetSession("MagicEffect_GetDeliveryType")
	MagicEffect this = Costile2.GetTESForm(session, 0) as MagicEffect
	return this.GetDeliveryType()
endFunction

