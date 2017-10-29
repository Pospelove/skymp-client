#pragma once

class SkympClientDll;
class ActivateEventSink;
class PlayerBowShotEventSink;

namespace ci
{
	class RemotePlayer;
	class LocalPlayer;
	class ItemType;

	struct HitEventData
	{
		bool powerAttack;
		const ci::ItemType *weapon;
	};

	class Object
	{
		friend class ::SkympClientDll;
		friend class ::ActivateEventSink;
		friend class ::PlayerBowShotEventSink;

		friend class RemotePlayer;
		friend class LocalPlayer;

	public:

		using OnActivate = std::function<void(bool)>;
		using OnContainerChanged = std::function<void(const ci::ItemType *, uint32_t, bool)>;
		using OnMove = std::function<void()>;
		using OnHit = std::function<void(const HitEventData &)>;

		Object(uint32_t existingReferenceID, 
			uint32_t baseID, 
			uint32_t locationID, 
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
		bool SetLocation(uint32_t locationID);
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

		void AddEventHandler(OnActivate onActivate);

		NiPoint3 GetPos() const;
		NiPoint3 GetRot() const;
		bool IsGrabbed() const;
		bool IsCrosshairRef() const;
		float GetSpeed() const;
		uint32_t GetBase() const;
		int32_t GetMotionType() const;

		enum {
			Motion_Dynamic = 1,
			Motion_SphereInertia,
			Motion_BoxInertia,
			Motion_Keyframed,
			Motion_Fixed,
			Motion_ThinBoxInertia,
			Motion_Character,
		};

	private:
		static void UpdateAll();
		static void UpdateAll_OT();
		void Update();
		void Update_OT();
		void ForceSpawn();
		void ForceDespawn(const std::string &reason);
		void UpdateContainer();
		uint32_t GetRefID() const;

		struct Impl;
		Impl *pimpl;
	};
}