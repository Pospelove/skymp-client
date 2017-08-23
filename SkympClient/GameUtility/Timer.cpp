#include "../stdafx.h"
#include "Timer.h"

std::list<Timer::callback> callbacks;
std::recursive_mutex Timer::mutex;

void Timer::Set(UInt32 ms, callback fn) 
{
	try
	{
		/*if (!ms)
		{
			std::lock_guard<std::recursive_mutex> l(mutex);
			callbacks.push_back(fn);
			return;
		}*/

		std::thread([=] {
			enum { min_sleep = 20 };
			auto num_sleeps = ms / min_sleep;
			if (!num_sleeps)
				num_sleeps++;
			for (UInt32 i = 0; i < num_sleeps; ++i)
			{
				static auto &pause = *(UInt32 *)0x1B3E428;
				while (pause)
					Sleep(1);
				Sleep(min_sleep);
			}
			std::lock_guard<std::recursive_mutex> l(mutex);
			callbacks.push_back(fn);
		}).detach();
	}
	catch (const std::exception &e)
	{
		sd::PrintNote((char *)e.what());
	}
}

void Timer::Update()
{
	try
	{
		std::lock_guard<std::recursive_mutex> l(mutex);
		for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
			it->operator()();
		callbacks.clear();
	}
	catch (const std::exception &e)
	{
		sd::PrintNote((char *)e.what());
	}
}