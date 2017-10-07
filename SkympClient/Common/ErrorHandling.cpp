#include "../stdafx.h"
#include "ErrorHandling.h"

namespace ErrorHandling
{
	void SendError(const char *fmt, ...)
	{
		char buffer[4096];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer, sizeof(buffer), fmt, args);
		va_end(args);
		std::string str = buffer;
		Timer::Set(0, [=] {
			sd::PrintNote((char *)str.data());
		});

		std::thread([=] {
			try {
				static std::mutex mutex;
				std::lock_guard<std::mutex> l(mutex);

				tm *newtime;
				time_t aclock;
				time(&aclock);
				newtime = localtime(&aclock);
				auto result = std::string(asctime(newtime));
				const auto pos = result.find(L':');
				result = '[' + std::string(result.begin() + pos - 2, result.begin() + pos + 6) + "]";

				std::ofstream ofstream;
				ofstream.open("skymp_log.txt", std::ios_base::app);

				static bool firstWrite = true;
				if (firstWrite)
				{
					ofstream << std::endl;
					firstWrite = false;
				}

				ofstream << result << ' ' << str << std::endl;
			}
			catch (...) {
			}
		}).detach();
	}

	struct DeadlockFreeMutex::Impl
	{
		std::recursive_mutex mutex;
		int32_t lockCount = 0;
	};

	DeadlockFreeMutex::DeadlockFreeMutex() : pimpl(new Impl)
	{
	}

	DeadlockFreeMutex::~DeadlockFreeMutex()
	{
		delete pimpl;
	}

	void DeadlockFreeMutex::lock()
	{
		enum {
			limitMs = 300,
		};
		clock_t startMoment = clock();
		while (1)
		{
			bool locked = pimpl->mutex.try_lock();
			if (locked)
			{
				pimpl->lockCount++;
				return;
			}
			Sleep(1);
			if (clock() - limitMs > startMoment)
			{
				SendError("ERROR:Mutex Would deadlock");
				return;
			}
		}
	}

	void DeadlockFreeMutex::unlock()
	{
		if (pimpl->lockCount > 0)
		{
			pimpl->lockCount--;
			pimpl->mutex.unlock();
		}
	}

	bool DeadlockFreeMutex::try_lock()
	{
		const bool locked = pimpl->mutex.try_lock();
		if (locked)
			pimpl->lockCount++;
		return locked;
	}
}