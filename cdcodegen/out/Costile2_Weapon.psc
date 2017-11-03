Scriptname Costile2_Weapon Hidden

;
;
;
; script Weapon.psc
;
;
;

; void Weapon::Fire(...) native
function Weapon_Fire() global
	int session = Costile2.GetSession("Weapon_Fire")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	objectreference cdcg_akSource = Costile2.GetTESForm(session, 1) as objectreference
	ammo cdcg_akAmmo = Costile2.GetTESForm(session, 2) as ammo
	this.Fire(cdcg_akSource, cdcg_akAmmo)
endFunction

; int Weapon::GetBaseDamage(...) native
int function Weapon_GetBaseDamage() global
	int session = Costile2.GetSession("Weapon_GetBaseDamage")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetBaseDamage()
endFunction

; void Weapon::SetBaseDamage(...) native
function Weapon_SetBaseDamage() global
	int session = Costile2.GetSession("Weapon_SetBaseDamage")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	int cdcg_damage = Costile2.GetInt(session, 1)
	this.SetBaseDamage(cdcg_damage)
endFunction

; int Weapon::GetCritDamage(...) native
int function Weapon_GetCritDamage() global
	int session = Costile2.GetSession("Weapon_GetCritDamage")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetCritDamage()
endFunction

; void Weapon::SetCritDamage(...) native
function Weapon_SetCritDamage() global
	int session = Costile2.GetSession("Weapon_SetCritDamage")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	int cdcg_damage = Costile2.GetInt(session, 1)
	this.SetCritDamage(cdcg_damage)
endFunction

; float Weapon::GetReach(...) native
float function Weapon_GetReach() global
	int session = Costile2.GetSession("Weapon_GetReach")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetReach()
endFunction

; void Weapon::SetReach(...) native
function Weapon_SetReach() global
	int session = Costile2.GetSession("Weapon_SetReach")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	float cdcg_reach = Costile2.GetFloat(session, 1)
	this.SetReach(cdcg_reach)
endFunction

; float Weapon::GetMinRange(...) native
float function Weapon_GetMinRange() global
	int session = Costile2.GetSession("Weapon_GetMinRange")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetMinRange()
endFunction

; void Weapon::SetMinRange(...) native
function Weapon_SetMinRange() global
	int session = Costile2.GetSession("Weapon_SetMinRange")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	float cdcg_minRange = Costile2.GetFloat(session, 1)
	this.SetMinRange(cdcg_minRange)
endFunction

; float Weapon::GetMaxRange(...) native
float function Weapon_GetMaxRange() global
	int session = Costile2.GetSession("Weapon_GetMaxRange")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetMaxRange()
endFunction

; void Weapon::SetMaxRange(...) native
function Weapon_SetMaxRange() global
	int session = Costile2.GetSession("Weapon_SetMaxRange")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	float cdcg_maxRange = Costile2.GetFloat(session, 1)
	this.SetMaxRange(cdcg_maxRange)
endFunction

; float Weapon::GetSpeed(...) native
float function Weapon_GetSpeed() global
	int session = Costile2.GetSession("Weapon_GetSpeed")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetSpeed()
endFunction

; void Weapon::SetSpeed(...) native
function Weapon_SetSpeed() global
	int session = Costile2.GetSession("Weapon_SetSpeed")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	float cdcg_speed = Costile2.GetFloat(session, 1)
	this.SetSpeed(cdcg_speed)
endFunction

; float Weapon::GetStagger(...) native
float function Weapon_GetStagger() global
	int session = Costile2.GetSession("Weapon_GetStagger")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetStagger()
endFunction

; void Weapon::SetStagger(...) native
function Weapon_SetStagger() global
	int session = Costile2.GetSession("Weapon_SetStagger")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	float cdcg_stagger = Costile2.GetFloat(session, 1)
	this.SetStagger(cdcg_stagger)
endFunction

; int Weapon::GetWeaponType(...) native
int function Weapon_GetWeaponType() global
	int session = Costile2.GetSession("Weapon_GetWeaponType")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetWeaponType()
endFunction

; void Weapon::SetWeaponType(...) native
function Weapon_SetWeaponType() global
	int session = Costile2.GetSession("Weapon_SetWeaponType")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	int cdcg_type = Costile2.GetInt(session, 1)
	this.SetWeaponType(cdcg_type)
endFunction

; string Weapon::GetModelPath(...) native
string function Weapon_GetModelPath() global
	int session = Costile2.GetSession("Weapon_GetModelPath")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetModelPath()
endFunction

; void Weapon::SetModelPath(...) native
function Weapon_SetModelPath() global
	int session = Costile2.GetSession("Weapon_SetModelPath")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	string cdcg_path = Costile2.GetString(session, 1)
	this.SetModelPath(cdcg_path)
endFunction

; string Weapon::GetIconPath(...) native
string function Weapon_GetIconPath() global
	int session = Costile2.GetSession("Weapon_GetIconPath")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetIconPath()
endFunction

; void Weapon::SetIconPath(...) native
function Weapon_SetIconPath() global
	int session = Costile2.GetSession("Weapon_SetIconPath")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	string cdcg_path = Costile2.GetString(session, 1)
	this.SetIconPath(cdcg_path)
endFunction

; string Weapon::GetMessageIconPath(...) native
string function Weapon_GetMessageIconPath() global
	int session = Costile2.GetSession("Weapon_GetMessageIconPath")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetMessageIconPath()
endFunction

; void Weapon::SetMessageIconPath(...) native
function Weapon_SetMessageIconPath() global
	int session = Costile2.GetSession("Weapon_SetMessageIconPath")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	string cdcg_path = Costile2.GetString(session, 1)
	this.SetMessageIconPath(cdcg_path)
endFunction

; enchantment Weapon::GetEnchantment(...) native
enchantment function Weapon_GetEnchantment() global
	int session = Costile2.GetSession("Weapon_GetEnchantment")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetEnchantment()
endFunction

; void Weapon::SetEnchantment(...) native
function Weapon_SetEnchantment() global
	int session = Costile2.GetSession("Weapon_SetEnchantment")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	enchantment cdcg_e = Costile2.GetTESForm(session, 1) as enchantment
	this.SetEnchantment(cdcg_e)
endFunction

; int Weapon::GetEnchantmentValue(...) native
int function Weapon_GetEnchantmentValue() global
	int session = Costile2.GetSession("Weapon_GetEnchantmentValue")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetEnchantmentValue()
endFunction

; void Weapon::SetEnchantmentValue(...) native
function Weapon_SetEnchantmentValue() global
	int session = Costile2.GetSession("Weapon_SetEnchantmentValue")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	int cdcg_value = Costile2.GetInt(session, 1)
	this.SetEnchantmentValue(cdcg_value)
endFunction

; static Weapon::GetEquippedModel(...) native
static function Weapon_GetEquippedModel() global
	int session = Costile2.GetSession("Weapon_GetEquippedModel")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetEquippedModel()
endFunction

; void Weapon::SetEquippedModel(...) native
function Weapon_SetEquippedModel() global
	int session = Costile2.GetSession("Weapon_SetEquippedModel")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	static cdcg_model = Costile2.GetTESForm(session, 1) as static
	this.SetEquippedModel(cdcg_model)
endFunction

; equipslot Weapon::GetEquipType(...) native
equipslot function Weapon_GetEquipType() global
	int session = Costile2.GetSession("Weapon_GetEquipType")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetEquipType()
endFunction

; void Weapon::SetEquipType(...) native
function Weapon_SetEquipType() global
	int session = Costile2.GetSession("Weapon_SetEquipType")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	equipslot cdcg_type = Costile2.GetTESForm(session, 1) as equipslot
	this.SetEquipType(cdcg_type)
endFunction

; string Weapon::GetSkill(...) native
string function Weapon_GetSkill() global
	int session = Costile2.GetSession("Weapon_GetSkill")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetSkill()
endFunction

; void Weapon::SetSkill(...) native
function Weapon_SetSkill() global
	int session = Costile2.GetSession("Weapon_SetSkill")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	string cdcg_skill = Costile2.GetString(session, 1)
	this.SetSkill(cdcg_skill)
endFunction

; string Weapon::GetResist(...) native
string function Weapon_GetResist() global
	int session = Costile2.GetSession("Weapon_GetResist")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetResist()
endFunction

; void Weapon::SetResist(...) native
function Weapon_SetResist() global
	int session = Costile2.GetSession("Weapon_SetResist")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	string cdcg_resist = Costile2.GetString(session, 1)
	this.SetResist(cdcg_resist)
endFunction

; spell Weapon::GetCritEffect(...) native
spell function Weapon_GetCritEffect() global
	int session = Costile2.GetSession("Weapon_GetCritEffect")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetCritEffect()
endFunction

; void Weapon::SetCritEffect(...) native
function Weapon_SetCritEffect() global
	int session = Costile2.GetSession("Weapon_SetCritEffect")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	spell cdcg_ce = Costile2.GetTESForm(session, 1) as spell
	this.SetCritEffect(cdcg_ce)
endFunction

; bool Weapon::GetCritEffectOnDeath(...) native
bool function Weapon_GetCritEffectOnDeath() global
	int session = Costile2.GetSession("Weapon_GetCritEffectOnDeath")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetCritEffectOnDeath()
endFunction

; void Weapon::SetCritEffectOnDeath(...) native
function Weapon_SetCritEffectOnDeath() global
	int session = Costile2.GetSession("Weapon_SetCritEffectOnDeath")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	bool cdcg_ceod = Costile2.GetBool(session, 1)
	this.SetCritEffectOnDeath(cdcg_ceod)
endFunction

; float Weapon::GetCritMultiplier(...) native
float function Weapon_GetCritMultiplier() global
	int session = Costile2.GetSession("Weapon_GetCritMultiplier")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetCritMultiplier()
endFunction

; void Weapon::SetCritMultiplier(...) native
function Weapon_SetCritMultiplier() global
	int session = Costile2.GetSession("Weapon_SetCritMultiplier")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	float cdcg_crit = Costile2.GetFloat(session, 1)
	this.SetCritMultiplier(cdcg_crit)
endFunction

; weapon Weapon::GetTemplate(...) native
weapon function Weapon_GetTemplate() global
	int session = Costile2.GetSession("Weapon_GetTemplate")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.GetTemplate()
endFunction

; bool Weapon::IsBattleaxe(...)
bool function Weapon_IsBattleaxe() global
	int session = Costile2.GetSession("Weapon_IsBattleaxe")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.IsBattleaxe()
endFunction

; bool Weapon::IsBow(...)
bool function Weapon_IsBow() global
	int session = Costile2.GetSession("Weapon_IsBow")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.IsBow()
endFunction

; bool Weapon::IsDagger(...)
bool function Weapon_IsDagger() global
	int session = Costile2.GetSession("Weapon_IsDagger")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.IsDagger()
endFunction

; bool Weapon::IsGreatsword(...)
bool function Weapon_IsGreatsword() global
	int session = Costile2.GetSession("Weapon_IsGreatsword")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.IsGreatsword()
endFunction

; bool Weapon::IsMace(...)
bool function Weapon_IsMace() global
	int session = Costile2.GetSession("Weapon_IsMace")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.IsMace()
endFunction

; bool Weapon::IsStaff(...)
bool function Weapon_IsStaff() global
	int session = Costile2.GetSession("Weapon_IsStaff")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.IsStaff()
endFunction

; bool Weapon::IsSword(...)
bool function Weapon_IsSword() global
	int session = Costile2.GetSession("Weapon_IsSword")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.IsSword()
endFunction

; bool Weapon::IsWarhammer(...)
bool function Weapon_IsWarhammer() global
	int session = Costile2.GetSession("Weapon_IsWarhammer")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.IsWarhammer()
endFunction

; bool Weapon::IsWarAxe(...)
bool function Weapon_IsWarAxe() global
	int session = Costile2.GetSession("Weapon_IsWarAxe")
	Weapon this = Costile2.GetTESForm(session, 0) as Weapon
	return this.IsWarAxe()
endFunction

