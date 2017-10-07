#pragma once
#include "../CoreInterface/ciTypes.h"

class ILookSynchronizer
{
public:
	virtual TESNPC *Apply(const ci::LookData &lookData) = 0;
	virtual void Apply(const ci::LookData &lookData, Actor *actor) = 0;
	virtual void ApplyTintMasks(TESNPC *npc, const std::vector<ci::LookData::TintMask> &result_tintmasks) = 0;
	virtual ci::LookData GetFromPlayer(bool noCache = false) = 0;
	virtual ci::LookData GetFromNPC(TESNPC *npc) = 0;
	virtual void ShowRaceMenu() = 0;

	static ILookSynchronizer *GetV17();
	static ILookSynchronizer *GetV16();
};