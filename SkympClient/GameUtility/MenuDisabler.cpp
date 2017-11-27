#include "../stdafx.h"
#include "MenuDisabler.h"

namespace MenuDisabler
{
	std::mutex mutex;
	std::set<std::string> disabledMenus;

	auto menus = {
		"BarterMenu",
		"Book Menu",
		"Console",
		"Console Native UI Menu",
		"ContainerMenu",
		"Crafting Menu",
		"Credits Menu",
		"Cursor Menu",
		"Debug Text Menu",
		"Dialogue Menu",
		"Fader Menu",
		"FavoritesMenu",
		"GiftMenu",
		"HUD Menu",
		"InventoryMenu",
		"Journal Menu",
		"Kinect Menu",
		"LevelUp Menu",
		"Loading Menu",
		"Lockpicking Menu",
		"MagicMenu",
		"Main Menu",
		"MapMenu",
		"MessageBoxMenu",
		"Mist Menu",
		"Overlay Interaction Menu",
		"Overlay Menu",
		"Quantity Menu",
		"RaceSex Menu",
		"Sleep / Wait Menu",
		"StatsMenu",
		"TitleSequence Menu",
		"Top Menu",
		"Training Menu",
		"Tutorial Menu",
		"TweenMenu"
	};
}

void MenuDisabler::SetDisabled(const char *m, bool disabled)
{
	std::lock_guard<std::mutex> lck(mutex);

	if (disabled)
		disabledMenus.insert(m);
	else
	{
		auto it = disabledMenus.find(m);
		if (it != disabledMenus.end())
			disabledMenus.erase(it);
	}
}

void MenuDisabler::Update_OT()
{
	std::lock_guard<std::mutex> lck(mutex);

	auto mm = MenuManager::GetSingleton(); 
	if (mm != nullptr)
		for (auto it = menus.begin(); it != menus.end(); ++it)
		{
			if (mm->IsMenuOpen(*it) && disabledMenus.find(*it) != disabledMenus.end())
			{
				if (*it == (std::string)"Console")
				{
					keybd_event(VK_OEM_3, 41, NULL, NULL);
					keybd_event(VK_OEM_3, 41, KEYEVENTF_KEYUP, NULL);
					Sleep(100);
				}
				else
					mm->CloseMenu(*it);
			}
		}
}