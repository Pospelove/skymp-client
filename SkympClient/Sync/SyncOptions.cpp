#include "../stdafx.h"
#include "SyncOptions.h"

using Options = std::map<std::string, std::string>;

struct SyncOptions::Impl
{
	Options options;
	dlf_mutex m;
};

const auto cfgFile = "skymp_sync_options.ini";

Options ReadOptions()
{
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
				key.erase(std::remove_if(key.begin(), key.end(), isspace), key.end());
				value.erase(std::remove_if(value.begin(), value.end(), isspace), value.end());
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

void WriteDefaultOptions()
{
	auto s = "; ci::RemotePlayer\n"
		"DRAW_DISTANCE = 7002\n"
		"NICKNAME_DISTANCE = 512\n"
		"GHOST_AXE_OFFSET_Z = 2048\n"
		"GHOST_AXE_UPDATE_RATE = 750\n"
		"MAX_HARDSYNCED_PLAYERS = 5\n"
		"MAX_PLAYERS_SYNCED_SAFE = 5\n"
		"INVISIBLE_FOX_ENGINE = 2\n"
		"MAX_INVISIBLE_FOXES = 3\n"
		"MAX_CD_LAG = 1000\n"
		"CD_LAG_RECOVER_TIME = 3333\n"
		"MAX_OUT_OF_POS = 256\n"
		"DISPENSER_OFFSET_Z = 120\n"
		"DISPENSER_OFFSET_Z_SNEAKING = 64\n"
		"DISPENSER_OFFSET_DISTANCE = 32\n"

		"ci::LocalPlayer\n"
		"DISABLE_PLAYER_DAMAGE = 1\n"
		"NORMAL_PROCESSING_WITH_MENUS = 1\n"
		"MUTE_SOUND_ON_TP = 1\n"

		"; ci::Object\n"
		"SAFE_ITEM_REMOVE = 0\n"

		"; MovementData\n"
		"FirstApplyDelay = 300\n"
		"Hard.smartAngle = 0\n"
		"Hard.weapDrawnUpdateRate = 1000\n"
		"Hard.positionUpdateRate = 0\n"
		"Hard.sneakingUpdateRate = 1333\n"
		"Hard.blockingUpdateRate = 500\n"
		"Hard.unsafeSDFuncsUpdateRate = 0\n"
		"Hard.jumpingSync = 1\n"
		"Hard.unsafeSDFuncsEnabled = 1\n"
		"Hard.maxLagDistance = 64\n"
		"Hard.maxLagDistanceOnRotate = 48\n"
		"Hard.negligibleTime = 100\n"
		"Hard.headtrackingDisabled = 1\n"
		"Normal.smartAngle = 0\n"
		"Normal.weapDrawnUpdateRate = 4500\n"
		"Normal.positionUpdateRate = 50\n"
		"Normal.sneakingUpdateRate = 2000\n"
		"Normal.blockingUpdateRate = 10000\n"
		"Normal.unsafeSDFuncsUpdateRate = 0\n"
		"Normal.jumpingSync = 1\n"
		"Normal.unsafeSDFuncsEnabled = 0\n"
		"Normal.maxLagDistance = 128\n"
		"Normal.maxLagDistanceOnRotate = 128\n"
		"Normal.negligibleTime = 100\n"
		"Normal.headtrackingDisabled = 1\n"
		"Light.smartAngle = 1\n"
		"Light.weapDrawnUpdateRate = 11000\n"
		"Light.positionUpdateRate = 400\n"
		"Light.sneakingUpdateRate = 11000\n"
		"Light.blockingUpdateRate = 11000\n"
		"Light.unsafeSDFuncsUpdateRate = 0\n"
		"Light.jumpingSync = 0\n"
		"Light.unsafeSDFuncsEnabled = 0\n"
		"Light.maxLagDistance = 128\n"
		"Light.maxLagDistanceOnRotate = 128\n"
		"Light.negligibleTime = 200\n"
		"Light.headtrackingDisabled = 0\n";

	std::ofstream ofs(cfgFile);
	ofs << s;
	ofs.close();
}

SyncOptions * SyncOptions::GetSingleton()
{
	static SyncOptions sncOps;
	return &sncOps;
}

SyncOptions::SyncOptions() : pimpl(new Impl)
{
	auto ops = ReadOptions();

	if (ops.size() == 0)
	{
		WriteDefaultOptions();
		ops = ReadOptions();
	}

	for (auto &pair : ops)
	{
		pimpl->options[pair.first] = pair.second;
	}
}

int32_t SyncOptions::GetInt(std::string o) const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return atoi(pimpl->options[o].data());
}

float SyncOptions::GetFloat(std::string o) const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return atof(pimpl->options[o].data());
}

std::string SyncOptions::GetString(std::string o) const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return pimpl->options[o];
}