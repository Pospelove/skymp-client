#include "../stdafx.h"
#include "CoreInterface.h"

namespace ci
{
	bool IsBadSpell(uint32_t baseID)
	{
		return baseID == 0x0004DBA4;
	}

	struct Spell::Impl
	{
		uint32_t existingSpellID = 0;
		MagicItem *spell = nullptr;
		float cost = 0;
		dlf_mutex m{"ci_spell_impl"};
	};

	Spell::Spell(uint32_t existingSpellID) : pimpl(new Impl)
	{
		auto srcSpell = (MagicItem *)LookupFormByID(existingSpellID);
		if (srcSpell == nullptr || srcSpell->formType != FormType::Spell)
		{
			std::stringstream ss;
			ss << "ERROR:Spell Invalid form ID " << std::hex << "0x" << existingSpellID;
			ErrorHandling::SendError(ss.str().data());
			throw ~0;
		}

		pimpl->existingSpellID = existingSpellID;

		if (IsBadSpell(existingSpellID))
		{
			pimpl->spell = srcSpell;
		}
		else
		{
			pimpl->spell = FormHeap_Allocate<SpellItem>();
			memcpy(pimpl->spell, srcSpell, sizeof SpellItem);
			pimpl->spell->formID = 0;
			pimpl->spell->SetFormID(Utility::NewFormID(), 1);

			auto &el = pimpl->spell->effectItemList;
			auto newArr = new BSTArray<EffectItem *>();
			memcpy(&el, newArr, sizeof(*newArr));
			el.clear();
		}
	}

	void Spell::SetCost(float c)
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		pimpl->cost = c;
	}

	uint32_t Spell::GetBase() const
	{
		return pimpl->existingSpellID;
	}

	float Spell::GetCost() const
	{
		return pimpl->cost;
	}

	uint32_t Spell::GetFormID() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		return pimpl->spell->GetFormID();
	}

	MagicItem *Spell::GetMagicItem() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		if (IsBadSpell(pimpl->existingSpellID))
			return nullptr;
		return pimpl->spell;
	}
}