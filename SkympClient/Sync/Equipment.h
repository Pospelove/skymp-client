#pragma once

namespace Equipment_
{
	class Equipment
	{
	public:
		Equipment() {
			hands[0] = hands[1] = nullptr;
		}

		TESForm *hands[2];
		std::set<TESForm *> other;
	};


	// Note: Different spells in different hands are not supported
	// Note: Fast equipping/unequipping spells will cause crash
	void ApplyHands(Actor *actor, const Equipment &equipment);

	// For armor, ammo and torches
	void ApplyOther(Actor *actor, Equipment equipment);

	TESForm *GetEquippedObject(Actor *actor, int32_t handID);
}