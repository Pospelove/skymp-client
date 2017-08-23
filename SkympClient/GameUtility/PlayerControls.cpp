#include "../stdafx.h"
#include "PlayerControls.h"

namespace PlayerControls_
{
	enum {
		MIN_ENABLED_STATE = 0,
	};

	std::vector<SInt64> state(CONTROLS_COUNT, MIN_ENABLED_STATE);
	std::recursive_mutex mutex;

	void Update()
	{
		Timer::Set(0, [] {
			std::lock_guard<std::recursive_mutex> l(mutex);
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
			MenuDisabler::SetDisabled("Console", state[(UInt32)Control::Console] < MIN_ENABLED_STATE);
		});
	}

	void SetEnabled(Control control, bool enabled)
	{
		auto idx = (UInt32)control;
		std::lock_guard<std::recursive_mutex> l(mutex);
		if (enabled)
			state[idx]++;
		else
			state[idx]--;

		Update();
	}
}