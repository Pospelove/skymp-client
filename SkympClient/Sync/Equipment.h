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

	void ApplyHands(Actor *actor, const Equipment &equipment);
	void ApplyOther(Actor *actor, Equipment equipment);
}