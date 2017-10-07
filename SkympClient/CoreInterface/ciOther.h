#pragma once
#include <string>
#include <functional>
#include <memory>

namespace ci
{
	enum class CommandType : uint8_t
	{
		Console = 0,
		CDScript = 1,
	};

	void ExecuteCommand(CommandType cmdType, const std::wstring &consoleCmdString);
	void ExecuteCommand(CommandType cmdType, const std::string &consoleCmdString);

	void ShowRaceMenu();
	void ShowMainMenu();

	void SetTimer(uint32_t ms, std::function<void(void)> fn);
	void SetUpdateRate(uint32_t ms);

	bool IsLoadScreenOpen();
	bool IsInPause();
	bool IsInDebug();

	bool IsWorldSpace(uint32_t formID);
	bool IsCell(uint32_t formID);

	void SetWeather(uint32_t weatherFormID);
	void SetWeatherType(uint8_t type);
	void SetGlobalVariable(uint32_t globalID, float value);

	class Mutex
	{
	public:
		Mutex();
		~Mutex();
		void lock();
		void unlock();
		bool try_lock();

	private:
		struct Impl;
		Impl *pimpl;
	};

	void Log(const std::wstring &str);
	void Log(const std::string &str);
	void Log(const char *fmt, ...);
}