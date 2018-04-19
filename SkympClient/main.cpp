#include "stdafx.h"
#include "DirectX\DirectXHook.h"
#include "DInput\DInput.hpp"
#include "WinAPI\WinAPI.hpp"
#include "Overlay\GUI.h"
#include "Overlay\Chat.h"
#include "Overlay\Input.h"
#include "Overlay\3DText.h"
#include "Overlay\Loadscreen.h"

#include <iomanip>
#include <chrono>

#include "Costile\Costile2.h"  // Costile2::Register();
#include "Costile\CostileDragon.h"

#include "CoreInterface\CoreInterface.h"

#include "Skyrim/BSScript/BSScriptIFunctionArguments.h"
#include "Skyrim/Events/ScriptEvent.h"
#include "Skyrim\FileIO\TESDataHandler.h"

#include <process.h>
#include <Tlhelp32.h>
#include <shlobj.h>

void killProcessByName(const char *filename)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	PROCESSENTRY32 pEntry;
	pEntry.dwSize = sizeof(pEntry);
	BOOL hRes = Process32First(hSnapShot, &pEntry);
	while (hRes)
	{
		if (strcmp(pEntry.szExeFile, filename) == 0)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
				(DWORD)pEntry.th32ProcessID);
			if (hProcess != NULL)
			{
				TerminateProcess(hProcess, 9);
				CloseHandle(hProcess);
			}
		}
		hRes = Process32Next(hSnapShot, &pEntry);
	}
	CloseHandle(hSnapShot);
}

LockTimer closeCursorMenuLock(1000);
UInt32 g_SDThreadId = ~0;
namespace ci
{
	extern UInt32 updateRateMS;
}

void dummy()
{
}

void other_thread(void *);

void DrawSheatheWeapon()
{
}

class SkympClientDll : public SKSEPlugin
{
public:

	Loadscreen *ls = nullptr;
	std::unique_ptr<Loadscreen> dummyLs;
	bool needDummy = true;

	SkympClientDll()
	{
	}

	virtual bool InitInstance() override
	{
		//SafeWrite32(0x006B5DB0, (DWORD)FakeFn);

		if (!Requires(kSKSEVersion_1_7_1, SKSEPapyrusInterface::Version_1))
		{
			ErrorHandling::SendError("FATAL:Client your skse version is too old");
			return false;
		}

		this->SetName("SkyMP");
		this->SetVersion(1);
		HookDirect3D9();
		HookDirectInput();
		HookWinAPI();
		return true;
	}

	virtual bool OnLoad() override
	{
		gLog << "Loaded" << std::endl;
		_beginthread(other_thread, 0, nullptr);
		return true;
	}

	virtual void OnModLoaded() override
	{
		Costile2::Register();
	}

	void OnScriptDragonLoaded()
	{
		try {
			auto &logic = ci::IClientLogic::clientLogic;
			if (logic)
			{
				ci::IClientLogic::QueueCallback([=] {
					logic->OnWorldInit();
				});
			}
		}
		catch (...) {
			ErrorHandling::SendError("ERROR:ClientLogic OnWorldInit()");
		}

		PlayerControls_::SetEnabled(Control::SaveGame, false);
		PlayerControls_::SetEnabled(Control::Wait, false);
		PlayerControls_::SetEnabled(Control::FastTravel, false);
		PlayerControls_::SetEnabled(Control::Console, false);

		sd::ExecuteConsoleCommand("player.aps Costile _PlaceAtMe_", 0);
		sd::ExecuteConsoleCommand("player.aps Costile _SetPosition_", 0);
		sd::ExecuteConsoleCommand("player.aps Costile _SendAnimationEvent_", 0);
		sd::ExecuteConsoleCommand("player.aps Costile _ShowList_", 0);
		for (SInt32 i = 0; i != 10; ++i)
			sd::ExecuteConsoleCommand("player.aps Costile _KeepOffsetFromActor_", 0);
		for (SInt32 i = 0; i != 10; ++i)
			sd::ExecuteConsoleCommand("player.aps Costile _TranslateTo_", 0);

		SET_TIMER(0, [this] {
			sd::ExecuteConsoleCommand("player.aps Costile PostLoad", 0);
			sd::FadeOutGame(0, 1, 0.3, 0.5);
			dummyLs.reset(nullptr);
			needDummy = false;
		});

		while (1)
		{
			try {
				this->lastUpdateMT = clock();

				//while (sd::GetKeyPressed(VK_F9))
				//	sd::QuitToMainMenu();

				sd::Wait(0);

				auto callAndGetTime = [](std::function<void()> callable) {
					clock_t startTime = clock();
					callable();
					clock_t now = clock();
					return now - startTime;
				};

				auto cChat = callAndGetTime([&] {
					try {
						if (TheChat)
							TheChat->Update();
					}
					catch (...) {
						ErrorHandling::SendError("ERROR:Chat");
					}
				});

				auto cTimer = callAndGetTime([&] {
					try {
						Timer::Update();
					}
					catch (...) {
						ErrorHandling::SendError("ERROR:Timer");
					}
				});

				auto cWorldCleaner = callAndGetTime([&] {
					try {
						WorldCleaner::GetSingleton()->Update();
					}
					catch (...) {
						ErrorHandling::SendError("ERROR:WorldCleaner");
					}
				});

				auto cRemotePlayer = callAndGetTime([&] {
					try {
						ci::RemotePlayer::UpdateAll();
					}
					catch (...) {
						ErrorHandling::SendError("ERROR:RemotePlayer");
					}
				});

				auto cLocalPlayer = callAndGetTime([&] {
					try {
						ci::LocalPlayer::GetSingleton()->Update();
					}
					catch (...) {
						ErrorHandling::SendError("ERROR:LocalPlayer");
					}
				});

				auto cObject = callAndGetTime([&] {
					try {
						ci::Object::UpdateAll();
					}
					catch (...) {
						ErrorHandling::SendError("ERROR:Object");
					}
				});

				auto cSimpleRef = callAndGetTime([&] {
					try {
						SimpleRef::UpdateAll();
					}
					catch (...) {
						ErrorHandling::SendError("ERROR:SimpleRef");
					}
				});

				if (sd::GetKeyPressed(0x31))
				{
					std::wstringstream ss;
					ss << L"Chat:"		<< cChat << L' ';
					ss << L"Timer:"		<< cTimer << L' ';
					ss << L"WC:"		<< cWorldCleaner << L' ';
					ss << L"RemotePl:"	<< cRemotePlayer << L' ';
					ss << L"LocalPl:"	<< cLocalPlayer << L' ';
					ss << L"Object:"	<< cObject << L' ';
					ss << L"SimpleRef:" << cSimpleRef;
					//ci::Chat::AddMessage(ss.str());
				}

			}
			catch (...) {
				ErrorHandling::SendError("ERROR:Client Exception in MT");
			}
		}
	}

	void OnOtherThreadReady()
	{
		ci::SetTimer(1000, [] {
			const auto hModule = GetModuleHandle("SkyrimSouls.dll");
			if (!hModule)
			{
				ErrorHandling::SendError("FATAL:Client SkyrimSouls.dll not found");
				Sleep(1000);
				std::exit(EXIT_FAILURE);
			}
		});

		HWND fore = //GetForegroundWindow(); // Every window has it's own current language (keyboard layout)
			Utility::FindMainWindow();
		DWORD tpid = GetWindowThreadProcessId(fore, 0);
		HKL hKL = GetKeyboardLayout(tpid);
		auto languageCode = LOWORD(hKL);
		auto english = 1033;
		if (languageCode != english)
		{
			keybd_event(VK_LSHIFT, DIK_LSHIFT, NULL, NULL);
			keybd_event(VK_LCONTROL, DIK_LCONTROL, NULL, NULL);

			keybd_event(VK_LSHIFT, DIK_LSHIFT, KEYEVENTF_KEYUP, NULL);
			keybd_event(VK_LCONTROL, DIK_LCONTROL, KEYEVENTF_KEYUP, NULL);

			keybd_event(VK_LSHIFT, DIK_LSHIFT, NULL, NULL);
			keybd_event(VK_MENU, DIK_LALT, NULL, NULL);

			keybd_event(VK_LSHIFT, DIK_LSHIFT, KEYEVENTF_KEYUP, NULL);
			keybd_event(VK_MENU, DIK_LALT, KEYEVENTF_KEYUP, NULL);
		}

		IDirect3D9 *pDevice;
		while (g_pIDirect3DDevice9 == nullptr)
			Sleep(1);
		g_pIDirect3DDevice9->GetDirect3D(&pDevice);
		TheInputManager = new InputManager;

		while (TheIInputHook == nullptr)
			Sleep(1);
		TheIInputHook->AddListener(TheInputManager);
		TheIInputHook->AddListener(KeyboardManager::GetSingletone());

		MenuDisabler::SetDisabled("Tutorial Menu", true);
		MenuDisabler::SetDisabled("Console", true);
		//PlayerControls_::SetEnabled(Control::Console, true);

		const static auto renameQuicksave = [] {
			char Buffer[MAX_PATH];
			SHGetSpecialFolderPath(NULL, Buffer, CSIDL_MYDOCUMENTS, 0);
			auto oldname = std::string(Buffer) + "\\My Games\\Skyrim\\Saves\\quicksave.ess";
			auto  newname = std::string(Buffer) + "\\My Games\\Skyrim\\Saves\\quicksave.ess_RENAMED_BY_SKYMP";
			auto result = rename(oldname.data(), newname.data());
			if (result == 0)
				return true;
			else
				return false;
		};
		renameQuicksave();

		auto &logic = ci::IClientLogic::clientLogic;

		std::thread([this] {
			while (1)
			{
				Sleep(2000);

				if (renameQuicksave())
				{
					std::exit(0);
				}

				if (ci::IsInPause() || !Utility::IsForegroundProcess())
					this->lastUpdateMT = NULL;

				if (this->lastUpdateMT != NULL && this->lastUpdateMT + 15000 < clock())
				{
					if (GameSettings::IsFullScreen())
					{
						ErrorHandling::SendError("FATAL:Client Game thread is not responding");
						Sleep(1000);
						std::exit(EXIT_FAILURE);
					}
					else
					{
						ErrorHandling::SendError("ERROR:Client Game thread is not responding");
					}
				}
			}
		}).detach();

		if (this->ls == nullptr)
		{
			this->ls = new Loadscreen("MyGUI\\logo.png");
		}

		while (1) 
		{
			if (MenuManager::GetSingleton()->IsMenuOpen("Console"))
			{
				auto &allowTextInput = (*(uint8_t *)((*(uint32_t *)0x012E7458) + 0x98));
				allowTextInput = false;
			}

			static auto menuManager = MenuManager::GetSingleton();
			try
			{
				if (sd::GetKeyPressed(VK_LMENU) && sd::GetKeyPressed(VK_F4))
				{
					if (Utility::IsForegroundProcess())
						std::exit(0);
				}

				ci::LocalPlayer::GetSingleton()->Update_OT();

				if (TheChat)
					TheChat->Update_OT();

				MenuDisabler::Update_OT();

				static bool mainMenuWasOpen = false;
				static bool attached = false;

				if (menuManager && !menuManager->IsMenuOpen("Main Menu"))
				{
					if (mainMenuWasOpen)
					{
						if (this->ls != nullptr)
						{
							auto ls = this->ls;
							ci::SetTimer(1000, [ls] {
								delete ls;
							});
							this->ls = nullptr;
						}
						if (!attached)
						{
							sd::ExecuteConsoleCommand("player.aps Costile PreLoad", 0);
							attached = true;
						}
					}
				}

				static clock_t lastReset = 0;
				if (clock() - lastReset > 1000)
				{
					if (menuManager && !menuManager->IsMenuOpen("Loading Menu") && needDummy && ls == nullptr)
					{
						lastReset = clock();
						dummyLs.reset(new Loadscreen("MyGUI\\dummy.png"));
					}
					else
						dummyLs.reset(nullptr);
				}

				if (menuManager && menuManager->IsMenuOpen("Main Menu"))
				{
					WorldCleaner::GetSingleton()->SetFormProtected(0, true);
					WorldCleaner::GetSingleton()->SetFormProtected(0, false);

					mainMenuWasOpen = true;
					attached = false;

					static class DisablePause
					{
					public:
						DisablePause()
						{
							enum {
								safe = true
							};
							auto menus = { "Console","TutorialMenu","MessageBoxMenu","TweenMenu","InventoryMenu","MagicMenu","ContainerMenu","FavoritesMenu","BarterMenu","TrainingMenu","LockpickingMenu","BookMenu","GiftMenu", "SleepWaitMenu","CustomMenu","FadeOutDist" };
							for (auto menu : menus)
								PauseDisabler::SetPauseDisabledInMenu(menu, true, safe);
							auto menusWithPause = { "JournalMenu" };
							for (auto menu : menusWithPause)
								PauseDisabler::SetPauseDisabledInMenu(menu, false, safe);
						}
					} disablePause_;

					static class LoadMods
					{
					public:
						LoadMods()
						{
							//TESDataHandler::GetSingleton()->HotLoadPlugin("skymp_jump");
						}
					} loadMods_;

					static bool onStartupCalled = false;
					if (!onStartupCalled)
					{
						if (logic)
						{
							onStartupCalled = true;
							ci::SetTimer(800, [=] {
								try {
									ci::IClientLogic::QueueCallback([=] {
										logic->OnStartup();
									});
								}
								catch (...) {
									ErrorHandling::SendError("ERROR:ClientLogic OnStartup()");
								}
							});
						}
					}

					if (!TheChat || !TheChat->IsTyping())
					{
						if (Utility::IsForegroundProcess())
						{
							keybd_event(VK_UP, DIK_DOWNARROW, NULL, NULL);
							keybd_event(VK_UP, DIK_DOWNARROW, KEYEVENTF_KEYUP, NULL);

							if (KeyboardManager::GetSingletone()->GetKeyPressed(DIK_LALT) == false)
							{
								keybd_event(VK_TAB, DIK_TAB, NULL, NULL);
								keybd_event(VK_TAB, DIK_TAB, KEYEVENTF_KEYUP, NULL);
							}
							else
							{
								keybd_event(VK_ESCAPE, DIK_ESCAPE, NULL, NULL);
								keybd_event(VK_ESCAPE, DIK_ESCAPE, KEYEVENTF_KEYUP, NULL);
							}
						}
					}

					static std::function<void()> tryCloseCursorMenu;
					if (!tryCloseCursorMenu)
					{
						tryCloseCursorMenu = [&] {
							if (menuManager->IsMenuOpen("Cursor Menu"))
							{
								if (closeCursorMenuLock.TryLock(&tryCloseCursorMenu))
								{
									mouse_event(NULL, 1000000, 1000000, NULL, NULL);
									menuManager->CloseMenu("Cursor Menu");
								}
							}
						};
					}
					tryCloseCursorMenu();
				}

				using MsgList = std::vector<std::wstring>;
				MsgList myMessages = TheChat ? TheChat->GetMyMessages() : MsgList();
				static MsgList myMessagesWas = {};
				if (myMessages.size() != myMessagesWas.size())
				{
					myMessagesWas = myMessages;
					if (myMessages.empty() == false)
					{
						const std::wstring str = myMessages.back();
						std::vector<std::wstring> lexs;

						const WCHAR *pch = wcstok(const_cast<WCHAR *>(str.data()), L" ");
						while (pch)
						{
							lexs.push_back(pch);
							pch = wcstok(NULL, L" ");
						}

						MyGUI::UString otherLexs_ = "";
						if (lexs.size() >= 2)
						{
							for (auto it = lexs.begin() + 1; it != lexs.end(); ++it)
								otherLexs_ = otherLexs_ + ' ' + *it;
						}

						if (logic && !lexs.empty() && !lexs[0].empty())
						{
							ci::IClientLogic::QueueCallback([=] {
								if (lexs[0][0] == L'/')
								{
									const auto cmdText = lexs[0];
									const std::vector<std::wstring> args = { lexs.begin() + 1, lexs.end() };
									try {
										logic->OnChatCommand(cmdText, args);
									}
									catch (...) {
										ErrorHandling::SendError("ERROR:ClientLogic OnChatCommand()");
									}
								}
								else
								{
									std::wstring msg;
									for (auto elem : lexs)
										msg += elem + L' ';
									msg.erase(--msg.end());
									try {
										logic->OnChatMessage(msg);
									}
									catch (...) {
										ErrorHandling::SendError("ERROR:ClientLogic OnChatMessage()");
									}
								}
							});
						}
					}
				}

				ci::RemotePlayer::UpdateAll_OT();
				ci::Object::UpdateAll_OT();

				static clock_t lastKill = 0;
				if (clock() - lastKill > 1000)
				{
					lastKill = clock();
					killProcessByName("skyrimv193208+31tr.exe");
				}

				Sleep(1);
			}
			catch (const std::exception &e)
			{
				ErrorHandling::SendError("ERROR:Client Exception in OT '%s'", e.what());
			}
			catch (...)
			{
				ErrorHandling::SendError("ERROR:Client Exception in OT");
			}
		}
	}

private:
	clock_t lastUpdateMT = 0;

} thePlugin;

void __declspec(dllexport) main()
{
	g_SDThreadId = GetCurrentThreadId();
	thePlugin.OnScriptDragonLoaded();
}

void other_thread(void *)
{
	thePlugin.OnOtherThreadReady();
}