#include "../stdafx.h"
#include "GameSettings.h"
#include <shlobj.h>

std::wstring GetFolder()
{
	std::mutex mutex;
	std::lock_guard<std::mutex> l(mutex);

	static std::wstring pathStr;
	if (pathStr.empty())
	{
		PWSTR path = nullptr;
		CoInitialize(NULL);
		SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_SIMPLE_IDLIST, NULL, &path);
		pathStr = path; 
		pathStr += L"\\My Games\\Skyrim\\";
	}
	return pathStr;
}

std::map<std::string, std::string> map;

std::string GameSettings::GetSkyrimPrefsValue(const std::string &variableName)
{
	std::mutex mutex;
	std::lock_guard<std::mutex> l(mutex);

	if (map.empty())
	{
		std::wstring cfgFile = GetFolder() + L"SkyrimPrefs.ini";
		std::ifstream is_file(cfgFile.data());

		if (is_file.bad())
		{
			ErrorHandling::SendError("ERROR:GameSettings wrong path '%s'", WstringToString(cfgFile).data());
			return "";
		}

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
					map.insert({ key, value });
				}
			}
		}

		is_file.close();
	}
	try {
		return map.at(variableName);
	}
	catch (...) {
		ErrorHandling::SendError("ERROR:GameSettings '%s'", variableName.data());
		return "";
	}
}