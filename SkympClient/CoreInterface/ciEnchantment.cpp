#include "../stdafx.h"
#include "CoreInterface.h"

dlf_mutex enchMutex{"ci_enchantment"};

struct ci::Enchantment::Impl
{
	uint32_t exitsintEnchID = 0;
	EnchantmentItem *ench = nullptr;
};

extern std::map<TESForm *, const ci::Enchantment *> knownEnch;

ci::Enchantment::Enchantment(uint32_t existingEnchID, bool isCustomEnch) : pimpl(new Impl)
{
	std::lock_guard<dlf_mutex> l(enchMutex);

	auto srcEnch = (EnchantmentItem *)LookupFormByID(existingEnchID);
	if (srcEnch == nullptr || srcEnch->formType != FormType::Enchantment)
	{
		std::stringstream ss;
		ss << "ERROR:Enchantment Invalid form ID " << std::hex << "0x" << existingEnchID;
		ErrorHandling::SendError(ss.str().data());
		throw ~0;
	}

	static std::set<TESForm *> used;
	if (isCustomEnch || used.count(srcEnch) != 0)
	{
		pimpl->ench = FormHeap_Allocate<EnchantmentItem>();
		memcpy(pimpl->ench, srcEnch, sizeof EnchantmentItem);
		pimpl->ench->formID = 0;
		auto newFormID = Utility::NewFormID();
		pimpl->ench->SetFormID(newFormID, 1);
	}
	else
	{
		used.insert(srcEnch);
		pimpl->ench = srcEnch;
	}

	auto &el = pimpl->ench->effectItemList;
	auto newArr = new BSTArray<EffectItem *>();
	memcpy(&el, newArr, sizeof(*newArr));
	el.clear();

	pimpl->exitsintEnchID = existingEnchID;
	pimpl->ench->data.baseEnchantment = nullptr;

	this->SetPlayerKnows(false);

	knownEnch[pimpl->ench] = this;
}

uint32_t ci::Enchantment::GetBase() const
{
	return pimpl->exitsintEnchID;
}

MagicItem *ci::Enchantment::GetMagicItem() const
{
	return pimpl->ench;
}

uint32_t ci::Enchantment::GetFormID() const
{
	return pimpl->ench->GetFormID();
}

void ci::Enchantment::SetPlayerKnows(bool knows)
{
	std::lock_guard<dlf_mutex> l(enchMutex);

	const auto ench = pimpl->ench;
	ench->flags.playerKnows = knows;
	for (size_t i = 0; i != ((MagicItem *)ench)->effectItemList.GetSize(); ++i)
		((MagicItem *)ench)->effectItemList[i]->mgef->flags.playerKnows = knows;
}

bool ci::Enchantment::GetPlayerKnows() const
{
	std::lock_guard<dlf_mutex> l(enchMutex);

	const auto ench = pimpl->ench;
	bool result = ench->flags.playerKnows;
	for (size_t i = 0; i != ((MagicItem *)ench)->effectItemList.GetSize(); ++i)
		result = result || ((MagicItem *)ench)->effectItemList[i]->mgef->flags.playerKnows;
	return result;
}