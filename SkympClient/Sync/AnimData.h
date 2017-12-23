#pragma once
#include "../CoreInterface/CoreInterface.h"
#include <memory>

namespace AnimData_
{
	using AnimID = uint32_t;

	enum class AnimSource
	{
		PlayableRace,
		Server,
	};

	// SD Thread only:
	bool Apply(Actor *actor, AnimID animID, bool unsafe);
	void RegisterAnimation(const std::string &animationEvent, AnimID animID);
	bool IsPowerAttack(AnimID hitAnimID);
	void Register(Actor *actor = g_thePlayer);
	std::shared_ptr<AnimID> UpdatePlayer(); // place it in while(1)
	std::shared_ptr<AnimID> UpdateActor(uint32_t actorID);

	AnimSource GetAnimSource(AnimID animID);
}