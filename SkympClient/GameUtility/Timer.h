#pragma once

class Timer
{
public:
	using callback = std::function<void(void)>;

	static void Set(UInt32 ms, callback fn, uint32_t dbgLine = 0, std::string dbgFile = "");
	static void SetLight(UInt32 ms, callback fn, uint32_t dbgLine = 0, std::string dbgFile = "");
	static void Update();

private:
	Timer();
	static dlf_mutex mutex;
};