#include "../stdafx.h"
#include "CoreInterface.h"

struct ci::Perk::Impl
{
	BGSPerk *perk;
	float *requiredSkillLevelPtr = nullptr;
	float requiresSkillLevel = 0;
	uint32_t requiredPerkID = 0;
	dlf_mutex m;
};

ci::Perk::Perk(uint32_t formID) : pimpl(new Impl)
{
	pimpl->perk = (BGSPerk *)LookupFormByID(formID);
	if (pimpl->perk && pimpl->perk->formType != FormType::Perk)
		pimpl->perk = nullptr;

	if (pimpl->perk == nullptr)
	{
		std::stringstream ss;
		ss << std::hex << "Bad perk formID " << formID;
		ErrorHandling::SendError("%s", ss.str().data());
		throw ~0;
	}

	try {
		if (pimpl->perk->conditions.nodes != nullptr)
		{
			auto first = pimpl->perk->conditions.nodes;
			decltype(first) second = nullptr;
			if (first != nullptr)
			{
				pimpl->requiredSkillLevelPtr = (float *)&first->comparisonValue;
				second = first->next;
				if (second != nullptr)
				{
					auto form = (TESForm *)second->param1;
					if (form != nullptr)
						pimpl->requiredPerkID = form->formID;
				}
			}
			if (pimpl->requiredSkillLevelPtr != nullptr && *pimpl->requiredSkillLevelPtr == 1.0f) // oops
			{
				if (second != nullptr)
					pimpl->requiredSkillLevelPtr = (float *)&second->comparisonValue;
				if (first != nullptr)
				{
					auto form = (TESForm *)first->param1;
					if (form != nullptr)
						pimpl->requiredPerkID = form->formID;
				}
			}
		}
	}
	catch (...) {
		if (pimpl->perk->conditions.nodes != nullptr)
		{
			auto first = pimpl->perk->conditions.nodes;
			decltype(first) second = nullptr;
			if (first != nullptr)
			{
				auto form = (TESForm *)first->param1;
				if (form != nullptr)
					pimpl->requiredPerkID = form->formID;
				second = first->next;
				if (second != nullptr)
				{
					pimpl->requiredSkillLevelPtr = (float *)&second->comparisonValue;
				}
			}
		}
	}
}

uint32_t ci::Perk::GetFormID() const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return pimpl->perk->formID;
}

void ci::Perk::UpdateRequiredSkillLevel()
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	if (pimpl->requiredSkillLevelPtr)
		*pimpl->requiredSkillLevelPtr = this->GetRequiredSkillLevel();
}

void ci::Perk::SetRequiredSkillLevel(float skillLevel)
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	pimpl->requiresSkillLevel = skillLevel;
	this->UpdateRequiredSkillLevel();
}

float ci::Perk::GetRequiredSkillLevel() const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return pimpl->requiresSkillLevel;
}

uint32_t ci::Perk::GetLocalRequiredPerkFormID() const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return pimpl->requiredPerkID;
}

const std::list<const ci::Perk *> ci::Perk::GetLocalRequirements() const
{
	std::list<uint32_t> perksToAdd;
	perksToAdd.push_back(this->GetFormID());

	static std::map<uint32_t, const ci::Perk *> internalPerkMap;
	internalPerkMap[this->GetFormID()] = this;

	auto ciPerkByID = [](uint32_t id)->const ci::Perk *{
		try {
		return internalPerkMap.at(id);
	}
	catch (...) {
		internalPerkMap[id] = new ci::Perk(id);
		return internalPerkMap[id];
	}
	};

	auto requiredPerk = (BGSPerk *)LookupFormByID(this->GetLocalRequiredPerkFormID());
	while (true)
	{
		if (requiredPerk != nullptr && requiredPerk->formType != FormType::Perk)
			requiredPerk = nullptr;

		if (requiredPerk == nullptr)
			break;

		perksToAdd.push_front(requiredPerk->GetFormID());
		try {
			requiredPerk = (BGSPerk *)LookupFormByID(ciPerkByID(requiredPerk->GetFormID())->GetLocalRequiredPerkFormID());
		}
		catch (...) { // i don't know why
			requiredPerk = nullptr;
		}
	}

	std::list<const ci::Perk *> ciPerksToAdd;
	for (auto formID : perksToAdd)
		ciPerksToAdd.push_back(ciPerkByID(formID));
	return ciPerksToAdd;
}