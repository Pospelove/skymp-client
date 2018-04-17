#include "../stdafx.h"
#include "TaskRunner.h"

#include "../CoreInterface/CoreInterface.h"

void _() {
	TaskRunner<TASK_RUNNER_1000_MS>::AddTask([] {}, 1);
}

template <uint32_t delay>
void TaskRunner<delay>::AddTask(std::function<void()> fn, int64_t priority)
{
	static dlf_mutex m{"taskrunner"};
	std::lock_guard<dlf_mutex> l(m);

	static std::map<int64_t, std::list<std::function<void()>>> tasks;
	tasks[priority].push_back(fn);

	static bool updateStarted = false;

	static std::function<void()> update = nullptr;
	if (update == nullptr)
	{
		update = [] {
			std::lock_guard<dlf_mutex> l(m);
			SET_TIMER_LIGHT(delay, update);
			for (auto &entry : tasks)
			{
				if (entry.second.empty() == false)
				{
					entry.second.front()();
					entry.second.pop_front();
					break;
				}
			}
		};
	}

	if (!updateStarted)
	{
		updateStarted = true;
		SET_TIMER_LIGHT(1, update);
	}
}