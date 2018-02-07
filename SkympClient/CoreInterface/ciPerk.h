#pragma once
#include <list>

namespace ci
{
	class Perk
	{
	public:
		Perk(uint32_t formID);
		uint32_t GetFormID() const;
		void UpdateRequiredSkillLevel();
		void SetRequiredSkillLevel(float skillLevel);
		float GetRequiredSkillLevel() const;

		uint32_t GetLocalRequiredPerkFormID() const;
		const std::list<const ci::Perk *> ci::Perk::GetLocalRequirements() const;

	private:
		~Perk();

		struct Impl;
		Impl *const pimpl;
	};
}