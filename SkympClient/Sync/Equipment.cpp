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

	void Apply(Actor *actor, const Equipment &equipment) 
	{
		/*static std::map<Actor *, Equipment> equipmentLast;
		if (equipment.hands[0] != equipmentLast[actor].hands[0]
			|| equipment.hands[1] != equipmentLast[actor].hands[1])
		{
			if (equipment.hands[0])
				sd::AddItem(actor, equipment.hands[0], 1, true);
			if (equipment.hands[1])
				sd::AddItem(actor, equipment.hands[1], 1, true);
			equipmentLast[actor] = equipment;
		}*/

		SAFE_CALL("Equipment", [&] {
			//static std::map<Actor *, Equipment> equipmentLast;
			//if (equipment.hands[0] != equipmentLast[actor].hands[0] 
			//	|| equipment.hands[1] != equipmentLast[actor].hands[1])
			{
				if (equipment.hands[0] && equipment.hands[1]) // DualWield
				{
					for (int32_t i = 0; i <= 1; ++i)
					{
						auto form = sd::GetEquippedWeapon(actor, i);
						auto custom = CustomWeapHand(equipment.hands[i], i);
						if (form != nullptr)
						{
							if(form != custom)
							{
								sd::UnequipItem(actor, form, false, true);
								sd::RemoveItem(actor, form, -1, true, nullptr);
							}
						}
						if (form != custom)
							sd::EquipItem(actor, custom, false, true);
					}
				}
				if (equipment.hands[0] && !equipment.hands[1]) // RightOnly
				{
					auto weapLeft = sd::GetEquippedWeapon(actor, true);
					if (weapLeft != nullptr)
					{
						sd::UnequipItem(actor, weapLeft, false, true);
						sd::RemoveItem(actor, weapLeft, -1, true, nullptr);
					}
					auto weapRight = sd::GetEquippedWeapon(actor, false);
					if (weapRight != CustomWeapHand(equipment.hands[0], 0))
					{
						if (weapRight != nullptr)
						{
							sd::UnequipItem(actor, weapRight, false, true);
							sd::RemoveItem(actor, weapRight, -1, true, nullptr);
						}
						sd::EquipItem(actor, CustomWeapHand(equipment.hands[0], 0), false, true);
					}
				}
				if (!equipment.hands[0] && equipment.hands[1]) // LeftOnly
				{
					static auto unarmedRight = CustomWeapHand(nullptr, 0);
					auto weapRight = sd::GetEquippedWeapon(actor, false);
					if (weapRight != unarmedRight)
					{
						if (weapRight != nullptr)
						{
							sd::UnequipItem(actor, weapRight, false, true);
							sd::RemoveItem(actor, weapRight, -1, true, nullptr);
						}
						sd::EquipItem(actor, unarmedRight, false, true);
					}
					auto weapLeft = sd::GetEquippedWeapon(actor, true);
					auto custom = CustomWeapHand(equipment.hands[1], 1);
					if (weapLeft != custom)
					{
						if (weapLeft != nullptr)
						{
							sd::UnequipItem(actor, weapLeft, false, true);
							sd::RemoveItem(actor, weapLeft, -1, true, nullptr);
						}
						if (custom->GetEquipSlot() != GetLeftHandSlot())
							ErrorHandling::SendError("ERROR:Equipment Unassigned leftHand slot");
						if (custom == equipment.hands[1])
							ErrorHandling::SendError("ERROR:Equipment CustomWeapHand 1");
						if (custom->formID == equipment.hands[1]->formID)
							ErrorHandling::SendError("ERROR:Equipment CustomWeapHand 2");
						sd::EquipItem(actor, custom, false, true);
					}
				}
				if (!equipment.hands[0] && !equipment.hands[1]) // NoWeapon
				{
					for (int32_t i = 0; i <= 1; ++i)
					{
						auto form = sd::GetEquippedWeapon(actor, i);
						if (form != nullptr)
						{
							sd::UnequipItem(actor, form, false, true);
							sd::RemoveItem(actor, form, -1, true, nullptr);
						}
					}
				}
				//equipmentLast[actor] = equipment;
			}
		});

		SAFE_CALL("Equipment", [&] {
			static std::set<TESForm *> known;
			for (auto form : known)
			{
				if (sd::IsEquipped(actor, form) 
					&& equipment.other.find(form) == equipment.other.end())
				{
					sd::UnequipItem(actor, form, false, true);
				}
			}
			for (auto form : equipment.other)
			{
				if (sd::IsEquipped(actor, form) == false)
				{
					sd::EquipItem(actor, form, false, true);
					known.insert(form);
				}
			}
		});

	}
}