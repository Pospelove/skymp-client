#include "../stdafx.h"
#include "WorldCleaner.h"
#include "../CoreInterface/CoreInterface.h"
#include  <iomanip>

#define CELL_SEARCH_DEEPTH				40

bool forceUpdate = false;

WorldCleaner *WorldCleaner::GetSingleton()
{
	static WorldCleaner worldCleaner;
	return &worldCleaner;
}

void WorldCleaner::SetFormProtected(RefHandle formID, bool val)
{
	std::lock_guard<dlf_mutex> lock(mutex);

	static std::map<RefHandle, SInt32> sums;
	if (sums.find(formID) == sums.end())
		sums.insert({ formID, 0 });
	if (val)
		++sums[formID];
	else
		--sums[formID];

	if (formID)
	{
		std::lock_guard<dlf_mutex> l(this->mutex);
		if (sums[formID] > 0)
			this->protectedForms.insert(formID);
		else
			this->protectedForms.erase(formID);
	}
	forceUpdate = true;
}

bool WorldCleaner::IsFormProtected(RefHandle formID) const
{
	std::lock_guard<dlf_mutex> lock(mutex);
	if (formID == g_thePlayer->formID)
		return true;
	if (!formID)
		return false;
	std::lock_guard<dlf_mutex> l(this->mutex);
	return this->protectedForms.find(formID) != this->protectedForms.end();
}

void WorldCleaner::SetCallback(FormType t, std::function<void(TESObjectREFR *)> f)
{
	std::lock_guard<dlf_mutex> lock(mutex);
	callbacks[t] = f;
}

void WorldCleaner::DealWithReference(TESObjectREFR *ref)
{
	std::lock_guard<dlf_mutex> lock(mutex);
	if (ref == nullptr)
		return;

	auto refID = ref->formID;
	auto baseForm = ref->baseForm;

	auto baseFormID = baseForm->formID;
	const auto formType = baseForm->formType;

	if (!refID || !baseFormID)
		return;
	if (this->IsFormProtected(refID) || this->IsFormProtected(baseFormID))
		return;

	if (callbacks[formType] != nullptr)
	{
		callbacks[formType](ref);
	}

	switch (formType)
	{
	case FormType::NAVI:
		ErrorHandling::SendError("ERROR:WorldCleaner Wow! NAVI found");
		break;
	case FormType::NAVM:
		ErrorHandling::SendError("ERROR:WorldCleaner Wow! NAVM found");
		break;
	case FormType::NPC:
	case FormType::LeveledCharacter:
		sd::Delete(ref); // was cd::delete
		return;
	case FormType::Projectile:
		sd::SetDestroyed(ref, true);
		sd::BlockActivation(ref, true);
		return;
	case FormType::Ammo:
	case FormType::Ingredient:
	case FormType::ScrollItem:
	case FormType::Armor:
	case FormType::Book:
	case FormType::Misc:
	case FormType::Weapon:
	case FormType::Key:
	case FormType::Potion:
	case FormType::SoulGem:
	case FormType::LeveledItem:
		//case FormType::MovableStatic:
		sd::Delete(ref);
		return;
	case FormType::Tree:
		if (((TESObjectTREE *)baseForm)->produce)
		{
			sd::Delete(ref);
			return;
		}
		break;
	case FormType::Flora:
		if (((TESFlora *)baseForm)->produce)
			sd::Delete(ref);
		return;
	case FormType::Container:
		sd::SetActorOwner(ref, (TESNPC *)g_thePlayer->baseForm);
		sd::RemoveAllItems(ref, nullptr, false, true);
		sd::BlockActivation(ref, true);
		sd::SetDestroyed(ref, true);
		break;
	case FormType::Door:
		sd::Lock(ref, false, false);
		sd::SetOpen(ref, true);
		sd::BlockActivation(ref, true);
		sd::SetDestroyed(ref, true);
		//sd::PrintNote("Deal with door %08x", ref->formID);
		break;
	case FormType::Furniture:
		sd::BlockActivation(ref, true);
		sd::SetDestroyed(ref, true);
		break;
	case FormType::Activator:
		//return;
		sd::BlockActivation(ref, true);
		sd::SetDestroyed(ref, true);
		break;
	default:
		break;
	}

	static const std::set<UInt32> toDel = {
		//891015, 891013, 891011, 891009, 891007, 891003, 891005, 464980, 1050496,	// Алтари
		0xd95c5, 0xd95c7, 0xc2d3f,  0xddf4c, 0x22219, 0x2221e, 0xb97af,  0xa9169,	// Летающие насекомые
		0x10581f, 0x105824,															// Птицы
		0xad0cc, 0xd337f, 0x106d28, 0x106d29, 0x106d2A, 0x106d2B, 0x106d2C,			// Рыбы
		0xe1fb2, 0xb6fb9, 0x89a8a, 0xe2ff9, 0xb6fcb,								// Корни нирна, корзины с яйцами (оригмы?)
		0x10b035, 0x10c3ba															// waterfall salmons
	};
	if (toDel.find(ref->baseForm->formID) != toDel.end())
		sd::Delete(ref);

	if (ref->formID >= 0xFF000000 &&
		(ref->baseForm->formID <= 0xFF000000 || ref->baseForm->formID >= 0xFF00F000)) // To prevent water deletion
	{
		sd::Delete(ref);
	}
}

void WorldCleaner::Update()
{
	std::lock_guard<dlf_mutex> lock(mutex);

	const NiPoint3 pos = cd::GetPosition(g_thePlayer);

	// Far actors
	{
		for (SInt32 i = 0; i != 20; ++i)
		{
			auto ac = sd::FindRandomActor(pos.x, pos.y, pos.z, 100 * 1024 * 1024.0);
			if (!ac || ac == g_thePlayer)
				break;
			this->DealWithReference(ac);
		}
	}

	static void *cellWas = nullptr;
	auto currentCell = sd::GetParentCell(g_thePlayer);
	if (currentCell != cellWas || forceUpdate)
	{
		cellWas = currentCell;
		forceUpdate = false;
		if (currentCell)
		{
			const UInt32 formID = currentCell->formID;
			const auto deepth = CELL_SEARCH_DEEPTH;
			for (UInt32 tmpFormID = (formID > deepth) ? formID - deepth : 0; tmpFormID != formID + deepth; ++tmpFormID)
			{
				TESObjectCELL *tmpCell = (TESObjectCELL *)LookupFormByID(tmpFormID);
				if (tmpCell && tmpCell->formType == FormType::Cell)
				{
					//std::lock_guard<BSSpinLock> l(tmpCell->cellRefLock);
					std::for_each(tmpCell->objectList.begin(), tmpCell->objectList.end(), [](TESObjectREFR *refr) {
						if (refr)
							WorldCleaner::GetSingleton()->DealWithReference(refr);
					});
				}
			}
		}
	}
}