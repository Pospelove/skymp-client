Scriptname Debug Hidden

; Returns the platform name
string Function GetPlatformName() native global

; Returns the version number string
string Function GetVersionNumber() native global

; Displays an in-game message box
Function MessageBox(string asMessageBoxText) native global

; Displays an in-game notification
Function Notification(string asNotificationText) native global

; Quits the game
Function QuitGame() native global

; Toggles Foot IK on/off
Function SetFootIK(bool abFootIK) native global

; TGM functionality
Function SetGodMode(bool abGodMode) native global

; Forcibly sends an animation event to a reference's behavior graph
; used to bypass actor limitation on the ObjectReference version
Function SendAnimationEvent(ObjectReference arRef, string asEventName) native global


; Takes a screenshot (Xenon only)
Function TakeScreenshot(string asFilename) native global

; ToggleAI
Function ToggleAI() native global

; TCL functionality
Function ToggleCollisions() native global

; Toggles menus on/off
Function ToggleMenus() native global
