#include "../stdafx.h"
#include "ErrorHandling.h"

#include "../Overlay/Chat.h"

namespace ErrorHandling
{
	void SendError(const char *fmt, ...)
	{
		char buffer[4096];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer, sizeof(buffer), fmt, args);
		va_end(args);
		const std::string str = buffer;

		static bool wasOpen = false;
		if (MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
			wasOpen = true;

		if ("ERROR:Mutex Would deadlock (timer)" == str) // false warning
			return;

		if (wasOpen && !MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
		{
			if (TheChat != nullptr && !memcmp(str.data(), "ERROR:", sizeof("ERROR:") - 1))
			{
				TheChat->AddChatMessage("#FF0000" + str);
			}
		}
		try {
			static std::mutex mutex;
			std::lock_guard<std::mutex> l(mutex);

			tm *newtime;
			time_t aclock;
			time(&aclock);
			newtime = localtime(&aclock);
			auto timeStamp = std::string(asctime(newtime));
			const auto pos = timeStamp.find(L':');
			timeStamp = '[' + std::string(timeStamp.begin() + pos - 2, timeStamp.begin() + pos + 6) + "]";

			std::ofstream ofstream;
			ofstream.open("skymp_log.txt", std::ios_base::app);

			static std::string lastOutput = "";
			static uint64_t i = 0;

			if (lastOutput.empty())
				ofstream << std::endl;

			std::string output = timeStamp + ' ' + str;
			if (lastOutput == output)
			{
				++i;
				if (i % 5 == 0 || i == 1)
					output = ' ' + std::to_string(i);
				else
					output = "";
			}
			else
			{
				lastOutput = output;
				output = "\n" + output;
				i =0;
			}

			ofstream << output;
			ofstream.flush();
		}
		catch (...) {
		}
	}
}