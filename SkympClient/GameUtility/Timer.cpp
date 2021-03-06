#include "../stdafx.h"
#include "Timer.h"

struct Info
{
	Timer::callback callback;
	uint32_t dbgLine;
	std::string dbgFunc;
	clock_t endMoment;
};

std::list<Info> callbacks;
decltype(Timer::mutex) Timer::mutex{"timer"};

void Timer::Set(UInt32 ms, callback fn, uint32_t dbgLine, std::string dbgFunc)
{
	static dlf_mutex numThreadsMutex{"timer_numthreads"};
	static uint32_t numThreads;
	static std::map<std::string, uint32_t> topUsedFunctions;
	try
	{
		std::thread([=] {
			{
				std::lock_guard<dlf_mutex> l(numThreadsMutex);
				++numThreads;
				++topUsedFunctions[dbgFunc];
			}
			enum {
				maxThreads = 500,
			};
			if (numThreads > maxThreads)
			{
				std::string topFn = "NULL";
				{
					uint32_t topVal = 0;
					std::lock_guard<dlf_mutex> l(numThreadsMutex);
					for (auto &pair : topUsedFunctions)
					{
						if (pair.second > topVal)
						{
							topVal = pair.second;
							topFn = pair.first;
						}
					}
				}
				static bool sent = false;
				if (!sent)
				{
					sent = true;
					ErrorHandling::SendError("FATAL:Timer Too many threads started (%u) (%s)", numThreads, topFn.data());
				}
				Sleep(1000);
				std::exit(EXIT_FAILURE);
			}
			enum {
				maxMs = 60 * 60 * 1000,
			};
			if (ms > maxMs)
			{
				ErrorHandling::SendError("FATAL:Timer Ms must not be greater than %d", maxMs);
				Sleep(1000);
				std::exit(EXIT_FAILURE);
			}
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
			{
				std::lock_guard<dlf_mutex> l(mutex);
				callbacks.push_back({ fn, dbgLine, dbgFunc });
			}
			{
				std::lock_guard<dlf_mutex> l(numThreadsMutex);
				--numThreads;
			}
		}).detach();
	}
	catch (const std::exception &e)
	{
		sd::PrintNote((char *)e.what());
		ErrorHandling::SendError("FATAL:Timer %s", e.what());
		Sleep(1000);
		std::exit(EXIT_FAILURE);
	}
}

void Timer::SetLight(UInt32 ms, callback fn, uint32_t dbgLine, std::string dbgFunc)
{
	try
	{
		Info info;
		info.callback = fn;
		info.dbgFunc = dbgFunc;
		info.dbgLine = dbgLine;
		info.endMoment = clock() + ms;
		std::lock_guard<dlf_mutex> l(mutex);
		callbacks.push_back(info);
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
		std::lock_guard<dlf_mutex> l(mutex);
		//if(sd::GetKeyPressed(0x31))
			//sd::PrintNote("%u", (uint32_t)callbacks.size());

		decltype(callbacks) notCalled = {};
		for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
		{
			if (it->endMoment > clock())
			{
				notCalled.push_back(*it);
				continue;
			}
			try {
				it->callback.operator()();
			}
			catch (...) {
				ErrorHandling::SendError("ERROR:MT:Timer %s %d", it->dbgFunc.data(), it->dbgLine);
			}
		}
		callbacks.clear();
		callbacks = std::move(notCalled);
	}
	catch (const std::exception &e)
	{
		sd::PrintNote((char *)e.what());
	}
}