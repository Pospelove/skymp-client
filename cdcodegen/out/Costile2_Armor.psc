Scriptname Costile2_Armor Hidden

;
;
;
; script Armor.psc
;
;
;

; int Armor::GetArmorRating(...) native
int function Armor_GetArmorRating() global
	int session = Costile2.GetSession("Armor_GetArmorRating")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.GetArmorRating()
endFunction

; int Armor::GetAR(...)
int function Armor_GetAR() global
	int session = Costile2.GetSession("Armor_GetAR")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.GetAR()
endFunction

; void Armor::SetArmorRating(...) native
function Armor_SetArmorRating() global
	int session = Costile2.GetSession("Armor_SetArmorRating")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	int cdcg_armorRating = Costile2.GetInt(session, 1)
	this.SetArmorRating(cdcg_armorRating)
endFunction

; void Armor::SetAR(...)
function Armor_SetAR() global
	int session = Costile2.GetSession("Armor_SetAR")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	int cdcg_armorRating = Costile2.GetInt(session, 1)
	this.SetAR(cdcg_armorRating)
endFunction

; void Armor::ModArmorRating(...) native
function Armor_ModArmorRating() global
	int session = Costile2.GetSession("Armor_ModArmorRating")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	int cdcg_modBy = Costile2.GetInt(session, 1)
	this.ModArmorRating(cdcg_modBy)
endFunction

; void Armor::ModAR(...)
function Armor_ModAR() global
	int session = Costile2.GetSession("Armor_ModAR")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	int cdcg_modBy = Costile2.GetInt(session, 1)
	this.ModAR(cdcg_modBy)
endFunction

; string Armor::GetModelPath(...) native
string function Armor_GetModelPath() global
	int session = Costile2.GetSession("Armor_GetModelPath")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	bool cdcg_bFemalePath = Costile2.GetBool(session, 1)
	return this.GetModelPath(cdcg_bFemalePath)
endFunction

; void Armor::SetModelPath(...) native
function Armor_SetModelPath() global
	int session = Costile2.GetSession("Armor_SetModelPath")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	string cdcg_path = Costile2.GetString(session, 1)
	bool cdcg_bFemalePath = Costile2.GetBool(session, 2)
	this.SetModelPath(cdcg_path, cdcg_bFemalePath)
endFunction

; string Armor::GetIconPath(...) native
string function Armor_GetIconPath() global
	int session = Costile2.GetSession("Armor_GetIconPath")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	bool cdcg_bFemalePath = Costile2.GetBool(session, 1)
	return this.GetIconPath(cdcg_bFemalePath)
endFunction

; void Armor::SetIconPath(...) native
function Armor_SetIconPath() global
	int session = Costile2.GetSession("Armor_SetIconPath")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	string cdcg_path = Costile2.GetString(session, 1)
	bool cdcg_bFemalePath = Costile2.GetBool(session, 2)
	this.SetIconPath(cdcg_path, cdcg_bFemalePath)
endFunction

; string Armor::GetMessageIconPath(...) native
string function Armor_GetMessageIconPath() global
	int session = Costile2.GetSession("Armor_GetMessageIconPath")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	bool cdcg_bFemalePath = Costile2.GetBool(session, 1)
	return this.GetMessageIconPath(cdcg_bFemalePath)
endFunction

; void Armor::SetMessageIconPath(...) native
function Armor_SetMessageIconPath() global
	int session = Costile2.GetSession("Armor_SetMessageIconPath")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	string cdcg_path = Costile2.GetString(session, 1)
	bool cdcg_bFemalePath = Costile2.GetBool(session, 2)
	this.SetMessageIconPath(cdcg_path, cdcg_bFemalePath)
endFunction

; int Armor::GetWeightClass(...) native
int function Armor_GetWeightClass() global
	int session = Costile2.GetSession("Armor_GetWeightClass")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.GetWeightClass()
endFunction

; void Armor::SetWeightClass(...) native
function Armor_SetWeightClass() global
	int session = Costile2.GetSession("Armor_SetWeightClass")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	int cdcg_weightClass = Costile2.GetInt(session, 1)
	this.SetWeightClass(cdcg_weightClass)
endFunction

; enchantment Armor::GetEnchantment(...) native
enchantment function Armor_GetEnchantment() global
	int session = Costile2.GetSession("Armor_GetEnchantment")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.GetEnchantment()
endFunction

; void Armor::SetEnchantment(...) native
function Armor_SetEnchantment() global
	int session = Costile2.GetSession("Armor_SetEnchantment")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	enchantment cdcg_e = Costile2.GetTESForm(session, 1) as enchantment
	this.SetEnchantment(cdcg_e)
endFunction

; bool Armor::IsLightArmor(...)
bool function Armor_IsLightArmor() global
	int session = Costile2.GetSession("Armor_IsLightArmor")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsLightArmor()
endFunction

; bool Armor::IsHeavyArmor(...)
bool function Armor_IsHeavyArmor() global
	int session = Costile2.GetSession("Armor_IsHeavyArmor")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsHeavyArmor()
endFunction

; bool Armor::IsClothing(...)
bool function Armor_IsClothing() global
	int session = Costile2.GetSession("Armor_IsClothing")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsClothing()
endFunction

; bool Armor::IsBoots(...)
bool function Armor_IsBoots() global
	int session = Costile2.GetSession("Armor_IsBoots")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsBoots()
endFunction

; bool Armor::IsCuirass(...)
bool function Armor_IsCuirass() global
	int session = Costile2.GetSession("Armor_IsCuirass")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsCuirass()
endFunction

; bool Armor::IsGauntlets(...)
bool function Armor_IsGauntlets() global
	int session = Costile2.GetSession("Armor_IsGauntlets")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsGauntlets()
endFunction

; bool Armor::IsHelmet(...)
bool function Armor_IsHelmet() global
	int session = Costile2.GetSession("Armor_IsHelmet")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsHelmet()
endFunction

; bool Armor::IsShield(...)
bool function Armor_IsShield() global
	int session = Costile2.GetSession("Armor_IsShield")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsShield()
endFunction

; bool Armor::IsJewelry(...)
bool function Armor_IsJewelry() global
	int session = Costile2.GetSession("Armor_IsJewelry")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsJewelry()
endFunction

; bool Armor::IsClothingHead(...)
bool function Armor_IsClothingHead() global
	int session = Costile2.GetSession("Armor_IsClothingHead")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsClothingHead()
endFunction

; bool Armor::IsClothingBody(...)
bool function Armor_IsClothingBody() global
	int session = Costile2.GetSession("Armor_IsClothingBody")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsClothingBody()
endFunction

; bool Armor::IsClothingFeet(...)
bool function Armor_IsClothingFeet() global
	int session = Costile2.GetSession("Armor_IsClothingFeet")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsClothingFeet()
endFunction

; bool Armor::IsClothingHands(...)
bool function Armor_IsClothingHands() global
	int session = Costile2.GetSession("Armor_IsClothingHands")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsClothingHands()
endFunction

; bool Armor::IsClothingRing(...)
bool function Armor_IsClothingRing() global
	int session = Costile2.GetSession("Armor_IsClothingRing")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsClothingRing()
endFunction

; bool Armor::IsClothingRich(...)
bool function Armor_IsClothingRich() global
	int session = Costile2.GetSession("Armor_IsClothingRich")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsClothingRich()
endFunction

; bool Armor::IsClothingPoor(...)
bool function Armor_IsClothingPoor() global
	int session = Costile2.GetSession("Armor_IsClothingPoor")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.IsClothingPoor()
endFunction

; int Armor::GetSlotMask(...) native
int function Armor_GetSlotMask() global
	int session = Costile2.GetSession("Armor_GetSlotMask")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.GetSlotMask()
endFunction

; void Armor::SetSlotMask(...) native
function Armor_SetSlotMask() global
	int session = Costile2.GetSession("Armor_SetSlotMask")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	int cdcg_slotMask = Costile2.GetInt(session, 1)
	this.SetSlotMask(cdcg_slotMask)
endFunction

; int Armor::AddSlotToMask(...) native
int function Armor_AddSlotToMask() global
	int session = Costile2.GetSession("Armor_AddSlotToMask")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	int cdcg_slotMask = Costile2.GetInt(session, 1)
	return this.AddSlotToMask(cdcg_slotMask)
endFunction

; int Armor::RemoveSlotFromMask(...) native
int function Armor_RemoveSlotFromMask() global
	int session = Costile2.GetSession("Armor_RemoveSlotFromMask")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	int cdcg_slotMask = Costile2.GetInt(session, 1)
	return this.RemoveSlotFromMask(cdcg_slotMask)
endFunction

; int Armor::GetMaskForSlot(...) native global
int function Armor_GetMaskForSlot() global
	int session = Costile2.GetSession("Armor_GetMaskForSlot")
	int cdcg_slot = Costile2.GetInt(session, 0)
	return Armor.GetMaskForSlot(cdcg_slot)
endFunction

; int Armor::GetNumArmorAddons(...) native
int function Armor_GetNumArmorAddons() global
	int session = Costile2.GetSession("Armor_GetNumArmorAddons")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	return this.GetNumArmorAddons()
endFunction

; armoraddon Armor::GetNthArmorAddon(...) native
armoraddon function Armor_GetNthArmorAddon() global
	int session = Costile2.GetSession("Armor_GetNthArmorAddon")
	Armor this = Costile2.GetTESForm(session, 0) as Armor
	int cdcg_n = Costile2.GetInt(session, 1)
	return this.GetNthArmorAddon(cdcg_n)
endFunction

