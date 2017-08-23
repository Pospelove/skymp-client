#include "../stdafx.h"
#include "CoreInterface.h"
#include "../Sync/LookData.h"
#include "../Sync/MovementData.h"

#pragma comment(lib, "winmm")

void MoveTo(uint32_t markerRefID)
{
	std::wostringstream os;
	os << L"player.moveto " << std::hex << markerRefID;
	ci::ExecuteConsoleCommand(os.str());
	DWORD dwVolume;

	if (waveOutGetVolume(NULL, &dwVolume) == MMSYSERR_NOERROR)
		waveOutSetVolume(NULL, 0); // mute volume

	Timer::Set(0, [=] {
		Timer::Set(1500, [=] {
			waveOutSetVolume(NULL, dwVolume ? dwVolume : 0xFFFF);
		});
	});
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
			Timer::Set(100, [=] {
				marker->parentCell = cellWas;
				marker->pos = posWas;
				marker->rot = rotWas;
			});
			return true;
		}
	}
	return false;
}

static std::recursive_mutex localPlMutex;
std::wstring *localPlName = nullptr;
UInt32 localPlCellID = 0, 
	localPlWorldSpaceID = 0;
ci::LookData localPlLookData = {};
ci::MovementData localPlMovementData = {};
clock_t lastLocalPlUpdate = 0;
bool registered = false;

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

void ci::LocalPlayer::SetName(const std::wstring &name)
{
	std::lock_guard<std::recursive_mutex> l(localPlMutex);
	if (!localPlName)
		localPlName = new std::wstring;
	*localPlName = name;

	Timer::Set(0, [=] {
		cd::SetDisplayName(g_thePlayer, WstringToString(name), true);
		cd::SetName(g_thePlayer->baseForm, WstringToString(name));
	});
}

void ci::LocalPlayer::DoTeleport_OT(bool ot)
{
	if (lastLocalPlUpdate + 100 < clock())
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

	//if (ot)
	//	Sleep(2000);
	//else
	//	sd::Wait(2000);
}

void ci::LocalPlayer::SetPos(NiPoint3 pos)
{	
	MovementData movData;
	movData.pos = pos;
	if(task.movDataPtr != nullptr)
		movData.angleZ = task.movDataPtr->angleZ;
	else
		movData.angleZ = this->GetMovementData().angleZ;
	this->ApplyMovementData(movData);
}

void ci::LocalPlayer::SetAngleZ(float angleZ)
{
	std::lock_guard<std::recursive_mutex> l(localPlMutex);
	if (lastLocalPlUpdate + 100 < clock())
	{
		if (task.movDataPtr != nullptr)
			task.movDataPtr->angleZ = (UInt16)angleZ;
		else
			task.angleZWaitingForSetPos = std::make_shared<UInt16>(angleZ);
	}
	else
	{
		Timer::Set(350, [=] {
			sd::SetAngle(g_thePlayer, 0, 0, angleZ);
		});
	}
	Timer::Set(0, [=] {
		sd::SetAngle(g_thePlayer, 0, 0, angleZ);
	});
}

void ci::LocalPlayer::ApplyMovementData(const ci::MovementData &movementData)
{
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

void ci::LocalPlayer::SetCell(uint32_t cellID)
{
	if (cellID == 0)
		return;
	std::lock_guard<std::recursive_mutex> l(localPlMutex);
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
		std::lock_guard<std::recursive_mutex> l(localPlMutex);
		task.markerIDs = markers;
	}
}

void ci::LocalPlayer::ApplyLookData(const LookData &lookData)
{
	Timer::Set(0, [=] {
		if (this->GetLookData() != lookData)
			LookData_::Apply(lookData, g_thePlayer);
	});
}

std::wstring ci::LocalPlayer::GetName() const
{
	std::lock_guard<std::recursive_mutex> l(localPlMutex);
	if (!localPlName)
	{
		static std::mutex nameMtx;
		std::lock_guard<std::mutex> l(nameMtx);
		return StringToWstring(g_thePlayer->GetReferenceName());
	}
	return *localPlName;
}

NiPoint3 ci::LocalPlayer::GetPos() const
{
	if (lastLocalPlUpdate + 100 < clock())
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
	std::lock_guard<std::recursive_mutex> l(localPlMutex);
	if (ci::IsInPause())
		localPlCellID = 0;
	return localPlCellID;
}

uint32_t ci::LocalPlayer::GetWorldSpace() const
{
	std::lock_guard<std::recursive_mutex> l(localPlMutex);
	if (ci::IsInPause())
		localPlWorldSpaceID = 0;
	return localPlWorldSpaceID;
}

ci::LookData ci::LocalPlayer::GetLookData() const
{
	std::lock_guard<std::recursive_mutex> l(localPlMutex);
	if (lastLocalPlUpdate + 100 < clock())
		localPlLookData = {};
	return localPlLookData;
}

ci::MovementData ci::LocalPlayer::GetMovementData() const
{
	std::lock_guard<std::recursive_mutex> l(localPlMutex);
	if (lastLocalPlUpdate + 100 < clock())
		localPlMovementData = {};
	return localPlMovementData;
}

void ci::LocalPlayer::Update()
{
	std::lock_guard<std::recursive_mutex> l(localPlMutex);

	auto movDataPtr = task.movDataPtr; // Делаем копию movDataPtr, ибо DoTeleport_OT() вызовет task = {} в случае удачного выполнения
	this->DoTeleport_OT(false);
	if (movDataPtr != nullptr)
		MovementData_::Apply(*movDataPtr, g_thePlayer, nullptr, {});
	task = {}; // Чтобы гарантированно убрать за собой

	auto cell = sd::GetParentCell(g_thePlayer);
	localPlCellID = (cell && sd::IsInterior(cell)) ? cell->formID : 0;

	auto worldSpace = sd::GetWorldSpace(g_thePlayer);
	localPlWorldSpaceID = worldSpace ? worldSpace->formID : 0;

	auto lookData = LookData_::GetFromPlayer();
	localPlLookData = lookData;

	auto movData = MovementData_::GetFromPlayer();
	float kkk;
	if (movData.direction == 45 || movData.direction == 360 - 45)
		kkk = 1.4;
	else
		kkk = 1.21;
	const float defaultSpeedmult = 100.0;
	float newSpeedmult = defaultSpeedmult;
	if (movData.runMode == ci::MovementData::RunMode::Walking)
		newSpeedmult *= 0.80;
	else if (movData.direction % 90 != 0)
		newSpeedmult *= (movData.runMode == ci::MovementData::RunMode::Walking) ? 1.00 : kkk;

	const bool wasDiagonale = localPlMovementData.direction % 90 != 0,
		isDiagonale = movData.direction % 90 != 0;
	if (wasDiagonale != isDiagonale || movData.runMode != localPlMovementData.runMode || movData.direction != localPlMovementData.direction)
	{
		sd::ForceActorValue(g_thePlayer, "Speedmult", newSpeedmult);
		static auto trash = LookupFormByID(0x000CC84D);
		sd::AddItem(g_thePlayer, trash, 1, 1);
		sd::RemoveItem(g_thePlayer, trash, 1, 1, nullptr);
	}
	localPlMovementData = movData;

	if (!registered)
	{
		cd::SendAnimationEvent(g_thePlayer, "Skymp_Register");
		registered = true;
	}

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

	lastLocalPlUpdate = clock();
}