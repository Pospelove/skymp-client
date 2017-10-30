#pragma once

class WorldCleaner
{
public:
	static WorldCleaner *GetSingleton();
	void Update();

	void SetFormProtected(uint32_t formID, bool val);
	bool IsFormProtected(uint32_t formID) const;

private:
	void DealWithReference(TESObjectREFR *ref);

	std::set<RefHandle> protectedForms;
	mutable dlf_mutex mutex;
};