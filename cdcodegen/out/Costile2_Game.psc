Scriptname Costile2_Game Hidden

;
;
;
; script Game.psc
;
;
;

; void Game::AddAchievement(...) native global
function Game_AddAchievement() global
	int session = Costile2.GetSession("Game_AddAchievement")
	int cdcg_aiAchievementID = Costile2.GetInt(session, 0)
	Game.AddAchievement(cdcg_aiAchievementID)
endFunction

; void Game::AddPerkPoints(...) native global
function Game_AddPerkPoints() global
	int session = Costile2.GetSession("Game_AddPerkPoints")
	int cdcg_aiPerkPoints = Costile2.GetInt(session, 0)
	Game.AddPerkPoints(cdcg_aiPerkPoints)
endFunction

; void Game::AdvanceSkill(...) native global
function Game_AdvanceSkill() global
	int session = Costile2.GetSession("Game_AdvanceSkill")
	string cdcg_asSkillName = Costile2.GetString(session, 0)
	float cdcg_afMagnitude = Costile2.GetFloat(session, 1)
	Game.AdvanceSkill(cdcg_asSkillName, cdcg_afMagnitude)
endFunction

; bool Game::AddHavokBallAndSocketConstraint(...) native global
bool function Game_AddHavokBallAndSocketConstraint() global
	int session = Costile2.GetSession("Game_AddHavokBallAndSocketConstraint")
	objectreference cdcg_arRefA = Costile2.GetTESForm(session, 0) as objectreference
	string cdcg_arRefANode = Costile2.GetString(session, 1)
	objectreference cdcg_arRefB = Costile2.GetTESForm(session, 2) as objectreference
	string cdcg_arRefBNode = Costile2.GetString(session, 3)
	float cdcg_afRefALocalOffsetX = Costile2.GetFloat(session, 4)
	float cdcg_afRefALocalOffsetY = Costile2.GetFloat(session, 5)
	float cdcg_afRefALocalOffsetZ = Costile2.GetFloat(session, 6)
	float cdcg_afRefBLocalOffsetX = Costile2.GetFloat(session, 7)
	float cdcg_afRefBLocalOffsetY = Costile2.GetFloat(session, 8)
	float cdcg_afRefBLocalOffsetZ = Costile2.GetFloat(session, 9)
	return Game.AddHavokBallAndSocketConstraint(cdcg_arRefA, cdcg_arRefANode, cdcg_arRefB, cdcg_arRefBNode, cdcg_afRefALocalOffsetX, cdcg_afRefALocalOffsetY, cdcg_afRefALocalOffsetZ, cdcg_afRefBLocalOffsetX, cdcg_afRefBLocalOffsetY, cdcg_afRefBLocalOffsetZ)
endFunction

; bool Game::RemoveHavokConstraints(...) native global
bool function Game_RemoveHavokConstraints() global
	int session = Costile2.GetSession("Game_RemoveHavokConstraints")
	objectreference cdcg_arFirstRef = Costile2.GetTESForm(session, 0) as objectreference
	string cdcg_arFirstRefNodeName = Costile2.GetString(session, 1)
	objectreference cdcg_arSecondRef = Costile2.GetTESForm(session, 2) as objectreference
	string cdcg_arSecondRefNodeName = Costile2.GetString(session, 3)
	return Game.RemoveHavokConstraints(cdcg_arFirstRef, cdcg_arFirstRefNodeName, cdcg_arSecondRef, cdcg_arSecondRefNodeName)
endFunction

; int Game::CalculateFavorCost(...) native global
int function Game_CalculateFavorCost() global
	int session = Costile2.GetSession("Game_CalculateFavorCost")
	int cdcg_aiFavorPrice = Costile2.GetInt(session, 0)
	return Game.CalculateFavorCost(cdcg_aiFavorPrice)
endFunction

; void Game::ClearPrison(...) native global
function Game_ClearPrison() global
	int session = Costile2.GetSession("Game_ClearPrison")
	Game.ClearPrison()
endFunction

; void Game::ClearTempEffects(...) native global
function Game_ClearTempEffects() global
	int session = Costile2.GetSession("Game_ClearTempEffects")
	Game.ClearTempEffects()
endFunction

; void Game::DisablePlayerControls(...) native global
function Game_DisablePlayerControls() global
	int session = Costile2.GetSession("Game_DisablePlayerControls")
	bool cdcg_abMovement = Costile2.GetBool(session, 0)
	bool cdcg_abFighting = Costile2.GetBool(session, 1)
	bool cdcg_abCamSwitch = Costile2.GetBool(session, 2)
	bool cdcg_abLooking = Costile2.GetBool(session, 3)
	bool cdcg_abSneaking = Costile2.GetBool(session, 4)
	bool cdcg_abMenu = Costile2.GetBool(session, 5)
	bool cdcg_abActivate = Costile2.GetBool(session, 6)
	bool cdcg_abJournalTabs = Costile2.GetBool(session, 7)
	int cdcg_aiDisablePOVType = Costile2.GetInt(session, 8)
	Game.DisablePlayerControls(cdcg_abMovement, cdcg_abFighting, cdcg_abCamSwitch, cdcg_abLooking, cdcg_abSneaking, cdcg_abMenu, cdcg_abActivate, cdcg_abJournalTabs, cdcg_aiDisablePOVType)
endFunction

; void Game::EnablePlayerControls(...) native global
function Game_EnablePlayerControls() global
	int session = Costile2.GetSession("Game_EnablePlayerControls")
	bool cdcg_abMovement = Costile2.GetBool(session, 0)
	bool cdcg_abFighting = Costile2.GetBool(session, 1)
	bool cdcg_abCamSwitch = Costile2.GetBool(session, 2)
	bool cdcg_abLooking = Costile2.GetBool(session, 3)
	bool cdcg_abSneaking = Costile2.GetBool(session, 4)
	bool cdcg_abMenu = Costile2.GetBool(session, 5)
	bool cdcg_abActivate = Costile2.GetBool(session, 6)
	bool cdcg_abJournalTabs = Costile2.GetBool(session, 7)
	int cdcg_aiDisablePOVType = Costile2.GetInt(session, 8)
	Game.EnablePlayerControls(cdcg_abMovement, cdcg_abFighting, cdcg_abCamSwitch, cdcg_abLooking, cdcg_abSneaking, cdcg_abMenu, cdcg_abActivate, cdcg_abJournalTabs, cdcg_aiDisablePOVType)
endFunction

; void Game::EnableFastTravel(...) native global
function Game_EnableFastTravel() global
	int session = Costile2.GetSession("Game_EnableFastTravel")
	bool cdcg_abEnable = Costile2.GetBool(session, 0)
	Game.EnableFastTravel(cdcg_abEnable)
endFunction

; void Game::FadeOutGame(...) native global
function Game_FadeOutGame() global
	int session = Costile2.GetSession("Game_FadeOutGame")
	bool cdcg_abFadingOut = Costile2.GetBool(session, 0)
	bool cdcg_abBlackFade = Costile2.GetBool(session, 1)
	float cdcg_afSecsBeforeFade = Costile2.GetFloat(session, 2)
	float cdcg_afFadeDuration = Costile2.GetFloat(session, 3)
	Game.FadeOutGame(cdcg_abFadingOut, cdcg_abBlackFade, cdcg_afSecsBeforeFade, cdcg_afFadeDuration)
endFunction

; void Game::FastTravel(...) native global
function Game_FastTravel() global
	int session = Costile2.GetSession("Game_FastTravel")
	objectreference cdcg_akDestination = Costile2.GetTESForm(session, 0) as objectreference
	Game.FastTravel(cdcg_akDestination)
endFunction

; objectreference Game::FindClosestReferenceOfType(...) native global
objectreference function Game_FindClosestReferenceOfType() global
	int session = Costile2.GetSession("Game_FindClosestReferenceOfType")
	form cdcg_arBaseObject = Costile2.GetTESForm(session, 0) as form
	float cdcg_afX = Costile2.GetFloat(session, 1)
	float cdcg_afY = Costile2.GetFloat(session, 2)
	float cdcg_afZ = Costile2.GetFloat(session, 3)
	float cdcg_afRadius = Costile2.GetFloat(session, 4)
	return Game.FindClosestReferenceOfType(cdcg_arBaseObject, cdcg_afX, cdcg_afY, cdcg_afZ, cdcg_afRadius)
endFunction

; objectreference Game::FindRandomReferenceOfType(...) native global
objectreference function Game_FindRandomReferenceOfType() global
	int session = Costile2.GetSession("Game_FindRandomReferenceOfType")
	form cdcg_arBaseObject = Costile2.GetTESForm(session, 0) as form
	float cdcg_afX = Costile2.GetFloat(session, 1)
	float cdcg_afY = Costile2.GetFloat(session, 2)
	float cdcg_afZ = Costile2.GetFloat(session, 3)
	float cdcg_afRadius = Costile2.GetFloat(session, 4)
	return Game.FindRandomReferenceOfType(cdcg_arBaseObject, cdcg_afX, cdcg_afY, cdcg_afZ, cdcg_afRadius)
endFunction

; objectreference Game::FindClosestReferenceOfAnyTypeInList(...) native global
objectreference function Game_FindClosestReferenceOfAnyTypeInList() global
	int session = Costile2.GetSession("Game_FindClosestReferenceOfAnyTypeInList")
	formlist cdcg_arBaseObjects = Costile2.GetTESForm(session, 0) as formlist
	float cdcg_afX = Costile2.GetFloat(session, 1)
	float cdcg_afY = Costile2.GetFloat(session, 2)
	float cdcg_afZ = Costile2.GetFloat(session, 3)
	float cdcg_afRadius = Costile2.GetFloat(session, 4)
	return Game.FindClosestReferenceOfAnyTypeInList(cdcg_arBaseObjects, cdcg_afX, cdcg_afY, cdcg_afZ, cdcg_afRadius)
endFunction

; objectreference Game::FindRandomReferenceOfAnyTypeInList(...) native global
objectreference function Game_FindRandomReferenceOfAnyTypeInList() global
	int session = Costile2.GetSession("Game_FindRandomReferenceOfAnyTypeInList")
	formlist cdcg_arBaseObjects = Costile2.GetTESForm(session, 0) as formlist
	float cdcg_afX = Costile2.GetFloat(session, 1)
	float cdcg_afY = Costile2.GetFloat(session, 2)
	float cdcg_afZ = Costile2.GetFloat(session, 3)
	float cdcg_afRadius = Costile2.GetFloat(session, 4)
	return Game.FindRandomReferenceOfAnyTypeInList(cdcg_arBaseObjects, cdcg_afX, cdcg_afY, cdcg_afZ, cdcg_afRadius)
endFunction

; objectreference Game::FindClosestReferenceOfTypeFromRef(...) global
objectreference function Game_FindClosestReferenceOfTypeFromRef() global
	int session = Costile2.GetSession("Game_FindClosestReferenceOfTypeFromRef")
	form cdcg_arBaseObject = Costile2.GetTESForm(session, 0) as form
	objectreference cdcg_arCenter = Costile2.GetTESForm(session, 1) as objectreference
	float cdcg_afRadius = Costile2.GetFloat(session, 2)
	return Game.FindClosestReferenceOfTypeFromRef(cdcg_arBaseObject, cdcg_arCenter, cdcg_afRadius)
endFunction

; objectreference Game::FindRandomReferenceOfTypeFromRef(...) global
objectreference function Game_FindRandomReferenceOfTypeFromRef() global
	int session = Costile2.GetSession("Game_FindRandomReferenceOfTypeFromRef")
	form cdcg_arBaseObject = Costile2.GetTESForm(session, 0) as form
	objectreference cdcg_arCenter = Costile2.GetTESForm(session, 1) as objectreference
	float cdcg_afRadius = Costile2.GetFloat(session, 2)
	return Game.FindRandomReferenceOfTypeFromRef(cdcg_arBaseObject, cdcg_arCenter, cdcg_afRadius)
endFunction

; objectreference Game::FindClosestReferenceOfAnyTypeInListFromRef(...) global
objectreference function Game_FindClosestReferenceOfAnyTypeInListFromRef() global
	int session = Costile2.GetSession("Game_FindClosestReferenceOfAnyTypeInListFromRef")
	formlist cdcg_arBaseObjects = Costile2.GetTESForm(session, 0) as formlist
	objectreference cdcg_arCenter = Costile2.GetTESForm(session, 1) as objectreference
	float cdcg_afRadius = Costile2.GetFloat(session, 2)
	return Game.FindClosestReferenceOfAnyTypeInListFromRef(cdcg_arBaseObjects, cdcg_arCenter, cdcg_afRadius)
endFunction

; objectreference Game::FindRandomReferenceOfAnyTypeInListFromRef(...) global
objectreference function Game_FindRandomReferenceOfAnyTypeInListFromRef() global
	int session = Costile2.GetSession("Game_FindRandomReferenceOfAnyTypeInListFromRef")
	formlist cdcg_arBaseObjects = Costile2.GetTESForm(session, 0) as formlist
	objectreference cdcg_arCenter = Costile2.GetTESForm(session, 1) as objectreference
	float cdcg_afRadius = Costile2.GetFloat(session, 2)
	return Game.FindRandomReferenceOfAnyTypeInListFromRef(cdcg_arBaseObjects, cdcg_arCenter, cdcg_afRadius)
endFunction

; actor Game::FindClosestActor(...) native global
actor function Game_FindClosestActor() global
	int session = Costile2.GetSession("Game_FindClosestActor")
	float cdcg_afX = Costile2.GetFloat(session, 0)
	float cdcg_afY = Costile2.GetFloat(session, 1)
	float cdcg_afZ = Costile2.GetFloat(session, 2)
	float cdcg_afRadius = Costile2.GetFloat(session, 3)
	return Game.FindClosestActor(cdcg_afX, cdcg_afY, cdcg_afZ, cdcg_afRadius)
endFunction

; actor Game::FindRandomActor(...) native global
actor function Game_FindRandomActor() global
	int session = Costile2.GetSession("Game_FindRandomActor")
	float cdcg_afX = Costile2.GetFloat(session, 0)
	float cdcg_afY = Costile2.GetFloat(session, 1)
	float cdcg_afZ = Costile2.GetFloat(session, 2)
	float cdcg_afRadius = Costile2.GetFloat(session, 3)
	return Game.FindRandomActor(cdcg_afX, cdcg_afY, cdcg_afZ, cdcg_afRadius)
endFunction

; actor Game::FindClosestActorFromRef(...) global
actor function Game_FindClosestActorFromRef() global
	int session = Costile2.GetSession("Game_FindClosestActorFromRef")
	objectreference cdcg_arCenter = Costile2.GetTESForm(session, 0) as objectreference
	float cdcg_afRadius = Costile2.GetFloat(session, 1)
	return Game.FindClosestActorFromRef(cdcg_arCenter, cdcg_afRadius)
endFunction

; actor Game::FindRandomActorFromRef(...) global
actor function Game_FindRandomActorFromRef() global
	int session = Costile2.GetSession("Game_FindRandomActorFromRef")
	objectreference cdcg_arCenter = Costile2.GetTESForm(session, 0) as objectreference
	float cdcg_afRadius = Costile2.GetFloat(session, 1)
	return Game.FindRandomActorFromRef(cdcg_arCenter, cdcg_afRadius)
endFunction

; void Game::ForceThirdPerson(...) native global
function Game_ForceThirdPerson() global
	int session = Costile2.GetSession("Game_ForceThirdPerson")
	Game.ForceThirdPerson()
endFunction

; void Game::ForceFirstPerson(...) native global
function Game_ForceFirstPerson() global
	int session = Costile2.GetSession("Game_ForceFirstPerson")
	Game.ForceFirstPerson()
endFunction

; void Game::ShowFirstPersonGeometry(...) native global
function Game_ShowFirstPersonGeometry() global
	int session = Costile2.GetSession("Game_ShowFirstPersonGeometry")
	bool cdcg_abShow = Costile2.GetBool(session, 0)
	Game.ShowFirstPersonGeometry(cdcg_abShow)
endFunction

; form Game::GetForm(...) native global
form function Game_GetForm() global
	int session = Costile2.GetSession("Game_GetForm")
	int cdcg_aiFormID = Costile2.GetInt(session, 0)
	return Game.GetForm(cdcg_aiFormID)
endFunction

; form Game::GetFormFromFile(...) native global
form function Game_GetFormFromFile() global
	int session = Costile2.GetSession("Game_GetFormFromFile")
	int cdcg_aiFormID = Costile2.GetInt(session, 0)
	string cdcg_asFilename = Costile2.GetString(session, 1)
	return Game.GetFormFromFile(cdcg_aiFormID, cdcg_asFilename)
endFunction

; float Game::GetGameSettingFloat(...) native global
float function Game_GetGameSettingFloat() global
	int session = Costile2.GetSession("Game_GetGameSettingFloat")
	string cdcg_asGameSetting = Costile2.GetString(session, 0)
	return Game.GetGameSettingFloat(cdcg_asGameSetting)
endFunction

; int Game::GetGameSettingInt(...) native global
int function Game_GetGameSettingInt() global
	int session = Costile2.GetSession("Game_GetGameSettingInt")
	string cdcg_asGameSetting = Costile2.GetString(session, 0)
	return Game.GetGameSettingInt(cdcg_asGameSetting)
endFunction

; string Game::GetGameSettingString(...) native global
string function Game_GetGameSettingString() global
	int session = Costile2.GetSession("Game_GetGameSettingString")
	string cdcg_asGameSetting = Costile2.GetString(session, 0)
	return Game.GetGameSettingString(cdcg_asGameSetting)
endFunction

; actor Game::GetPlayer(...) native global
actor function Game_GetPlayer() global
	int session = Costile2.GetSession("Game_GetPlayer")
	return Game.GetPlayer()
endFunction

; objectreference Game::GetPlayerGrabbedRef(...) native global
objectreference function Game_GetPlayerGrabbedRef() global
	int session = Costile2.GetSession("Game_GetPlayerGrabbedRef")
	return Game.GetPlayerGrabbedRef()
endFunction

; actor Game::GetPlayersLastRiddenHorse(...) native global
actor function Game_GetPlayersLastRiddenHorse() global
	int session = Costile2.GetSession("Game_GetPlayersLastRiddenHorse")
	return Game.GetPlayersLastRiddenHorse()
endFunction

; float Game::GetSunPositionX(...) native global
float function Game_GetSunPositionX() global
	int session = Costile2.GetSession("Game_GetSunPositionX")
	return Game.GetSunPositionX()
endFunction

; float Game::GetSunPositionY(...) native global
float function Game_GetSunPositionY() global
	int session = Costile2.GetSession("Game_GetSunPositionY")
	return Game.GetSunPositionY()
endFunction

; float Game::GetSunPositionZ(...) native global
float function Game_GetSunPositionZ() global
	int session = Costile2.GetSession("Game_GetSunPositionZ")
	return Game.GetSunPositionZ()
endFunction

; float Game::GetRealHoursPassed(...) native global
float function Game_GetRealHoursPassed() global
	int session = Costile2.GetSession("Game_GetRealHoursPassed")
	return Game.GetRealHoursPassed()
endFunction

; void Game::IncrementSkill(...) native global
function Game_IncrementSkill() global
	int session = Costile2.GetSession("Game_IncrementSkill")
	string cdcg_asSkillName = Costile2.GetString(session, 0)
	Game.IncrementSkill(cdcg_asSkillName)
endFunction

; void Game::IncrementSkillBy(...) native global
function Game_IncrementSkillBy() global
	int session = Costile2.GetSession("Game_IncrementSkillBy")
	string cdcg_asSkillName = Costile2.GetString(session, 0)
	int cdcg_aiCount = Costile2.GetInt(session, 1)
	Game.IncrementSkillBy(cdcg_asSkillName, cdcg_aiCount)
endFunction

; void Game::IncrementStat(...) native global
function Game_IncrementStat() global
	int session = Costile2.GetSession("Game_IncrementStat")
	string cdcg_asStatName = Costile2.GetString(session, 0)
	int cdcg_aiModAmount = Costile2.GetInt(session, 1)
	Game.IncrementStat(cdcg_asStatName, cdcg_aiModAmount)
endFunction

; bool Game::IsActivateControlsEnabled(...) native global
bool function Game_IsActivateControlsEnabled() global
	int session = Costile2.GetSession("Game_IsActivateControlsEnabled")
	return Game.IsActivateControlsEnabled()
endFunction

; bool Game::IsCamSwitchControlsEnabled(...) native global
bool function Game_IsCamSwitchControlsEnabled() global
	int session = Costile2.GetSession("Game_IsCamSwitchControlsEnabled")
	return Game.IsCamSwitchControlsEnabled()
endFunction

; bool Game::IsFastTravelControlsEnabled(...) native global
bool function Game_IsFastTravelControlsEnabled() global
	int session = Costile2.GetSession("Game_IsFastTravelControlsEnabled")
	return Game.IsFastTravelControlsEnabled()
endFunction

; bool Game::IsFastTravelEnabled(...) native global
bool function Game_IsFastTravelEnabled() global
	int session = Costile2.GetSession("Game_IsFastTravelEnabled")
	return Game.IsFastTravelEnabled()
endFunction

; bool Game::IsFightingControlsEnabled(...) native global
bool function Game_IsFightingControlsEnabled() global
	int session = Costile2.GetSession("Game_IsFightingControlsEnabled")
	return Game.IsFightingControlsEnabled()
endFunction

; bool Game::IsJournalControlsEnabled(...) native global
bool function Game_IsJournalControlsEnabled() global
	int session = Costile2.GetSession("Game_IsJournalControlsEnabled")
	return Game.IsJournalControlsEnabled()
endFunction

; bool Game::IsLookingControlsEnabled(...) native global
bool function Game_IsLookingControlsEnabled() global
	int session = Costile2.GetSession("Game_IsLookingControlsEnabled")
	return Game.IsLookingControlsEnabled()
endFunction

; bool Game::IsMenuControlsEnabled(...) native global
bool function Game_IsMenuControlsEnabled() global
	int session = Costile2.GetSession("Game_IsMenuControlsEnabled")
	return Game.IsMenuControlsEnabled()
endFunction

; bool Game::IsMovementControlsEnabled(...) native global
bool function Game_IsMovementControlsEnabled() global
	int session = Costile2.GetSession("Game_IsMovementControlsEnabled")
	return Game.IsMovementControlsEnabled()
endFunction

; bool Game::IsPlayerSungazing(...) native global
bool function Game_IsPlayerSungazing() global
	int session = Costile2.GetSession("Game_IsPlayerSungazing")
	return Game.IsPlayerSungazing()
endFunction

; bool Game::IsSneakingControlsEnabled(...) native global
bool function Game_IsSneakingControlsEnabled() global
	int session = Costile2.GetSession("Game_IsSneakingControlsEnabled")
	return Game.IsSneakingControlsEnabled()
endFunction

; bool Game::IsWordUnlocked(...) native global
bool function Game_IsWordUnlocked() global
	int session = Costile2.GetSession("Game_IsWordUnlocked")
	wordofpower cdcg_akWord = Costile2.GetTESForm(session, 0) as wordofpower
	return Game.IsWordUnlocked(cdcg_akWord)
endFunction

; void Game::PlayBink(...) native global
function Game_PlayBink() global
	int session = Costile2.GetSession("Game_PlayBink")
	string cdcg_asFileName = Costile2.GetString(session, 0)
	bool cdcg_abInterruptible = Costile2.GetBool(session, 1)
	bool cdcg_abMuteAudio = Costile2.GetBool(session, 2)
	bool cdcg_abMuteMusic = Costile2.GetBool(session, 3)
	bool cdcg_abLetterbox = Costile2.GetBool(session, 4)
	Game.PlayBink(cdcg_asFileName, cdcg_abInterruptible, cdcg_abMuteAudio, cdcg_abMuteMusic, cdcg_abLetterbox)
endFunction

; void Game::PrecacheCharGen(...) native global
function Game_PrecacheCharGen() global
	int session = Costile2.GetSession("Game_PrecacheCharGen")
	Game.PrecacheCharGen()
endFunction

; void Game::PrecacheCharGenClear(...) native global
function Game_PrecacheCharGenClear() global
	int session = Costile2.GetSession("Game_PrecacheCharGenClear")
	Game.PrecacheCharGenClear()
endFunction

; int Game::QueryStat(...) native global
int function Game_QueryStat() global
	int session = Costile2.GetSession("Game_QueryStat")
	string cdcg_asStat = Costile2.GetString(session, 0)
	return Game.QueryStat(cdcg_asStat)
endFunction

; void Game::QuitToMainMenu(...) native global
function Game_QuitToMainMenu() global
	int session = Costile2.GetSession("Game_QuitToMainMenu")
	Game.QuitToMainMenu()
endFunction

; void Game::RequestAutoSave(...) native global
function Game_RequestAutoSave() global
	int session = Costile2.GetSession("Game_RequestAutoSave")
	Game.RequestAutoSave()
endFunction

; void Game::RequestModel(...) native global
function Game_RequestModel() global
	int session = Costile2.GetSession("Game_RequestModel")
	string cdcg_asModelName = Costile2.GetString(session, 0)
	Game.RequestModel(cdcg_asModelName)
endFunction

; void Game::RequestSave(...) native global
function Game_RequestSave() global
	int session = Costile2.GetSession("Game_RequestSave")
	Game.RequestSave()
endFunction

; void Game::ServeTime(...) native global
function Game_ServeTime() global
	int session = Costile2.GetSession("Game_ServeTime")
	Game.ServeTime()
endFunction

; void Game::SendWereWolfTransformation(...) native global
function Game_SendWereWolfTransformation() global
	int session = Costile2.GetSession("Game_SendWereWolfTransformation")
	Game.SendWereWolfTransformation()
endFunction

; void Game::SetBeastForm(...) native global
function Game_SetBeastForm() global
	int session = Costile2.GetSession("Game_SetBeastForm")
	bool cdcg_abEntering = Costile2.GetBool(session, 0)
	Game.SetBeastForm(cdcg_abEntering)
endFunction

; void Game::SetCameraTarget(...) native global
function Game_SetCameraTarget() global
	int session = Costile2.GetSession("Game_SetCameraTarget")
	actor cdcg_arTarget = Costile2.GetTESForm(session, 0) as actor
	Game.SetCameraTarget(cdcg_arTarget)
endFunction

; void Game::SetHudCartMode(...) native global
function Game_SetHudCartMode() global
	int session = Costile2.GetSession("Game_SetHudCartMode")
	bool cdcg_abSetCartMode = Costile2.GetBool(session, 0)
	Game.SetHudCartMode(cdcg_abSetCartMode)
endFunction

; void Game::SetInChargen(...) native global
function Game_SetInChargen() global
	int session = Costile2.GetSession("Game_SetInChargen")
	bool cdcg_abDisableSaving = Costile2.GetBool(session, 0)
	bool cdcg_abDisableWaiting = Costile2.GetBool(session, 1)
	bool cdcg_abShowControlsDisabledMessage = Costile2.GetBool(session, 2)
	Game.SetInChargen(cdcg_abDisableSaving, cdcg_abDisableWaiting, cdcg_abShowControlsDisabledMessage)
endFunction

; void Game::SetPlayerAIDriven(...) native global
function Game_SetPlayerAIDriven() global
	int session = Costile2.GetSession("Game_SetPlayerAIDriven")
	bool cdcg_abAIDriven = Costile2.GetBool(session, 0)
	Game.SetPlayerAIDriven(cdcg_abAIDriven)
endFunction

; void Game::SetPlayerReportCrime(...) native global
function Game_SetPlayerReportCrime() global
	int session = Costile2.GetSession("Game_SetPlayerReportCrime")
	bool cdcg_abReportCrime = Costile2.GetBool(session, 0)
	Game.SetPlayerReportCrime(cdcg_abReportCrime)
endFunction

; void Game::SetSittingRotation(...) native global
function Game_SetSittingRotation() global
	int session = Costile2.GetSession("Game_SetSittingRotation")
	float cdcg_afValue = Costile2.GetFloat(session, 0)
	Game.SetSittingRotation(cdcg_afValue)
endFunction

; void Game::ShakeCamera(...) native global
function Game_ShakeCamera() global
	int session = Costile2.GetSession("Game_ShakeCamera")
	objectreference cdcg_akSource = Costile2.GetTESForm(session, 0) as objectreference
	float cdcg_afStrength = Costile2.GetFloat(session, 1)
	float cdcg_afDuration = Costile2.GetFloat(session, 2)
	Game.ShakeCamera(cdcg_akSource, cdcg_afStrength, cdcg_afDuration)
endFunction

; void Game::ShakeController(...) native global
function Game_ShakeController() global
	int session = Costile2.GetSession("Game_ShakeController")
	float cdcg_afSmallMotorStrength = Costile2.GetFloat(session, 0)
	float cdcg_afBigMotorStreangth = Costile2.GetFloat(session, 1)
	float cdcg_afDuration = Costile2.GetFloat(session, 2)
	Game.ShakeController(cdcg_afSmallMotorStrength, cdcg_afBigMotorStreangth, cdcg_afDuration)
endFunction

; void Game::ShowRaceMenu(...) native global
function Game_ShowRaceMenu() global
	int session = Costile2.GetSession("Game_ShowRaceMenu")
	Game.ShowRaceMenu()
endFunction

; void Game::ShowLimitedRaceMenu(...) native global
function Game_ShowLimitedRaceMenu() global
	int session = Costile2.GetSession("Game_ShowLimitedRaceMenu")
	Game.ShowLimitedRaceMenu()
endFunction

; void Game::ShowTitleSequenceMenu(...) native global
function Game_ShowTitleSequenceMenu() global
	int session = Costile2.GetSession("Game_ShowTitleSequenceMenu")
	Game.ShowTitleSequenceMenu()
endFunction

; void Game::HideTitleSequenceMenu(...) native global
function Game_HideTitleSequenceMenu() global
	int session = Costile2.GetSession("Game_HideTitleSequenceMenu")
	Game.HideTitleSequenceMenu()
endFunction

; void Game::StartTitleSequence(...) native global
function Game_StartTitleSequence() global
	int session = Costile2.GetSession("Game_StartTitleSequence")
	string cdcg_asSequenceName = Costile2.GetString(session, 0)
	Game.StartTitleSequence(cdcg_asSequenceName)
endFunction

; void Game::SetAllowFlyingMountLandingRequests(...) native global
function Game_SetAllowFlyingMountLandingRequests() global
	int session = Costile2.GetSession("Game_SetAllowFlyingMountLandingRequests")
	bool cdcg_abAllow = Costile2.GetBool(session, 0)
	Game.SetAllowFlyingMountLandingRequests(cdcg_abAllow)
endFunction

; void Game::SetSunGazeImageSpaceModifier(...) native global
function Game_SetSunGazeImageSpaceModifier() global
	int session = Costile2.GetSession("Game_SetSunGazeImageSpaceModifier")
	imagespacemodifier cdcg_apImod = Costile2.GetTESForm(session, 0) as imagespacemodifier
	Game.SetSunGazeImageSpaceModifier(cdcg_apImod)
endFunction

; void Game::ShowTrainingMenu(...) native global
function Game_ShowTrainingMenu() global
	int session = Costile2.GetSession("Game_ShowTrainingMenu")
	actor cdcg_aTrainer = Costile2.GetTESForm(session, 0) as actor
	Game.ShowTrainingMenu(cdcg_aTrainer)
endFunction

; void Game::TeachWord(...) native global
function Game_TeachWord() global
	int session = Costile2.GetSession("Game_TeachWord")
	wordofpower cdcg_akWord = Costile2.GetTESForm(session, 0) as wordofpower
	Game.TeachWord(cdcg_akWord)
endFunction

; void Game::TriggerScreenBlood(...) native global
function Game_TriggerScreenBlood() global
	int session = Costile2.GetSession("Game_TriggerScreenBlood")
	int cdcg_aiValue = Costile2.GetInt(session, 0)
	Game.TriggerScreenBlood(cdcg_aiValue)
endFunction

; void Game::UnlockWord(...) native global
function Game_UnlockWord() global
	int session = Costile2.GetSession("Game_UnlockWord")
	wordofpower cdcg_akWord = Costile2.GetTESForm(session, 0) as wordofpower
	Game.UnlockWord(cdcg_akWord)
endFunction

; bool Game::UsingGamepad(...) native global
bool function Game_UsingGamepad() global
	int session = Costile2.GetSession("Game_UsingGamepad")
	return Game.UsingGamepad()
endFunction

; int Game::GetPerkPoints(...) native global
int function Game_GetPerkPoints() global
	int session = Costile2.GetSession("Game_GetPerkPoints")
	return Game.GetPerkPoints()
endFunction

; void Game::SetPerkPoints(...) native global
function Game_SetPerkPoints() global
	int session = Costile2.GetSession("Game_SetPerkPoints")
	int cdcg_perkPoints = Costile2.GetInt(session, 0)
	Game.SetPerkPoints(cdcg_perkPoints)
endFunction

; void Game::ModPerkPoints(...) native global
function Game_ModPerkPoints() global
	int session = Costile2.GetSession("Game_ModPerkPoints")
	int cdcg_perkPoints = Costile2.GetInt(session, 0)
	Game.ModPerkPoints(cdcg_perkPoints)
endFunction

; int Game::GetModCount(...) native global
int function Game_GetModCount() global
	int session = Costile2.GetSession("Game_GetModCount")
	return Game.GetModCount()
endFunction

; int Game::GetModByName(...) native global
int function Game_GetModByName() global
	int session = Costile2.GetSession("Game_GetModByName")
	string cdcg_name = Costile2.GetString(session, 0)
	return Game.GetModByName(cdcg_name)
endFunction

; string Game::GetModName(...) native global
string function Game_GetModName() global
	int session = Costile2.GetSession("Game_GetModName")
	int cdcg_modIndex = Costile2.GetInt(session, 0)
	return Game.GetModName(cdcg_modIndex)
endFunction

; string Game::GetModAuthor(...) native global
string function Game_GetModAuthor() global
	int session = Costile2.GetSession("Game_GetModAuthor")
	int cdcg_modIndex = Costile2.GetInt(session, 0)
	return Game.GetModAuthor(cdcg_modIndex)
endFunction

; string Game::GetModDescription(...) native global
string function Game_GetModDescription() global
	int session = Costile2.GetSession("Game_GetModDescription")
	int cdcg_modIndex = Costile2.GetInt(session, 0)
	return Game.GetModDescription(cdcg_modIndex)
endFunction

; int Game::GetModDependencyCount(...) native global
int function Game_GetModDependencyCount() global
	int session = Costile2.GetSession("Game_GetModDependencyCount")
	int cdcg_modIndex = Costile2.GetInt(session, 0)
	return Game.GetModDependencyCount(cdcg_modIndex)
endFunction

; int Game::GetNthModDependency(...) native global
int function Game_GetNthModDependency() global
	int session = Costile2.GetSession("Game_GetNthModDependency")
	int cdcg_modIndex = Costile2.GetInt(session, 0)
	int cdcg_n = Costile2.GetInt(session, 1)
	return Game.GetNthModDependency(cdcg_modIndex, cdcg_n)
endFunction

; void Game::SetGameSettingFloat(...) native global
function Game_SetGameSettingFloat() global
	int session = Costile2.GetSession("Game_SetGameSettingFloat")
	string cdcg_setting = Costile2.GetString(session, 0)
	float cdcg_value = Costile2.GetFloat(session, 1)
	Game.SetGameSettingFloat(cdcg_setting, cdcg_value)
endFunction

; void Game::SetGameSettingInt(...) native global
function Game_SetGameSettingInt() global
	int session = Costile2.GetSession("Game_SetGameSettingInt")
	string cdcg_setting = Costile2.GetString(session, 0)
	int cdcg_value = Costile2.GetInt(session, 1)
	Game.SetGameSettingInt(cdcg_setting, cdcg_value)
endFunction

; void Game::SetGameSettingBool(...) native global
function Game_SetGameSettingBool() global
	int session = Costile2.GetSession("Game_SetGameSettingBool")
	string cdcg_setting = Costile2.GetString(session, 0)
	bool cdcg_value = Costile2.GetBool(session, 1)
	Game.SetGameSettingBool(cdcg_setting, cdcg_value)
endFunction

; void Game::SetGameSettingString(...) native global
function Game_SetGameSettingString() global
	int session = Costile2.GetSession("Game_SetGameSettingString")
	string cdcg_setting = Costile2.GetString(session, 0)
	string cdcg_value = Costile2.GetString(session, 1)
	Game.SetGameSettingString(cdcg_setting, cdcg_value)
endFunction

; void Game::SaveGame(...) native global
function Game_SaveGame() global
	int session = Costile2.GetSession("Game_SaveGame")
	string cdcg_name = Costile2.GetString(session, 0)
	Game.SaveGame(cdcg_name)
endFunction

; void Game::LoadGame(...) native global
function Game_LoadGame() global
	int session = Costile2.GetSession("Game_LoadGame")
	string cdcg_name = Costile2.GetString(session, 0)
	Game.LoadGame(cdcg_name)
endFunction

; int Game::GetNumTintMasks(...) native global
int function Game_GetNumTintMasks() global
	int session = Costile2.GetSession("Game_GetNumTintMasks")
	return Game.GetNumTintMasks()
endFunction

; int Game::GetNthTintMaskColor(...) native global
int function Game_GetNthTintMaskColor() global
	int session = Costile2.GetSession("Game_GetNthTintMaskColor")
	int cdcg_n = Costile2.GetInt(session, 0)
	return Game.GetNthTintMaskColor(cdcg_n)
endFunction

; int Game::GetNthTintMaskType(...) native global
int function Game_GetNthTintMaskType() global
	int session = Costile2.GetSession("Game_GetNthTintMaskType")
	int cdcg_n = Costile2.GetInt(session, 0)
	return Game.GetNthTintMaskType(cdcg_n)
endFunction

; void Game::SetNthTintMaskColor(...) native global
function Game_SetNthTintMaskColor() global
	int session = Costile2.GetSession("Game_SetNthTintMaskColor")
	int cdcg_n = Costile2.GetInt(session, 0)
	int cdcg_color = Costile2.GetInt(session, 1)
	Game.SetNthTintMaskColor(cdcg_n, cdcg_color)
endFunction

; string Game::GetNthTintMaskTexturePath(...) native global
string function Game_GetNthTintMaskTexturePath() global
	int session = Costile2.GetSession("Game_GetNthTintMaskTexturePath")
	int cdcg_n = Costile2.GetInt(session, 0)
	return Game.GetNthTintMaskTexturePath(cdcg_n)
endFunction

; void Game::SetNthTintMaskTexturePath(...) native global
function Game_SetNthTintMaskTexturePath() global
	int session = Costile2.GetSession("Game_SetNthTintMaskTexturePath")
	string cdcg_path = Costile2.GetString(session, 0)
	int cdcg_n = Costile2.GetInt(session, 1)
	Game.SetNthTintMaskTexturePath(cdcg_path, cdcg_n)
endFunction

; int Game::GetNumTintsByType(...) native global
int function Game_GetNumTintsByType() global
	int session = Costile2.GetSession("Game_GetNumTintsByType")
	int cdcg_type = Costile2.GetInt(session, 0)
	return Game.GetNumTintsByType(cdcg_type)
endFunction

; int Game::GetTintMaskColor(...) native global
int function Game_GetTintMaskColor() global
	int session = Costile2.GetSession("Game_GetTintMaskColor")
	int cdcg_type = Costile2.GetInt(session, 0)
	int cdcg_index = Costile2.GetInt(session, 1)
	return Game.GetTintMaskColor(cdcg_type, cdcg_index)
endFunction

; void Game::SetTintMaskColor(...) native global
function Game_SetTintMaskColor() global
	int session = Costile2.GetSession("Game_SetTintMaskColor")
	int cdcg_color = Costile2.GetInt(session, 0)
	int cdcg_type = Costile2.GetInt(session, 1)
	int cdcg_index = Costile2.GetInt(session, 2)
	Game.SetTintMaskColor(cdcg_color, cdcg_type, cdcg_index)
endFunction

; string Game::GetTintMaskTexturePath(...) native global
string function Game_GetTintMaskTexturePath() global
	int session = Costile2.GetSession("Game_GetTintMaskTexturePath")
	int cdcg_type = Costile2.GetInt(session, 0)
	int cdcg_index = Costile2.GetInt(session, 1)
	return Game.GetTintMaskTexturePath(cdcg_type, cdcg_index)
endFunction

; void Game::SetTintMaskTexturePath(...) native global
function Game_SetTintMaskTexturePath() global
	int session = Costile2.GetSession("Game_SetTintMaskTexturePath")
	string cdcg_path = Costile2.GetString(session, 0)
	int cdcg_type = Costile2.GetInt(session, 1)
	int cdcg_index = Costile2.GetInt(session, 2)
	Game.SetTintMaskTexturePath(cdcg_path, cdcg_type, cdcg_index)
endFunction

; void Game::UpdateTintMaskColors(...) native global
function Game_UpdateTintMaskColors() global
	int session = Costile2.GetSession("Game_UpdateTintMaskColors")
	Game.UpdateTintMaskColors()
endFunction

; void Game::UpdateHairColor(...) native global
function Game_UpdateHairColor() global
	int session = Costile2.GetSession("Game_UpdateHairColor")
	Game.UpdateHairColor()
endFunction

; int Game::GetCameraState(...) native global
int function Game_GetCameraState() global
	int session = Costile2.GetSession("Game_GetCameraState")
	return Game.GetCameraState()
endFunction

; void Game::SetMiscStat(...) native global
function Game_SetMiscStat() global
	int session = Costile2.GetSession("Game_SetMiscStat")
	string cdcg_name = Costile2.GetString(session, 0)
	int cdcg_value = Costile2.GetInt(session, 1)
	Game.SetMiscStat(cdcg_name, cdcg_value)
endFunction

; void Game::SetPlayersLastRiddenHorse(...) native global
function Game_SetPlayersLastRiddenHorse() global
	int session = Costile2.GetSession("Game_SetPlayersLastRiddenHorse")
	actor cdcg_horse = Costile2.GetTESForm(session, 0) as actor
	Game.SetPlayersLastRiddenHorse(cdcg_horse)
endFunction

; int Game::GetSkillLegendaryLevel(...) global
int function Game_GetSkillLegendaryLevel() global
	int session = Costile2.GetSession("Game_GetSkillLegendaryLevel")
	string cdcg_actorValue = Costile2.GetString(session, 0)
	return Game.GetSkillLegendaryLevel(cdcg_actorValue)
endFunction

; void Game::SetSkillLegendaryLevel(...) global
function Game_SetSkillLegendaryLevel() global
	int session = Costile2.GetSession("Game_SetSkillLegendaryLevel")
	string cdcg_actorValue = Costile2.GetString(session, 0)
	int cdcg_level = Costile2.GetInt(session, 1)
	Game.SetSkillLegendaryLevel(cdcg_actorValue, cdcg_level)
endFunction

; float Game::GetPlayerExperience(...) native global
float function Game_GetPlayerExperience() global
	int session = Costile2.GetSession("Game_GetPlayerExperience")
	return Game.GetPlayerExperience()
endFunction

; void Game::SetPlayerExperience(...) native global
function Game_SetPlayerExperience() global
	int session = Costile2.GetSession("Game_SetPlayerExperience")
	float cdcg_exp = Costile2.GetFloat(session, 0)
	Game.SetPlayerExperience(cdcg_exp)
endFunction

; float Game::GetExperienceForLevel(...) native global
float function Game_GetExperienceForLevel() global
	int session = Costile2.GetSession("Game_GetExperienceForLevel")
	int cdcg_currentLevel = Costile2.GetInt(session, 0)
	return Game.GetExperienceForLevel(cdcg_currentLevel)
endFunction

; bool Game::GetPlayerMovementMode(...) native global
bool function Game_GetPlayerMovementMode() global
	int session = Costile2.GetSession("Game_GetPlayerMovementMode")
	return Game.GetPlayerMovementMode()
endFunction

; void Game::UpdateThirdPerson(...) native global
function Game_UpdateThirdPerson() global
	int session = Costile2.GetSession("Game_UpdateThirdPerson")
	Game.UpdateThirdPerson()
endFunction

; void Game::UnbindObjectHotkey(...) native global
function Game_UnbindObjectHotkey() global
	int session = Costile2.GetSession("Game_UnbindObjectHotkey")
	int cdcg_hotkey = Costile2.GetInt(session, 0)
	Game.UnbindObjectHotkey(cdcg_hotkey)
endFunction

; form Game::GetHotkeyBoundObject(...) native global
form function Game_GetHotkeyBoundObject() global
	int session = Costile2.GetSession("Game_GetHotkeyBoundObject")
	int cdcg_hotkey = Costile2.GetInt(session, 0)
	return Game.GetHotkeyBoundObject(cdcg_hotkey)
endFunction

; bool Game::IsObjectFavorited(...) native global
bool function Game_IsObjectFavorited() global
	int session = Costile2.GetSession("Game_IsObjectFavorited")
	form cdcg_form = Costile2.GetTESForm(session, 0) as form
	return Game.IsObjectFavorited(cdcg_form)
endFunction

; form Game::GetFormEx(...) native global
form function Game_GetFormEx() global
	int session = Costile2.GetSession("Game_GetFormEx")
	int cdcg_formId = Costile2.GetInt(session, 0)
	return Game.GetFormEx(cdcg_formId)
endFunction

; objectreference Game::GetDialogueTarget(...) native global
objectreference function Game_GetDialogueTarget() global
	int session = Costile2.GetSession("Game_GetDialogueTarget")
	return Game.GetDialogueTarget()
endFunction

; objectreference Game::GetCurrentCrosshairRef(...) native global
objectreference function Game_GetCurrentCrosshairRef() global
	int session = Costile2.GetSession("Game_GetCurrentCrosshairRef")
	return Game.GetCurrentCrosshairRef()
endFunction

; objectreference Game::GetCurrentConsoleRef(...) native global
objectreference function Game_GetCurrentConsoleRef() global
	int session = Costile2.GetSession("Game_GetCurrentConsoleRef")
	return Game.GetCurrentConsoleRef()
endFunction

; void Game::SetPlayerLevel(...) native global
function Game_SetPlayerLevel() global
	int session = Costile2.GetSession("Game_SetPlayerLevel")
	int cdcg_level = Costile2.GetInt(session, 0)
	Game.SetPlayerLevel(cdcg_level)
endFunction

