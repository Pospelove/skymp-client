#include "../stdafx.h"
#include "Equipment.h"
#include <array>

namespace Equipment_
{
	TESObjectWEAP *GetUnarmedBothHands() {
		enum {
			UnarmedBothHandsID = 0x000001F4
		};
		return (TESObjectWEAP *)LookupFormByID(UnarmedBothHandsID);
	}

	class EquipSlots
	{
	public:
		EquipSlots() {
			std::set<TESForm *> s;
			s.insert(this->GetBothHands());
			s.insert(this->GetEitherHand());
			s.insert(this->GetLeftHand());
			s.insert(this->GetRightHand());
			assert(s.size() == 4);
		}

		BGSEquipSlot *GetLeftHand() {
			return (BGSEquipSlot *)LookupFormByID(0x00013F43);
		}
		BGSEquipSlot *GetRightHand() {
			return (BGSEquipSlot *)LookupFormByID(0x00013F42);
		}
		BGSEquipSlot *GetEitherHand() {
			return (BGSEquipSlot *)LookupFormByID(0x00013F44);
		}
		BGSEquipSlot *GetBothHands() {
			return (BGSEquipSlot *)LookupFormByID(0x00013F45);
		}
	};

	void Apply(Actor *actor, const Equipment &equipment) 
	{
		SAFE_CALL("Equipment", [&] {
			// Hands
			for (int32_t i = 0; i != 2; ++i)
			{
				auto weap = sd::GetEquippedWeapon(actor, i != 0);
				auto newWeap = equipment.hands[i];
				if (weap != newWeap)
				{
					if (weap != nullptr)
						sd::UnequipItem(actor, weap, false, true);
					if (equipment.hands[i] != nullptr)
						sd::EquipItem(actor, newWeap, false, true);
				}
			}
		});

		SAFE_CALL("Equipment", [&] {
			std::set<TESForm *> equippedNonWeap;
			auto extraContainerChanges = (ExtraContainerChanges *)actor->extraData.GetByType(ExtraDataType::ContainerChanges);
			if (extraContainerChanges != nullptr)
			{
				auto &entryList = *extraContainerChanges->changes->entryList;
				for (auto &entry : entryList)
				{
					auto form = entry->baseForm;
					if (form->formType == FormType::Armor
						|| form->formType == FormType::Ammo)
					{
						if (sd::IsEquipped(actor, form))
							equippedNonWeap.insert(form);
					}
				}
			}
			else
			{
				static bool sent = false;
				if (!sent)
				{
					ErrorHandling::SendError("ERROR:Equipment null extraContainerChanges");
					sent = true;
				}
			}
			for (auto form : equippedNonWeap)
			{
				if (equipment.other.find(form) == equipment.other.end())
					sd::UnequipItem(actor, form, false, true);
			}
		});
		SAFE_CALL("Equipment", [&] {
			for (auto form : equipment.other)
			{
				if (sd::IsEquipped(actor, form) == false)
					sd::EquipItem(actor, form, false, true);
			}
		});

	}
}