#pragma once
#include <memory>
#include <string>

namespace ci
{
	struct AVData
	{
		float base = 100.0f; 
		float modifier = 0.0f;  
		float percentage = 1.0f;

		friend bool operator==(const AVData &lhs, const AVData &rhs) {
			return lhs.base == rhs.base && lhs.modifier == rhs.modifier && lhs.percentage == rhs.percentage;
		};

		friend bool operator!=(const AVData &lhs, const AVData &rhs) {
			return !(lhs == rhs);
		};
	};

	class IActor
	{
	public:
		virtual void SetName(const std::wstring &name) = 0;
		virtual void SetPos(NiPoint3 pos) = 0;
		virtual void SetAngleZ(float angle) = 0;
		virtual void SetCell(uint32_t cellID) = 0;
		virtual void SetWorldSpace(uint32_t worldSpaceID) = 0;
		virtual void ApplyLookData(const LookData &lookData) = 0;
		virtual void ApplyMovementData(const MovementData &movementData) = 0;
		virtual void UseFurniture(const Object *target, bool withAnimation) = 0;
		virtual void AddItem(const ItemType *item, uint32_t count, bool silent) = 0;
		virtual void RemoveItem(const ItemType *item, uint32_t count, bool silent) = 0;
		virtual void EquipItem(const ItemType *item, bool silent, bool preventRemoval, bool leftHand) = 0;
		virtual void UnequipItem(const ItemType *item, bool silent, bool preventEquip, bool leftHand) = 0;
		virtual void AddSpell(const Spell *spell, bool silent) = 0;
		virtual void RemoveSpell(const Spell *spell, bool silent) = 0;
		virtual void EquipSpell(const Spell *spell, bool leftHand) = 0;
		virtual void UnequipSpell(const Spell *spell, bool leftHand) = 0;
		virtual void PlayHitAnimation(uint8_t animID) = 0;
		virtual void UpdateAVData(const std::string &avName, const AVData &data) = 0;

		virtual std::wstring GetName() const = 0;
		virtual NiPoint3 GetPos() const = 0;
		virtual float GetAngleZ() const = 0;
		virtual uint32_t GetCell() const = 0;
		virtual uint32_t GetWorldSpace() const = 0;
		virtual LookData GetLookData() const = 0;
		virtual MovementData GetMovementData() const = 0;
		virtual std::vector<ci::ItemType *> GetEquippedArmor() const = 0;
		virtual ci::ItemType *GetEquippedWeapon(bool isLeftHand = false) const = 0;
		virtual ci::ItemType *GetEquippedAmmo() const = 0;
		virtual const Spell *GetEquippedSpell(bool isLeftHand = false) const = 0;
		virtual const Spell *GetEquippedShout() const = 0;
		virtual AVData GetAVData(const std::string &avName) const = 0;

		virtual ~IActor() {}
	};
}