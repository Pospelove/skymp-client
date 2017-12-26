#include "../stdafx.h"
#include "CoreInterface.h"
#include "../Sync/LookData.h"
#include "../Sync/MovementData.h"
#include "../Sync/AnimData.h"
#include "../Sync/SyncOptions.h"
#include "Skyrim/Events/ScriptEvent.h"
#include "Skyrim/Camera/PlayerCamera.h"
#include <queue>

#pragma comment(lib, "winmm")

void PreventCrash() {
	g_thePlayer->GetActorBase()->numHeadParts = 0;
	g_thePlayer->GetActorBase()->headparts = nullptr;
}

void MoveTo(uint32_t markerRefID)
{
	std::wostringstream os;
	os << L"player.moveto " << std::hex << markerRefID;
	ci::ExecuteCommand(ci::CommandType::Console, os.str());

	if (SyncOptions::GetSingleton()->GetInt("MUTE_SOUND_ON_TP") != FALSE)
	{
		auto setVolume = [](DWORD v) {
			WAVEFORMATEX    wfx = { 0 };

			wfx.wFormatTag = WAVE_FORMAT_PCM;
			wfx.cbSize = 0;
			wfx.nChannels = 1;
			wfx.nSamplesPerSec = 1000;
			wfx.wBitsPerSample = 16;
			wfx.nBlockAlign = wfx.wBitsPerSample / 8;
			wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

			auto hEvent = CreateEvent(0, TRUE, FALSE, 0);
			if (hEvent)
			{
				HWAVEOUT out;
				waveOutOpen(&out,
					WAVE_MAPPER,
					&wfx,
					(DWORD)0,
					(DWORD)GetModuleHandle(NULL),
					CALLBACK_NULL);
				waveOutSetVolume(out, v);
			}
		};

		setVolume(0);

		SET_TIMER(0, [=] {
			SET_TIMER(1500, [=] {
				setVolume(-1);
			});
		});
	}
}

bool CenterOnCell(uint32_t cellID, NiPoint3 pos, float angleZ)
{
	if (cellID == NULL)
		return false;
	auto cell = (TESObjectCELL *)LookupFormByID(cellID);

	if (!cell)
		return false;

	if (cell->formType == FormType::Cell)
	{
		auto marker = (TESObjectREFR *)LookupFormByID(0x000C44AD);
		if (marker != nullptr)
		{
			auto cellWas = marker->parentCell;
			auto posWas = marker->pos;
			auto rotWas = marker->rot;
			marker->parentCell = cell;
			marker->pos = pos;
			marker->rot = { 0,0,angleZ };
			MoveTo(marker->formID);
			SET_TIMER(100, [=] {
				marker->parentCell = cellWas;
				marker->pos = posWas;
				marker->rot = rotWas;
			});
			return true;
		}
	}
	return false;
}

auto lookSync = ILookSynchronizer::GetV17();

ErrorHandling::DeadlockFreeMutex<0> localPlMutex;
std::wstring *localPlName = nullptr;
UInt32 localPlCellID = 0,
localPlWorldSpaceID = 0;
ci::LookData localPlLookData = {};
ci::MovementData localPlMovementData = {};
uint32_t localPlCrosshairRef = 0;
std::vector<const ci::ItemType *> localPlEquippedArmor = {};
const ci::ItemType *localPlEquippedWeap[] = { nullptr, nullptr };
const ci::Spell *localPlEquippedSpells[] = { nullptr, nullptr };
const ci::ItemType *localPlEquippedAmmo = nullptr;
clock_t lastLocalPlUpdate = 0;
bool registered = false;
clock_t localPlCrosshairRefUpdateMoment = 0;
uint32_t lastFurniture = 0;

std::map<const ci::ItemType *, uint32_t> inventory;
std::set<SpellItem *> spellList;
std::map<TESForm *, const ci::ItemType *> knownItems;
std::map<TESForm *, const ci::Spell *> knownSpells;
std::map<TESForm *, const ci::MagicEffect *> knownEffects;
std::map<TESForm *, const ci::Enchantment *> knownEnch;

struct ActiveEffectInfo
{
	float uiDisplayDuration;
	float uiDisplayMagnitude;

	friend bool operator==(const ActiveEffectInfo &l, const ActiveEffectInfo &r) {
		return l.uiDisplayDuration == r.uiDisplayDuration && l.uiDisplayMagnitude == r.uiDisplayMagnitude;
	}
};

std::map<const ci::MagicEffect *, ActiveEffectInfo> localPlActiveEffects;

const ci::ItemType *GetEquippedAmmo(TESForm **outTESForm = nullptr) 
{
	for (auto it1 = inventory.begin(); it1 != inventory.end(); ++it1)
	{
		auto itemType = it1->first;
		TESForm *form = nullptr;
		for (auto it = knownItems.begin(); it != knownItems.end(); ++it)
			if (it->second == itemType)
			{
				form = it->first;
				break;
			}
		if (form != nullptr 
			&& form->formType == FormType::Ammo
			&& sd::IsEquipped(g_thePlayer, form))
		{
			if(outTESForm != nullptr)
				*outTESForm = form;
			return itemType;
		}
	};
	return nullptr;
}

void RegisterAddItem(const ci::ItemType *itemType, uint32_t count)
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	inventory[itemType] += count;
}

void RegisterRemoveItem(const ci::ItemType *itemType, uint32_t count)
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	if (inventory[itemType] > count)
		inventory[itemType] -= count;
	else
		inventory.erase(itemType);
}

class CIAccess
{
public:
	static void OnItemDropped(const ci::ItemType *itemType, uint32_t count)
	{
		std::thread([=] {
			auto logic = ci::IClientLogic::clientLogic;
			if (logic != nullptr)
			{
				std::lock_guard<ci::Mutex> l(logic->callbacksMutex);
				logic->OnItemDropped(itemType, count);
			}
			RegisterRemoveItem(itemType, count);
		}).detach();
	}

	static void OnItemUsed(const ci::ItemType *itemType)
	{
		std::thread([=] {
			auto logic = ci::IClientLogic::clientLogic;
			if (logic != nullptr)
			{
				std::lock_guard<ci::Mutex> l(logic->callbacksMutex);
				logic->OnItemUsed(itemType);
			}
			RegisterRemoveItem(itemType, 1);
		}).detach();
	}

	static void OnItemUsedInCraft(const ci::ItemType *itemType, uint32_t count)
	{
		std::thread([=] {
			auto logic = ci::IClientLogic::clientLogic;
			if (logic != nullptr)
			{
				std::lock_guard<ci::Mutex> l(logic->callbacksMutex);
				logic->OnItemUsedInCraft(itemType, count);
			}
			RegisterRemoveItem(itemType, count);
		}).detach();
	}

	static void OnCraftFinish(bool isPoison, const ci::ItemType *itemType, uint32_t craftedItemsCount)
	{
		std::thread([=] {
			auto logic = ci::IClientLogic::clientLogic;
			if (logic != nullptr)
			{
				std::lock_guard<ci::Mutex> l(logic->callbacksMutex);
				logic->OnCraftFinish(isPoison, itemType, craftedItemsCount);
			}
			RegisterAddItem(itemType, craftedItemsCount);
		}).detach();
	}

	static void OnLockpick()
	{
		std::thread([=] {
			auto logic = ci::IClientLogic::clientLogic;
			if (logic != nullptr)
			{
				std::lock_guard<ci::Mutex> l(logic->callbacksMutex);
				logic->OnLockpick();
			}
		}).detach();
	}

	static void OnEnch(const ci::ItemType *itemType, const ci::Enchantment *ench)
	{
		std::thread([=] {
			auto logic = ci::IClientLogic::clientLogic;
			if (logic != nullptr)
			{
				std::lock_guard<ci::Mutex> l(logic->callbacksMutex);
				logic->OnItemEnchanting(itemType, ench);
			}
		}).detach();
	}

	static ci::Mutex &GetMutex() {
		return ci::IClientLogic::callbacksMutex;
	}
};

void IgnoreItemRemove();

void DetectEnchanting()
{
	const auto changes = g_thePlayer->extraData.GetExtraData<ExtraContainerChanges>();

	if (changes->next != nullptr)
	{
		//ErrorHandling::SendError("WARN:LocalPlaer ExtraData - next found");
	}

	if (changes->changes->entryList == nullptr)
	{
		ErrorHandling::SendError("ERROR:LocalPlaer Unable to load ExtraData");
		return;
	}

	const auto &entryList = *changes->changes->entryList;
	for (auto &entry : entryList)
	{
		if (!entry->extraList)
			continue;
		auto &entryExtraList = *entry->extraList;
		for (auto &extraList : entryExtraList)
		{
			auto extraEnch = extraList->GetExtraData<ExtraEnchantment>();
			if (extraEnch != nullptr)
			{
				try {
					auto itemType = knownItems.at(entry->baseForm);
					for (auto &pair : knownEnch)
					{
						auto form = (EnchantmentItem *)pair.first;
						auto &ciEnch = pair.second;

						auto mgef = form->effectItemList.front()->mgef;
						if (mgef == extraEnch->enchant->effectItemList.front()->mgef)
						{
							CIAccess::OnEnch(itemType, ciEnch);
							const auto count = sd::GetItemCount(g_thePlayer, entry->baseForm);
							for (int32_t i = 0; i != count; ++i)
								IgnoreItemRemove();
							sd::RemoveItem(g_thePlayer, entry->baseForm, -1, true, nullptr);
							sd::AddItem(g_thePlayer, entry->baseForm, count, true);
							break;
						}
					}
				}
				catch (...) {
					ErrorHandling::SendError("ERROR:LocalPlayer DetectEnchanting() unknown form");
				}
			}
		}
	}
}

class ContainerChangedEventSink : public BSTEventSink<TESContainerChangedEvent>
{
public:
	static ContainerChangedEventSink *GetSingleton()
	{
		static ContainerChangedEventSink sink;
		return &sink;
	}

	virtual ~ContainerChangedEventSink() override
	{
	}

	void IgnoreItemRemove()
	{
		std::lock_guard<std::mutex> l(this->removesToIgnoreMutex);
		this->removesToIgnore++;
	}

	void IgnoreItemAdd()
	{
		std::lock_guard<std::mutex> l(this->removesToIgnoreMutex);
		this->addsToIgnore++;
	}

	virtual	EventResult	ReceiveEvent(TESContainerChangedEvent *evn, BSTEventSource<TESContainerChangedEvent> * source) override
	{
		{
			if (evn->to == g_thePlayer->formID)
				if (evn->from == 0)
				{
					auto count = evn->count;
					auto form = LookupFormByID(evn->item);

					bool ignore = 0;
					{
						std::lock_guard<std::mutex> l(this->removesToIgnoreMutex);
						if (addsToIgnore)
						{
							ignore = true;
							addsToIgnore--;
						}
					}
					if (!ignore)
					{
						if (MenuManager::GetSingleton()->IsMenuOpen("Crafting Menu"))
						{
							const bool isIllegal = knownItems.find(form) == knownItems.end();
							if (!isIllegal)
							{
								SET_TIMER_LIGHT(230, [=] {
									try {
										CIAccess().OnCraftFinish(false, knownItems.at(form), count);
									}
									catch (...) {
										ErrorHandling::SendError("ERROR:LocalPlayer OnCraftFinish()");
									}
								});
							}
						}
					}
				}
		}

		if(evn->to == g_thePlayer->formID)
			if (evn->from == 0)
			{
				auto count = evn->count;
				auto form = LookupFormByID(evn->item);
				if (count == 1)
				{
					const bool isIllegal = knownItems.find(form) == knownItems.end();
					const bool isPotion = form->formType == FormType::Potion;
					SET_TIMER_LIGHT(isPotion ? 25 : 250, [=] {
						bool isIllgalCount = false;
						if (isIllegal == false)
						{
							try {
								isIllgalCount =
									(inventory[knownItems.at(form)] != sd::GetItemCount(g_thePlayer, form));
							}
							catch (...) {
								ErrorHandling::SendError("WARN:LocalPlayer knownItems.at()");
								isIllgalCount = false;
							}
						}
						if (isIllegal && isPotion)
						{
							const bool isPoison = ((AlchemyItem *)form)->IsPoison();
							CIAccess().OnCraftFinish(isPoison, nullptr, 1);
							sd::RemoveItem(g_thePlayer, form, -1, true, nullptr);
						}
					});
				}
			}

		if (evn->from == g_thePlayer->formID)
			if (evn->to == 0)
			{
				WorldCleaner::GetSingleton()->SetFormProtected(0, true);
				WorldCleaner::GetSingleton()->SetFormProtected(0, false);
				auto count = evn->count;
				auto form = LookupFormByID(evn->item);
				const bool toRef = evn->toReference != 0;
				auto f = [=] {
					if (sd::IsDead(g_thePlayer))
						return;

					std::lock_guard<std::mutex> l(this->removesToIgnoreMutex);
					if (this->removesToIgnore > 0)
						this->removesToIgnore--;
					else
					{
						try {
							auto itemType = knownItems.at(form);
							if (toRef)
								CIAccess().OnItemDropped(itemType, count);
							else if (MenuManager::GetSingleton()->IsMenuOpen("Crafting Menu"))
							{
								CIAccess().OnItemUsedInCraft(itemType, count);
								if (itemType->GetClass() == ci::ItemType::Class::SoulGem)
								{
									SET_TIMER_LIGHT(200, DetectEnchanting);
								}
							}
							else
								CIAccess().OnItemUsed(itemType);

							std::lock_guard<dlf_mutex> l(localPlMutex);
							if (inventory[itemType] > count)
								inventory[itemType] -= count;
							else
								inventory.erase(itemType);
						}
						catch (...) {
						}
					}
				};
				SET_TIMER_LIGHT(0,f);
			}
		return EventResult::kEvent_Continue;
	}

private:
	ContainerChangedEventSink()
	{
	}

	int32_t removesToIgnore = 0, addsToIgnore = 0;
	std::mutex removesToIgnoreMutex;
};

void IgnoreItemRemove()
{
	ContainerChangedEventSink::GetSingleton()->IgnoreItemRemove();
}

void IgnoreItemAdd()
{
	ContainerChangedEventSink::GetSingleton()->IgnoreItemAdd();
}

class PlayerBowShotEventSink : public BSTEventSink<TESPlayerBowShotEvent>
{
public:
	static PlayerBowShotEventSink *GetSingleton()
	{
		static PlayerBowShotEventSink sink;
		return &sink;
	}

	~PlayerBowShotEventSink() override
	{
	}

	virtual	EventResult	ReceiveEvent(TESPlayerBowShotEvent *evn, BSTEventSource<TESPlayerBowShotEvent> *) override
	{
		struct MyTESPlayerBowShotEvent
		{
			uint32_t weapID;
			uint32_t ammoID;
			float power;
		};
		auto evnCopy = *(MyTESPlayerBowShotEvent *)evn;

		ContainerChangedEventSink::GetSingleton()->IgnoreItemRemove();
		auto f = [evnCopy] {
			auto weap = (TESObjectWEAP *)LookupFormByID(evnCopy.weapID);
			if (weap == nullptr)
				return;
			TESAmmo *ammo = (TESAmmo *)LookupFormByID(evnCopy.ammoID);
			if (ammo == nullptr)
				return;
			std::thread([=] {
				std::lock_guard<ci::Mutex> l(CIAccess::GetMutex());
				ci::LocalPlayer::GetSingleton()->onPlayerBowShot(evnCopy.power);
			}).detach();
		};
		SET_TIMER_LIGHT(0, f);
		return EventResult::kEvent_Continue;
	}

private:
	PlayerBowShotEventSink()
	{
	}
};

struct Task
{
	std::shared_ptr<ci::MovementData> movDataPtr = nullptr;
	uint32_t cellID = 0;
	std::vector<uint32_t> markerIDs;
	std::shared_ptr<UInt16> angleZWaitingForSetPos;
} task;

struct CheckTeleport
{
	std::function<void()> reTP;
	NiPoint3 destPos;
	clock_t lastReTP = 0;

	bool IsEmpty() const {
		return reTP == nullptr;
	}
} checkTeleport;

ci::LocalPlayer *ci::LocalPlayer::GetSingleton()
{
	static LocalPlayer pl;
	return &pl;
}

ci::LocalPlayer::LocalPlayer()
{
	g_containerChangedEventSource.AddEventSink(ContainerChangedEventSink::GetSingleton());
	g_playerBowShotEventSource.AddEventSink(PlayerBowShotEventSink::GetSingleton());
}

void ci::LocalPlayer::SetName(const std::wstring &name)
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	if (!localPlName)
		localPlName = new std::wstring;
	*localPlName = name;

	SET_TIMER(0, [=] {
		cd::SetDisplayName(g_thePlayer, WstringToString(name), true);
		cd::SetName(g_thePlayer->baseForm, WstringToString(name));
	});
}

void ci::LocalPlayer::DoTeleport_OT(bool ot)
{
	if (lastLocalPlUpdate + 1000 < clock())
		registered = false; // force re-register for animation events

	auto movDataPtr = task.movDataPtr;
	if (task.cellID != 0 && movDataPtr != nullptr)
	{
		CenterOnCell(task.cellID, movDataPtr->pos, movDataPtr->angleZ);
		task = {};
	}
	if (task.markerIDs.empty() == false && movDataPtr != nullptr)
	{
		uint32_t nearestMarker = task.markerIDs.front();
		const auto posLocalPl =
			task.movDataPtr != nullptr ? task.movDataPtr->pos : this->GetPos();
		for (auto it = task.markerIDs.begin(); it != task.markerIDs.end(); ++it)
		{
			const auto posNearest = cd::GetPosition(cd::Value<TESObjectREFR>(nearestMarker)),
				posI = cd::GetPosition(cd::Value<TESObjectREFR>(*it));
			if ((posNearest - posLocalPl).Length() > (posI - posLocalPl).Length())
				nearestMarker = *it;
		}
		MoveTo(nearestMarker);
		task.movDataPtr = movDataPtr;
		task = {};
	}
}

void ci::LocalPlayer::SetPos(NiPoint3 pos)
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	MovementData movData;
	movData.pos = pos;
	if (task.movDataPtr != nullptr)
		movData.angleZ = task.movDataPtr->angleZ;
	else
		movData.angleZ = this->GetMovementData().angleZ;
	this->ApplyMovementData(movData);
}

void ci::LocalPlayer::SetAngleZ(float angleZ)
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	if (lastLocalPlUpdate + 100 < clock())
	{
		if (task.movDataPtr != nullptr)
			task.movDataPtr->angleZ = (UInt16)angleZ;
		else
			task.angleZWaitingForSetPos = std::make_shared<UInt16>(angleZ);
	}
	else
	{
		SET_TIMER(350, [=] {
			sd::SetAngle(g_thePlayer, 0, 0, angleZ);
		});
	}
	SET_TIMER(0, [=] {
		sd::SetAngle(g_thePlayer, 0, 0, angleZ);
	});
}

void ci::LocalPlayer::ApplyMovementData(const ci::MovementData &movementData)
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	auto movementData_ = movementData;
	if (task.angleZWaitingForSetPos != nullptr)
	{
		movementData_.angleZ = *task.angleZWaitingForSetPos;
		task.angleZWaitingForSetPos = nullptr;
	}

	auto teleport = [=] {
		localPlMovementData = movementData_;
		task.movDataPtr = std::make_unique<ci::MovementData>(localPlMovementData);
		if (lastLocalPlUpdate + 100 < clock())
			this->DoTeleport_OT();
	};
	if (lastLocalPlUpdate + 100 < clock())
	{
		checkTeleport.reTP = teleport;
		checkTeleport.destPos = movementData_.pos;
	}
	teleport();
}

void ci::LocalPlayer::UseFurniture(const Object *target, bool anim)
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	auto refID = target->GetRefID();
	SET_TIMER(0, [=] {
		auto ref = (TESObjectREFR *)LookupFormByID(refID);
		if (ref)
		{
			if (anim)
			{
				auto baseFormID = ref->baseForm->formID;
				switch (baseFormID)
				{
				case 0x31897:
				case 0x351EB:
				case 0x180D8:
					//break;
				default:
					if (MenuManager::GetSingleton()->IsMenuOpen("Lockpicking Menu"))
						break;
					sd::Activate(ref, g_thePlayer, true);
					lastFurniture = ref->formID;
					break;
				}
			}
			else
			{
				sd::MoveTo(g_thePlayer, ref, 0, 0, 0, false);
			}
		}
	});
}
void ci::LocalPlayer::AddItem(const ItemType *item, uint32_t count, bool silent)
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	if (item)
	{
		auto form = LookupFormByID(item->GetFormID());

		SET_TIMER_LIGHT(1, [=] {
			knownItems[form] = item;
		});

		if (count > 0)
		{
			inventory[item] += count;

			if (form && !MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
			{
				SET_TIMER(0, [=] {
					ContainerChangedEventSink::GetSingleton()->IgnoreItemAdd();
					sd::AddItem(g_thePlayer, form, count, silent || MenuManager::GetSingleton()->IsMenuOpen("Crafting Menu"));
				});
			}
		}
	}
}
void ci::LocalPlayer::RemoveItem(const ItemType *item, uint32_t count, bool silent)
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	if (item && count)
	{
		if (inventory[item] > count)
			inventory[item] -= count;
		else
			inventory.erase(item);

		auto form = LookupFormByID(item->GetFormID());
		if (form && !MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
		{
			SET_TIMER(0, [=] {
				ContainerChangedEventSink::GetSingleton()->IgnoreItemRemove();
				if (silent)
					sd::RemoveItem(g_thePlayer, form, count, silent, nullptr);
				else
					cd::RemoveItem(g_thePlayer, form, count, silent, nullptr);
			});
		}
	}
}

void ci::LocalPlayer::EquipItem(const ItemType *item, bool silent, bool preventRemoval, bool leftHand)
{
	if (!item)
		return;
	SET_TIMER(200, [=] {
		auto form = LookupFormByID(item->GetFormID());
		if (form)
		{
			int32_t count;
			try {
				count = inventory.at(knownItems.at(form));
			}
			catch (...) {
				count = 0;
			}

			if (!count)
				return;

			if (sd::GetItemCount(g_thePlayer, form) == 0)
			{
				SET_TIMER(200, [=] {
					this->EquipItem(item, silent, preventRemoval, leftHand);
				});
				return;
			}
			sd::EquipItem(g_thePlayer, form, preventRemoval, silent);
		}
	});
}

void ci::LocalPlayer::UnequipItem(const ItemType *item, bool silent, bool preventEquip, bool isLeftHand)
{
	if (!item)
		return;
	SET_TIMER(0, [=] {
		auto form = LookupFormByID(item->GetFormID());
		if (form)
			sd::UnequipItem(g_thePlayer, form, preventEquip, silent);
	});
}

void ci::LocalPlayer::AddSpell(const Spell *spell, bool silent)
{
	if (!spell || spell->GetNumEffects() == 0)
		return;

	auto form = (SpellItem *)LookupFormByID(spell->GetFormID());
	if (form)
	{
		SET_TIMER(300, [=] {
			knownSpells[form] = spell;
			sd::AddSpell(g_thePlayer, form, !silent);
		});
	}
	std::lock_guard<dlf_mutex> l(localPlMutex);
	spellList.insert(form);
}

void ci::LocalPlayer::RemoveSpell(const Spell *spell, bool silent)
{
	if (!spell || spell->GetNumEffects() == 0)
		return;
	auto form = (SpellItem *)LookupFormByID(spell->GetFormID());
	if (form)
	{
		SET_TIMER(300, [=] {
			sd::RemoveSpell(g_thePlayer, form);
		});
	}
	std::lock_guard<dlf_mutex> l(localPlMutex);
	spellList.erase(form);
}

void ci::LocalPlayer::EquipSpell(const Spell *spell, bool leftHand)
{
	if (!spell || spell->GetNumEffects() == 0)
		return;
	SET_TIMER(300, [=] {
		auto form = (SpellItem *)LookupFormByID(spell->GetFormID());
		if (form)
		{
			knownSpells[form] = spell;
			sd::EquipSpell(g_thePlayer, form, leftHand ? 0 : 1);
		}
	});
}

void ci::LocalPlayer::UnequipSpell(const Spell *spell, bool leftHand)
{
	if (!spell || spell->GetNumEffects() == 0)
		return;
	SET_TIMER(300, [=] {
		auto form = (SpellItem *)LookupFormByID(spell->GetFormID());
		if (form)
			sd::UnequipSpell(g_thePlayer, form, leftHand ? 0 : 1);
	});
}


void ci::LocalPlayer::PlayAnimation(uint32_t hitAnimID)
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	SET_TIMER_LIGHT(0, [=] {
		AnimData_::Apply(g_thePlayer, hitAnimID, true);
	});
}

std::map<std::string, ci::AVData> localPlAVData;

void ci::LocalPlayer::UpdateAVData(const std::string &avName_, const AVData &avData)
{
	if (avName_.empty())
		return;
	if (avName_[0] == '_')
	{
		SET_TIMER_LIGHT(0, [=] {
			cd::SetGameSettingFloat(&avName_.data()[1], avData.base + avData.modifier, {});
		});
	}

	std::lock_guard<dlf_mutex> l(localPlMutex);
	auto avName = avName_;
	std::transform(avName_.begin(), avName_.end(), avName.begin(), ::tolower);

	localPlAVData[avName] = avData;

	const auto base = avData.base,
		modifier = avData.modifier,
		percentage = avData.percentage;

	struct AVData
	{
		float base, modifier, percentage;
	};
	static std::map<std::string, AVData> data;

	SET_TIMER_LIGHT(0, [=] {
		const auto av = base + modifier;
		if (av == av)
			sd::SetActorValue(g_thePlayer, (char *)avName.data(), av);

		auto cstr = (char *)avName.data();
		auto val = sd::GetActorValue(g_thePlayer, cstr);
		auto dest = val / sd::GetActorValuePercentage(g_thePlayer, cstr) * percentage;
		auto toRestore = dest - val;
		if (dest == 0)
		{
			sd::ForceThirdPerson();
			sd::Wait(500);
		}
		if (toRestore == toRestore)
		{
			if (toRestore > 0)
				sd::RestoreActorValue(g_thePlayer, cstr, toRestore);
			else
				sd::DamageActorValue(g_thePlayer, cstr, -toRestore);
		}

	});

	data[avName] = { base, modifier, percentage };
}

void ci::LocalPlayer::AddActiveEffect(const ci::MagicEffect *effect, float uiDisplayDuration, float uiDisplayMagnitude)
{
	if (nullptr == effect)
		return;
	std::lock_guard<dlf_mutex> l(localPlMutex);
	localPlActiveEffects[effect] = { uiDisplayDuration, uiDisplayMagnitude };
	knownEffects[LookupFormByID(effect->GetFormID())] = effect;
}

void ci::LocalPlayer::RemoveActiveEffect(const ci::MagicEffect *effect)
{
	if (nullptr == effect)
		return;
	std::lock_guard<dlf_mutex> l(localPlMutex);
	localPlActiveEffects.erase(effect);
}

void ci::LocalPlayer::SetCell(uint32_t cellID)
{
	if (cellID == 0)
		return;
	std::lock_guard<dlf_mutex> l(localPlMutex);
	task.cellID = cellID;
	if (lastLocalPlUpdate + 100 < clock())
		this->DoTeleport_OT();
}

void ci::LocalPlayer::SetWorldSpace(uint32_t worldSpaceID)
{
	if (this->GetWorldSpace() == worldSpaceID)
		return;
	if (worldSpaceID == 0)
		return;
	auto markers = Utility::GetLoadedRefs(worldSpaceID);
	if (!markers.empty())
	{
		std::lock_guard<dlf_mutex> l(localPlMutex);
		task.markerIDs = markers;
	}
}

ci::LookData lastAppliedLook;

void ci::LocalPlayer::ApplyLookData(const LookData &lookData)
{
	SET_TIMER(0, [=] {
		//if (this->GetLookData() != lookData)
			lookSync->Apply(lookData, g_thePlayer);
		lastAppliedLook = lookData;
	});
}

std::wstring ci::LocalPlayer::GetName() const
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	if (!localPlName)
	{
		static std::mutex nameMtx;
		std::lock_guard<std::mutex> l(nameMtx);
		return StringToWstring(g_thePlayer->GetReferenceName());
	}
	return *localPlName;
}

auto mayReturn0 = [] {
	if (SyncOptions::GetSingleton()->GetInt("NORMAL_PROCESSING_WITH_MENUS") != FALSE)
		return MenuManager::GetSingleton()->IsMenuOpen("Main Menu");

	enum {
		MagicNumberMs = 500,
	};
	return lastLocalPlUpdate + MagicNumberMs < clock();
};

NiPoint3 ci::LocalPlayer::GetPos() const
{
	if (mayReturn0())
	{
		localPlMovementData = {};
		return localPlMovementData.pos;
	}
	return this->GetMovementData().pos;
}

float ci::LocalPlayer::GetAngleZ() const
{
	return this->GetMovementData().angleZ;
}

uint32_t ci::LocalPlayer::GetCell() const
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	if (mayReturn0())
		localPlCellID = 0;
	return localPlCellID;
}

uint32_t ci::LocalPlayer::GetWorldSpace() const
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	if (mayReturn0())
		localPlWorldSpaceID = 0;
	return localPlWorldSpaceID;
}

ci::LookData ci::LocalPlayer::GetLookData() const
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	//if (mayReturn0())
	//	localPlLookData = {};
	return localPlLookData;
}

ci::MovementData ci::LocalPlayer::GetMovementData() const
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	if (mayReturn0())
		localPlMovementData = {};
	return localPlMovementData;
}

std::vector<ci::ItemType *> ci::LocalPlayer::GetEquippedArmor() const
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	if (mayReturn0())
		localPlEquippedArmor = {};
	return *reinterpret_cast<std::vector<ci::ItemType *> *>(&localPlEquippedArmor);
}

ci::ItemType *ci::LocalPlayer::GetEquippedWeapon(bool isLeftHand) const
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	return const_cast<ci::ItemType *>(localPlEquippedWeap[isLeftHand]);
}

ci::ItemType *ci::LocalPlayer::GetEquippedAmmo() const
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	return const_cast<ci::ItemType *>(localPlEquippedAmmo);
}

const ci::Spell *ci::LocalPlayer::GetEquippedSpell(bool isLeftHand) const
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	return localPlEquippedSpells[isLeftHand];
}

const ci::Spell *ci::LocalPlayer::GetEquippedShout() const
{
	return nullptr;
	// Not implemented
}

ci::AVData ci::LocalPlayer::GetAVData(const std::string &avName_) const
{
	auto avName = avName_;
	std::transform(avName_.begin(), avName_.end(), avName.begin(), ::tolower);

	std::lock_guard<dlf_mutex> l(localPlMutex);
	try {
		return localPlAVData.at(avName);
	}
	catch (...) {
		return {};
	}
}

std::queue<std::shared_ptr<AnimData_::AnimID>> pcAttacks;

std::shared_ptr<AnimData_::AnimID> ci::LocalPlayer::GetNextHitAnim()
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	if (pcAttacks.empty())
		return nullptr;
	auto result = pcAttacks.front();
	pcAttacks.pop();
	return result;
}

uint32_t pcGold = 0;

uint32_t ci::LocalPlayer::GetDisplayGold() const
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	return pcGold;
}

void ci::LocalPlayer::SetDisplayGold(uint32_t count)
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	pcGold = count;
}

void ci::LocalPlayer::AddActiveEffect(const ci::Spell *parentSpell, const ci::MagicEffect *effect, const ci::IActor *caster, float magnitude, int64_t endMoment)
{
	if (effect && parentSpell && caster != nullptr)
	{
		SET_TIMER_LIGHT(0, [=] {
			auto spellForm = (SpellItem *)LookupFormByID(parentSpell->GetFormID());
			if (spellForm == nullptr)
				return;

			sd::Spell::RemoteCast(spellForm, g_thePlayer, g_thePlayer, g_thePlayer);
		});
	}
	std::lock_guard<dlf_mutex> l(localPlMutex);
}

void ci::LocalPlayer::ClearActiveEffects()
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
}

void ci::LocalPlayer::Resurrect()
{
	SET_TIMER_LIGHT(1, [=] {
		if (sd::IsDead(g_thePlayer) || sd::GetActorValue(g_thePlayer, "Health") == 0)
		{
			sd::RestoreActorValue(g_thePlayer, "Health", 99999);
			sd::Wait(1000);
		}
	});
}

clock_t timer = clock() + 5000;
bool set = false;

auto inventoryLikeMenus = { "BarterMenu",
	"Book Menu",
	"ContainerMenu",
	"Crafting Menu",
	"FavoritesMenu",
	"GiftMenu",
	"InventoryMenu",
	"MagicMenu"
};

template <int32_t hand>
void PreventKillmoves()
{
	static TESForm *eqWas = nullptr;
	auto eq = sd::GetEquippedWeapon(g_thePlayer, hand);

	if (eq && g_thePlayer->IsWeaponDrawn() == false)
		eq = nullptr;

	if(eq)
		for (auto menu : inventoryLikeMenus)
		{
			if (MenuManager::GetSingleton()->IsMenuOpen(menu))
			{
				eq = nullptr;
				break;
			}
		}

	static std::map<TESForm *, float> dmg;
	if (eqWas != eq)
	{
		if (eq)
		{
			if (eq->GetAttackDamage() != 0)
				dmg[eq] = eq->GetAttackDamage();
			eq->attackDamage = 2;
		}

		if (eqWas)
		{
			try {
				((ci::ItemType *)knownItems.at(eqWas))->SetDamage(dmg[eqWas]);
			}
			catch (...) {
			}
		}
		eqWas = eq;
	}
}

template <int32_t hand>
void PreventMagicEffects()
{
	static SpellItem *eqWas = nullptr;
	auto eq = sd::GetEquippedSpell(g_thePlayer, !hand);

	if (eq && g_thePlayer->IsWeaponDrawn() == false)
		eq = nullptr;

	if (eq)
		for (auto menu : inventoryLikeMenus)
		{
			if (MenuManager::GetSingleton()->IsMenuOpen(menu))
			{
				eq = nullptr;
				break;
			}
		}

	static std::map <TESForm *, std::vector <EffectItem>> dmg;
	if (eqWas != eq)
	{
		if (eq)
		{
			if (eq->effectItemList.size() != 0)
			{
				std::vector<EffectItem> vec;
				for (size_t i = 0; i != eq->effectItemList.size(); ++i)
				{
					EffectItem item;
					item = *eq->effectItemList[i];
					vec.push_back(item);
					eq->effectItemList[i]->magnitude = 0.0f;
				}
				bool fail = false;
				for (auto ei : vec)
					if (ei.magnitude == 0)
					{
						fail = true;
						break;
					}
				if (!fail)
					dmg[eq] = vec;
			}
		}

		if (eqWas)
		{
			try {
				for (size_t i = 0; i != eqWas->effectItemList.size(); ++i)
				{
					eqWas->effectItemList[i]->magnitude = dmg[eqWas][i].magnitude;
					if (dmg[eqWas][i].magnitude == 0)
					{
						ErrorHandling::SendError("ERROR:LocalPlayer Effect magnitude");
					}
				}
			}
			catch (...) {
			}
		}
		eqWas = eq;
	}
}

template <int32_t hand>
void SendMagicReleaseEvent(const ci::Spell *spell, SpellItem *spellForm)
{
	using CastStage = ci::MovementData::CastStage;
	auto this_ = ci::LocalPlayer::GetSingleton();

	static CastStage was = CastStage(-1);
	const auto movData = this_->GetMovementData();
	CastStage now = movData.castStage[hand];

	if (was != now)
	{
		if (was == CastStage::Fire && now == CastStage::Release) 
		{
			if (spell && spell->GetCastingType() == ci::Spell::CastingType::FireAndForget)
			{
				if (g_thePlayer->IsWeaponDrawn())
				{
					auto sendEvent = [=] {
						std::thread([=] {
							std::lock_guard<ci::Mutex> l(CIAccess::GetMutex());
							this_->onPlayerMagicRelease(hand);
						}).detach();
					};


					const auto delayMs = SyncOptions::GetSingleton()->GetInt("FF_CAST_DELAY");
					if(delayMs == 0)
						sendEvent();
					else
					{
						SET_TIMER_LIGHT(delayMs, [=] {
							auto proj = spellForm->effectItemList.front()->mgef->properties.projectile;
							const bool found = !!sd::FindClosestReferenceOfType(proj, movData.pos.x, movData.pos.y, movData.pos.z, 1024.0f);
							if (found)
								sendEvent();
						});
					}
				}
			}
		}
		was = now;
	}
}

void UpdateSpellsCost()
{
	bool open = false;
	for (auto menu : inventoryLikeMenus)
		open = open || MenuManager::GetSingleton()->IsMenuOpen(menu);

	for (auto &pair : knownSpells)
	{
		auto spellForm = (SpellItem *)pair.first;
		if (spellForm->formType != FormType::Spell)
			continue;

		try {
			const float realCost = knownSpells.at(spellForm)->GetCost();

			float newCost;
			if (open)
				newCost = realCost;
			else
				newCost = sd::GetActorValue(g_thePlayer, "Magicka") >= realCost ? 0 : 1000000000;
			spellForm->effectItemList[0]->cost = newCost;
		}
		catch (...) {
			ErrorHandling::SendError("ERROR:LocalPlayer Effect cost");
		}
	}
}

struct ActiveEffectLocal
{
	const ci::MagicEffect *mgef;
	ActiveEffectInfo info;

	friend bool operator<(const ActiveEffectLocal &l, const ActiveEffectLocal &r) {
		return std::make_tuple(l.mgef, l.info.uiDisplayDuration, l.info.uiDisplayMagnitude) < std::make_tuple(r.mgef, r.info.uiDisplayDuration, r.info.uiDisplayMagnitude);
	}
};

void dispell() 
{
	SET_TIMER_LIGHT(1, [] {
		sd::DispelAllSpells(g_thePlayer);

		auto activeEffects = g_thePlayer->GetActiveEffects();
		std::set<ActiveEffect *> toDisp;
		for (auto it = activeEffects->begin(); it != activeEffects->end(); ++it)
		{
			auto activeEffect = (*it);
			auto mgef = activeEffect->GetBaseObject();
			if (knownEffects.count(mgef) == 0)
			{
				toDisp.insert(activeEffect);
				continue;
			}
			auto ciEffect = knownEffects[mgef];
			if (localPlActiveEffects.count(ciEffect) == 0)
			{
				toDisp.insert(activeEffect);
			}
		}
		for (auto actEf : toDisp)
			sd::Dispel(actEf);
	});
};

void ci::LocalPlayer::UpdateActiveEffects()
{
	std::lock_guard<dlf_mutex> l(localPlMutex);

	static std::map<ActiveEffectLocal, ::SpellItem *> fakeSpells;

	using NativeEI = ::EffectItem;

	static auto convert = [](const ActiveEffectLocal &activeEffect)->NativeEI *{
		auto res = new NativeEI;
		res->area = 0;
		res->magnitude = activeEffect.info.uiDisplayMagnitude;
		res->duration = activeEffect.info.uiDisplayDuration * 1;
		res->cost = 0;
		res->mgef = (EffectSetting *)LookupFormByID(activeEffect.mgef->GetFormID());
		return res;
	};

	static auto findBaseFor = [](const ActiveEffectLocal &activeEffect)->SpellItem * {
		auto argMgef = (EffectSetting *)LookupFormByID(activeEffect.mgef->GetFormID());

		static auto defaultBase = (SpellItem *)LookupFormByID(ID_SpellItem::ShieldLesser);
		for (auto &p : knownSpells)
		{
			auto spell = (SpellItem *)p.first;
			std::set<EffectSetting *> spellEffects;
			for (auto it = spell->effectItemList.begin(); it != spell->effectItemList.end(); ++it)
			{
				auto mgef = (*it)->mgef;
				spellEffects.insert(mgef);
			}
			if (spellEffects.count(argMgef) != 0)
				return spell;

		}
		return defaultBase;
	};

	static auto newFakeSpell = [](const ActiveEffectLocal &activeEffect)->SpellItem * {

		auto base = findBaseFor(activeEffect);

		auto result = FormHeap_Allocate<SpellItem>();
		memcpy(result, base, sizeof SpellItem);

		result->formID = 0;
		result->SetFormID(Utility::NewFormID(), 1);

		auto &el = result->effectItemList;
		auto newArr = new BSTArray<NativeEI *>();
		memcpy(&el, newArr, sizeof(*newArr));
		el.clear();

		el.push_back(convert(activeEffect));

		return result;
	};

	auto fn = [&] {
		static decltype(localPlActiveEffects) last;
		//if (last != localPlActiveEffects)
		{
			for (auto p : localPlActiveEffects)
			{
				const ActiveEffectLocal key = { p.first, p.second };
				if (fakeSpells[key] == nullptr)
				{
					fakeSpells[key] = newFakeSpell(key);
				}
				for (auto it = fakeSpells[key]->effectItemList.begin(); it != fakeSpells[key]->effectItemList.end(); ++it)
				{
					(*it)->duration = key.info.uiDisplayDuration;
					(*it)->magnitude = key.info.uiDisplayMagnitude;
				}
				//sd::Spell::RemoteCast(fakeSpells[key], g_thePlayer, g_thePlayer, g_thePlayer);
				sd::Spell::Cast(fakeSpells[key], nullptr, g_thePlayer);
			}
		}
		last = localPlActiveEffects;
	};
	SAFE_CALL("LocalPlayer", fn);
}

void ci::LocalPlayer::RecoverAVs()
{
	static uint8_t numIgnores = 1; // PVS
	if (numIgnores != 0)
		--numIgnores;
	else
	{
		SET_TIMER(1300, [this] {
			std::lock_guard<dlf_mutex> l(localPlMutex);
			for (auto it = localPlAVData.begin(); it != localPlAVData.end(); ++it)
			{
				if (it->second.percentage <= 0 && it->first == "health")
					continue;
				this->UpdateAVData(it->first, it->second);
			}
		});
	}
}

void ci::LocalPlayer::RecoverInventory()
{
	SET_TIMER(0, [] {
		std::lock_guard<dlf_mutex> l(localPlMutex);
		int32_t added = 0;
		for (auto it = inventory.begin(); it != inventory.end(); ++it)
		{
			auto form = LookupFormByID(it->first ? it->first->GetFormID() : NULL);
			if (form)
			{
				sd::AddItem(g_thePlayer, form, it->second, true);
				added++;
			}
		}
	});
}

void ci::LocalPlayer::RecoverSpells()
{
	SET_TIMER_LIGHT(0, [] {
		std::lock_guard<dlf_mutex> l(localPlMutex);
		for (auto spell : spellList)
			sd::AddSpell(g_thePlayer, spell, false);
	});
}

class LocalPlInputListener : public InputListener
{
public:

	clock_t GetLastPress() const {
		return this->lastEscOrTab;
	}

private:
	virtual void OnPress(uint8_t code) override
	{
		if (code == 1 || code == 15) // esc, tab
			lastEscOrTab = clock();

		if (code == 18) // 'E'
		{
			if (MenuManager::GetSingleton()->IsMenuOpen("Crafting Menu"))
			{
				// ...
			}
		}
	}

	virtual void OnRelease(uint8_t code) override
	{
	}

	virtual void OnMousePress(uint8_t code) override
	{
	}

	virtual void OnMouseRelease(uint8_t code) override
	{
	}

	virtual void OnMouseMove(unsigned int x, unsigned int y, unsigned int z) override
	{
	}

	clock_t lastEscOrTab = 0;
};

bool UpdateLockpicking()
{
	bool result = false;

	static auto listener = (LocalPlInputListener *)nullptr;
	if (!listener)
	{
		listener = new LocalPlInputListener;
		TheIInputHook->AddListener(listener);
	}

	static bool openWas = false;
	const bool open = MenuManager::GetSingleton()->IsMenuOpen("Lockpicking Menu");
	if (openWas != open)
	{
		if (!open 
			&& clock() - listener->GetLastPress() > 1000)
		{
			CIAccess().OnLockpick();
			result = true;
		}
		openWas = open;
	}

	return result;
}

class DeathHandler
{
public:
	DeathHandler()
	{
		auto base = (TESNPC *)g_thePlayer->baseForm;
		base->TESActorBaseData::flags.essential = true;
	}

	void Update()
	{
		const bool isDead = sd::GetActorValue(g_thePlayer, "Health") == 0;
		if (isDead != this->wasDead)
		{
			this->OnDeath();
			this->wasDead = isDead;
		}
	}

	void OnDeath()
	{
		sd::PushActorAway(g_thePlayer, g_thePlayer, 1.f);
	}

private:
	~DeathHandler();
	bool wasDead = false;
};

void ci::LocalPlayer::Update()
{
	std::lock_guard<dlf_mutex> l(localPlMutex);

	if (SyncOptions::GetSingleton()->GetInt("SAFE_DEATH"))
	{
		static auto deathH = new DeathHandler;
		deathH->Update();
	}

	using EventT = TESActiveEffectApplyRemoveEvent;

	class Sink : public BSTEventSink<EventT>
	{
		virtual ~Sink() {}
		virtual	EventResult	ReceiveEvent(EventT * evn, BSTEventSource<EventT> * source) {
			return EventResult::kEvent_Continue;
		}
	};

	static BSTEventSink<EventT> *activeEffectSink = nullptr;
	if (!activeEffectSink)
	{
		activeEffectSink = new Sink;
		g_activeEffectApplyRemoveEventSource.AddEventSink(activeEffectSink);
	}

	SAFE_CALL("LocalPlayer", [&] {
		if (UpdateLockpicking())
		{
			auto ref = (TESObjectREFR *)LookupFormByID(lastFurniture);
			if (ref && ref->formType == FormType::Reference)
			{
				sd::Lock(ref, false, false);
			}
		}
	});

	SAFE_CALL("LocalPlayer", [&] {

		if (!set)
		{
			SET_TIMER(0, [] {
				sd::SetActorValue(g_thePlayer, "HealRate", 0);
				sd::SetActorValue(g_thePlayer, "MagickaRate", 0);
				sd::SetActorValue(g_thePlayer, "StaminaRate", 0);
				cd::SetGameSettingFloat("fSprintStaminaDrainMult", 0.0f);
				cd::SetGameSettingFloat("fDifficultyDamageMultiplier", 0.0f);
				cd::SetGameSettingFloat("fVATSSkillFactor", 0.0f);
				cd::SetGameSettingFloat("fVATSChanceHitMult", 0.0f);
				cd::SetGameSettingFloat("fVATSMaxChance", 0.0f);
				cd::SetGameSettingFloat("fPowerAttackDefaultBonus", 0.0f);
				cd::SetGameSettingFloat("fArrowFakeMass", 10000.f);
				cd::SetGameSettingFloat("fPlayerMaxResistance", 100);
				sd::SetActorValue(g_thePlayer, "MagicResist", 99);
			});
			set = true;

			auto npc = g_thePlayer->baseForm;
			UInt32 max = ((TESNPC *)npc)->TESSpellList::unk04->numSpells;
			for (UInt32 id = 0; id < max; ++id)
				((TESNPC *)npc)->TESSpellList::unk04->spells[id] = nullptr;
		}
	});

	PreventKillmoves<0>();
	PreventKillmoves<1>();

	PreventMagicEffects<0>();
	PreventMagicEffects<1>();

	UpdateSpellsCost();

	const auto spell0 = this->GetEquippedSpell(0), 
		spell1 = this->GetEquippedSpell(1);
	SendMagicReleaseEvent<0>(spell0, spell0 ? (SpellItem *)LookupFormByID(spell0->GetFormID()) : nullptr);
	SendMagicReleaseEvent<1>(spell1, spell1 ? (SpellItem *)LookupFormByID(spell1->GetFormID()) : nullptr);

	SAFE_CALL("LocalPlayer", [&] {
		this->UpdateActiveEffects();
	});

	// Update Display Gold
	SAFE_CALL("LocalPlayer", [&] {
		static clock_t tmr = 0;
		if (clock() - tmr > 1000)
		{
			tmr = clock();

			if (this->GetMovementData().runMode == ci::MovementData::RunMode::Standing)
			{
				auto form = (TESObjectMISC *)LookupFormByID(0x0000000F);
				const auto count = sd::GetItemCount(g_thePlayer, form);
				if (count > pcGold)
					sd::RemoveItem(g_thePlayer, form, count - pcGold, true, nullptr);
				else if (count < pcGold)
					sd::AddItem(g_thePlayer, form, pcGold - count, true);
			}
		}
	});
	
	SAFE_CALL("LocalPlayer", [&] {
		static TESRace *oldrace = nullptr;
		TESRace *newrace = (TESRace *)sd::GetRace(g_thePlayer);
		if (oldrace != newrace)
		{
			oldrace = newrace;
			auto max = newrace->GetSpellCount();
			for (SInt32 id = 0; id < max; ++id)
				newrace->TESSpellList::unk04->spells[id] = nullptr;
			max = newrace->TESSpellList::unk04->numShouts;
			for (SInt32 id = 0; id < max; ++id)
				newrace->TESSpellList::unk04->shouts[id] = nullptr;

			auto &effects = *((Actor*)(g_thePlayer))->GetActiveEffects();
			// ...
		}
	});

	SAFE_CALL("LocalPlayer", [&] {
		if (SyncOptions::GetSingleton()->GetInt("DISABLE_PLAYER_DAMAGE"))
		{
			float dmgMult;
			static float dmgMultLast = -1;
			if (sd::Obscript::GetAttackState(g_thePlayer) != 0)
				dmgMult = 0;
			else
				dmgMult = 1;
			if (dmgMult != dmgMultLast)
			{
				dmgMultLast = dmgMult;
				sd::SetActorValue(g_thePlayer, "attackdamagemult", dmgMult);
			}
		}
	});

	SAFE_CALL("LocalPlayer", [&] {
		static const std::list<char *> avNames = {
			"health", "magicka", "stamina"
		};
		for (auto avName : avNames)
		{
			try {
				localPlAVData.at(avName).percentage = sd::GetActorValue(g_thePlayer, avName) / (localPlAVData[avName].base + localPlAVData[avName].modifier);
			}
			catch (...) {
				continue;
			}
		}
	});

	SAFE_CALL("LocalPlayer", [&] {
		auto result = AnimData_::UpdatePlayer();
		if (result != nullptr)
			pcAttacks.push(result);
	});

	SAFE_CALL("LocalPlayer", [&] {
		static bool calling = false;
		if (!calling)
		{
			calling = true;
			cd::GetCurrentCrosshairRef([](cd::Value<TESObjectREFR> ref) {
				localPlCrosshairRefUpdateMoment = clock();
				localPlCrosshairRef = ref.GetFormID();
				ci::SetTimer(200, [] {
					calling = false;
				});
			});
		}
	});

	SAFE_CALL("LocalPlayer", [&] {
		localPlEquippedAmmo = ::GetEquippedAmmo();
	});

	SAFE_CALL("LocalPlayer", [&] {
		for (int32_t i = 0; i != 2; ++i)
		{
			const bool isLeftHand = (i == 1);
			try {
				localPlEquippedSpells[isLeftHand] = knownSpells.at(sd::GetEquippedSpell(g_thePlayer, isLeftHand ? 0 : 1));
			}
			catch (...) {
				localPlEquippedSpells[isLeftHand] = nullptr;
			}
		}
	});

	SAFE_CALL("LocalPlayer", [&] {
		localPlEquippedArmor = {};
		for (int32_t i = 0; i != 2; ++i)
			localPlEquippedWeap[i] = nullptr;
		for (int32_t i = 0; i != 2; ++i)
		{
			try {
				localPlEquippedWeap[i] = knownItems.at(sd::GetEquippedWeapon(g_thePlayer, i != FALSE));
			}
			catch (...) {}
		}
		for (auto it = knownItems.begin(); it != knownItems.end(); ++it)
		{
			const auto &itemType = it->second;
			auto form = it->first;

			if (itemType->GetSubclass() == ItemType::Subclass::MISC_Torch
				&& itemType->GetClass() == ItemType::Class::Misc)
			{
				if (sd::IsEquipped(g_thePlayer, form))
				{
					localPlEquippedWeap[1] = itemType;
				}
			}
		}

		for (auto it = inventory.begin(); it != inventory.end(); ++it)
		{
			const auto formID = it->first ? it->first->GetFormID() : NULL;
			const auto form = LookupFormByID(formID);
			if (form)
			{
				if (sd::IsEquipped(g_thePlayer, form))
				{
					switch(form->formType)
					{
					case FormType::Armor:
						localPlEquippedArmor.push_back(it->first);
						break;
					}
				}
			}
		}

		enum {
			Left = 0
		};
		const auto type = sd::GetEquippedItemType(g_thePlayer, Left);
		enum {
			Torch = 11
		};
		if (type == Torch && sd::GetEquippedWeapon(g_thePlayer, true) == nullptr)
		{
			for (auto it = inventory.begin(); it != inventory.end(); ++it)
			{
				const auto formID = it->first ? it->first->GetFormID() : NULL;
				const auto form = LookupFormByID(formID);
				if (form && form->formType == FormType::Light)
				{
					localPlEquippedArmor.push_back(it->first);
					break;
				}
			}
		}
	});

	// Иногда вызывает вылет при выходе в главное меню
	// Практически бесполезная система. Только ширину шеи исправляет. Убрать в будущем.
	// UPD: Если слабый ПК, то внешность отображается неправильно, а эта штука всё исправляет.
	// UPD2: Добавлен вызов при переключении camera mode для исправления некорректной внешности
	// UPD3: Попробую раскомментить, а то какая-то х*йня творится с внешнностью иногда
	// UPD4: Сделаю, чтобы во время прыжка он это не выполнял
	static int32_t fixes = 0;
	if (fixes < 0)
		fixes = 0;
	SAFE_CALL("LocalPlayer", [&] { 
		const bool fp = PlayerCamera::GetSingleton()->IsFirstPerson();
		static bool fpWas = true;

		if (fpWas != fp)
		{
			if (!fp)
				timer = 0, --fixes;
			fpWas = fp;
		}
		if (timer < clock() && this->GetMovementData().jumpStage == ci::MovementData::JumpStage::Landed)
		{
			timer = clock() + 5000;
			auto ld = lastAppliedLook;
			auto ld2 = lookSync->GetFromPlayer(true);
			ld.tintmasks = ld2.tintmasks = {};
			if (ld != ld2 && ld.isEmpty() == false)
			{
				const int32_t maxFixes = 3;
				if (fixes < maxFixes)
				{
					++fixes;
					SET_TIMER(0, [] {
						lookSync->Apply(lastAppliedLook, g_thePlayer);
					});
				}
			}
		}
	});

	SAFE_CALL("LocalPlayer", [&] {
		auto movDataPtr = task.movDataPtr; // Делаем копию movDataPtr, ибо DoTeleport_OT() вызовет task = {} в случае удачного выполнения
		this->DoTeleport_OT(false);
		if (movDataPtr != nullptr)
			MovementData_::Apply(*movDataPtr, g_thePlayer, nullptr, {});
		task = {}; // Чтобы гарантированно убрать за собой
	});

	SAFE_CALL("LocalPlayer", [&] {
		auto cell = sd::GetParentCell(g_thePlayer);
		localPlCellID = (cell && sd::IsInterior(cell)) ? cell->formID : 0;
			
		auto worldSpace = sd::GetWorldSpace(g_thePlayer);
		localPlWorldSpaceID = worldSpace ? worldSpace->formID : 0;
	});

	SAFE_CALL("LocalPlayer", [&] {
		auto lookData = lookSync->GetFromPlayer();
		localPlLookData = lookData;
	});

	auto movData = MovementData_::GetFromPlayer();
	float kkk;
	if (movData.direction == 45 || movData.direction == 360 - 45)
		kkk = 1.4;
	else
		kkk = 1.21;
	static float defaultSpeedmultWas = 100.0;
	static float defaultSpeedmult = 100.0;
	static bool combPressedWas = 0;

	float newSpeedmult = defaultSpeedmult;
	if (movData.runMode == ci::MovementData::RunMode::Walking)
		newSpeedmult *= 0.80;
	else if (movData.direction % 90 != 0)
		newSpeedmult *= (movData.runMode == ci::MovementData::RunMode::Walking) ? 1.00 : kkk;

	const bool wasDiagonale = localPlMovementData.direction % 90 != 0,
		isDiagonale = movData.direction % 90 != 0;
	const bool isRunning = movData.runMode != ci::MovementData::RunMode::Walking,
		isRunningWas = localPlMovementData.runMode != ci::MovementData::RunMode::Walking;

	static float mod = 0.5;
	SAFE_CALL("LocalPlayer", [&] {
		std::array<bool, 5> pr = { 
			sd::GetKeyPressed('W'), 
			sd::GetKeyPressed('A'), 
			sd::GetKeyPressed('S'), 
			sd::GetKeyPressed('D'), 
			PlayerCamera::GetSingleton()->IsFirstPerson() 
		};
		static std::array<bool, 5> prWas;
		static clock_t lastSpeedmultUpdate1 = 0;
		if (pr != prWas && lastSpeedmultUpdate1 + 200 < clock())
		{
			lastSpeedmultUpdate1 = clock();
			sd::ForceActorValue(g_thePlayer, "Speedmult", newSpeedmult);
			sd::ModActorValue(g_thePlayer, "CarryWeight", mod = -mod);
			prWas = pr;
		}

		static clock_t lastSpeedmultUpdate = 0;
		if (lastSpeedmultUpdate + 166 < clock())
		{
			if (defaultSpeedmultWas != defaultSpeedmult || wasDiagonale != isDiagonale || isRunningWas != isRunning || movData.direction != localPlMovementData.direction)
			{
				lastSpeedmultUpdate = clock();
				defaultSpeedmultWas = defaultSpeedmult;
				if (abs(sd::GetActorValue(g_thePlayer, "Speedmult") - newSpeedmult) > 1.0)
				{
					sd::ForceActorValue(g_thePlayer, "Speedmult", newSpeedmult);
					sd::ModActorValue(g_thePlayer, "CarryWeight", mod = -mod);
				}
			}
		}
	});
	localPlMovementData = movData;

	SAFE_CALL("LocalPlayer", [&] {
		if (!registered)
		{
			AnimData_::Register();
			registered = true;
		}
	});

	SAFE_CALL("LocalPlayer", [&] {
		if (checkTeleport.IsEmpty() == false)
		{
			if (((cd::GetPosition(g_thePlayer) - checkTeleport.destPos).Length() > 128) && checkTeleport.lastReTP + 333 < clock())
			{
				checkTeleport.reTP();
				checkTeleport.lastReTP = clock();
			}
			else
				checkTeleport = {};
		}
	});

	SAFE_CALL("LocalPlayer", [&] {
		const bool dead = sd::IsDead(g_thePlayer);
		if (dead)
		{
			//PreventCrash((TESNPC *)g_thePlayer->baseForm);
			PreventCrash();
		}
	});

	lastLocalPlUpdate = clock();
}

void ci::LocalPlayer::Update_OT()
{
	std::lock_guard<dlf_mutex> l(localPlMutex);

	SAFE_CALL("LocalPlayer", [&] {
		static bool openWas = false;
		bool open = MenuManager::GetSingleton()->IsMenuOpen("Main Menu");
		if (openWas != open)
		{
			openWas = open;
			if (open)
				set = false;
		}
	});

	auto menuManager = MenuManager::GetSingleton();
	static bool openWas = true;
	static BSFixedString *fsMainMenuPtr = new BSFixedString("Main Menu");
	const bool open = menuManager->IsMenuOpen(*fsMainMenuPtr);
	static bool gameInited = false;

	if (open)
	{
		timer = clock() + 20000;
		gameInited = true;
	}

	if (openWas != open)
	{
		openWas = open;

		if (open)
			localPlCrosshairRef = NULL;

		if (!open)
		{
			dispell();
			this->RecoverAVs();
			this->RecoverInventory();
			this->RecoverSpells();
		}
	}

	bool isOpen = (menuManager->IsMenuOpen("Journal Menu"));
	static bool wasOpen = false;
	static clock_t c = 0;
	if (c + 200 < clock())
	{
		c = clock();
		if (isOpen != wasOpen)
		{
			wasOpen = isOpen;
			if (isOpen)
			{
				//LookData_::PreventCrash((TESNPC *)g_thePlayer->baseForm);
				PreventCrash();
			}
		}
	}

	// Fix Broken Khajiit Tail
	enum {
		KhajiitRace = 79685,
	};
	static bool madeKhajiit = false;
	if (gameInited && madeKhajiit == false)
	{
		if (g_thePlayer && g_thePlayer->GetActorBase() && LookupFormByID(KhajiitRace))
		{
			madeKhajiit = true;
			Sleep(100);
			g_thePlayer->GetActorBase()->race = (TESRace *)LookupFormByID(KhajiitRace);
		}
	}
}

uint32_t ci::LocalPlayer::GetRefID() const
{
	return g_thePlayer->GetFormID();
}