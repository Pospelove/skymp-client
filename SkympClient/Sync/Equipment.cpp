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

	TESObjectWEAP *CustomWeapHand(TESForm *source, bool isRightHand)
	{
		static auto eqSlots = EquipSlots();
		if (!source)
			source = LookupFormByID(UnarmedBothHandsID);
		else if ((int32_t)source->formType == TESObjectWEAP::kTypeID &&
			((TESObjectWEAP *)source)->GetEquipSlot() == eqSlots.bothHands)
			return (TESObjectWEAP *)source;

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
				data[source] = { leftHandWeap, rightHandWeap };
			}
			return data[source][isRightHand];
		}
		return nullptr;
	}

	void Apply(Actor *actor, const Equipment &equipment) 
	{
		//SAFE_CALL("Equipment", [&] {
			// Hands
		static std::map<Actor *, Equipment> eqWas;
		static std::map<Actor *, std::set<TESForm *>> known;
		if (eqWas[actor].hands[0] != equipment.hands[0]
			|| eqWas[actor].hands[1] != equipment.hands[1])
		{
			for (auto form : known[actor])
			{
				if (form && form != CustomWeapHand(equipment.hands[0], true))
				{
					if (form != CustomWeapHand(equipment.hands[1], false))
					{
						sd::RemoveItem(actor, form, -1, true, nullptr);
					}
				}
			}

			for (int32_t i = 0; i < 2; ++i)
			{
				const bool isLeftHand = (i == 1);
				auto form = CustomWeapHand(equipment.hands[i], !isLeftHand);
				sd::EquipItem(actor, form, false, true);
				known[actor].insert(form);
			}

			eqWas[actor] = equipment;
		}

		//});

		/*SAFE_CALL("Equipment", [&] {
			std::set<TESForm *> equipped;
			auto extraContainerChanges = (ExtraContainerChanges *)actor->extraData.GetByType(ExtraDataType::ContainerChanges);
			if (extraContainerChanges != nullptr)
			{
				auto &entryList = *extraContainerChanges->changes->entryList;
				for (auto &entry : entryList)
				{
					auto form = entry->baseForm;
					if (sd::IsEquipped(actor, form))
						equipped.insert(form);
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
			for (auto form : equipped)
			{
				if (equipment.other.find(form) == equipment.other.end())
				{
					if (form->formType != FormType::Weapon)
						sd::UnequipItem(actor, form, false, true);
				}
			}
		});
		SAFE_CALL("Equipment", [&] {
			for (auto form : equipment.other)
			{
				if (sd::IsEquipped(actor, form) == false)
					sd::EquipItem(actor, form, false, true);
			}
		});*/

	}
}