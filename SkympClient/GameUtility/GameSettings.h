#pragma once

namespace GameSettings
{
	std::string GetSkyrimPrefsValue(const std::string &variableName);

	inline bool IsFullScreen() {
		return GetSkyrimPrefsValue("bFull Screen") != "0";
	}
}