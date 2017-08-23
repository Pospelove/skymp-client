#pragma once

class Timer
{
public:
	using callback = std::function<void(void)>;

	static void Set(UInt32 ms, callback fn);
	static void Update();

private:
	Timer();
	static std::recursive_mutex mutex;
};