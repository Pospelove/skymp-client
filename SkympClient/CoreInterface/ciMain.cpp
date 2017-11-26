#include "../stdafx.h"
#include "CoreInterface.h"

ci::Plugin *ci::Plugin::clientLogic = nullptr;
ci::Mutex ci::Plugin::callbacksMutex;

ci::Plugin::Plugin()
{
	if (!clientLogic)
		clientLogic = this;
}