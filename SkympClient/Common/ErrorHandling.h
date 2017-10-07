#pragma once
#include <memory>
#include <mutex>

namespace ErrorHandling
{
	void SendError(const char *fmt, ...);

	class DeadlockFreeMutex
	{
	public:
		DeadlockFreeMutex();
		~DeadlockFreeMutex();
		void lock();
		void unlock();
		bool try_lock();

	private:
		struct Impl;
		Impl *pimpl;
	};
}

using dlf_mutex = ErrorHandling::DeadlockFreeMutex;

#define SET_TIMER(...) Timer:: Set( __VA_ARGS__, __LINE__, __FUNCTION__)
#define SET_TIMER_LIGHT(...) Timer:: SetLight( __VA_ARGS__, __LINE__, __FUNCTION__)
#define SAFE_CALL(systemName, ...) try { __VA_ARGS__(); } catch(...) { ErrorHandling::SendError("ERROR:%s %s %u", std::string(systemName).data(), __FUNCTION__, __LINE__); };
#define SAFE_CALL_RETURN(systemName, fn, result) try { result = fn(); } catch(...) { ErrorHandling::SendError("ERROR:%s %s %u", std::string(systemName).data(), __FUNCTION__, __LINE__); };
