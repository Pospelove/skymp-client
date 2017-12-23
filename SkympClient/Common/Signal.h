#pragma once
#pragma unmanaged

#include <functional>
#include <mutex>
#include <vector>

template <class Func>
class Signal
{
public:

	template <class T>
	void Add(T f)
	{
		std::lock_guard<std::mutex> l(mutex);
		mFunctions.push_back(f);
	}

	template <class... Args>
	void operator()(Args... args)
	{
		std::lock_guard<std::mutex> l(mutex);
		for (auto itor : mFunctions)
		{
			try {
				if (itor != nullptr)
					itor(args...);
			}
			catch (...) {
				ErrorHandling::SendError("ERROR:Signal Bad signal");
			}
		}
	}

private:
	mutable std::mutex mutex;
	std::vector<std::function<Func>> mFunctions;
};