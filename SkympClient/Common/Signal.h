#pragma once
#pragma unmanaged

#include <functional>
#include <mutex>
#include <vector>
#include <string>

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
		int32_t i = 0;
		for (auto itor : mFunctions)
		{
			++i;
			try {
				if (itor != nullptr)
					itor(args...);
			}
			catch (...) {
				auto str = "ERROR:Signal Bad signal " + std::to_string(i);
				ErrorHandling::SendError(str.data());
			}
		}
	}

private:
	mutable std::mutex mutex;
	std::vector<std::function<Func>> mFunctions;
};