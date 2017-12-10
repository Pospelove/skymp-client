#pragma once
#include <vector>

namespace Utility
{
	uint32_t NewFormID();
	std::vector<FormID> GetLoadedRefs(FormID worldSpaceID);
	bool IsForegroundProcess();
	HWND FindMainWindow();
	NiPoint3 GetNodeWorldPosition(TESObjectREFR *obj, BSFixedString nodeName, bool firstPerson);
	void MuteItemsSound();
	TESObjectSTAT *GetCastingArt(EffectSetting *effect);

	void RemoveAllKeywords(BGSKeywordForm *form);

	// Side effect: Removes standard keywords from form
	void AddKeyword(BGSKeywordForm *form, std::string keywordStr);
}