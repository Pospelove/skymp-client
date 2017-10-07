#pragma once
#include "../CoreInterface/CoreInterface.h"
#include <memory>

namespace HitData_
{
	void Apply(Actor *actor, uint8_t hitAnimID, bool unsafe);
	bool IsPowerAttack(uint8_t hitAnimID);
	std::shared_ptr<uint8_t> UpdatePlayer();
}