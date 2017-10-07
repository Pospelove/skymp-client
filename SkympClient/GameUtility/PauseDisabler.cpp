#include "../stdafx.h"
#include "PauseDisabler.h"


bool PauseDisabler::SetPauseDisabledInMenu(const BSFixedString &menuName, bool disabled, bool sendCmdStrictlyFromGameThread)
{
	std::string cmd = "SetSkyrimSoulsVariable";
	cmd += ' ';
	cmd += menuName.c_str();
	cmd += ' ';
	const SInt32 val = disabled ? 1 : 0;
	cmd += std::to_string(val);

	auto exec = [=] {
		sd::ExecuteConsoleCommand(const_cast<char *>(cmd.data()), nullptr);
	};
	if (sendCmdStrictlyFromGameThread)
		SET_TIMER(0, exec);
	else
		exec();

	static const std::set<std::string> availableMenus = {
		"Console",
		"TutorialMenu",
		"MessageBoxMenu",
		"TweenMenu",
		"InventoryMenu",
		"MagicMenu",
		"ContainerMenu",
		"FavoritesMenu",
		"BarterMenu",
		"TrainingMenu",
		"LockpickingMenu",
		"BookMenu",
		"GiftMenu",
		"JournalMenu",
		"SleepWaitMenu",
		"CustomMenu",
		"FadeOutDist"
	};
	const bool found =  availableMenus.find(menuName.c_str()) != availableMenus.end();
	return found;
}