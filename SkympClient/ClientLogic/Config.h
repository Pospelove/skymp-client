#pragma once
#include <sstream>
#include <fstream>
#include <unordered_map>

using cfg_t = std::unordered_map<std::string, std::string>;

inline cfg_t &GetCfg() {
	static cfg_t cfg;
	return cfg;
}

#define g_config (GetCfg())

#define CONFIG_NAME std::string("name")
#define CONFIG_IP std::string("server_ip")
#define CONFIG_PORT std::string("server_port")
#define CONFIG_SERVER_PASSWORD std::string("server_password")
#define CONFIG_TCP_ONLY std::string("tcp_only")
#define CONFIG_DRAW_DISTANCE std::string("draw_distance")
#define CONFIG_FOV std::string("fov")
#define CONFIG_TRANSLATE std::string("translate")
#define CONFIG_CHAT_OFFSET_X std::string("chat_offset_x")
#define CONFIG_CHAT_OFFSET_Y std::string("chat_offset_y")

#define cfgFile "skymp_config.ini"

inline int32_t read_cfg()
{
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
				g_config.insert({ key, value });
				++lines;
			}
		}
	}

	is_file.close();
	return lines;
}