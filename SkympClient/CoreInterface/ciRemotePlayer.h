#pragma once

class SkympClientDll;
class TESNPC;

namespace ci
{
	class RemotePlayer : public IActor
	{
		friend class ::SkympClientDll;
	public:

		using OnHit = std::function<void(const HitEventData &)>;

		RemotePlayer(
			const std::wstring &name,
			const LookData &lookData,
			NiPoint3 spawnPoint,
			uint32_t cellID,
			uint32_t worldSpaceID,
			OnHit onHit = nullptr);

		RemotePlayer(const IActor &);

		RemotePlayer(const RemotePlayer &src) : RemotePlayer((IActor &)src) {}

		virtual ~RemotePlayer() override;

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
		void PlayHitAnimation(uint8_t animID) override;
		void UpdateAVData(const std::string &avName, const AVData &data) override;
		std::wstring GetName() const override;
		NiPoint3 GetPos() const  override;
		float GetAngleZ() const override;
		uint32_t GetCell() const override;
		uint32_t GetWorldSpace() const override;
		LookData GetLookData() const override;
		MovementData GetMovementData() const override;
		std::vector<ci::ItemType *> GetEquippedArmor() const override;
		ci::ItemType *GetEquippedWeapon(bool isLeftHand = false) const override;
		ci::ItemType *GetEquippedAmmo() const override;
		AVData GetAVData(const std::string &avName) const override;

		void SetInAFK(bool val);
		void Fire(float power);

	private:
		static void UpdateAll();
		static void UpdateAll_OT();
		void Update();
		void Update_OT();
		void ForceSpawn(uint32_t npcID);
		void ForceDespawn(const wchar_t *reason = L"");

	protected:

		virtual TESNPC *AllocateNPC() const;
		virtual void ApplyMovementDataImpl();

		bool IsDerived() const {
			return this->GetName() == L"Ghost Axe" || this->GetName() == L"Invisible Fox";
		}

		struct Impl;
		std::unique_ptr<Impl> pimpl;
	};
}