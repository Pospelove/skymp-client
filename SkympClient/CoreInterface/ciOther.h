#pragma once
#include <string>
#include <functional>
#include <memory>
#include <vector>

namespace ci
{
	enum class CommandType : uint8_t
	{
		Console = 0,
		CDScript = 1,
	};

	void ExecuteCommand(CommandType cmdType, const std::wstring &consoleCmdString);
	void ExecuteCommand(CommandType cmdType, const std::string &consoleCmdString);

	void RegisterAnimation(std::string animationName, uint32_t animationID);
	bool IsCombatAnimID(uint32_t animationID);

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

	void TraceCDCalls(bool trace);

	template <class Func>
	class Signal
	{
	public:
		Signal() : mutex(new ci::Mutex)
		{
		}

		template <class T>
		void Add(T f)
		{
			std::lock_guard<ci::Mutex> l(*mutex);
			mFunctions.push_back(f);
		}

		template <class... Args>
		void operator()(Args... args) const
		{
			std::lock_guard<ci::Mutex> l(*mutex);
			for (auto itor : mFunctions)
			{
				if (itor != nullptr)
					itor(args...);
			}
		}

		operator bool() const
		{
			return mFunctions.size() > 0;
		}

	private:
		std::shared_ptr<ci::Mutex> mutex;
		std::vector<std::function<Func>> mFunctions;
	};

	void RemoveAllKeywords(uint32_t baseID);
	void AddKeyword(uint32_t baseID, std::string keywordString);
}