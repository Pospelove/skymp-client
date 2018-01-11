#pragma once

class SkympClientDll;
class TESNPC;
class Actor;

namespace ci
{
	class RemotePlayer : public IActor
	{
		friend class ::SkympClientDll;
		friend class IRemotePlayerEngine;
		friend class RPEngineInput;
		friend class RPEngineIO;
	public:

		using OnHit = std::function<void(const HitEventData &)>;

		RemotePlayer(
			const std::wstring &name,
			const LookData &lookData,
			NiPoint3 spawnPoint,
			uint32_t cellID,
			uint32_t worldSpaceID,
			OnHit onHit = nullptr,
			const std::string &engineName = "RPEngineInput");

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
		void AddSpell(const Spell *spell, bool silent) override;
		void RemoveSpell(const Spell *spell, bool silent) override;
		void EquipSpell(const Spell *spell, bool leftHand) override;
		void UnequipSpell(const Spell *spell, bool leftHand) override;
		void PlayAnimation(uint32_t animID) override;
		void UpdateAVData(const std::string &avName, const AVData &data) override;
		void AddActiveEffect(const ci::MagicEffect *effect, float uiDisplayDuration, float uiDisplayMagnitude) override;
		void RemoveActiveEffect(const ci::MagicEffect *effect) override;

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
		const Spell *GetEquippedSpell(bool isLeftHand = false) const override;
		const Spell *GetEquippedShout() const override;
		AVData GetAVData(const std::string &avName) const override;

		void SetEngine(const std::string &engineName);
		std::string GetEngine() const;
		void PathTo(const NiPoint3 &pos, bool walk);
		void StopPathing();
		void SetBaseNPC(uint32_t npcID);
		void SetMark(const std::string &mark);
		std::string GetMark() const;
		void SetCombatTarget(const IActor *target);
		std::shared_ptr<uint32_t> GetNextHitAnim();

		void SetInAFK(bool val);
		void SetNicknameVisible(bool visible);
		void Fire(float power);
		void FireMagic(const ci::Spell *spell, int32_t handID);
		void SetHeight(float height);
		void MagicAttackBegin(int32_t handID);
		void MagicAttackEnd(int32_t handID);
		void SetVisualMagicEffect(const ci::Spell *source);
		bool IsBroken() const;
		void TestMakeBroken();
		NiPoint3 GetNicknamePos() const;
		float GetNicknameDrawDistance() const;
		bool IsSpawned() const;

	private:

		RemotePlayer(const IActor &); // Deprecated

		RemotePlayer(const RemotePlayer &src) : RemotePlayer((IActor &)src) {} // Deprecated

		static void UpdateAll();
		static void UpdateAll_OT();
		static void UpdateWorldSpell();
		static void ApplyWorldSpell();

		void AsyncFoxDestroy();
		void AsyncFoxCreate();
		void CreateGnomes();
		void DestroyGnomes();

		void UpdateNonSpawned();

		void UpdateSpawning();

		void UpdateGnomes(Actor *actor);
		void UpdateNickname(Actor *actor);
		void DeleteProjectile(Actor *actor);
		void UpdateMovement(Actor *actor);
		void ApplyHitAnimations(Actor *actor);
		void ApplyActorValues(Actor *actor);
		void ManageMyFox(Actor *actor);
		void UpdateDispenser(Actor *actor);
		void CreateDestroyMyFox(Actor *actor);
		void ManageMagicEquipment(Actor *actor);
		void ApplyEquipmentHands(Actor *actor);
		void ApplyEquipmentOther(Actor *actor);
		void DespawnIfNeed(Actor *actor);
		void FixGreyFaceBug(Actor *actor);

		void UpdateSpawned();

		void Update();
		void Update_OT();

		void ForceSpawn(TESNPC *npc);
		void ForceDespawn(const wchar_t *reason = L"");

		int32_t GetSyncMode() const;
		int32_t GetSpawnStage() const;
		uint32_t GetRefID() const override;
		bool IsBowEquipped() const;
		bool IsSpellEquipped() const; 
		bool NeedsGnome(int32_t handID) const;
		uint32_t GetLocationID() const;

		static size_t GetNumInstances();
		static uint32_t GetGhostAxeFormID();

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