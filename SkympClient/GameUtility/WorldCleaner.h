#pragma once

class WorldCleaner
{
public:
	static WorldCleaner *GetSingleton();
	void Update();

	void SetFormProtected(RefHandle formID, bool val);
	bool IsFormProtected(RefHandle formID) const;

private:
	void DealWithReference(TESObjectREFR *ref);

	std::set<RefHandle> protectedForms;
	mutable std::recursive_mutex mutex;
};