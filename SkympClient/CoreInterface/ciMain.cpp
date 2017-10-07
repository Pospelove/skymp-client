#include "../stdafx.h"
#include "CoreInterface.h"

ci::IClientLogic *ci::IClientLogic::clientLogic = nullptr;
ci::Mutex ci::IClientLogic::callbacksMutex;

ci::IClientLogic::IClientLogic()
{
	if (!clientLogic)
		clientLogic = this;
}