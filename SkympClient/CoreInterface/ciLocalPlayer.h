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
		void PlayHitAnimation(uint8_t animID) override;
		void UpdateAVData(const std::string &avName, const AVData &data) override;

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

		std::shared_ptr<uint8_t> GetNextHitAnim();
		uint32_t GetDisplayGold() const;
		void SetDisplayGold(uint32_t count);

		using _Power = float;
		ci::Signal<void(_Power)> onPlayerBowShot;


		virtual ~LocalPlayer() {}

	private:
		void DoTeleport_OT(bool ot = true);
		void Update();
		void Update_OT();
		LocalPlayer();
	};
}