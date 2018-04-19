#include "../stdafx.h"
#include "CoreInterface.h"

ci::IClientLogic *ci::IClientLogic::clientLogic = nullptr;

ci::IClientLogic::IClientLogic()
{
	if (!clientLogic)
		clientLogic = this;
}

void ci::IClientLogic::QueueCallback(std::function<void()> fn)
{
	static std::queue<std::function<void()>> queue;
	static std::recursive_mutex m;

	std::lock_guard<std::recursive_mutex> l(m);
	queue.push(fn);

	class ThreadStarter
	{
	public:
		ThreadStarter()
		{
			std::thread([]{
				while (true)
				{
					Sleep(10);
					std::lock_guard<std::recursive_mutex> l(m);

					if (queue.empty() == false)
					{
						queue.front()();
						queue.pop();
					}

					try {
						IClientLogic::clientLogic->OnUpdate();
					}
					catch (...) {
						ErrorHandling::SendError("ERROR:ClientLogic OnUpdate()");
					}
				}
			}).detach();
		}
	};
	static ThreadStarter thrStarter;
}