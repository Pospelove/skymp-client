#include "../stdafx.h"
#include "CoreInterface.h"

ci::IClientLogic *ci::IClientLogic::clientLogic = nullptr;
std::mutex ci::IClientLogic::callbacksMutex;

ci::IClientLogic::IClientLogic()
{
	if (!clientLogic)
		clientLogic = this;
}