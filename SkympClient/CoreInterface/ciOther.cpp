#include "../stdafx.h"
#include "../Sync/LookData.h"
#include "ciGUI.h"
#include "ciOther.h"
#include <stdlib.h>
#include <mutex>
#include "../ScriptDragon/enums.h"

extern LockTimer closeCursorMenuLock;

namespace ci
{
	void ExecuteConsoleCommandImpl(std::string cmd)
	{
		if (!closeCursorMenuLock.TryLock(ExecuteConsoleCommandImpl))
		{
			ci::SetTimer(100, [=] {
				ExecuteConsoleCommandImpl(cmd);
			});
		}
		else
			sd::ExecuteConsoleCommand(const_cast<char*>(cmd.c_str()), nullptr);
	}

	void ExecuteConsoleCommand(const std::wstring &consoleCmdString)
	{
		if (consoleCmdString.empty())
			return;

		if (consoleCmdString == L"skymp qnnu")
		{
			Timer::Set(0, [] {
				g_thePlayer->QueueNiNodeUpdate(false);
			});
			return;
		}

		static std::mutex consoleMutex;
		std::lock_guard<std::mutex> l(consoleMutex);

		ExecuteConsoleCommandImpl(WstringToString(consoleCmdString));
	}

	void SetTimer(uint32_t ms, std::function<void()> fn)
	{
		static std::mutex fnMutex;

		if (ms == 0)
		{
			std::lock_guard<std::mutex> l(fnMutex);
			return fn();
		}

		std::thread([=] {
			Sleep(ms);
			std::lock_guard<std::mutex> l(fnMutex);
			fn();
		}).detach();
	}

	clock_t greyFaceFixMoment = ~0;

	void ShowRaceMenu()
	{
		Timer::Set(0, [] {
			Timer::Set(300, [] {
				LookData_::ShowRaceMenu();
				Timer::Set(0, [] {
					sd::ForceFirstPerson();
					Timer::Set(300, [] {
						cd::SendAnimationEvent(g_thePlayer, "Skymp_Register");
					});
				});
			});
		});
	}

	void ShowMainMenu()
	{
		static const BSFixedString fsMainMenu = "Main Menu";
		if (MenuManager::GetSingleton()->IsMenuOpen(fsMainMenu))
			return;
		Timer::Set(0, [] {
			MenuManager::GetSingleton()->OpenMenu(fsMainMenu);
		});
	}


	UInt32 updateRateMS = 0;
	void SetUpdateRate(uint32_t ms)
	{
		updateRateMS = ms;
	}

	bool IsLoadScreenOpen()
	{
		auto menuManager = MenuManager::GetSingleton();
		return menuManager->IsMenuOpen("Loading Menu");
	}

	bool IsInPause()
	{
		return *((unsigned long *)0x1B3E428) != 0;
	}

	bool IsInDebug()
	{
		return ::IsSkympDebug();
	}

	bool IsWorldSpace(uint32_t formID)
	{
		auto form = LookupFormByID(formID);
		return form && form->formType == FormType::WorldSpace;
	}

	bool IsCell(uint32_t formID)
	{
		auto form = LookupFormByID(formID);
		return form && form->formType == FormType::Cell;
	}
}