#include "../stdafx.h"
#include "CoreInterface.h"
#include "../Sync/LookData.h"
#include "../Sync/MovementData.h"
#include "../Sync/HitData.h"
#include "Skyrim/Events/ScriptEvent.h"
#include "Skyrim/Camera/PlayerCamera.h"
#include <queue>

#pragma comment(lib, "winmm")

#define DISABLE_PLAYER_DAMAGE			FALSE
#define NORMAL_PROCESSING_WITH_MENUS	TRUE
#define MUTE_SOUND_ON_TP				TRUE

void PreventCrash() {
	g_thePlayer->GetActorBase()->numHeadParts = 0;
	g_thePlayer->GetActorBase()->headparts = nullptr;
}

void MoveTo(uint32_t markerRefID)
{
	std::wostringstream os;
	os << L"player.moveto " << std::hex << markerRefID;
	ci::ExecuteCommand(ci::CommandType::Console, os.str());

	if (MUTE_SOUND_ON_TP != FALSE)
	{
		DWORD dwVolume;
		if (waveOutGetVolume(NULL, &dwVolume) == MMSYSERR_NOERROR)
			waveOutSetVolume(NULL, 0); // mute volume

		SET_TIMER(0, [=] {
			SET_TIMER(1500, [=] {
				waveOutSetVolume(NULL, dwVolume ? dwVolume : 0xFFFF);
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

dlf_mutex localPlMutex;
std::wstring *localPlName = nullptr;
UInt32 localPlCellID = 0,
localPlWorldSpaceID = 0;
ci::LookData localPlLookData = {};
ci::MovementData localPlMovementData = {};
uint32_t localPlCrosshairRef = 0;
std::vector<const ci::ItemType *> localPlEquippedArmor = {};
const ci::ItemType *localPlEquippedWeap[] = { nullptr, nullptr };
const ci::ItemType *localPlEquippedAmmo = nullptr;
clock_t lastLocalPlUpdate = 0;
bool registered = false;

std::map<const ci::ItemType *, uint32_t> inventory;
std::map<TESForm *, const ci::ItemType *> knownItems;

const ci::ItemType *GetEquippedAmmo() 
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
			return itemType;
		}
	};
	return nullptr;
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
		}).detach();
	}
};

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

	virtual	EventResult	ReceiveEvent(TESContainerChangedEvent *evn, BSTEventSource<TESContainerChangedEvent> * source) override
	{
		if (evn->from == g_thePlayer->formID)
			if (evn->to == 0)
			{
				std::lock_guard<std::mutex> l(this->removesToIgnoreMutex);
				if (this->removesToIgnore > 0)
					this->removesToIgnore--;
				else
				{
					WorldCleaner::GetSingleton()->SetFormProtected(0, true);
					WorldCleaner::GetSingleton()->SetFormProtected(0, false);
					auto count = evn->count;
					auto form = LookupFormByID(evn->item);
					SET_TIMER(0, [=] {
						try {
							auto itemType = knownItems.at(form);
							CIAccess().OnItemDropped(itemType, count);

							std::lock_guard<dlf_mutex> l(localPlMutex);
							if (inventory[itemType] > count)
								inventory[itemType] -= count;
							else
								inventory.erase(itemType);
						}
						catch (...) {
						}
					});
				}
			}
		return EventResult::kEvent_Continue;
	}

private:
	ContainerChangedEventSink()
	{
	}

	int32_t removesToIgnore = 0;
	std::mutex removesToIgnoreMutex;
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
					sd::Activate(ref, g_thePlayer, true);
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
	if (item && count)
	{
		inventory[item] += count;

		auto form = LookupFormByID(item->GetFormID());

		SET_TIMER(0, [=] {
			knownItems[form] = item;
		});

		if (form && !MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
		{
			SET_TIMER(0, [=] {
				sd::AddItem(g_thePlayer, form, count, silent);
			});
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
	SET_TIMER(0, [=] {
		auto form = LookupFormByID(item->GetFormID());
		if (form)
		{
			if (sd::GetItemCount(g_thePlayer, form) == 0)
			{
				//ci::Chat::AddMessage(L"EquipItem forced AddItem");
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


void ci::LocalPlayer::PlayHitAnimation(uint8_t hitAnimID)
{
	std::lock_guard<dlf_mutex> l(localPlMutex);
	SET_TIMER(0, [=] {
		HitData_::Apply(g_thePlayer, hitAnimID, true);
	});
}

std::map<std::string, ci::AVData> localPlAVData;

void ci::LocalPlayer::UpdateAVData(const std::string &avName_, const AVData &avData)
{
	if (avName_.empty())
		return;
	if (avName_[0] == '_')
	{
		SET_TIMER(0, [=] {
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

	SET_TIMER(0, [=] {
		sd::SetActorValue(g_thePlayer, (char *)avName.data(), base + modifier);
	});
	/*const auto modOld = data[avName].modifier;
	if (modOld != 0)
	{
		SET_TIMER(0, [=] {
			sd::ModActorValue(g_thePlayer, (char *)avName.data(), modifier - modOld);
		});
	}*/

	SET_TIMER(0, [=] {
		auto cstr = (char *)avName.data();
		auto val = sd::GetActorValue(g_thePlayer, cstr);
		auto dest = val / sd::GetActorValuePercentage(g_thePlayer, cstr) * percentage;
		auto toRestore = dest - val;
		if (toRestore > 0)
			sd::RestoreActorValue(g_thePlayer, cstr, toRestore);
		else
			sd::DamageActorValue(g_thePlayer, cstr, -toRestore);

	});

	data[avName] = { base, modifier, percentage };
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
		if (this->GetLookData() != lookData)
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
	if (NORMAL_PROCESSING_WITH_MENUS != FALSE)
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

std::queue<std::shared_ptr<uint8_t>> pcAttacks;

std::shared_ptr<uint8_t> ci::LocalPlayer::GetNextHitAnim()
{
	if (pcAttacks.empty())
		return nullptr;
	auto result = pcAttacks.front();
	pcAttacks.pop();
	return result;
}

clock_t timer = clock() + 5000;

void ci::LocalPlayer::Update()
{
	std::lock_guard<dlf_mutex> l(localPlMutex);

	SAFE_CALL("LocalPlayer", [&] {
		static bool set = false;
		if (!set)
		{
			SET_TIMER(0, [] {
				sd::SetActorValue(g_thePlayer, "HealRate", 0);
				sd::SetActorValue(g_thePlayer, "MagickaRate", 0);
				sd::SetActorValue(g_thePlayer, "StaminaRate", 0);
				cd::SetGameSettingFloat("fSprintStaminaDrainMult", 0.0f);
				cd::SetGameSettingFloat("fDifficultyDamageMultiplier", 0.0f);
				if (DISABLE_PLAYER_DAMAGE)
					sd::SetActorValue(g_thePlayer, "attackdamagemult", 0.0);
				cd::SetGameSettingFloat("fVATSSkillFactor", 0.0f);
				cd::SetGameSettingFloat("fVATSChanceHitMult", 0.0f);
				cd::SetGameSettingFloat("fVATSMaxChance", 0.0f);
				cd::SetGameSettingFloat("fPowerAttackDefaultBonus", 0.0f);
			});
			set = true;
		}
	});
	
	SAFE_CALL("LocalPlayer", [&] {
		if (DISABLE_PLAYER_DAMAGE)
		{
			const bool cursorOpen = MenuManager::GetSingleton()->IsMenuOpen("Cursor Menu");
			static bool cursorOpenWas = false;
			if (cursorOpen != cursorOpenWas)
			{
				cursorOpenWas = cursorOpen;
				sd::SetActorValue(g_thePlayer, "attackdamagemult", cursorOpen ? 1.0 : 0.0);
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
		auto result = HitData_::UpdatePlayer();
		if (result != nullptr)
			pcAttacks.push(result);
	});

	SAFE_CALL("LocalPlayer", [&] {
		static bool calling = false;
		if (!calling)
		{
			calling = true;
			cd::GetCurrentCrosshairRef([](cd::Value<TESObjectREFR> ref) {
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
		localPlEquippedArmor = {};
		enum {
			hands = 2
		};
		for (int32_t i = 0; i != hands; ++i)
			localPlEquippedWeap[i] = nullptr;
		for (int32_t i = 0; i != hands; ++i)
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
	});

	// ������ �������� ����� ��� ������ � ������� ����
	// ����������� ����������� �������. ������ ������ ��� ����������. ������ � �������.
	// UPD: ���� ������ ��, �� ��������� ������������ �����������, � ��� ����� �� ����������.
	// UPD2: �������� ����� ��� ������������ camera mode ��� ����������� ������������ ���������
	static int32_t fixes = 0;
	if (fixes < 0)
		fixes = 0;
	SAFE_CALL("LocalPlayer", [&] { 
		/*const bool fp = PlayerCamera::GetSingleton()->IsFirstPerson();
		static bool fpWas = true;

		if (fpWas != fp)
		{
			if (!fp)
				timer = 0, --fixes;
			fpWas = fp;
		}
		}
		if (timer < clock())
		{
			timer = clock() + 5000;
			auto ld = lastAppliedLook;
			auto ld2 = LookData_::GetFromPlayer(true);
			ld.tintmasks = ld2.tintmasks = {};
			if (ld != ld2 && ld.isEmpty() == false)
			{
				const int32_t maxFixes = 3;
				if (fixes < maxFixes)
				{
					++fixes;
					SET_TIMER(0, [] {
						LookData_::Apply(lastAppliedLook, g_thePlayer);
					});
				}
			}
		}*/
	});

	SAFE_CALL("LocalPlayer", [&] {
		auto movDataPtr = task.movDataPtr; // ������ ����� movDataPtr, ��� DoTeleport_OT() ������� task = {} � ������ �������� ����������
		this->DoTeleport_OT(false);
		if (movDataPtr != nullptr)
			MovementData_::Apply(*movDataPtr, g_thePlayer, nullptr, {});
		task = {}; // ����� �������������� ������ �� �����
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

	/*auto combPressed = sd::GetKeyPressed(0x31) && sd::GetKeyPressed(0x32) && sd::GetKeyPressed(0x33);
	if (combPressed != combPressedWas)
	{
		combPressedWas = combPressed;
		if (combPressed)
		{
			if (defaultSpeedmult == 100)
			{
				sd::SetGodMode(true);
				defaultSpeedmult = 1000;
			}
			else if (defaultSpeedmult == 1000)
			{
				defaultSpeedmult = 100;
				sd::SetGodMode(false);
			}
		}
	}

	{
		auto combPressed = sd::GetKeyPressed(0x34);
		static bool combPressedWas = false;
		if (combPressed != combPressedWas)
		{
			combPressedWas = combPressed;
			if (combPressed)
			{
				sd::ExecuteConsoleCommand("tcl", nullptr);
			}
		}
	}

	{
		auto combPressed = sd::GetKeyPressed(0x35);
		if (combPressed)
		{
			Sleep(5000);
		}
	}
	*/

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
			cd::SendAnimationEvent(g_thePlayer, "Skymp_Register");
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
		{
			localPlCrosshairRef = NULL;
		}

		if (!open)
		{
			static uint8_t numIgnores = 1; // PVS
			if (numIgnores != 0)
				--numIgnores;
			else
			{
				SET_TIMER(1300, [this] {
					for (auto it = localPlAVData.begin(); it != localPlAVData.end(); ++it)
						this->UpdateAVData(it->first, it->second);
				});
			}

			SET_TIMER(0, [] {
				for (auto it = inventory.begin(); it != inventory.end(); ++it)
				{
					auto form = LookupFormByID(it->first ? it->first->GetFormID() : NULL);
					if (form)
						sd::AddItem(g_thePlayer, form, it->second, true);
				}
			});
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