#include "../stdafx.h"
#include "CoreInterface.h"
#include "ciRPPackageApplier.h"

#include "ciRPEngineBase.h"

#include "Skyrim\Camera\PlayerCamera.h"

namespace ci
{
	class MagicAccess
	{
	public:
		template <class T>
		static uint32_t GetFormID(T *some) {
			return some->GetFormID();
		}
	};
}

namespace ci
{
	std::vector<std::pair<uint32_t, uint32_t>> packages = {
		{ ID_TESPackage::WCollegePracticeCastWard, ID_SpellItem::CollegePracticeWard },
	};

	struct WorldSpellData
	{
		const ci::Spell *spell = nullptr;
		uint32_t casters = 0;
		SpellItem *casterRealEquippedSpell = nullptr;
		uint32_t differentSpells = 0;
	};

	WorldSpellData worldSpell;
}

inline void SetPackageCondition_ZeroVariable01(TESPackage *package)
{
	static auto ambushSitPackage = (TESPackage *)LookupFormByID(ID_TESPackage::ambushSitPackage); // Condition: Variable01 == 0
	package->unk3C = ambushSitPackage->unk3C;
}

void ci::ApplyPackage(TESNPC *npc)
{
	// forceav variable10 1.0/0.0 to start/stop combat
	// forceav variable01 0.0/1.0 to start/stop casting

	auto nonDrawn = (TESPackage *)LookupFormByID(0x000654E2);
	auto drawn = (TESPackage *)LookupFormByID(0x0004E4BB);

	/* static */ auto useMagic = (TESPackage *) nullptr;
	if (!useMagic)
	{
		const size_t packageN = 0;
		const auto defaultSpell = ID_SpellItem::Flames;

		useMagic = (TESPackage *)LookupFormByID(packages[packageN].first);
		SetPackageCondition_ZeroVariable01(useMagic);
		useMagic->packageFlags = drawn->packageFlags;

		SetPackageSpell(packageN, defaultSpell);
	}

	if (!npc)
		return;
	auto &data = npc->TESAIForm::unk10;
	data.unk0 = (UInt32)useMagic;
	data.next = FormHeap_Allocate<TESAIForm::Data>();
	data.next->unk0 = (UInt32)drawn;
	data.next->next = FormHeap_Allocate<TESAIForm::Data>();
	data.next->next->unk0 = (UInt32)nonDrawn;
	data.next->next->next = nullptr;
}

void ci::SetPackageSpell(size_t packageN, uint32_t spellID)
{
	const auto collegePracticeWard = (SpellItem *)LookupFormByID(packages[packageN].second);
	const auto formID = collegePracticeWard->formID;
	const auto s = (SpellItem *)LookupFormByID(spellID);
	memcpy(collegePracticeWard, s, sizeof SpellItem);
	collegePracticeWard->data.calculations.cost = 0;
	collegePracticeWard->formID = formID;
}

void ci::WorldSpell::Apply()
{
	auto spell = worldSpell.spell ? LookupFormByID(MagicAccess::GetFormID(worldSpell.spell)) : nullptr;
	if (worldSpell.differentSpells == 1 || worldSpell.casters == 1)
		spell = worldSpell.casterRealEquippedSpell;

	static TESForm *appliedSpell = nullptr;
	if (appliedSpell != spell)
	{
		if (spell != nullptr)
		{
			auto dest = (SpellItem *)LookupFormByID(packages[0].second);

			const auto formID = dest->formID;
			memcpy(dest, spell, sizeof SpellItem);
			dest->formID = formID;

			if (dest->effectItemList.size() > 0)
			{
				const bool isTelekinesis = dest->effectItemList.front()->mgef->properties.archetype == EffectSetting::Properties::Archetype::kArchetype_Telekinesis;
				if (!isTelekinesis) // Clear effectItemList of Telekinesis would remove unique cast anim
					dest->effectItemList.clear();
			}
		}
		appliedSpell = spell;
	}
}

void ci::WorldSpell::SetCasterRealEquippedSpell(SpellItem *sp)
{
	worldSpell.casterRealEquippedSpell = sp;
}

void ci::WorldSpell::Set(const ci::Spell *spell, uint32_t numCasters, uint32_t numDifferentSpells)
{
	worldSpell.spell = spell;
	worldSpell.casters = numCasters;
	worldSpell.differentSpells = numDifferentSpells;
}

const ci::Spell *ci::WorldSpell::Get()
{
	return worldSpell.spell;
}

void ci::WorldSpell::Update()
{
	std::lock_guard<dlf_mutex> l(gMutex);

	std::map<const ci::Spell *, int32_t> worldSpellElection;
	int32_t max = -1;
	const ci::Spell *bestSpell = nullptr;

	std::set<const ci::Spell *> spells;

	std::for_each(allRemotePlayers.begin(), allRemotePlayers.end(), [&](RemotePlayer *p) {

		if (p->GetSyncMode() != (int32_t)MovementData_::SyncMode::Hard)
			return;

		{
			std::lock_guard<dlf_mutex> l(p->pimpl->mutex);
			if (p->pimpl->isSpawned == false)
				return;
			auto actor = (Actor *)LookupFormByID(p->pimpl->formID);
			if (actor == nullptr)
				return;
			const auto pos = p->GetPos();
			if (PlayerCamera::GetSingleton()->IsInScreen(pos)
				|| PlayerCamera::GetSingleton()->IsInScreen(pos + NiPoint3{ 0,0,64 })
				|| PlayerCamera::GetSingleton()->IsInScreen(pos + NiPoint3{ 0,0,128 }))
			{
			}
			else
				return;
		}

		for (int32_t i = 0; i <= 1; ++i)
			spells.insert(p->GetEquippedSpell(i));
		spells.erase(nullptr);

		const auto md = p->GetMovementData();
		for (int32_t i = 0; i <= 1; ++i)
		{
			if (md.castStage[i] != ci::MovementData::CastStage::None)
			{
				auto activeSpell = p->GetEquippedSpell(i);
				if (activeSpell != nullptr)
				{
					++worldSpellElection[activeSpell];
					if (worldSpellElection[activeSpell] > max)
					{
						max = worldSpellElection[activeSpell];
						bestSpell = activeSpell;

						auto actor = (Actor *)LookupFormByID(p->pimpl->formID);
						if (actor != nullptr)
							ci::WorldSpell::SetCasterRealEquippedSpell(
								sd::GetEquippedSpell(actor, !i)
							);
					}
					break;
				}
			}
		}

	});

	if (bestSpell != nullptr)
	{
		ci::WorldSpell::Set(bestSpell, max, (uint32_t)spells.size());
	}
}