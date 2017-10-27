#include "../stdafx.h"
#include "Equipment.h"
#include <array>

namespace Equipment_
{
	enum
	{
		UnarmedBothHandsID = 0x000001F4,
		Torch = 0x0001D4EC,
	};

	struct EquipSlots
	{
		BGSEquipSlot *leftHand = (BGSEquipSlot *)LookupFormByID(0x00013F43),
			*rightHand = (BGSEquipSlot *)LookupFormByID(0x00013F42),
			*eitherHand = (BGSEquipSlot *)LookupFormByID(0x00013F44),
			*bothHands = (BGSEquipSlot *)LookupFormByID(0x00013F45);
	};

	TESObjectWEAP *CustomWeapHand(TESForm *source, bool isLeftHand)
	{
		static auto eqSlots = EquipSlots();
		if (!source)
			source = LookupFormByID(UnarmedBothHandsID);

		if (source && (int32_t)source->formType == TESObjectWEAP::kTypeID)
		{
			static std::map<TESForm *, std::array<TESObjectWEAP *, 2>> data;
			auto it = data.find(source);
			if (it == data.end())
			{
				auto leftHandWeap = (TESObjectWEAP *)malloc(sizeof TESObjectWEAP),
					rightHandWeap = (TESObjectWEAP *)malloc(sizeof TESObjectWEAP);
				memcpy(leftHandWeap, source, sizeof TESObjectWEAP);
				memcpy(rightHandWeap, source, sizeof TESObjectWEAP);
				leftHandWeap->formID = 0;
				leftHandWeap->SetFormID(Utility::NewFormID(), 1);
				rightHandWeap->formID = 0;
				rightHandWeap->SetFormID(Utility::NewFormID(), 1);
				leftHandWeap->SetEquipSlot(eqSlots.leftHand);
				rightHandWeap->SetEquipSlot(eqSlots.rightHand);
				data[source] = { rightHandWeap, leftHandWeap };
			}
			return data[source][isLeftHand];
		}
		return nullptr;
	}

	void ApplyOther(Actor *actor, Equipment equipment)
	{
		enum {
			IronArrow = 0x0001397D
		};
		static auto src = (TESAmmo *)LookupFormByID(IronArrow);
		static auto invisibleAmmo = (TESAmmo *)nullptr;
		SAFE_CALL("Equipment", [&] {
			if (!invisibleAmmo)
			{
				invisibleAmmo = FormHeap_Allocate<TESAmmo>();
				memcpy(invisibleAmmo, src, sizeof TESAmmo);
				invisibleAmmo->formID = 0;
				invisibleAmmo->SetFormID(Utility::NewFormID(), 1);
				invisibleAmmo->SetModelName("");
				invisibleAmmo->settings.damage = 2;
			}
		});

		bool hasAmmo = false;
		for (auto form : equipment.other)
			if (form->formType == FormType::Ammo)
			{
				hasAmmo = true;
				break;
			}
		if (!hasAmmo)
			equipment.other.insert(invisibleAmmo);

		SAFE_CALL("Equipment", [&] {
			static std::set<TESForm *> known;
			for (auto form : known)
			{
				if (sd::IsEquipped(actor, form)
					&& equipment.other.find(form) == equipment.other.end()
					&& form != invisibleAmmo)
				{
					sd::UnequipItem(actor, form, false, true);
					sd::RemoveItem(actor, form, -1, true, nullptr);
				}
			}
			for (auto form : equipment.other)
			{
				if (sd::IsEquipped(actor, form) == false)
				{
					if (form->formType == FormType::Ammo)
						sd::AddItem(actor, form, 100000000, true);
					sd::EquipItem(actor, form, true, true);
					known.insert(form);
				}
			}
		});
	}


	void ApplyHands(Actor *actor, const Equipment &equipment)
	{
		static std::set<TESForm *> known;
		for (auto form : known)
		{
			sd::UnequipItem(actor, form, false, true);
			sd::RemoveItem(actor, form, -1, true, nullptr);
		}
		for (int32_t i = 0; i <= 1; ++i)
		{
			auto src = equipment.hands[i];
			if (equipment.hands[0] == nullptr && equipment.hands[1] != nullptr)
				src = CustomWeapHand(src, i);
			if (src)
			{
				sd::AddItem(actor, src, 1, true);
				known.insert(src);
			}
		}

		static std::map<uint32_t, Equipment> lastEquipment;

		const auto formID = actor->formID;
		lastEquipment[formID] = equipment;

		// Check if equipped incorrect
		SET_TIMER(400, [=] {
			auto actor = (Actor *)LookupFormByID(formID);
			if (!actor || actor->formType != FormType::Reference)
				return;
			
			for (int32_t i = 0; i <= 1; ++i)
				if (equipment.hands[i] != lastEquipment[formID].hands[i])
					return;

			for (int32_t i = 0; i <= 1; ++i)
			{
				const bool isLeftHand = (i == 1); 
				const auto form = sd::GetEquippedWeapon(actor, isLeftHand);
				if (equipment.hands[i] == nullptr)
				{
					if (form != nullptr && form != CustomWeapHand(nullptr, isLeftHand))
					{
						ErrorHandling::SendError("ERROR:Equipment 1");
						sd::RemoveAllItems(actor, 0, 0, 0);
						return ApplyHands(actor, equipment);
					}
				}
				else
				{
					if (form == nullptr || form == CustomWeapHand(nullptr, isLeftHand))
					{
						ErrorHandling::SendError("ERROR:Equipment 2");
						sd::RemoveAllItems(actor, 0, 0, 0);
						return ApplyHands(actor, equipment);
					}
				}
			}
		});
	}
}