#include "../stdafx.h"
#include "WorldCleaner.h"
#include "../CoreInterface/CoreInterface.h"
#include  <iomanip>

bool forceUpdate = false;

WorldCleaner *WorldCleaner::GetSingleton()
{
	static WorldCleaner worldCleaner;
	return &worldCleaner;
}


void WorldCleaner::SetFormProtected(RefHandle formID, bool val)
{
	static std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);

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
	if (formID == g_thePlayer->formID)
		return true;
	if (!formID)
		return false;
	std::lock_guard<dlf_mutex> l(this->mutex);
	return this->protectedForms.find(formID) != this->protectedForms.end();
}

void WorldCleaner::DealWithReference(TESObjectREFR *ref)
{
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

	if (IsSkympDebug())
	{
		switch (formType)
		{
		case FormType::NPC:
		case FormType::LeveledCharacter:
			sd::Delete(ref);
			break;
		case FormType::Door:
		case FormType::Container:
		{
			static std::set<uint32_t> saved;
			if (saved.insert(ref->formID).second)
			{
				uint32_t locationID = 0;
				const char *locationName = "";
				auto cell = sd::GetParentCell(ref);
				if (!cell)
				{
					saved.erase(ref->formID);
					return;
				}
				if (cell->IsInterior())
				{
					locationID = cell->formID;
					locationName = cell->GetName();
				}
				else
				{
					locationID = ref->GetWorldSpace()->formID;
					locationName = ref->GetWorldSpace()->GetName();
				}

				std::stringstream ss;
				//ss << "Debug\\Auto\\" << std::hex << std::setfill('0') << std::setw(8) << locationID << "_" << locationName << ".txt";

				ss << "Debug\\Auto\\" << std::hex << std::setfill('0') << std::setw(8) << 0 << "_" << "Any" << ".txt";

				std::ofstream of(ss.str().data(), std::ios::app);
				of << "obj = Object.Create(";
				of << "0x" << std::hex << std::setfill('0') << std::setw(8) << ref->formID << ", ";
				of << "0x" << std::hex << std::setfill('0') << std::setw(8) << ref->baseForm->formID << ", ";
				of << "Location(" << "0x" << std::hex << std::setfill('0') << std::setw(8) << locationID << "), ";
				of << ref->GetPositionX() << ", " << ref->GetPositionY() << ", " << ref->GetPositionZ() << ")" << std::endl;

				of << "obj:SetAngle(" << sd::GetAngleX(ref) << ", " << sd::GetAngleY(ref) << ", " << sd::GetAngleZ(ref) << ")" << std::endl;
				if (ref->baseForm->formType == FormType::Door)
					of << "obj:RegisterAsDoor()" << std::endl;
				if (ref->baseForm->formType == FormType::Container)
					of << "obj:RegisterAsContainer()" << std::endl;
			}

		}
		default:
			sd::Lock(ref, false, false);
			break;
		}
		return;
	}
	switch (formType)
	{
	case FormType::NPC:
	case FormType::LeveledCharacter:
		sd::Delete(ref); // was cd::delete
		break;
	case FormType::Ingredient:
	case FormType::ScrollItem:
	case FormType::Armor:
	case FormType::Book:
	case FormType::Misc:
	case FormType::Weapon:
	case FormType::Ammo:
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
		break;
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
	if (ref->formID >= 0xFF000000 && (ref->baseForm->formID <= 0xFF000000 || ref->baseForm->formID >= 0xFF00F000))
	{
		sd::Delete(ref);
	}
}

void WorldCleaner::Update()
{
	const NiPoint3 pos = { sd::GetPositionX(g_thePlayer), sd::GetPositionY(g_thePlayer), sd::GetPositionZ(g_thePlayer) };

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

	static const std::set<UInt32> baseIDsToDestroy = {
		//891015, 891013, 891011, 891009, 891007, 891003, 891005, 464980, 1050496,	// Алтари
		/*0xd95c5, 0xd95c7, 0xc2d3f,  0xddf4c, 0x22219, 0x2221e, 0xb97af,  0xa9169,	// Летающие насекомые
		0x10581f, 0x105824,															// Птицы
		0xad0cc, 0xd337f, 0x106d28, 0x106d29, 0x106d2A, 0x106d2B, 0x106d2C,			// Рыбы
		0xe1fb2, 0xb6fb9, 0x89a8a, 0xe2ff9, 0xb6fcb,								// Корни нирна, корзины с яйцами (оригмы?)
		0x10b035, 0x10c3ba															// waterfall salmons
	*/};
	std::for_each(baseIDsToDestroy.begin(), baseIDsToDestroy.end(), [=] (UInt32 formID) {
		TESForm *form = LookupFormByID(formID);
		for (SInt32 i = 0; i != 10; ++i)
		{
			auto refr = sd::FindClosestReferenceOfType(form, pos.x, pos.y, pos.z, 100 * 1024);
			if (!refr)
				break;
			else
				sd::Delete(refr);
		}
	});

	static void *cellWas = nullptr;
	auto currentCell = sd::GetParentCell(g_thePlayer);
	if (currentCell != cellWas || forceUpdate)
	{
		cellWas = currentCell;
		forceUpdate = false;
		if (currentCell)
		{
			const UInt32 formID = currentCell->formID;
			enum { deepth = 40 };
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