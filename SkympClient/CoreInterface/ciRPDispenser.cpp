#include "../stdafx.h"
#include "CoreInterface.h"
#include "ciRPDispenser.h"

#include "../Sync/SyncOptions.h"

namespace ci
{
	class ObjectAccess
	{
	public:
		static uint32_t GetRefID(const Object &obj) {
			return obj.GetRefID();
		}
	};

	class ItemTypeAccess
	{
	public:
		static uint32_t GetFormID(const ci::ItemType &itemType) {
			return itemType.GetFormID();
		}
	};
}

struct ci::Dispenser::Impl
{
	std::unique_ptr<ci::Object> dispenser = nullptr;
};

ci::Dispenser::Dispenser()
{
	pimpl.reset(new Impl);
}

void ci::Dispenser::SetActive(bool active)
{
	active ? this->Create() : this->Destroy();
}

void ci::Dispenser::UpdateAndMoveTo(const MovementData &movement, bool isSpellMode)
{
	auto f = [&] {
		if (pimpl->dispenser != nullptr)
		{
			enum Var {
				OffsetZSneaking,
				OffsetZ,
				OffsetDistance,
				NumVars,
			};

			enum DispenserMode {
				DispenserModeBow,
				DispenserModeMagic,
				NumModes,
			};

			static const char *vars[NumVars][NumModes] = {
				{ "DISPENSER_OFFSET_Z_SNEAKING",	"MAGIC_DISPENSER_OFFSET_Z_SNEAKING" },
				{ "DISPENSER_OFFSET_Z",				"MAGIC_DISPENSER_OFFSET_Z" },
				{ "DISPENSER_OFFSET_DISTANCE",		"MAGIC_DISPENSER_OFFSET_DISTANCE" },
			};

			const DispenserMode mode = isSpellMode ? DispenserModeMagic : DispenserModeBow;

			auto pos = movement.pos;
			pos += {0, 0, movement.isSneaking ?
				SyncOptions::GetSingleton()->GetFloat(vars[OffsetZSneaking][mode]) :
				SyncOptions::GetSingleton()->GetFloat(vars[OffsetZ][mode])
			};
			const float distance = SyncOptions::GetSingleton()->GetFloat(vars[OffsetDistance][mode]);
			const float angleRad = (float)movement.angleZ / 180 * acos(-1);
			pos += {distance * sin(angleRad), distance * cos(angleRad), 0};

			const auto rot = NiPoint3{
				(float)movement.aimingAngle,
				0.0f,
				(float)movement.angleZ
			};

			pimpl->dispenser->TranslateTo(pos, rot, 10000.f, 10000.f);
		}
	};
	SAFE_CALL("RemotePlayer", f);
}

void ci::Dispenser::Fire(const ItemType *bow, const ItemType *ammo, float power)
{
	if (ammo == nullptr
		|| bow == nullptr || bow->GetSubclass() != ci::ItemType::Subclass::WEAP_Bow)
		return;
	if (pimpl->dispenser == nullptr)
		return;
	const auto dispenserID = ObjectAccess::GetRefID(*pimpl->dispenser.get());
	auto f = [=] {
		auto dispenserRef = (TESObjectREFR *)LookupFormByID(dispenserID);
		if (dispenserRef != nullptr)
		{
			auto ammoSrc = (TESAmmo *)LookupFormByID(ItemTypeAccess::GetFormID(*ammo));

			auto ammoCopy = FormHeap_Allocate<TESAmmo>();
			memcpy(ammoCopy, ammoSrc, sizeof TESAmmo);
			ammoCopy->formID = 0;
			ammoCopy->SetFormID(Utility::NewFormID(), 1);

			auto projSrc = ammoSrc->settings.projectile;
			auto projCopy = FormHeap_Allocate<BGSProjectile>();
			memcpy(projCopy, projSrc, sizeof BGSProjectile);
			projCopy->formID = 0;
			projCopy->SetFormID(Utility::NewFormID(), 1);

			ammoCopy->settings.projectile = projCopy;
			ammoCopy->settings.damage = 0;

			auto &speed = projCopy->data.unk08;
			auto &range = projCopy->data.unk0C;
			auto &fadeDuration = projCopy->data.unk30;
			auto &collisionRadius = projCopy->data.unk48;

			const auto lastSpeed = speed;
			speed = pow(power, 2) * speed;

			auto bowSrc = (TESObjectWEAP *)LookupFormByID(ItemTypeAccess::GetFormID(*bow));
			auto bowCopy = FormHeap_Allocate<TESObjectWEAP>();
			memcpy(bowCopy, bowSrc, sizeof TESObjectWEAP);
			bowCopy->formID = 0;
			bowCopy->SetFormID(Utility::NewFormID(), 1);
			bowCopy->attackDamage = 0;

			sd::Weapon::Fire(bowCopy, dispenserRef, ammoCopy);

			SET_TIMER(3000, [=] {
				sd::RemoveItem(g_thePlayer, ammoCopy, -1, true, nullptr);
			});
		}
	};
	SET_TIMER_LIGHT(1, f);
}

void ci::Dispenser::Create()
{
	SAFE_CALL("Dispenser", [&] {
		const auto location = ci::LocalPlayer::GetSingleton()->GetLocation();
		if (pimpl->dispenser == nullptr)
		{
			pimpl->dispenser.reset(
				new ci::Object(0, ID_TESObjectSTAT::XMarkerHeading, location, { 0,0,0 }, { 0,0,0 })
			);
			pimpl->dispenser->SetMotionType(Object::Motion_Keyframed);
		}
		pimpl->dispenser->SetLocation(location);
	});
}

void ci::Dispenser::Destroy()
{
	SAFE_CALL("Dispenser", [&] {
		if (pimpl->dispenser != nullptr)
			pimpl->dispenser->SetLocation(-1);
	});
}