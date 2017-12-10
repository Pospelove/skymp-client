#pragma once
#include <memory>

namespace ci
{
	class EffectItem;
	class Spell;
	class MagicEffect
	{
		friend class EffectItem;
		friend class Spell;

		friend class IActor;
		friend class RemotePlayer;
		friend class LocalPlayer;

	public:

		enum class Archetype : uint8_t
		{
			ValueMod = 0,
			Script,
			Dispel,
			CureDisease,
			Absorb,
			DualValueMod,
			Calm,
			Demoralize,
			Frenzy,
			Disarm,
			CommandSummoned,
			Invisibility,
			Light,
			Lock = 15,
			Open,
			BoundWeapon,
			SummonCreature,
			DetectLife,
			Telekinesis,
			Paralysis,
			Reanimate,
			SoulTrap,
			TurnUndead,
			Guide,
			WerewolfFeed,
			CureParalysis,
			CureAddiction,
			CurePoison,
			Concussion,
			ValueAndParts,
			AccumulateMagnitude,
			Stagger,
			PeakValueMod,
			Cloak,
			Werewolf,
			SlowTime,
			Rally,
			EnhanceWeapon,
			SpawnHazard,
			Etherealize,
			Banish,
			Disguise = 44,
			GrabActor,
			VampireLord,
		};

		enum class CastingType : uint8_t
		{
			ConstantEffect = 0,
			FireAndForget = 1,
			Concentration = 2,
		};

		enum class Delivery : uint8_t
		{
			Self = 0,
			Contact = 1,
			Aimed = 2,
			TargetActor = 3,
			TargetLocation = 4,
		};

		enum class Volume : uint8_t
		{
			Loud = 0,
			Normal = 1,
			Silent = 2,
			VeryLoud = 3,
		};

		MagicEffect(Archetype archetype, uint32_t existingEffectID, CastingType castingType, Delivery delivery, Volume volume = Volume::Normal);
		MagicEffect(const MagicEffect &r);

		Archetype GetArchetype() const;
		Delivery GetDelivery() const;
		CastingType GetCastingType() const;
		uint32_t GetBase() const;
		Volume GetVolume() const;

		void SetDelivery(Delivery delivery);
		void SetCastingType(CastingType type);
		void SetVolume(Volume volume);

	private:
		void Init(Archetype archetype, uint32_t existingEffectID, CastingType castingType, Delivery delivery, Volume volume);

		~MagicEffect();
		struct Impl;
		Impl *pimpl;

	protected:
		uint32_t GetFormID() const;
	};

	inline MagicEffect *Duplicate(const MagicEffect *src)
	{
		auto result = new MagicEffect(*src);
		return result;
	}
}