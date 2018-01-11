#pragma once

enum class Control
{
	Movement = 0,
	Fighting = 1,
	CamSwitch = 2,
	Looking = 3,
	Sneaking = 4,
	Menu = 5,
	Activate = 6,
	JournalTabs = 7,
	SaveGame = 8,
	Wait = 9,
	FastTravel = 10,
	Console = 11,
	BeastForm = 12,
	COUNT = 13,
};

enum {
	CONTROLS_COUNT = (UInt32)Control::COUNT
};

namespace PlayerControls_
{
	// Recursive
	void SetEnabled(Control control, bool enabled);
}