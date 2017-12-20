#include "../stdafx.h"
#include "CoreInterface.h"
#include "../Overlay/Chat.h"

namespace Helper
{
	struct {
		dlf_mutex M;
		std::map<TESForm *, float> smithLevel;
	} smith;

	void SetSmithLevel(TESForm *form, float level)
	{
		std::lock_guard<dlf_mutex> l(smith.M);

		if (level <= 1)
			smith.smithLevel.erase(form);
		else
			smith.smithLevel[form] = level;
	}

	void UpdateItemsSmith()
	{
		SET_TIMER_LIGHT(300, UpdateItemsSmith);

		if (MenuManager::GetSingleton()->IsMenuOpen("Crafting Menu"))
			return;

		std::lock_guard<dlf_mutex> l(smith.M);

		static auto SKSESetItemHealthPercent = [](TESForm * baseForm, BaseExtraList* extraData, float value)
		{
			// Object must be a weapon, or armor
			if (baseForm) {
				if ((int32_t)baseForm->formType == TESObjectWEAP::kTypeID || (int32_t)baseForm->formType == TESObjectARMO::kTypeID) {
					ExtraHealth* xHealth = static_cast<ExtraHealth*>(extraData->GetExtraData<ExtraHealth>());
					if (xHealth) {
						xHealth->health = value;
					}
					else {
						ExtraHealth* newHealth = ExtraHealth::Create();
						newHealth->health = value;
						extraData->Add(ExtraDataType::Health, newHealth);
					}
				}
			}
		};


		const auto changes = g_thePlayer->extraData.GetExtraData<ExtraContainerChanges>();

		if (changes->changes->entryList == nullptr)
		{
			ErrorHandling::SendError("ERROR:LocalPlayer UpdateItemsSmith() Unable to load ExtraData");
			return;
		}

		const auto &entryList = *changes->changes->entryList;
		for (auto &entry : entryList)
		{
			if (!entry->extraList)
				continue;
			auto &entryExtraList = *entry->extraList;
			for (auto &extraList : entryExtraList)
			{
				float lvl;
				try {
					lvl = smith.smithLevel.at(entry->baseForm);
				}
				catch (...) {
					lvl = 0;
				}

				float val = 1.0f + (lvl / 10.0f);
				SKSESetItemHealthPercent(entry->baseForm, extraList, val);
			}
		}
	}
}

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
		this->SetEnchantment(nullptr);
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
		this->SetEnchantment(nullptr);
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
		break;
	case Class::Potion:
		pimpl->item = FormHeap_Allocate<AlchemyItem>();
		memcpy(pimpl->item, LookupFormByID(existingItemID), sizeof AlchemyItem);
		pimpl->item->formID = 0;
		pimpl->item->SetFormID(Utility::NewFormID(), true);
		/*if (subclass == Subclass::ALCH_Food)
			((AlchemyItem *)pimpl->item)->data.flags |= AlchemyItem::kFlag_Food;
		if (subclass == Subclass::ALCH_Poison)
			((AlchemyItem *)pimpl->item)->data.flags |= AlchemyItem::kFlag_Poison;*/
		break;
	case Class::SoulGem:
		pimpl->item = FormHeap_Allocate<TESSoulGem>();
		memcpy(pimpl->item, LookupFormByID(existingItemID), sizeof TESSoulGem);
		pimpl->item->formID = 0;
		pimpl->item->SetFormID(Utility::NewFormID(), true);
		break;
	case Class::Key:
		pimpl->item = FormHeap_Allocate<TESKey>();
		memcpy(pimpl->item, LookupFormByID(existingItemID), sizeof TESKey);
		pimpl->item->formID = 0;
		pimpl->item->SetFormID(Utility::NewFormID(), true);
		break;
	}

	if (this->GetMagicItem() != nullptr)
	{
		auto &el = this->GetMagicItem()->effectItemList;
		auto newArr = new BSTArray<EffectItem *>();
		memcpy(&el, newArr, sizeof(*newArr));
		el.clear();
	}

	ci::LocalPlayer::GetSingleton()->AddItem(this, 0, true); // register 
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

bool ItemType::IsCustomPotion() const {
	return this->GetClass() == Class::Potion && pimpl->data.existingItemID == ID_AlchemyItem::DefaultPotion;
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

void ItemType::GenPotionName()
{
	if (this->IsCustomPotion() == false)
		return;
	const bool isRussian = TheChat != nullptr && TheChat->IsRussianUser();
	auto pot = (AlchemyItem *)this->pimpl->item;

	// ...
}

void ItemType::SetEnchantment(const ci::Enchantment *ench)
{
	const auto enchForm = ench ? (EnchantmentItem *)LookupFormByID(ench->GetFormID()) : nullptr;

	const auto weap = (TESObjectWEAP *)pimpl->item;
	if (weap && weap->formType == FormType::Weapon)
		weap->enchantment = enchForm;

	const auto armor = (TESObjectARMO *)pimpl->item;
	if (armor && armor->formType == FormType::Armor)
		armor->enchantment = enchForm;
}

void ItemType::SetSoulSize(int32_t s)
{
	auto gem = (TESSoulGem *)pimpl->item;
	if (gem->formType == FormType::SoulGem)
		gem->soulSize = (uint8_t)s;
}

void ItemType::SetCapacity(int32_t s)
{
	auto gem = (TESSoulGem *)pimpl->item;
	if (gem->formType == FormType::SoulGem)
	{
		gem->gemSize = (uint8_t)s;
		gem->gemSize = NULL;
	}
}

void ItemType::SetItemHealth(float h)
{
	Helper::SetSmithLevel(pimpl->item, h);

	static bool started = false;
	if (started == false)
	{
		started = true;
		SET_TIMER_LIGHT(1, Helper::UpdateItemsSmith);
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
	case FormType::Potion:
		return (MagicItem *)pimpl->item;
	default:
		return nullptr;
	}
}