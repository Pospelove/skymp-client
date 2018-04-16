// Requires include CoreInterface.h first
#pragma once

namespace ci
{
	void ApplyPackage(TESNPC *);
	void SetPackageSpell(size_t packageN, uint32_t spellID);

	class WorldSpell
	{
	public:
		static void Tick() { Update(); Apply(); };
		static const ci::Spell *Get();
	private:
		static void Update();
		static void Apply();
		static void SetCasterRealEquippedSpell(SpellItem *);
		static void Set(const ci::Spell *spell, uint32_t numCasters, uint32_t numDifferentSpells);
	};
}