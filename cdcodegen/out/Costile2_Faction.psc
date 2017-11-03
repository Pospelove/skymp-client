Scriptname Costile2_Faction Hidden

;
;
;
; script Faction.psc
;
;
;

; bool Faction::CanPayCrimeGold(...) native
bool function Faction_CanPayCrimeGold() global
	int session = Costile2.GetSession("Faction_CanPayCrimeGold")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.CanPayCrimeGold()
endFunction

; int Faction::GetCrimeGold(...) native
int function Faction_GetCrimeGold() global
	int session = Costile2.GetSession("Faction_GetCrimeGold")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetCrimeGold()
endFunction

; int Faction::GetCrimeGoldNonViolent(...) native
int function Faction_GetCrimeGoldNonViolent() global
	int session = Costile2.GetSession("Faction_GetCrimeGoldNonViolent")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetCrimeGoldNonViolent()
endFunction

; int Faction::GetCrimeGoldViolent(...) native
int function Faction_GetCrimeGoldViolent() global
	int session = Costile2.GetSession("Faction_GetCrimeGoldViolent")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetCrimeGoldViolent()
endFunction

; int Faction::GetInfamy(...) native
int function Faction_GetInfamy() global
	int session = Costile2.GetSession("Faction_GetInfamy")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetInfamy()
endFunction

; int Faction::GetInfamyNonViolent(...) native
int function Faction_GetInfamyNonViolent() global
	int session = Costile2.GetSession("Faction_GetInfamyNonViolent")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetInfamyNonViolent()
endFunction

; int Faction::GetInfamyViolent(...) native
int function Faction_GetInfamyViolent() global
	int session = Costile2.GetSession("Faction_GetInfamyViolent")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetInfamyViolent()
endFunction

; int Faction::GetReaction(...) native
int function Faction_GetReaction() global
	int session = Costile2.GetSession("Faction_GetReaction")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	faction cdcg_akOther = Costile2.GetTESForm(session, 1) as faction
	return this.GetReaction(cdcg_akOther)
endFunction

; int Faction::GetStolenItemValueCrime(...) native
int function Faction_GetStolenItemValueCrime() global
	int session = Costile2.GetSession("Faction_GetStolenItemValueCrime")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetStolenItemValueCrime()
endFunction

; int Faction::GetStolenItemValueNoCrime(...) native
int function Faction_GetStolenItemValueNoCrime() global
	int session = Costile2.GetSession("Faction_GetStolenItemValueNoCrime")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetStolenItemValueNoCrime()
endFunction

; bool Faction::IsFactionInCrimeGroup(...) native
bool function Faction_IsFactionInCrimeGroup() global
	int session = Costile2.GetSession("Faction_IsFactionInCrimeGroup")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	faction cdcg_akOther = Costile2.GetTESForm(session, 1) as faction
	return this.IsFactionInCrimeGroup(cdcg_akOther)
endFunction

; bool Faction::IsPlayerExpelled(...) native
bool function Faction_IsPlayerExpelled() global
	int session = Costile2.GetSession("Faction_IsPlayerExpelled")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.IsPlayerExpelled()
endFunction

; void Faction::ModCrimeGold(...) native
function Faction_ModCrimeGold() global
	int session = Costile2.GetSession("Faction_ModCrimeGold")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	int cdcg_aiAmount = Costile2.GetInt(session, 1)
	bool cdcg_abViolent = Costile2.GetBool(session, 2)
	this.ModCrimeGold(cdcg_aiAmount, cdcg_abViolent)
endFunction

; void Faction::ModReaction(...) native
function Faction_ModReaction() global
	int session = Costile2.GetSession("Faction_ModReaction")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	faction cdcg_akOther = Costile2.GetTESForm(session, 1) as faction
	int cdcg_aiAmount = Costile2.GetInt(session, 2)
	this.ModReaction(cdcg_akOther, cdcg_aiAmount)
endFunction

; void Faction::PlayerPayCrimeGold(...) native
function Faction_PlayerPayCrimeGold() global
	int session = Costile2.GetSession("Faction_PlayerPayCrimeGold")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	bool cdcg_abRemoveStolenItems = Costile2.GetBool(session, 1)
	bool cdcg_abGoToJail = Costile2.GetBool(session, 2)
	this.PlayerPayCrimeGold(cdcg_abRemoveStolenItems, cdcg_abGoToJail)
endFunction

; void Faction::SendAssaultAlarm(...) native
function Faction_SendAssaultAlarm() global
	int session = Costile2.GetSession("Faction_SendAssaultAlarm")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	this.SendAssaultAlarm()
endFunction

; void Faction::SendPlayerToJail(...) native
function Faction_SendPlayerToJail() global
	int session = Costile2.GetSession("Faction_SendPlayerToJail")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	bool cdcg_abRemoveInventory = Costile2.GetBool(session, 1)
	bool cdcg_abRealJail = Costile2.GetBool(session, 2)
	this.SendPlayerToJail(cdcg_abRemoveInventory, cdcg_abRealJail)
endFunction

; void Faction::SetAlly(...) native
function Faction_SetAlly() global
	int session = Costile2.GetSession("Faction_SetAlly")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	faction cdcg_akOther = Costile2.GetTESForm(session, 1) as faction
	bool cdcg_abSelfIsFriendToOther = Costile2.GetBool(session, 2)
	bool cdcg_abOtherIsFriendToSelf = Costile2.GetBool(session, 3)
	this.SetAlly(cdcg_akOther, cdcg_abSelfIsFriendToOther, cdcg_abOtherIsFriendToSelf)
endFunction

; void Faction::SetCrimeGold(...) native
function Faction_SetCrimeGold() global
	int session = Costile2.GetSession("Faction_SetCrimeGold")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	int cdcg_aiGold = Costile2.GetInt(session, 1)
	this.SetCrimeGold(cdcg_aiGold)
endFunction

; void Faction::SetCrimeGoldViolent(...) native
function Faction_SetCrimeGoldViolent() global
	int session = Costile2.GetSession("Faction_SetCrimeGoldViolent")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	int cdcg_aiGold = Costile2.GetInt(session, 1)
	this.SetCrimeGoldViolent(cdcg_aiGold)
endFunction

; void Faction::SetEnemy(...) native
function Faction_SetEnemy() global
	int session = Costile2.GetSession("Faction_SetEnemy")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	faction cdcg_akOther = Costile2.GetTESForm(session, 1) as faction
	bool cdcg_abSelfIsNeutralToOther = Costile2.GetBool(session, 2)
	bool cdcg_abOtherIsNeutralToSelf = Costile2.GetBool(session, 3)
	this.SetEnemy(cdcg_akOther, cdcg_abSelfIsNeutralToOther, cdcg_abOtherIsNeutralToSelf)
endFunction

; void Faction::SetPlayerEnemy(...) native
function Faction_SetPlayerEnemy() global
	int session = Costile2.GetSession("Faction_SetPlayerEnemy")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	bool cdcg_abIsEnemy = Costile2.GetBool(session, 1)
	this.SetPlayerEnemy(cdcg_abIsEnemy)
endFunction

; void Faction::SetPlayerExpelled(...) native
function Faction_SetPlayerExpelled() global
	int session = Costile2.GetSession("Faction_SetPlayerExpelled")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	bool cdcg_abIsExpelled = Costile2.GetBool(session, 1)
	this.SetPlayerExpelled(cdcg_abIsExpelled)
endFunction

; void Faction::SetReaction(...) native
function Faction_SetReaction() global
	int session = Costile2.GetSession("Faction_SetReaction")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	faction cdcg_akOther = Costile2.GetTESForm(session, 1) as faction
	int cdcg_aiNewValue = Costile2.GetInt(session, 2)
	this.SetReaction(cdcg_akOther, cdcg_aiNewValue)
endFunction

; void Faction::MakeVendor(...)
function Faction_MakeVendor() global
	int session = Costile2.GetSession("Faction_MakeVendor")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	this.MakeVendor()
endFunction

; bool Faction::IsVendor(...)
bool function Faction_IsVendor() global
	int session = Costile2.GetSession("Faction_IsVendor")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.IsVendor()
endFunction

; void Faction::ClearVendor(...)
function Faction_ClearVendor() global
	int session = Costile2.GetSession("Faction_ClearVendor")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	this.ClearVendor()
endFunction

; bool Faction::IsFactionFlagSet(...) native
bool function Faction_IsFactionFlagSet() global
	int session = Costile2.GetSession("Faction_IsFactionFlagSet")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	int cdcg_flag = Costile2.GetInt(session, 1)
	return this.IsFactionFlagSet(cdcg_flag)
endFunction

; void Faction::SetFactionFlag(...) native
function Faction_SetFactionFlag() global
	int session = Costile2.GetSession("Faction_SetFactionFlag")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	int cdcg_flag = Costile2.GetInt(session, 1)
	this.SetFactionFlag(cdcg_flag)
endFunction

; void Faction::ClearFactionFlag(...) native
function Faction_ClearFactionFlag() global
	int session = Costile2.GetSession("Faction_ClearFactionFlag")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	int cdcg_flag = Costile2.GetInt(session, 1)
	this.ClearFactionFlag(cdcg_flag)
endFunction

; bool Faction::OnlyBuysStolenItems(...) native
bool function Faction_OnlyBuysStolenItems() global
	int session = Costile2.GetSession("Faction_OnlyBuysStolenItems")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.OnlyBuysStolenItems()
endFunction

; void Faction::SetOnlyBuysStolenItems(...) native
function Faction_SetOnlyBuysStolenItems() global
	int session = Costile2.GetSession("Faction_SetOnlyBuysStolenItems")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	bool cdcg_onlyStolen = Costile2.GetBool(session, 1)
	this.SetOnlyBuysStolenItems(cdcg_onlyStolen)
endFunction

; int Faction::GetVendorStartHour(...) native
int function Faction_GetVendorStartHour() global
	int session = Costile2.GetSession("Faction_GetVendorStartHour")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetVendorStartHour()
endFunction

; void Faction::SetVendorStartHour(...) native
function Faction_SetVendorStartHour() global
	int session = Costile2.GetSession("Faction_SetVendorStartHour")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	int cdcg_hour = Costile2.GetInt(session, 1)
	this.SetVendorStartHour(cdcg_hour)
endFunction

; int Faction::GetVendorEndHour(...) native
int function Faction_GetVendorEndHour() global
	int session = Costile2.GetSession("Faction_GetVendorEndHour")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetVendorEndHour()
endFunction

; void Faction::SetVendorEndHour(...) native
function Faction_SetVendorEndHour() global
	int session = Costile2.GetSession("Faction_SetVendorEndHour")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	int cdcg_hour = Costile2.GetInt(session, 1)
	this.SetVendorEndHour(cdcg_hour)
endFunction

; int Faction::GetVendorRadius(...) native
int function Faction_GetVendorRadius() global
	int session = Costile2.GetSession("Faction_GetVendorRadius")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetVendorRadius()
endFunction

; void Faction::SetVendorRadius(...) native
function Faction_SetVendorRadius() global
	int session = Costile2.GetSession("Faction_SetVendorRadius")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	int cdcg_radius = Costile2.GetInt(session, 1)
	this.SetVendorRadius(cdcg_radius)
endFunction

; objectreference Faction::GetMerchantContainer(...) native
objectreference function Faction_GetMerchantContainer() global
	int session = Costile2.GetSession("Faction_GetMerchantContainer")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetMerchantContainer()
endFunction

; void Faction::SetMerchantContainer(...) native
function Faction_SetMerchantContainer() global
	int session = Costile2.GetSession("Faction_SetMerchantContainer")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	objectreference cdcg_akContainer = Costile2.GetTESForm(session, 1) as objectreference
	this.SetMerchantContainer(cdcg_akContainer)
endFunction

; bool Faction::IsNotSellBuy(...) native
bool function Faction_IsNotSellBuy() global
	int session = Costile2.GetSession("Faction_IsNotSellBuy")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.IsNotSellBuy()
endFunction

; void Faction::SetNotSellBuy(...) native
function Faction_SetNotSellBuy() global
	int session = Costile2.GetSession("Faction_SetNotSellBuy")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	bool cdcg_notSellBuy = Costile2.GetBool(session, 1)
	this.SetNotSellBuy(cdcg_notSellBuy)
endFunction

; formlist Faction::GetBuySellList(...) native
formlist function Faction_GetBuySellList() global
	int session = Costile2.GetSession("Faction_GetBuySellList")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	return this.GetBuySellList()
endFunction

; void Faction::SetBuySellList(...) native
function Faction_SetBuySellList() global
	int session = Costile2.GetSession("Faction_SetBuySellList")
	Faction this = Costile2.GetTESForm(session, 0) as Faction
	formlist cdcg_akList = Costile2.GetTESForm(session, 1) as formlist
	this.SetBuySellList(cdcg_akList)
endFunction

