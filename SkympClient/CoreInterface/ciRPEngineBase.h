// Requires include CoreInterface.h first
#pragma once

// It's OK to include files from Sync directory because ciRPEngineBase.h is private header
#include "../Sync/SyncEssential.h"

#include "ciRPInvisibleFox.h"
#include "ciRPDispenser.h"
#include "ciRPGnomeHost.h"

namespace ci
{
	class InvisibleFox;

	enum class SpawnStage
	{
		NonSpawned,
		Spawning,
		Spawned,
	};

	using RPGMUTEX = dlf_mutex;

	struct RemotePlayer::Impl
	{
		Impl() {
			isMagicAttackStarted[0] = isMagicAttackStarted[1] = false;
		}

		bool usl1, usl2, usl3, usl4, usl5;

		ci::InvisibleFox invisibleFox;
		ci::Dispenser dispenser;
		ci::GnomeHost gnomeHost;

		std::vector<std::function<void()>> tasksSpawned;

		dlf_mutex mutex;
		FormID formID = 0;
		std::wstring name;
		std::unique_ptr<ci::Text3D> nicknameLabel;
		std::unique_ptr<ci::IRemotePlayerEngine> engine;
		bool nicknameEnabled = true;
		bool hasLOS = true;
		clock_t lastLosCheck = 0;
		LookData lookData;
		TESObjectCELL *currentNonExteriorCell = nullptr;
		FormID worldSpaceID = 0;
		SpawnStage spawnStage = SpawnStage::NonSpawned;
		clock_t spawnMoment = 0;
		clock_t timer250ms = 0;
		clock_t unsafeSyncTimer = 0;
		clock_t lastOutOfPos = 0;
		bool greyFaceFixed = false;
		bool greyFaceFixedUnsafe = false;
		MovementData movementData, movementDataOut;
		MovementData_::SyncState syncState;
		clock_t lastDespawn = 0;
		UInt32 rating = 0;
		bool afk = false;
		bool stopProcessing = false;
		std::map<const ci::ItemType *, uint32_t> inventory;
		std::queue<uint32_t> hitAnimsToApply, hitAnimsOut;
		OnHit onHit = nullptr;
		OnActivate onActivate = nullptr;
		std::map<std::string, ci::AVData> avData;
		float height = 1;
		bool isMagicAttackStarted[2];
		const void *visualMagicEffect = nullptr;
		bool broken = false;
		std::function<void(Actor *)> posTask, angleTask;
		std::unique_ptr<ci::Object> pathToTarget;
		float pathingSpeedmult = 100;
		TESNPC *baseNpc = nullptr;
		std::string mark;
		uint32_t combatTarget = 0;
		std::function<void()> engineTask = nullptr;
		bool fullySpawned = false;
		std::set<const ci::Perk *> perks;

		struct Equipment
		{
			Equipment() {
				hands[0] = hands[1] = nullptr;
				handsMagic[0] = handsMagic[1] = nullptr;
			}

			std::array<const ci::ItemType *, 2> hands;
			std::array<const ci::Spell *, 2> handsMagic;
			std::set<const ci::ItemType *> armor;
			const ci::ItemType *ammo = nullptr;

			friend bool operator==(const Equipment &l, const Equipment &r) {
				return l.hands[0] == r.hands[0] && l.hands[1] == r.hands[1] && l.armor == r.armor && l.ammo == r.ammo;
			}
			friend bool operator!=(const Equipment &l, const Equipment &r) {
				return !(l == r);
			}
		} eq, eqLast;

		std::map<int32_t, const ci::Spell *> handsMagicProxy;
		clock_t lastMagicEquipmentChange = 0;

		clock_t lastWeaponsUpdate = 0;

		static std::map<TESForm *, const ci::ItemType *> *RemotePlayerKnownItems() { // lock gMutex to use this
			static std::map<TESForm *, const ci::ItemType *> remotePlayerKnownItems;
			return &remotePlayerKnownItems;
		}

		static std::map<TESForm *, const ci::Spell *> *RemotePlayerKnownSpells() { // lock gMutex to use this
			static std::map<TESForm *, const ci::Spell *> remotePlayerKnownSpells;
			return &remotePlayerKnownSpells;
		}

		class HitEventSinkGlobal;
		class ActivateEventSinkGlobal;
	};

	class IRemotePlayerEngine : public IActor
	{
	public:

		IRemotePlayerEngine(RemotePlayer *argParent);
		virtual ~IRemotePlayerEngine();

		void SetName(const std::wstring &name) override;
		void SetCell(uint32_t cellID) override;
		void SetWorldSpace(uint32_t worldSpaceID) override;
		void ApplyLookData(const LookData &lookData) override;
		void UseFurniture(const Object *target, bool withAnim) override;
		void AddItem(const ItemType *item, uint32_t count, bool silent) override;
		void RemoveItem(const ItemType *item, uint32_t count, bool silent) override;
		void EquipItem(const ItemType *item, bool silent, bool preventRemoval, bool leftHand) override;
		void UnequipItem(const ItemType *item, bool silent, bool preventEquip, bool isLeftHand) override;
		void AddSpell(const Spell *spell, bool silent) override;
		void RemoveSpell(const Spell *spell, bool silent) override;
		void PlayAnimation(uint32_t animID) override;
		void UpdateAVData(const std::string &avName_, const AVData &data) override;
		void AddActiveEffect(const ci::MagicEffect *effect, float uiDisplayDuration, float uiDisplayMagnitude) override;
		void RemoveActiveEffect(const ci::MagicEffect *effect) override;
		void AddPerk(const ci::Perk *perk) override;
		void RemovePerk(const ci::Perk *perk) override;

		std::wstring GetName() const override;
		uint32_t GetCell() const override;
		uint32_t GetWorldSpace() const override;
		LookData GetLookData() const override;
		std::vector<ci::ItemType *> GetEquippedArmor() const override;
		ci::ItemType *GetEquippedWeapon(bool isLeftHand) const;
		ci::ItemType *GetEquippedAmmo() const;
		const Spell *GetEquippedShout() const override;
		AVData GetAVData(const std::string &avName_) const override;
		uint32_t GetRefID() const override;
		TESNPC *AllocateNPC();

		virtual void ApplyMovementDataImpl() = 0;
		virtual const char *GetEngineName() const = 0;
		virtual void EngineUpdateSpawned(Actor *actor) = 0;
		virtual void EngineApplyFactions(Actor *actor) = 0;

		enum class FactionsState
		{
			Unknown,
			NoFactions,
			PlayerAllyFaction,
		};

		FactionsState factionsState = FactionsState::Unknown;

	protected:
		RemotePlayer *GetParent() const {
			return this->parent;
		}


		virtual std::unique_ptr<RemotePlayer::Impl> &GetImpl() const {
			return this->GetParent()->pimpl;
		}

	private:
		RemotePlayer *const parent;
	};
}

class CIAccess
{
public:
	static void OnPoisonAttack()
	{
		std::thread([=] {
			auto logic = ci::IClientLogic::clientLogic;
			if (logic != nullptr)
			{
				std::lock_guard<ci::Mutex> l(logic->callbacksMutex);
				logic->OnPoisonAttack();
			}
		}).detach();
	}

	static ci::Mutex &GetMutex() {
		return ci::IClientLogic::callbacksMutex;
	}
};