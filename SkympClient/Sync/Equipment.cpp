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
				leftHandWeap->SetEquipSlot(eqSlots.leftHand);
				rightHandWeap->SetEquipSlot(eqSlots.rightHand);
				leftHandWeap->formID = 0;
				leftHandWeap->SetFormID(Utility::NewFormID(), 1);
				rightHandWeap->formID = 0;
				rightHandWeap->SetFormID(Utility::NewFormID(), 1);
				//leftHandWeap->attackDamage = 0.001;
				//rightHandWeap->attackDamage = 0.001;
				data[source] = { leftHandWeap, rightHandWeap };

				SET_TIMER(0, [=] {
					auto funcs = { &cd::IsSword, &cd::IsWarAxe, &cd::IsDagger, &cd::IsMace };
					for (auto &func : funcs)
					{
						func((TESObjectWEAP *)source, [=](bool is) {
							if (is)
							{
								leftHandWeap->SetEquipSlot(eqSlots.bothHands);
								rightHandWeap->SetEquipSlot(eqSlots.bothHands);
							}
						});
					}
				});
			}
			return data[source][isRightHand];
		}
		return nullptr;
	}
	void SetEquippedObjects(Actor *actor, TESForm *leftHandWeap, TESForm *rightHandWeap)
	{
		static TESObjectWEAP *leftHandUnarmed, *rightHandUnarmed = nullptr;
		if (leftHandUnarmed == nullptr)
		{
			leftHandUnarmed = (TESObjectWEAP *)malloc(sizeof TESObjectWEAP);
			memcpy(leftHandUnarmed, LookupFormByID(UnarmedBothHandsID), sizeof TESObjectWEAP);
			leftHandUnarmed->SetEquipSlot(EquipSlots().leftHand);
		}
		if (rightHandUnarmed == nullptr)
		{
			rightHandUnarmed = (TESObjectWEAP *)malloc(sizeof TESObjectWEAP);
			memcpy(rightHandUnarmed, LookupFormByID(UnarmedBothHandsID), sizeof TESObjectWEAP);
			rightHandUnarmed->SetEquipSlot(EquipSlots().rightHand);
		}

		static std::set<TESForm *> known;
		for (auto it = known.begin(); it != known.end(); ++it)
		{
			if ((*it) && (*it)->IsWeapon())
				sd::RemoveItem(actor, *it, -1, 1, 0);
		}

		if (leftHandWeap)
		{
			if (/*leftHandWeap->formID == Torch*/ leftHandWeap->formType == FormType::Light)
			{
				sd::AddItem(actor, leftHandWeap, 1, 1);
				sd::EquipItem(actor, leftHandWeap, true, true);
				known.insert(leftHandWeap);
			}
			else if ((int32_t)leftHandWeap->formType == TESObjectWEAP::kTypeID)
			{
				leftHandWeap = CustomWeapHand(leftHandWeap, false);
				if (leftHandWeap)
				{
					sd::AddItem(actor, leftHandWeap, 1, 1);
					sd::EquipItem(actor, leftHandWeap, true, true);
					known.insert(leftHandWeap);
				}
			}
			else if ((int32_t)leftHandWeap->formType == SpellItem::kTypeID)
				sd::EquipSpell(actor, (SpellItem *)leftHandWeap, 0);
		}
		else
		{
			sd::AddItem(actor, leftHandUnarmed, 1, 1);
			known.insert(leftHandUnarmed);
			sd::EquipItem(actor, leftHandUnarmed, true, true);
		}

		if (rightHandWeap)
		{
			if ((int32_t)rightHandWeap->formType == TESObjectWEAP::kTypeID)
			{
				rightHandWeap = CustomWeapHand(rightHandWeap, true);
				if (rightHandWeap)
				{
					sd::AddItem(actor, rightHandWeap, 1, 1);
					known.insert(rightHandWeap);
					sd::EquipItem(actor, rightHandWeap, true, true);
				}
			}
			else if ((int32_t)rightHandWeap->formType == SpellItem::kTypeID)
				sd::EquipSpell(actor, (SpellItem *)rightHandWeap, 1);
		}
		else
		{
			sd::AddItem(actor, leftHandUnarmed, 1, 1);
			known.insert(rightHandWeap);
			sd::EquipItem(actor, rightHandUnarmed, true, true);
		}
	}
}