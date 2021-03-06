#pragma once

class SkympClientDll;
class ActivateEventSink;
class PlayerBowShotEventSink;

namespace ci
{
	class RemotePlayer;
	class LocalPlayer;
	class ItemType;
	class Object;
	class Spell;
	class IActor;

	struct HitEventData
	{
		bool powerAttack = false;
		const ci::ItemType *weapon = nullptr;
		const ci::Spell *spell = nullptr;
		std::string hitSrcMark = "";
	};

	class Object
	{
		friend class ::SkympClientDll;
		friend class ::ActivateEventSink;
		friend class ::PlayerBowShotEventSink;

		friend class RemotePlayer;
		friend class LocalPlayer;
		friend class ObjectAccess;

	public:

		using _IsOpening = bool;
		using _Source = ci::IActor *;
		using OnActivate = std::function<void(_IsOpening, _Source)>;

		using OnContainerChanged = std::function<void(const ci::ItemType *, uint32_t, bool)>;
		using OnMove = std::function<void()>;
		using OnHit = std::function<void(const HitEventData &)>;

		Object(uint32_t existingReferenceID, 
			uint32_t baseID, 
			NiPoint3 pos, 
			NiPoint3 rot, 
			OnActivate onActivate = nullptr,
			OnContainerChanged onContainerChanged = nullptr,
			OnHit onHit = nullptr
		);
		~Object();
		void SetDestroyed(bool destroyed);
		void SetOpen(bool open);
		void BlockActivation(bool block);
		void SetPosition(NiPoint3 pos);
		void SetAngle(NiPoint3 rot);
		void TranslateTo(NiPoint3 pos, NiPoint3 rot, float speed, float rotSpeed);
		void SetName(const std::wstring &name);
		void Lock(bool locked);
		void SetMotionType(int32_t type);
		void SetDisabled(bool disabled);
		void SetBase(const ci::ItemType *itemType);
		void SetCount(uint32_t count);
		void AddItem(const ItemType *item, uint32_t count);
		void RemoveItem(const ItemType *item, uint32_t count);
		void Respawn() {
			this->ForceDespawn("Script");
		}
		void SetLockLevel(uint8_t lockLevel);
		void Activate();

		void AddEventHandler(OnActivate onActivate);

		NiPoint3 GetPos() const;
		NiPoint3 GetRot() const;
		bool IsGrabbed() const;
		bool IsCrosshairRef() const;
		float GetSpeed() const;
		uint32_t GetBase() const;
		int32_t GetMotionType() const;
		bool IsActivationBlocked() const;
		uint8_t GetLockLevel() const;

		enum {
			Motion_Dynamic = 1,
			Motion_SphereInertia,
			Motion_BoxInertia,
			Motion_Keyframed,
			Motion_Fixed,
			Motion_ThinBoxInertia,
			Motion_Character,
		};

		uint32_t GetRefID() const;

	private:
		static uint32_t GetFarObject();
		static void UpdateAll();
		static void UpdateAll_OT() {};
		void Update();
		void ForceSpawn();
		void ForceDespawn(const std::string &reason);
		void UpdateContainer();

		struct Impl;
		Impl *pimpl;
	};
}