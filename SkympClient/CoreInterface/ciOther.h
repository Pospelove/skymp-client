#pragma once
#include <string>
#include <functional>

namespace ci
{
	void ExecuteConsoleCommand(const std::wstring &consoleCmdString);
	void ShowRaceMenu();
	void ShowMainMenu();

	void SetTimer(uint32_t ms, std::function<void(void)> fn);
	void SetUpdateRate(uint32_t ms);

	bool IsLoadScreenOpen();
	bool IsInPause();
	bool IsInDebug();

	bool IsWorldSpace(uint32_t formID);
	bool IsCell(uint32_t formID);
}