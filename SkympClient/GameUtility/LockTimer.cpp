#include "../stdafx.h"
#include "LockTimer.h"

LockTimer::LockTimer(uint32_t ms) : ms(ms)
{
	lockEndTime = 0;
	ownerFuncPtr = nullptr;
}

void LockTimer::Lock(void *ownerFuncPtr)
{
	if (ownerFuncPtr != this->ownerFuncPtr)
	{
		while (clock() <= this->lockEndTime)
			Sleep(1);
	}

	this->lockEndTime = clock() + this->ms;
	this->ownerFuncPtr = ownerFuncPtr;
}

bool LockTimer::TryLock(void *ownerFuncPtr)
{
	if (ownerFuncPtr == this->ownerFuncPtr)
		return true;
	if (clock() > this->lockEndTime)
	{
		this->Lock(ownerFuncPtr);
		return true;
	}
	return false;
}