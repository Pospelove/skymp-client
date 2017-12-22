#pragma once
#include "../CoreInterface/CoreInterface.h"
#include <memory>

namespace AnimData_
{
	using AnimID = uint32_t;

	bool Apply(Actor *actor, AnimID animID, bool unsafe);
	void RegisterAnimation(const std::string &animationEvent, AnimID animID);
	bool IsPowerAttack(AnimID hitAnimID);
	void Register();
	std::shared_ptr<AnimID> UpdatePlayer();
}