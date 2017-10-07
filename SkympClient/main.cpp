﻿#include "stdafx.h"
#include "DirectX\DirectXHook.h"
#include "DInput\DInput.hpp"
#include "WinAPI\WinAPI.hpp"
#include "Overlay\GUI.h"
#include "Overlay\Chat.h"
#include "Overlay\Input.h"

#include <iomanip>
#include <chrono>

#include "Costile\Costile2.h"  // Costile2::Register();
#include "Costile\CostileDragon.h"

#include "CoreInterface\CoreInterface.h"

#include "Skyrim/BSScript/BSScriptIFunctionArguments.h"
#include "Skyrim/Events/ScriptEvent.h"

LockTimer closeCursorMenuLock(1000);
UInt32 g_SDThreadId = ~0;
namespace ci
{
	extern UInt32 updateRateMS;
}

bool IsSkympDebug()
{
	static std::mutex mutex;
	std::lock_guard<std::mutex> l(mutex);
	static bool *isSkympDebugPtr = nullptr;
	if (isSkympDebugPtr == nullptr)
	{
		std::ifstream is_file("skymp_debug.txt");
		isSkympDebugPtr = new bool(is_file.good());
	}
	return *isSkympDebugPtr;
}

void other_thread(void *);

class SkympClientDll : public SKSEPlugin
{
public:
	SkympClientDll()
	{
	}

	virtual bool InitInstance() override
	{
		if (!Requires(kSKSEVersion_1_7_1, SKSEPapyrusInterface::Version_1))
		{
			//gLog << "ERROR: your skse version is too old" << std::endl;
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

	class DebugHitEventSink : public BSTEventSink<TESHitEvent>
	{
	public:
		virtual ~DebugHitEventSink()
		{
		}

	private:
		virtual	EventResult	ReceiveEvent(TESHitEvent *evn, BSTEventSource<TESHitEvent> *source) override
		{
			if (evn->caster != g_thePlayer)
				return {};
			auto ref = evn->target;
			SET_TIMER(0, [ref] {
				uint32_t locationID = 0;
				const char *locationName = "";
				auto cell = sd::GetParentCell(ref);
				if (!cell)
				{
					return;
				}
				if (cell->IsInterior())
				{
					locationID = cell->formID;
					locationName = cell->GetName();
				}
				else
				{
					locationID = ref->GetWorldSpace()->formID;
					locationName = ref->GetWorldSpace()->GetName();
				}

				std::stringstream ss;
				ss << "Debug\\OnHit\\Teleports.txt";

				std::ofstream of(ss.str().data(), std::ios::app);

				static std::stringstream *ssp = nullptr;
				if (ssp == nullptr)
				{
					ssp = new std::stringstream;
					*ssp << "obj1 = Object.Create(";
					*ssp << "0x" << std::hex << std::setfill('0') << std::setw(8) << ref->formID << ", ";
					*ssp << "0x" << std::hex << std::setfill('0') << std::setw(8) << ref->baseForm->formID << ", ";
					*ssp << "Location(" << "0x" << std::hex << std::setfill('0') << std::setw(8) << locationID << "), ";
					*ssp << ref->GetPositionX() << ", " << ref->GetPositionY() << ", " << ref->GetPositionZ() << ")" << std::endl;

					*ssp << "obj1:SetAngle(" << sd::GetAngleX(ref) << ", " << sd::GetAngleY(ref) << ", " << sd::GetAngleZ(ref) << ")" << std::endl;
					sd::PrintNote("Selected 1");
				}
				else
				{
					*ssp << "obj2 = Object.Create(";
					*ssp << "0x" << std::hex << std::setfill('0') << std::setw(8) << ref->formID << ", ";
					*ssp << "0x" << std::hex << std::setfill('0') << std::setw(8) << ref->baseForm->formID << ", ";
					*ssp << "Location(" << "0x" << std::hex << std::setfill('0') << std::setw(8) << locationID << "), ";
					*ssp << ref->GetPositionX() << ", " << ref->GetPositionY() << ", " << ref->GetPositionZ() << ")" << std::endl;

					*ssp << "obj2:SetAngle(" << sd::GetAngleX(ref) << ", " << sd::GetAngleY(ref) << ", " << sd::GetAngleZ(ref) << ")" << std::endl;

					*ssp << "obj1:RegisterAsTeleportDoor(obj2)" << std::endl;
					*ssp << "obj2:RegisterAsTeleportDoor(obj1)" << std::endl;

					of << ssp->str();
					ssp = nullptr;
					sd::PrintNote("Selected 2; Saved");
				}
			});
			return {};
		}
	};

	void OnScriptDragonLoaded()
	{
		if (IsSkympDebug())
			g_hitEventSource.AddEventSink(new DebugHitEventSink);

		auto &logic = ci::IClientLogic::clientLogic;
		if (logic)
		{
			std::lock_guard<ci::Mutex> l(logic->callbacksMutex);
			logic->OnWorldInit();
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

		SET_TIMER(0, [] {
			sd::ExecuteConsoleCommand("player.aps Costile PostLoad", 0);
			sd::FadeOutGame(0, 1, 0.3, 0.5);
		});

		while (1)
		{
			try {

				/*static int16_t stateWas;
				const auto state = (int16_t)sd::Obscript::GetAttackState(g_thePlayer);
				if (stateWas != state)
				{
					stateWas = state;
					ci::Chat::AddMessage(std::to_wstring(state));
				}*/

				this->lastUpdateMT = clock();

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

				if (sd::GetKeyPressed(0x31))
				{
					std::wstringstream ss;
					ss << L"Chat:"		<< cChat << L' ';
					ss << L"Timer:"		<< cTimer << L' ';
					ss << L"WC:"		<< cWorldCleaner << L' ';
					ss << L"RemotePl:"	<< cRemotePlayer << L' ';
					ss << L"LocalPl:"	<< cLocalPlayer << L' ';
					ss << L"Object:"	<< cObject;
					//ci::Chat::AddMessage(ss.str());
				}

			}
			catch (...) {
				ErrorHandling::SendError("ERROR:Unknown");
			}
		}
	}

	void OnOtherThreadReady()
	{
		if (IsSkympDebug() == false)
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
		}

		HWND fore = GetForegroundWindow(); // Every window has it's own current language (keyboard layout)
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

		auto &logic = ci::IClientLogic::clientLogic;

		std::thread([this] {
			while (1)
			{
				Sleep(2000);
				if (ci::IsInPause() || !Utility::IsForegroundProcess())
					this->lastUpdateMT = NULL;

				if (this->lastUpdateMT != NULL && this->lastUpdateMT + 5000 < clock())
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

		while (1)
		{
			try
			{
				if (sd::GetKeyPressed(VK_LMENU) && sd::GetKeyPressed(VK_F4))
				{
					if (Utility::IsForegroundProcess())
					{
						//if (ci::IsInPause())
						std::exit(0);
					}
				}

				ci::LocalPlayer::GetSingleton()->Update_OT();

				if (TheChat)
					TheChat->Update_OT();

				if (!IsSkympDebug())
					MenuDisabler::Update_OT();

				static auto menuManager = MenuManager::GetSingleton();
				static bool mainMenuWasOpen = false;
				static bool attached = false;

				if (menuManager && !menuManager->IsMenuOpen("Main Menu"))
				{
					if (mainMenuWasOpen)
					{
						if (!attached)
						{
							sd::ExecuteConsoleCommand("player.aps Costile PreLoad", 0);
							attached = true;
						}
					}
				}

				if (menuManager && menuManager->IsMenuOpen("Main Menu"))
				{
					// Hack to force update after Main Menu
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

					static bool onStartupCalled = false;
					if (!onStartupCalled)
					{
						if (logic)
						{
							onStartupCalled = true;
							ci::SetTimer(800, [=] {
								std::lock_guard<ci::Mutex> l(logic->callbacksMutex);
								logic->OnStartup();
							});
						}
					}

					if (!TheChat || !TheChat->IsTyping())
					{
						if (Utility::IsForegroundProcess() && !IsSkympDebug())
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
							if (menuManager->IsMenuOpen("Cursor Menu") && !IsSkympDebug())
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
							std::lock_guard<ci::Mutex> l(logic->callbacksMutex);
							if (lexs[0][0] == L'/')
							{
								const auto cmdText = lexs[0];
								const std::vector<std::wstring> args = { lexs.begin() + 1, lexs.end() };
								logic->OnChatCommand(cmdText, args);
							}
							else
							{
								std::wstring msg;
								for (auto elem : lexs)
									msg += elem + L' ';
								msg.erase(--msg.end());
								logic->OnChatMessage(msg);
							}
						}
					}
				}

				if (logic)
				{
					static clock_t lastOnUpdateCallMoment = 0;
					if (lastOnUpdateCallMoment + ci::updateRateMS <= clock())
					{
						lastOnUpdateCallMoment = clock();
						std::lock_guard<ci::Mutex> l(logic->callbacksMutex);
						logic->OnUpdate();
					}
				}

				ci::RemotePlayer::UpdateAll_OT();
				ci::Object::UpdateAll_OT();

				Sleep(1);
			}
			catch (...)
			{
				SET_TIMER(0, [] {
					sd::ShowMessageBox("Exception in OT");
				});
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