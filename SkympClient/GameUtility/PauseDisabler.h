#pragma once

class PauseDisabler
{
public:
	static bool SetPauseDisabledInMenu(const BSFixedString &menuName, bool disabled, bool sendCmdStrictlyFromGameThread = false);

private:
	PauseDisabler() {}
};