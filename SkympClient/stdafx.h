#pragma once

#ifndef STDAFX_INCLUDED
#define STDAFX_INCLUDED

#pragma warning(disable: 4091) // microsoft sdks\windows\v7.1a\include\shlobj.h(1151): warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared
#pragma warning(disable: 4275) // warning C4275: non dll-interface class 'std::exception' used as base for dll-interface class 'MyGUI::Exception'
#pragma warning(disable: 4477) // skymp\opensky\opensky\disasm\disasm_x86.c(3740): warning C4477: 'printf' : format string '%s' requires an argument of type 'char *', but variadic argument 2 has type 'int'
#pragma warning(disable: 4996) // SKSEOriginal\Utilities.cpp(186): warning C4996: 'GetVersionExA': объявлен deprecate

//#pragma comment(lib, "ws2_32")
#pragma comment(lib, "d3d9")
#pragma comment(lib, "dbghelp")
#pragma comment(lib, "lib/d3dx9")
#pragma comment(lib, "lib/MyGUI.lib")
#pragma comment(lib, "lib/MyGUIEngine.lib")
#pragma comment(lib, "lib/MyGUI.DirectXPlatform.lib")  

#define DIRECTINPUT_VERSION 0x0800

#include <SKSE.h>
#include <SKSE/PluginAPI.h>
#include <SKSE/DebugLog.h>
#include <SKSE/GameRTTI.h>
#include <SKSE/HookUtil.h>
#include <SKSE/GameForms.h>
#include <SKSE/GameObjects.h>
#include <SKSE/GameReferences.h>
#include <SKSE/GameExtraData.h>
#include <SKSE/GameMenus.h>
#include <SKSE/PapyrusFunctions.h>
#include <SKSE/SafeWrite.h>
#include "ScriptDragon\enums.h"
#include "ScriptDragon\types.h"
#include "ScriptDragon\plugin.h"
#include "ScriptDragon\skyscript.h"
#include "ScriptDragon\obscript.h"
#include <shlobj.h>
#include <process.h>
#include <d3d9.h>
#include <dinput.h>
#include <cstdint>
#include <mutex>
#include <thread>
#include <functional>
#include <algorithm>
#include <queue>

#include "Common\ErrorHandling.h"

#include "../include/MyGUI/MyGUI_Precompiled.h"

#include "Costile\CostileDragon.h"
#include "Costile\Value.h"

#include "GameUtility\MenuDisabler.h"
#include "GameUtility\Timer.h"
#include "GameUtility\KeyboardManager.h"
#include "GameUtility\PauseDisabler.h"
#include "GameUtility\LockTimer.h"
#include "GameUtility\WorldCleaner.h"
#include "GameUtility\PlayerControls.h"
#include "GameUtility\GameSettings.h"
#include "GameUtility\SkyUILib.h"
#include "GameUtility\MiscUtility.h"
#include "GameUtility\SimpleRef.h"
#include "GameUtility\CellUtil.h"

std::string WstringToString(const std::wstring &wstring);
std::wstring StringToWstring(const std::string &string);

#endif