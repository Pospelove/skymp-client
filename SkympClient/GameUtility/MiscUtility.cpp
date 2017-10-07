#include "../stdafx.h"
#include "MiscUtility.h"

namespace Utility
{
	uint32_t NewFormID()
	{
		static std::mutex mutex;
		std::lock_guard<std::mutex> l(mutex);
		static uint32_t id = ~0;
		do
		{
			id--;
		} while (LookupFormByID(id));
		return id;
	}

	std::vector<FormID> GetLoadedRefs(FormID worldSpaceID)
	{
		static std::map<TESWorldSpace *, std::vector<FormID>> data;

		static BSFixedString fsMainMenu = "Main Menu";
		if (MenuManager::GetSingleton()->IsMenuOpen(fsMainMenu))
			data.clear();

		if (data.empty())
		{
			for (uint32_t id = 0; id <= 0x00104217; ++id)
			{
				auto ws = (TESWorldSpace *)LookupFormByID(id);
				if (ws && ws->formType == FormType::WorldSpace)
					data.insert({ ws, {} });

			}
			for (uint32_t id = 0; id <= 0x00104217; ++id)
			{
				auto ref = (TESObjectREFR *)LookupFormByID(id);
				if (ref && ref->formType == FormType::Reference)
				{
					void *refWp = ref->GetWorldSpace();
					for (auto it = data.begin(); it != data.end(); ++it)
						if (refWp == it->first)
						{
							it->second.push_back(ref->formID);
							std::unique(it->second.begin(), it->second.end());
						}
				}
			}
		}

		return data[(TESWorldSpace *)LookupFormByID(worldSpaceID)];
	}

	bool IsForegroundProcess()
	{
		static auto isForegroundProcess = [](DWORD pid)
		{
			HWND hwnd = GetForegroundWindow();
			if (hwnd == NULL)
				return false;
			DWORD foregroundPid;
			if (GetWindowThreadProcessId(hwnd, &foregroundPid) == 0)
				return false;
			return (foregroundPid == pid);
		};
		return isForegroundProcess(GetCurrentProcessId());
	}
}