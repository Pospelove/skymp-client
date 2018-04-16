// Requires include CoreInterface.h first
#pragma once

namespace ci
{
	void ApplyPackage(TESNPC *);
	void SetPackageSpell(size_t packageN, uint32_t spellID);

	namespace WorldSpell
	{
		void Apply();
		void SetCasterRealEquippedSpell(SpellItem *);
		void Set(const ci::Spell *spell, uint32_t numCasters, uint32_t numDifferentSpells);
		const ci::Spell *Get();
	}
}