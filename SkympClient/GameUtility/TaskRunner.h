#pragma once

enum {
	TASK_RUNNER_1000_MS = 1000
};

template <uint32_t delay>
class TaskRunner
{
public:
	static void AddTask(std::function<void()> fn, int64_t priority);
};