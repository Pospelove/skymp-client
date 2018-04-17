#include "../stdafx.h"
#include "CoreInterface.h"
#include "ciRPEngineBase.h"

#include "../Sync/SyncEssential.h"
#include "Skyrim\Camera\PlayerCamera.h"

#include "ciRPPackageApplier.h"

namespace ci
{
	extern ILookSynchronizer *lookSync;
}

namespace ci
{
	IRemotePlayerEngine::IRemotePlayerEngine(RemotePlayer *argParent) : parent(argParent)
	{
	}

	IRemotePlayerEngine::~IRemotePlayerEngine()
	{
	}

	void IRemotePlayerEngine::SetName(const std::wstring &name)
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		if (pimpl->name != name)
		{
			pimpl->name = name;
			if(this->GetParent()->IsSpawned())
			{
				const auto formID = pimpl->formID;
				SET_TIMER_LIGHT(0, [=] {
					auto str = WstringToString(name);
					cd::SetDisplayName(cd::Value<TESObjectREFR>(formID), str, true);
				});
			}
		}
	}

	void IRemotePlayerEngine::SetCell(uint32_t cellID)
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		auto cell = CellUtil::LookupNonExteriorCellByID(cellID);
		pimpl->currentNonExteriorCell = cell;
	}

	void IRemotePlayerEngine::SetWorldSpace(uint32_t worldSpaceID)
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->worldSpaceID = worldSpaceID;
	}

	void IRemotePlayerEngine::ApplyLookData(const LookData &lookData)
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		pimpl->lookData = lookData;
		if (this->GetParent()->IsSpawned() && this->GetParent()->IsDerived() == false)
			pimpl->model.Despawn(L"Despawned: LookData updated");
	}

	void IRemotePlayerEngine::UseFurniture(const Object *target, bool withAnim)
	{
		// ...
	}

	void IRemotePlayerEngine::AddItem(const ItemType *item, uint32_t count, bool silent)
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (item && count)
			pimpl->inventory[item] += count;
	}

	void IRemotePlayerEngine::RemoveItem(const ItemType *item, uint32_t count, bool silent)
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (item && count)
		{
			if (pimpl->inventory[item] > count)
				pimpl->inventory[item] -= count;
			else
			{
				pimpl->inventory.erase(item);
				this->GetParent()->UnequipItem(item, silent, false, false);
				this->GetParent()->UnequipItem(item, silent, false, true);
			}
		}
	}

	void IRemotePlayerEngine::EquipItem(const ItemType *item, bool silent, bool preventRemoval, bool leftHand)
	{
		if (item != nullptr)
		{
			auto &pimpl = this->GetImpl();
			std::lock_guard<dlf_mutex> l(pimpl->mutex);

			if (item->GetClass() == ci::ItemType::Class::Armor)
			{
				pimpl->eq.armor.insert(item);
				return;
			}
			if (item->GetClass() == ci::ItemType::Class::Weapon)
			{
				pimpl->eq.hands[leftHand] = item;
				pimpl->handsMagicProxy[leftHand] = nullptr;
				return;
			}
			if (item->GetClass() == ci::ItemType::Class::Ammo)
			{
				pimpl->eq.ammo = item;
				return;
			}
		}
	}

	void IRemotePlayerEngine::UnequipItem(const ItemType *item, bool silent, bool preventEquip, bool isLeftHand)
	{
		if (item == nullptr)
			return;

		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (item->GetClass() == ci::ItemType::Class::Armor)
		{
			pimpl->eq.armor.erase(item);
			return;
		}
		if (item->GetClass() == ci::ItemType::Class::Weapon)
		{
			if (pimpl->eq.hands[isLeftHand] == item)
				pimpl->eq.hands[isLeftHand] = nullptr;
			return;
		}
		if (item->GetClass() == ci::ItemType::Class::Ammo)
		{
			if (pimpl->eq.ammo == item)
				pimpl->eq.ammo = nullptr;
			return;
		}
	}

	void IRemotePlayerEngine::AddSpell(const Spell *spell, bool silent)
	{
	}

	void IRemotePlayerEngine::RemoveSpell(const Spell *spell, bool silent)
	{
		for (int32_t i = 0; i != 2; ++i)
			this->GetParent()->UnequipSpell(spell, i != 0);
	}

	void IRemotePlayerEngine::PlayAnimation(uint32_t animID)
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->hitAnimsToApply.push(animID);
	}

	void IRemotePlayerEngine::UpdateAVData(const std::string &avName_, const AVData &data)
	{
		auto avName = avName_;
		std::transform(avName.begin(), avName.end(), avName.begin(), ::tolower);
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->avData[avName] = data;
	}

	void IRemotePlayerEngine::AddActiveEffect(const ci::MagicEffect *effect, float uiDisplayDuration, float uiDisplayMagnitude)
	{
		// ...
	}

	void IRemotePlayerEngine::RemoveActiveEffect(const ci::MagicEffect *effect)
	{
		// ...
	}

	void IRemotePlayerEngine::AddPerk(const ci::Perk *perk)
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->perks.insert(perk);
		pimpl->perks.erase(nullptr);
	}

	void IRemotePlayerEngine::RemovePerk(const ci::Perk *perk)
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		pimpl->perks.erase(perk);
	}

	std::wstring IRemotePlayerEngine::GetName() const
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->name;
	}

	uint32_t IRemotePlayerEngine::GetCell() const
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->currentNonExteriorCell ? pimpl->currentNonExteriorCell->formID : 0;
	}

	uint32_t IRemotePlayerEngine::GetWorldSpace() const
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->worldSpaceID;
	}

	LookData IRemotePlayerEngine::GetLookData() const
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->lookData;
	}

	std::vector<ci::ItemType *> IRemotePlayerEngine::GetEquippedArmor() const
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		std::vector<ci::ItemType *> result;
		for (auto item : pimpl->eq.armor)
			result.push_back(const_cast<ci::ItemType *>(item));
		return result;
	}

	ci::ItemType *IRemotePlayerEngine::GetEquippedWeapon(bool isLeftHand) const
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return const_cast<ci::ItemType *>(pimpl->eq.hands[isLeftHand]);
	}

	ci::ItemType *IRemotePlayerEngine::GetEquippedAmmo() const
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return const_cast<ci::ItemType *>(pimpl->eq.ammo);
	}

	const Spell *IRemotePlayerEngine::GetEquippedShout() const
	{
		return nullptr; // Not Implemented
	}

	AVData IRemotePlayerEngine::GetAVData(const std::string &avName_) const
	{
		try {
			auto avName = avName_;
			std::transform(avName.begin(), avName.end(), avName.begin(), ::tolower);
			auto &pimpl = this->GetImpl();
			return pimpl->avData.at(avName);
		}
		catch (...) {
			return {};
		}
	}

	uint32_t IRemotePlayerEngine::GetRefID() const
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		return pimpl->formID;
	}

	TESNPC *IRemotePlayerEngine::AllocateNPC()
	{
		auto &pimpl = this->GetImpl();
		std::lock_guard<dlf_mutex> l(pimpl->mutex);

		auto result = lookSync->Apply(pimpl->lookData);
		ci::ApplyPackage(result);
		result->TESActorBaseData::flags.bleedoutOverride = true;
		result->TESActorBaseData::flags.invulnerable = false;
		result->TESActorBaseData::flags.essential = false;
		result->TESActorBaseData::flags.ghost = false;
		result->combatStyle = (TESCombatStyle *)LookupFormByID(0x000F960C);
		result->combatStyle->general.magicMult = 5;
		result->combatStyle->general.meleeMult = 13.5;
		result->combatStyle->general.staffMult = 15;
		result->height = pimpl->height;

		if (pimpl->baseNpc != nullptr)
		{
			return pimpl->baseNpc;
		}

		return result;
	}
}