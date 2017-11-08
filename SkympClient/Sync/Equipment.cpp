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

	std::map<TESForm *, TESForm *> sources;

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

				sources[leftHandWeap] = sources[rightHandWeap] = (SpellItem *)source;
			}

			return data[source][isLeftHand];
		}
		return nullptr;
	}

	SpellItem *CustomSpellHand(TESForm *source, int32_t handID)
	{
		static auto eqSlots = EquipSlots();
		if (!source)
			return nullptr;

		if (source && (int32_t)source->formType == SpellItem::kTypeID)
		{
			/*static*/ std::map<TESForm *, std::array<SpellItem *, 3>> data;
			auto it = data.find(source);
			if (it == data.end())
			{
				auto leftHandWeap = (SpellItem *)malloc(sizeof SpellItem),
					rightHandWeap = (SpellItem *)malloc(sizeof SpellItem),
					bothHandsWeap = (SpellItem *)malloc(sizeof SpellItem);

				memcpy(leftHandWeap, source, sizeof SpellItem);
				memcpy(rightHandWeap, source, sizeof SpellItem);
				memcpy(bothHandsWeap, source, sizeof SpellItem);

				leftHandWeap->formID = 0;
				rightHandWeap->formID = 0;
				bothHandsWeap->formID = 0;

				leftHandWeap->SetFormID(Utility::NewFormID(), 1);
				rightHandWeap->SetFormID(Utility::NewFormID(), 1);
				bothHandsWeap->SetFormID(Utility::NewFormID(), 1);

				leftHandWeap->SetEquipSlot(eqSlots.leftHand);
				rightHandWeap->SetEquipSlot(eqSlots.rightHand);
				bothHandsWeap->SetEquipSlot(eqSlots.bothHands);

				data[source] = { rightHandWeap, leftHandWeap, bothHandsWeap };

				for (auto &sp : data[source])
				{
					sp->hostile = true;
					sp->unk44 = nullptr;
				}

				sources[leftHandWeap] = sources[rightHandWeap] = sources[bothHandsWeap] = (SpellItem *)source;
			}
			return data[source][handID];
		}
		return nullptr;
	}

	TESForm *GetSourceByCustom(TESForm *custom)
	{
		auto result = sources[custom];
		if (result == nullptr)
			return custom;
		if (result->formID == UnarmedBothHandsID)
			return nullptr;
		return result;
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

	std::set<TESForm *> known;
	std::map<uint32_t, clock_t> lastMagicApply;
	std::map<uint32_t, int32_t> sessionsMagic;
	std::map<uint32_t, int32_t> sessionsBow;
	std::map<uint32_t, std::function<bool()>> isAiming;

	bool IsAiming(Actor *actor)
	{
		if (!actor)
			return false;
		if (!isAiming[actor->formID])
			return false;
		return isAiming[actor->formID]();
	}

	void SetSpellTimer(cd::Value<Actor> cdAc, int32_t session, std::function<bool()> f)
	{
		try {
			if (sessionsMagic[cdAc.GetFormID()] == session)
			{
				if (!f())
				{
					SET_TIMER_LIGHT(1333, [=] {
						SetSpellTimer(cdAc, session, f);
					});
				}
			}
		}
		catch (...) {
			ErrorHandling::SendError("ERROR:Equipment SetSpellTimer()");
		}
	}

	void EquipBow(cd::Value<Actor> cdAc, int32_t session, TESForm *bowSrc, const bool lastIsAiming, uint32_t timerMS = 333)
	{
		try {
			if (!bowSrc)
				return ErrorHandling::SendError("ERROR:Equipment Bow is nullptr");

			if (session != sessionsBow[cdAc.GetFormID()])
				return; // Normal exit

			auto actor = (Actor *)cdAc;
			if (!actor)
				return;

			const bool isAiming = IsAiming(actor);

			const auto bowCustom = CustomWeapHand(bowSrc, 0);

			if (lastIsAiming != isAiming)
			{
				if (isAiming)
				{
					sd::RemoveItem(actor, bowSrc, -1, true, nullptr);
					sd::AddItem(actor, bowCustom, 1, true);
					timerMS = 4000;
				}
				else
				{
					sd::RemoveItem(actor, bowCustom, -1, true, nullptr);
					sd::AddItem(actor, bowSrc, 1, true);
					timerMS = 100;
				}
			}

			known.insert(bowCustom);
			known.insert(bowSrc);

			SET_TIMER_LIGHT(timerMS, [=] {
				EquipBow(cdAc, session, bowSrc, isAiming, timerMS);
			});
		}
		catch (...) {
			ErrorHandling::SendError("ERROR:Equipment EquipBow()");
		}
	}

	void ApplyHands(Actor *actor, const Equipment &equipment, std::function<bool()> isAiming)
	{
		Equipment_::isAiming[actor->formID] = isAiming;

		SAFE_CALL("Equipment", [&] {
			for (auto form : known)
			{
				switch (form->formType)
				{
				case FormType::Weapon:
					if (form != CustomWeapHand(equipment.hands[0], 0))
						if (form != CustomWeapHand(equipment.hands[1], 1))
							sd::RemoveItem(actor, form, -1, true, nullptr);
					break;
				case FormType::Spell:
					for (int32_t s = 0; s != 3; ++s)
					{
						sd::UnequipSpell(actor, (SpellItem *)form, s);
					}
					sd::RemoveSpell(actor, (SpellItem *)form);
					break;
				}
			}
		});

		SAFE_CALL("Equipment", [&] {
			for (int32_t i = 0; i <= 1; ++i)
			{
				const auto src = equipment.hands[i];
				TESForm *form = nullptr;
				if (!src || src->formType == FormType::Weapon)
				{
					switch (src ? ((TESObjectWEAP *)src)->gameData.type : NULL)
					{
					case TESObjectWEAP::GameData::kType_TwoHandAxe:
					case TESObjectWEAP::GameData::kType_TwoHandSword:
					case TESObjectWEAP::GameData::kType_2HS:
					case TESObjectWEAP::GameData::kType_2HA:
					case TESObjectWEAP::GameData::kType_CBow:
					case TESObjectWEAP::GameData::kType_Bow2:
						form = src;
						break;
					case TESObjectWEAP::GameData::kType_Bow:
					{
						const cd::Value<Actor> cdAc = actor;
						const int32_t session = rand();
						sessionsBow[cdAc.GetFormID()] = session;
						const bool notAiming = !IsAiming(actor);
						EquipBow(cdAc, session, src, notAiming);
						break;
					}
					default:
						if (i != 1 || src != nullptr)
							form = CustomWeapHand(src, i);
						break;

					}

					if (form != nullptr)
					{
						if (sd::GetItemCount(actor, form) == 0)
							sd::AddItem(actor, form, 1, true);
					}

					if (form != nullptr)
						known.insert(form);
				}
				else if (src->formType == FormType::Spell)
				{
					form = CustomSpellHand(src, i);

					if (src == equipment.hands[!i]) // Dual wield magic
					{
						enum {
							BothHands = 2,
						};
						form = CustomSpellHand(src, BothHands);
					}

					if (form != nullptr)
					{
						if (clock() - lastMagicApply[actor->formID] > 750)
						{
							sd::AddSpell(actor, (SpellItem *)form, rand() % 2 != 0);

							const cd::Value<Actor> cdActor = actor;

							auto f = [=] {
								for (int32_t j = 0; j != 25; ++j)
									cd::EquipSpell(cdActor, (SpellItem *)form, !i);
								return cdActor.operator Actor *() == nullptr
									|| sd::GetEquippedSpell(cdActor, 0) == form
									|| sd::GetEquippedSpell(cdActor, 1) == form;
							};

							const int32_t session = rand();
							sessionsMagic[actor->formID] = session;

							SetSpellTimer(cdActor, session, f);

							known.insert(form);
							lastMagicApply[actor->formID] = clock();

							// Collect garbage
							const auto copy = lastMagicApply;
							for (auto &pair : copy)
							{
								auto formID = pair.first;
								if (LookupFormByID(formID) == nullptr)
									lastMagicApply.erase(formID);
							}
						}
					}
				}
			}
		});
	}

	TESForm *GetEquippedObject(Actor *actor, int32_t handID)
	{
		auto weap = sd::GetEquippedWeapon(actor, handID);
		if (weap)
			return GetSourceByCustom(weap);

		auto spell = sd::GetEquippedSpell(actor, !handID);
		if (spell)
			return GetSourceByCustom(spell);

		return nullptr;
	}
}