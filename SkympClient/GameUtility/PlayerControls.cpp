#include "../stdafx.h"
#include "PlayerControls.h"

extern "C" {
	__declspec(dllexport) bool setControlEnabled(const char *name, bool enabled)
	{
		static std::map<std::string, Control> m;
		if (m.empty())
		{
			m["Movement"] = Control::Movement;
			m["Fighting"] = Control::Fighting;
			m["CamSwitch"] = Control::CamSwitch;
			m["Looking"] = Control::Looking;
			m["Sneaking"] = Control::Sneaking;
			m["Menu"] = Control::Menu;
			m["Activate"] = Control::Activate;
			m["JournalTabs"] = Control::JournalTabs;
			m["SaveGame"] = Control::SaveGame;
			m["Wait"] = Control::Wait;
			m["FastTravel"] = Control::FastTravel;
			m["Console"] = Control::Console;
			m["BeastForm"] = Control::BeastForm;
		}
		try {
			auto c = m[name];
			SET_TIMER_LIGHT(0, [=] {
				PlayerControls_::SetEnabled(c, enabled);
			});
			return true;
		}
		catch (...) {
			return false;
		}
	};
}

namespace PlayerControls_
{
	enum {
		MIN_ENABLED_STATE = 0,
	};

	std::vector<SInt64> state(CONTROLS_COUNT, MIN_ENABLED_STATE);
	dlf_mutex mutex{"playercontrols"};

	void Update()
	{
		SET_TIMER(0, [] {
			std::lock_guard<dlf_mutex> l(mutex);
			sd::EnablePlayerControls(state[(UInt32)Control::Movement] >= MIN_ENABLED_STATE,
				state[(UInt32)Control::Fighting] >= MIN_ENABLED_STATE,
				state[(UInt32)Control::CamSwitch] >= MIN_ENABLED_STATE,
				state[(UInt32)Control::Looking] >= MIN_ENABLED_STATE,
				state[(UInt32)Control::Sneaking] >= MIN_ENABLED_STATE,
				state[(UInt32)Control::Menu] >= MIN_ENABLED_STATE,
				state[(UInt32)Control::Activate] >= MIN_ENABLED_STATE,
				state[(UInt32)Control::JournalTabs] >= MIN_ENABLED_STATE,
				NULL);
			sd::DisablePlayerControls(state[(UInt32)Control::Movement] < MIN_ENABLED_STATE,
				state[(UInt32)Control::Fighting] < MIN_ENABLED_STATE,
				state[(UInt32)Control::CamSwitch] < MIN_ENABLED_STATE,
				state[(UInt32)Control::Looking] < MIN_ENABLED_STATE,
				state[(UInt32)Control::Sneaking] < MIN_ENABLED_STATE,
				state[(UInt32)Control::Menu] < MIN_ENABLED_STATE,
				state[(UInt32)Control::Activate] < MIN_ENABLED_STATE,
				state[(UInt32)Control::JournalTabs] < MIN_ENABLED_STATE,
				NULL);
			sd::EnableFastTravel(state[(UInt32)Control::FastTravel] >= MIN_ENABLED_STATE);
			sd::SetInChargen(state[(UInt32)Control::SaveGame] < MIN_ENABLED_STATE,
				state[(UInt32)Control::Wait] < MIN_ENABLED_STATE,
				false);
			sd::Game::SetBeastForm(state[(UInt32)Control::BeastForm] < MIN_ENABLED_STATE);
			MenuDisabler::SetDisabled("Console", state[(UInt32)Control::Console] < MIN_ENABLED_STATE);
		});
	}

	void SetEnabled(Control control, bool enabled)
	{
		auto idx = (UInt32)control;
		std::lock_guard<dlf_mutex> l(mutex);
		if (enabled)
			state[idx]++;
		else
			state[idx]--;

		Update();
	}
}