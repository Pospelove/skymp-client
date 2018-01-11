#pragma once

class WorldCleaner
{
public:
	static WorldCleaner *GetSingleton();
	void Update();

	void SetFormProtected(uint32_t formID, bool val);
	bool IsFormProtected(uint32_t formID) const;
	void SetCallback(FormType t, std::function<void(TESObjectREFR *)> f);
	TESObjectREFR *FindFarObject();

private:
	void DealWithReference(TESObjectREFR *ref);

	std::set<RefHandle> protectedForms;
	mutable dlf_mutex mutex;
	std::map<FormType, std::function<void(TESObjectREFR *)>> callbacks;
	std::set<uint32_t> dict;
};