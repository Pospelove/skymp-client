#pragma once

class LockTimer
{
public:
	explicit LockTimer(uint32_t ms);
	void Lock(void *ownerFuncPtr);
	bool TryLock(void *ownerFuncPtr);

	const uint32_t ms;

private:
	uint64_t lockEndTime;
	void *ownerFuncPtr;
};