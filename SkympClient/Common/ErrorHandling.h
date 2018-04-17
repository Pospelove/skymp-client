#pragma once
#include <memory>
#include <mutex>

namespace ErrorHandling
{
	void SendError(const char *fmt, ...);

	class DeadlockFreeMutex
	{
	public:

		DeadlockFreeMutex(const std::string &identifier) : identifier(identifier)
		{
		}

		void set_limit(uint32_t newLimitMs)
		{
			this->limitMs = newLimitMs;
		}

		void lock()
		{
			clock_t startMoment = clock();
			while (1)
			{
				bool locked = this->mutex.try_lock();
				if (locked)
				{
					this->lockCount++;
					return;
				}
				Sleep(1);
				if (clock() - limitMs > startMoment)
				{
					SendError("ERROR:Mutex Would deadlock (%s)", this->identifier.data());
					return;
				}
			}
		}

		void unlock()
		{
			if (this->lockCount > 0)
			{
				this->lockCount--;
				this->mutex.unlock();
			}
		}

		bool try_lock()
		{
			const bool locked = this->mutex.try_lock();
			if (locked)
				this->lockCount++;
			return locked;
		}

	private:
		std::recursive_mutex mutex;
		int32_t lockCount = 0;
		const std::string identifier;
		uint32_t limitMs = 300;
	};
}

using dlf_mutex = ErrorHandling::DeadlockFreeMutex;

#define SET_TIMER(...) Timer:: Set( __VA_ARGS__, __LINE__, __FUNCTION__)
#define SET_TIMER_LIGHT(...) Timer:: SetLight( __VA_ARGS__, __LINE__, __FUNCTION__)
#define SAFE_CALL(systemName, ...) try { __VA_ARGS__(); } catch(...) { ErrorHandling::SendError("ERROR:%s %s %u", std::string(systemName).data(), __FUNCTION__, __LINE__); };
#define SAFE_CALL_RETURN(systemName, fn, result) try { result = fn(); } catch(...) { ErrorHandling::SendError("ERROR:%s %s %u", std::string(systemName).data(), __FUNCTION__, __LINE__); };
