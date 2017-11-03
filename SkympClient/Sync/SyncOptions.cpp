#include "../stdafx.h"
#include "SyncOptions.h"

using Options = std::map<std::string, std::string>;

struct SyncOptions::Impl
{
	Options options;
	dlf_mutex m;
};

Options ReadOptions()
{
	const auto cfgFile = "skymp_sync_options.ini";

	Options options;

	int lines = 0;
	std::ifstream is_file(cfgFile);

	std::string line;
	while (std::getline(is_file, line))
	{
		std::istringstream is_line(line);
		std::string key;
		if (std::getline(is_line, key, '='))
		{
			std::string value;
			if (std::getline(is_line, value))
			{
				options.insert({ key, value });
				++lines;
			}
		}
	}

	is_file.close();

	if (!lines)
		ErrorHandling::SendError("ERROR:SyncOptions 0 lines read");

	return options;
}

Options GetDefault()
{
	Options result;

	// ciRemotePlayer.cpp
	result["DRAW_DISTANCE"] = "7002";
	result["NICKNAME_DISTANCE"] = "512";
	result["GHOST_AXE_OFFSET_Z"] = "2048";
	result["GHOST_AXE_UPDATE_RATE"] = "750";
	result["MAX_HARDSYNCED_PLAYERS"] = "5";
	result["MAX_PLAYERS_SYNCED_SAFE"] = "5";
	result["INVISIBLE_FOX_ENGINE"] = "2";
	result["MAX_INVISIBLE_FOXES"] = "3";
	result["MAX_CD_LAG"] = "1000";
	result["CD_LAG_RECOVER_TIME"] = "3333";
	result["MAX_OUT_OF_POS"] = "256";
	result["DISPENSER_OFFSET_Z"] = "120";
	result["DISPENSER_OFFSET_Z_SNEAKING"] = "64";
	result["DISPENSER_OFFSET_DISTANCE"] = "32";

	// ciLocalPlayer.cpp
	result["DISABLE_PLAYER_DAMAGE"] = "1";
	result["NORMAL_PROCESSING_WITH_MENUS"] = "1";
	result["MUTE_SOUND_ON_TP"] = "1";

	// ciObject.cpp
	result["SAFE_ITEM_REMOVE"] = "0";

	return result;
}

SyncOptions * SyncOptions::GetSingleton()
{
	static SyncOptions sncOps;
	return &sncOps;
}

SyncOptions::SyncOptions() : pimpl(new Impl)
{
	pimpl->options = GetDefault();
	auto ops = ReadOptions();
	for (auto &pair : ops)
	{
		pimpl->options[pair.first] = pair.second;
	}
}

int32_t SyncOptions::GetInt(const char *o) const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return atoi(pimpl->options[o].data());
}

float SyncOptions::GetFloat(const char *o) const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return atof(pimpl->options[o].data());
}

std::string SyncOptions::GetString(const char *o) const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return pimpl->options[o];
}