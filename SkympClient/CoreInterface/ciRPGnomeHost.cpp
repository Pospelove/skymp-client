#include "../stdafx.h"
#include "CoreInterface.h"
#include "ciRPGnomeHost.h"

#include "../Sync/SyncOptions.h"

std::map<uint32_t, clock_t> lastBehaviorUpdate;

inline void RunGnomeBehavior(TESObjectREFR *ref, const std::wstring &name)
{
	sd::EnableAI((Actor *)ref, false);
	sd::SetActorValue((Actor *)ref, "Invisibility", 100.0f);
	sd::AllowPCDialogue((Actor *)ref, false);
	cd::SetDisplayName(ref, WstringToString(name), true);
	sd::SetActorValue((Actor *)ref, "DamageResist", 100.0f);
	sd::SetActorValue((Actor *)ref, "MagicResist", 100.0f);
	sd::SetActorValue((Actor *)ref, "Health", 32000.f);
	sd::StopCombat((Actor *)ref);
}

struct ci::GnomeHost::Impl
{
	std::map<int32_t, std::unique_ptr<SimpleRef>> gnomes;
	std::wstring name = L" ";
};

ci::GnomeHost::GnomeHost()
{
	pimpl.reset(new Impl);
}

void ci::GnomeHost::SetActive(bool active)
{
	active ? this->Create() : this->Destroy();
}

bool ci::GnomeHost::IsGnomeExist(int32_t handID)
{
	try {
		return pimpl->gnomes.at(handID) != nullptr;
	}
	catch (...) {
		return false;
	}
}

uint32_t ci::GnomeHost::GetGnomeFormID(int32_t handID)
{
	try {
		return pimpl->gnomes.at(handID)->GetFormID();
	}
	catch (...) {
		return 0x00000000;
	}
}

void ci::GnomeHost::UpdateAndMoveTo(Actor *actor, const MovementData &movement, std::array<bool, 2> needsGnome)
{
	for (int32_t i = 0; i <= 1; ++i)
	{
		auto &gnome = pimpl->gnomes[i];
		if (gnome != nullptr)
		{
			NiPoint3 newPos;

			if (needsGnome[i])
			{
				newPos = Utility::GetNodeWorldPosition(actor, i ? "NPC L Hand [LHnd]" : "NPC R MagicNode [RMag]", false);

				const float angleRad = (float)movement.angleZ / 180.0f * acos(-1);
				float distance = SyncOptions::GetSingleton()->GetFloat("HANDGNOME_OFFSET_FORWARD_FROM_HAND");

				auto isCasting = [&](int32_t i) {
					return movement.castStage[i] != ci::MovementData::CastStage::None;
				};
				if (isCasting(!i) && !isCasting(i))
					newPos.z -= 48;

				if (sd::GetEquippedSpell(actor, !i) == nullptr || !actor->IsWeaponDrawn())
					distance += 48;
				else if (i == 1)
					distance += 44;
				newPos += {distance * sin(angleRad), distance * cos(angleRad), 0};
				newPos.z += SyncOptions::GetSingleton()->GetFloat("HANDGNOME_OFFSET_Z_FROM_HAND");
			}
			else
			{
				newPos = { 0, 0, -1000.f * 1000.f * 1000.f };
			}

			gnome->SetPos(newPos);
			gnome->SetRot({ 0,0,(float)movement.angleZ });

			lastBehaviorUpdate.erase(NULL);
			lastBehaviorUpdate.erase(~0);
			if (newPos.z > -100000000 && clock() - lastBehaviorUpdate[gnome->GetFormID()] > 12000)
			{
				const auto name = pimpl->name;
				gnome->TaskSingle([name](TESObjectREFR *ref) {
					RunGnomeBehavior(ref, name);
				});
				lastBehaviorUpdate[gnome->GetFormID()] = clock();
			}
		}
	}
}

void ci::GnomeHost::SetName(const std::wstring &name)
{
	pimpl->name = name;
}

void ci::GnomeHost::DoCombatSpellApply(uint32_t spellID, int32_t handID, uint32_t foxID)
{
	std::wstringstream ss;
	ss << L"DoCombatSpellApply " << spellID << L' ' << handID << L' ' << foxID;
	ci::Chat::AddMessage(ss.str());
	if (pimpl->gnomes[handID])
	{
		pimpl->gnomes[handID]->TaskSingle([=](TESObjectREFR *ref) {
			const auto refID = ref->formID;
			auto actor = (Actor *)LookupFormByID(refID);
			if (actor != nullptr)
			{
				auto foxRef = (TESObjectREFR *)LookupFormByID(foxID); // ok to be nullptr
				auto spellForm = (SpellItem *)LookupFormByID(spellID);
				if (spellForm != nullptr)
				{
					sd::DoCombatSpellApply(actor, spellForm, foxRef);
				}
			}
		});
	}
}

void ci::GnomeHost::Create()
{
	for (int32_t i = 0; i <= 1; ++i)
	{
		if (pimpl->gnomes[i] == nullptr)
		{
			static TESNPC *gnomeNpc = nullptr;
			if (gnomeNpc == nullptr)
			{
				const auto exampleNpc = ci::RemotePlayer(L" ", {}, {}, -1, -1, nullptr, "", nullptr).AllocateNPC();
				gnomeNpc = (TESNPC *)LookupFormByID(ID_TESNPC::WEAdventurerBattlemageBretonMFire);
				gnomeNpc->TESSpellList::unk04->numSpells = 0;
				gnomeNpc->TESSpellList::unk04->spells = nullptr;
				gnomeNpc->combatStyle = exampleNpc->GetCombatStyle();
				gnomeNpc->voiceType = exampleNpc->GetVoiceType();
				gnomeNpc->TESAIForm::unk10 = exampleNpc->TESAIForm::unk10;
			}

			gnomeNpc->defaultOutfit = gnomeNpc->sleepOutfit = nullptr;
			gnomeNpc->TESContainer::numEntries = 0;
			gnomeNpc->TESContainer::entries = nullptr;
			gnomeNpc->race = (TESRace *)LookupFormByID(0x00071E6A);
			gnomeNpc->height = 0.1f;
			gnomeNpc->TESSpellList::unk04->numSpells = 0;
			gnomeNpc->TESSpellList::unk04->spells = 0;
			gnomeNpc->TESSpellList::unk04->numShouts = 0;
			gnomeNpc->TESSpellList::unk04->shouts = 0;
			gnomeNpc->numHeadParts = 0;
			gnomeNpc->headparts = 0;

			pimpl->gnomes[i] = std::unique_ptr<SimpleRef>(new SimpleRef(gnomeNpc, { 0,0,0 }, SyncOptions::GetRespawnRadius(0)));
			const auto name = pimpl->name;
			pimpl->gnomes[i]->TaskPersist([=](TESObjectREFR *ref) {
				RunGnomeBehavior(ref, name);
			});
		}
	}
}

void ci::GnomeHost::Destroy()
{
	pimpl->gnomes.clear();
}