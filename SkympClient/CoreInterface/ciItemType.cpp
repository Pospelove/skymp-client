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
		break;
	case Class::Armor:
		pimpl->item = FormHeap_Allocate<TESObjectARMO>();
		memcpy(pimpl->item, LookupFormByID(existingItemID), sizeof TESObjectARMO);
		pimpl->item->formID = 0;
		pimpl->item->SetFormID(Utility::NewFormID(), true);
		break;
	case Class::Ammo:
		pimpl->item = FormHeap_Allocate<TESAmmo>();
		memcpy(pimpl->item, LookupFormByID(existingItemID), sizeof TESAmmo);
		pimpl->item->formID = 0;
		pimpl->item->SetFormID(Utility::NewFormID(), true);
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

void ItemType::SetEquipSlot(uint32_t id)
{
	auto item = pimpl->item;
	auto eqSlot = (BGSEquipSlot *)LookupFormByID(id);
	if (eqSlot == nullptr || eqSlot->formType != FormType::EquipSlot)
		return;
	((TESObjectWEAP *)item)->SetEquipSlot(eqSlot);
}

uint32_t ItemType::GetFormID() const
{
	auto formID = 0ul;
	if (pimpl->item != nullptr)
		formID = pimpl->item->formID;
	return formID;
}