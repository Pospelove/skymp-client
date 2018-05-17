/*
	THIS FILE IS A PART OF THE SKYRIM DRAGON SCRIPT PROJECT	
				(C) Alexander Blade 2011
			http://Alexander.SannyBuilder.com
*/

#include "plugin.h"
#include "skyscript.h"
#include <windows.h>
#include <stdio.h>

DWORD ___stack[MAX_STACK_LEN];
DWORD ___stackindex;
DWORD ___result;

#include <fstream> 

namespace sd
{
	TNativeCall NativeCall;
	TObscriptCall ObscriptCall;
	TGetPlayerObjectHandle GetPlayerObjectHandle; // same as Game::GetPlayer()
	TGetConsoleSelectedRef GetConsoleSelectedRef; // gets the object ref selected in the console menu
	Tdyn_cast dyn_cast;							 // object dynamic casting
	TRegisterPlugin RegisterPlugin;
	TWait Wait;
	TBSString_Create BSString_Create;
	TBSString_Free BSString_Free;
	TExecuteConsoleCommand ExecuteConsoleCommand;  // executes command just like you typed in in the console
												   // 2nd param is parent handle which can be 0 or point to a ref

	HMODULE g_hModule;

	void Error(char *pattern, ...)
	{
		char text[1024];
		va_list lst;
		va_start(lst, pattern);
		vsprintf_s(text, pattern, lst);
		va_end(lst);
		MessageBoxA(0, text, "ScriptDragon plugin critical error", MB_ICONERROR);
		ExitProcess(0);
	}

	std::string sd::GetKeyName(BYTE key)
	{
		DWORD sc = MapVirtualKeyA(key, 0);
		// check key for ascii
		BYTE buf[256];
		memset(buf, 0, 256);
		WORD temp;
		DWORD asc = (key <= 32);
		if (!asc && (key != VK_DIVIDE)) asc = ToAscii(key, sc, buf, &temp, 1);
		// set bits
		sc <<= 16;
		sc |= 0x1 << 25;  // <- don't care
		if (!asc) sc |= 0x1 << 24; // <- extended bit
		// convert to ansi string
		if (GetKeyNameTextA(sc, (char *)buf, sizeof(buf)))
			return (char *)buf;
		else return "";
	}

	std::string GetPathFromFilename(std::string filename)
	{
		return filename.substr(0, filename.rfind("\\") + 1);
	}

	int sd::IniReadInt(char *inifile, char *section, char *param, int def)
	{
		char curdir[MAX_PATH];
		GetModuleFileNameA(g_hModule, curdir, sizeof(curdir));
		std::string fname = GetPathFromFilename(curdir) + inifile;
		return GetPrivateProfileIntA(section, param, def, fname.c_str());
	}

	bool sd::GetKeyPressed(BYTE key, bool async)
	{
		if (async)
			return (GetAsyncKeyState(key) & 0x80000000) > 0;
		else
			return (GetKeyState(key) & 0x80000000) > 0;
	}

	void sd::PrintDebug(char *pattern, ...)
	{
		char text[1024];
		va_list lst;
		va_start(lst, pattern);
		vsprintf_s(text, pattern, lst);
		va_end(lst);
		OutputDebugStringA(text);
	}

	void sd::PrintNote(char *pattern, ...)
	{
		char text[1024];
		va_list lst;
		va_start(lst, pattern);
		vsprintf_s(text, pattern, lst);
		va_end(lst);
		OutputDebugStringA(text);
		sd::Debug::Notification(text);
	}

	/*
		DO NOT CHANGE ANYTHING BELOW FOR ANY MEANS
	*/

#define SCRIPT_DRAGON "Data/Scripts/Costile2_Thread.pex"

	void DragonPluginInit(HMODULE hModule)
	{
		std::ofstream f("skympsd.log");
		f << "initsd 1" << std::endl;
		HMODULE hDragon = LoadLibraryA(SCRIPT_DRAGON);
		/*
		In order to provide NORMAL support i need a plugins to be distributed without the DragonScript.dll engine
		cuz user always must have the latest version which cud be found ONLY on my web page
		*/
		f << "initsd 2" << std::endl;
		if (!hDragon) Error("Can't load %s, download latest version from http://alexander.sannybuilder.com/Files/tes/", SCRIPT_DRAGON);
		f << "initsd 3" << std::endl;
		NativeCall = (TNativeCall)GetProcAddress(hDragon, "Nativecall");
		ObscriptCall = (TObscriptCall)GetProcAddress(hDragon, "Obscriptcall");
		GetPlayerObjectHandle = (TGetPlayerObjectHandle)GetProcAddress(hDragon, "GetPlayerObjectHandle");
		ExecuteConsoleCommand = (TExecuteConsoleCommand)GetProcAddress(hDragon, "ExecuteConsoleCommand");
		GetConsoleSelectedRef = (TGetConsoleSelectedRef)GetProcAddress(hDragon, "GetConsoleSelectedRef");
		dyn_cast = (Tdyn_cast)GetProcAddress(hDragon, "dyn_cast");
		RegisterPlugin = (TRegisterPlugin)GetProcAddress(hDragon, "RegisterPlugin");
		Wait = (TWait)GetProcAddress(hDragon, "WaitMs");
		BSString_Create = (TBSString_Create)GetProcAddress(hDragon, "BSString_Create");
		BSString_Free = (TBSString_Free)GetProcAddress(hDragon, "BSString_Free");
		f << "initsd 4" << std::endl;

		if (!NativeCall || !ObscriptCall || !GetPlayerObjectHandle || !ExecuteConsoleCommand
			|| !GetConsoleSelectedRef || !dyn_cast || !RegisterPlugin || !Wait
			|| !BSString_Create || !BSString_Free)
		{
			Error("ScriptDragon engine dll `%s` has not all needed functions inside, exiting", SCRIPT_DRAGON);
		}

		f << "initsd 5" << std::endl;
		RegisterPlugin(hModule);
		f << "initsd 6" << std::endl;
	}
}

BOOL APIENTRY ScriptDragon_DllMain(HMODULE hModule, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		sd::g_hModule = hModule;
		sd::DragonPluginInit(hModule);
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		break;
	}
	}
	return TRUE;
}

extern "C" {
	__declspec(dllexport) bool GetKeyPressed(char key)
	{
		return sd::GetKeyPressed(key);
	}
}