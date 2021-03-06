#pragma once
#include <memory>

class SkympClientDll;
namespace ci
{
	class Object;

	class LocalPlayer : public IActor
	{
		friend class ::SkympClientDll;
	public:
		static LocalPlayer *GetSingleton();

		void SetName(const std::wstring &name) override;
		void SetPos(NiPoint3 pos) override;
		void SetAngleZ(float angle) override;
		void SetCell(uint32_t cellID) override;
		void SetWorldSpace(uint32_t worldSpaceID) override;
		void ApplyLookData(const LookData &lookData) override;
		void ApplyMovementData(const MovementData &movementData) override;
		void UseFurniture(const Object *target, bool withAnim) override;
		void AddItem(const ItemType *item, uint32_t count, bool silent) override;
		void RemoveItem(const ItemType *item, uint32_t count, bool silent) override;
		void EquipItem(const ItemType *item, bool silent, bool preventRemoval, bool leftHand) override;
		void UnequipItem(const ItemType *item, bool silent, bool preventEquip, bool leftHand) override;
		void AddSpell(const Spell *spell, bool silent) override;
		void RemoveSpell(const Spell *spell, bool silent) override;
		void EquipSpell(const Spell *spell, bool leftHand) override;
		void UnequipSpell(const Spell *spell, bool leftHand) override;
		void PlayAnimation(uint32_t animID) override;
		void UpdateAVData(const std::string &avName, const AVData &data) override;
		void AddActiveEffect(const ci::MagicEffect *effect, float uiDisplayDuration, float uiDisplayMagnitude) override;
		void RemoveActiveEffect(const ci::MagicEffect *effect) override;
		void AddPerk(const ci::Perk *perk) override;
		void RemovePerk(const ci::Perk *perk) override;

		std::wstring GetName() const override;
		NiPoint3 GetPos() const override;
		float GetAngleZ() const override;
		uint32_t GetCell() const override;
		uint32_t GetWorldSpace() const override;
		LookData GetLookData() const override;
		MovementData GetMovementData() const override;
		std::vector<ci::ItemType *> GetEquippedArmor() const override;
		ci::ItemType *GetEquippedWeapon(bool isLeftHand = false) const override;
		ci::ItemType *GetEquippedAmmo() const override;
		const Spell *GetEquippedSpell(bool isLeftHand = false) const override;
		const Spell *GetEquippedShout() const override;
		AVData GetAVData(const std::string &avName) const override;

		std::shared_ptr<uint32_t> GetNextHitAnim();
		uint32_t GetDisplayGold() const;
		void SetDisplayGold(uint32_t count);
		void AddActiveEffect(const ci::Spell *parentSpell, const ci::MagicEffect *effect, const ci::IActor *caster, float magnitude, int64_t endMoment);
		void ClearActiveEffects();
		void Resurrect();
		void EnterHorse(ci::IActor *targetHorse);
		void ExitHorse();
		bool IsOnMount();
		void SetSkillPointsPercent(const std::string &skillName, uint8_t skillPoints);
		void IncrementSkill(const std::string &skillName);
		void RemoveAllPerks();
		void SetSpeedmult(float value);
		uint32_t GetLocation() const;

		using _Power = float;
		ci::Signal<void(_Power)> onPlayerBowShot;

		using _HandID = int32_t;
		ci::Signal<void(_HandID)> onPlayerMagicRelease;

		using _HitData = ci::HitEventData;
		ci::Signal<void(_HitData)> onHit;

		using _IncreasedAV = std::string;
		ci::Signal<void(_IncreasedAV)> onLevelUp;

		using _PerkID = uint32_t;
		ci::Signal<void(_PerkID)> onPerkSelect;

		virtual ~LocalPlayer() {}

	private:
		void DoTeleport_OT(bool ot = true);
		void Update();
		void Update_OT();
		void UpdateActiveEffects();
		void RecoverAVs();
		void RecoverInventory();
		void RecoverSpells();
		void UpdInventoryDupe();
		LocalPlayer();
		uint32_t GetRefID() const override;
	};
}