#include "../stdafx.h"
#include "CoreInterface.h"

using ci::ItemType;

struct ItemType::Impl
{
	struct {
		Class cl;
		Subclass subcl;
		uint32_t existingItemID;
	} data;

	TESForm *item = nullptr;
};

ItemType::ItemType(Class class_, Subclass subclass, uint32_t existingItemID) : 
	pimpl(new Impl)
{
	pimpl->data = { class_, subclass, existingItemID };

	if (LookupFormByID(existingItemID) == 0)
	{
		std::stringstream ss;
		ss << "ERROR:ItemType Invalid form ID " << std::hex << "0x" << existingItemID;
		ErrorHandling::SendError(ss.str().data());
		throw ~0;
	}

	switch (pimpl->data.cl)
	{
	case Class::Misc:
		if (subclass == Subclass::MISC_Lockpick)
		{
			pimpl->item = (TESObjectMISC *)LookupFormByID(existingItemID);
		}
		else if (subclass == Subclass::MISC_Torch)
		{
			pimpl->item = FormHeap_Allocate<TESObjectLIGH>();
			memcpy(pimpl->item, LookupFormByID(existingItemID), sizeof TESObjectLIGH);
			pimpl->item->formID = 0;
			pimpl->item->SetFormID(Utility::NewFormID(), true);
		}
		else
		{
			pimpl->item = FormHeap_Allocate<TESObjectMISC>();
			memcpy(pimpl->item, LookupFormByID(existingItemID), sizeof TESObjectMISC);
			pimpl->item->formID = 0;
			pimpl->item->SetFormID(Utility::NewFormID(), true);
		}
		break;

	case Class::Weapon:
		pimpl->item = (TESForm *)FormHeap_Allocate(sizeof TESObjectWEAP);
		memcpy(pimpl->item, LookupFormByID(existingItemID), sizeof TESObjectWEAP);
		pimpl->item->formID = 0;
		pimpl->item->SetFormID(Utility::NewFormID(), true);
		//((TESObjectWEAP *)pimpl->item)->BGSEquipType::unk04 = GetEitherHandSlot();
		break;
	case Class::Armor:
	{
		auto existingForm = LookupFormByID(existingItemID);
		if (existingForm->formType == FormType::Light)
		{
			pimpl->item = FormHeap_Allocate<TESObjectLIGH>();
			memcpy(pimpl->item, existingForm, sizeof TESObjectLIGH);
		}
		else
		{
			pimpl->item = FormHeap_Allocate<TESObjectARMO>();
			memcpy(pimpl->item, existingForm, sizeof TESObjectARMO);
		}
		pimpl->item->formID = 0;
		pimpl->item->SetFormID(Utility::NewFormID(), true);
		break;
	}
	case Class::Ammo:
		pimpl->item = FormHeap_Allocate<TESAmmo>();
		memcpy(pimpl->item, LookupFormByID(existingItemID), sizeof TESAmmo);
		pimpl->item->formID = 0;
		pimpl->item->SetFormID(Utility::NewFormID(), true);
		break;
	case Class::Ingredient:
		pimpl->item = FormHeap_Allocate<IngredientItem>();
		memcpy(pimpl->item, LookupFormByID(existingItemID), sizeof IngredientItem);
		pimpl->item->formID = 0;
		pimpl->item->SetFormID(Utility::NewFormID(), true);
		if (this->GetMagicItem() != nullptr)
			this->GetMagicItem()->effectItemList.clear();
		break;
	}
}

ItemType::Class ItemType::GetClass() const {
	return pimpl->data.cl;
}

ItemType::Subclass ItemType::GetSubclass() const {
	return pimpl->data.subcl;
}

std::wstring ItemType::GetNativeName() const {
	return StringToWstring(pimpl->item->GetName());
}

void ItemType::SetWeight(float w)
{
	auto item = pimpl->item;
	SET_TIMER(0, [=] {
		cd::SetWeight(item, w);
	});
}

void ItemType::SetGoldValue(uint32_t v)
{
	auto item = pimpl->item;
	SET_TIMER(0, [=] {
		cd::SetGoldValue(item, v);
	});
}

void ItemType::SetArmorRating(float v)
{
	auto item = pimpl->item;
	if (item->formType == FormType::Armor)
	{
		SET_TIMER(0, [=] {
			cd::SetArmorRating((TESObjectARMO *)item, v);
		});
	}
}

void ItemType::SetDamage(float v)
{
	auto item = pimpl->item;
	if (item->formType == FormType::Weapon)
	{
		SET_TIMER(0, [=] {
			((TESObjectWEAP *)item)->attackDamage = (uint16_t)v;
		});
	}
	else if (item->formType == FormType::Ammo)
	{
		SET_TIMER(0, [=] {
			((TESAmmo *)item)->settings.damage = (uint16_t)v;
		});
	}
}

uint32_t ItemType::GetFormID() const
{
	auto formID = 0ul;
	if (pimpl->item != nullptr)
		formID = pimpl->item->formID;
	return formID;
}

MagicItem *ItemType::GetMagicItem() const
{
	if (!pimpl->item)
		return nullptr;
	switch (pimpl->item->formType)
	{
	case FormType::Ingredient:
		return (MagicItem *)pimpl->item;
	default:
		return nullptr;
	}
}