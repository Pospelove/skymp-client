#include "../stdafx.h"
#include "CoreInterface.h"

struct ci::Enchantment::Impl
{
	uint32_t exitsintEnchID = 0;
	EnchantmentItem *ench = nullptr;
};

ci::Enchantment::Enchantment(uint32_t existingEnchID) : pimpl(new Impl)
{
	auto srcEnch = (MagicItem *)LookupFormByID(existingEnchID);
	if (srcEnch == nullptr || srcEnch->formType != FormType::Enchantment)
	{
		std::stringstream ss;
		ss << "ERROR:Enchantment Invalid form ID " << std::hex << "0x" << existingEnchID;
		ErrorHandling::SendError(ss.str().data());
		throw ~0;
	}

	pimpl->ench = FormHeap_Allocate<EnchantmentItem>();
	memcpy(pimpl->ench, srcEnch, sizeof EnchantmentItem);
	pimpl->ench->formID = 0;
	pimpl->ench->SetFormID(Utility::NewFormID(), 1);

	auto &el = pimpl->ench->effectItemList;
	auto newArr = new BSTArray<EffectItem *>();
	memcpy(&el, newArr, sizeof(*newArr));
	el.clear();

	pimpl->exitsintEnchID = existingEnchID;
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