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
		friend class GhostAxeManager;
		friend class GnomeHost;
		friend class WorldSpell;
	public:

		using OnHit = std::function<void(const HitEventData &)>;
		using OnActivate = std::function<void()>;

		RemotePlayer(
			const std::wstring &name,
			const LookData &lookData,
			NiPoint3 spawnPoint,
			uint32_t cellID,
			uint32_t worldSpaceID,
			OnHit onHit,
			const std::string &engineName,
			OnActivate onActivate);

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
		void AddPerk(const ci::Perk *perk) override;
		void RemovePerk(const ci::Perk *perk) override;

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

		// Implemented in ciRPCommon.cpp
		void SetEngine(const std::string &engineName);
		std::string GetEngine() const;
		void PathTo(const NiPoint3 &pos, bool walk);
		void StopPathing();
		void SetBaseNPC(uint32_t npcID);
		void SetMark(const std::string &mark);
		std::string GetMark() const;
		std::shared_ptr<uint32_t> GetNextHitAnim();

		// Implemented in ciRPEngineIO.cpp
		void SetCombatTarget(const IActor *target);

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
		void SetAttachedHorse(ci::IActor *horse);
		bool HasAttachedHorse() const;
		void Respawn();

		uint32_t GetFormID() const;
		static RemotePlayer *LookupByFormID(uint32_t id);
		static void SetTracing(bool trace);

		struct Impl;

	private:
		static void UpdateAll();
		static void UpdateAll_OT();
		static void UpdateWorldSpell();

		void CreateGnomes();
		void DestroyGnomes();

		void UpdateNonSpawned();

		void SpawnIfNeed();
		void UndoSpawn();
		void DetectSpawningTimeout();

		void UpdateSpawning();

		void UpdateGnomes(Actor *actor);
		void UpdateNickname(Actor *actor);
		void UpdatePerks(Actor *actor);
		void DeleteProjectile(Actor *actor);
		void UpdateMovement(Actor *actor);
		void ApplyHitAnimations(Actor *actor);
		void ApplyActorValues(Actor *actor);
		void UpdateDispenser();
		void UpdateInvisibleFox();
		void ManageMagicEquipment(Actor *actor);
		void ApplyEquipmentHands(Actor *actor);
		void ApplyEquipmentOther(Actor *actor);
		void DespawnIfNeed();
		void FixGreyFaceBug(Actor *actor);
		void FixGreyFaceBugUnsafe(Actor *actor);

		void UpdateSpawned();

		void Update();
		void Update_OT() {};

		void ForceSpawn(TESNPC *npc);
		void ForceDespawn(const wchar_t *reason = L"");

		int32_t GetSyncMode() const;
		uint32_t GetRefID() const override;
		bool IsBowEquipped() const;
		bool IsSpellEquipped() const; 
		bool NeedsGnome(int32_t handID) const;
		uint32_t GetLocationID() const;

		static size_t GetNumInstances();

		static void FinishSpawning();

	protected:
		virtual TESNPC *AllocateNPC() const;
		virtual void ApplyMovementDataImpl();

		bool IsDerived() const {
			return this->GetName() == L"Ghost Axe" || this->GetName() == L"Invisible Fox";
		}
		std::unique_ptr<Impl> pimpl;
	};
}