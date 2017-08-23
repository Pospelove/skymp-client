#pragma once
#include "../CoreInterface/ciTypes.h"

namespace LookData_
{
	TESNPC *Apply(const ci::LookData &lookData);
	void Apply(const ci::LookData &lookData, Actor *actor);
	ci::LookData GetFromPlayer();
	void ShowRaceMenu();
	void ApplyTintMasks(TESNPC *npc, const std::vector<ci::LookData::TintMask> &result_tintmasks);
	void PreventCrash(TESNPC *npc);
	RefHandle FindNPCWithParams(TESRace *race, UInt8 isFemale, TESNPC::Color color);
}