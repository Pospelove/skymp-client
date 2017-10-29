#pragma once

class WorldCleaner
{
public:
	static WorldCleaner *GetSingleton();
	void Update();

	void SetFormProtected(uint32_t formID, bool val);
	bool IsFormProtected(uint32_t formID) const;
	void OverrideDefaultProcess(uint32_t formID, std::function<bool(TESObjectREFR *)> pred);

private:
	void DealWithReference(TESObjectREFR *ref);

	std::set<RefHandle> protectedForms;
	std::map<uint32_t, std::function<bool(TESObjectREFR *)>> preds;
	mutable dlf_mutex mutex;
};