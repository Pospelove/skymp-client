#include "../stdafx.h"
#include "CoreInterface.h"
#include "Skyrim/Events/ScriptEvent.h"

std::recursive_mutex gMutex;
std::set<ci::Object *> allObjects;

namespace ci
{
	float GetRespawnRadius(bool isInterior);
	extern FormID markerFormID;
}

void SetPositionSoftly(TESObjectREFR *ref, NiPoint3 pos)
{
	const float distance = (cd::GetPosition(ref) - pos).Length();
	if (distance > 4)
	{
		if (distance > 64)
			sd::SetPosition(ref, pos.x, pos.y, pos.z);
		else
			sd::TranslateTo(ref, pos.x, pos.y, pos.z, sd::GetAngleX(ref), sd::GetAngleY(ref), sd::GetAngleZ(ref), 1000.0, 0);
	}
}

void SetAngleSoftly(TESObjectREFR *ref, NiPoint3 rot)
{
	auto newRot = rot;
	while (newRot.x >= 360)
		newRot.x -= 360;
	while (newRot.y >= 360)
		newRot.y -= 360;
	while (newRot.z >= 360)
		newRot.z -= 360;

	NiPoint3 myRot{ sd::GetAngleX(ref), sd::GetAngleY(ref), sd::GetAngleZ(ref) };
	while (myRot.x >= 360)
		myRot.x -= 360;
	while (myRot.y >= 360)
		myRot.y -= 360;
	while (myRot.z >= 360)
		myRot.z -= 360;
	if (abs(myRot.x - newRot.x) > 5 || abs(myRot.y - newRot.y) > 5 || abs(myRot.z - newRot.z) > 5)
		sd::SetAngle(ref, newRot.x, newRot.y, newRot.z);
}

enum class OverrideChannel
{
	SetDestroyed,
	SetOpen,
	BlockActivation,
	SetPosition,
	SetAngle,
	SetName,
	Lock
};

struct ObjectTask
{
	std::function<void(TESObjectREFR *)> func;
};

struct GlobalTask
{
	std::function<bool()> func;
};

namespace ci
{
	std::vector<GlobalTask> globalTasks;
}

enum class SpawnStage
{
	NonSpawned,
	Spawning,
	Spawned,
	Persistent
};

class ActivateEventSink;

struct ci::Object::Impl
{
	std::recursive_mutex mutex;
	uint32_t refID = 0;
	uint32_t baseID = 0;

	uint32_t locationID = 0;
	NiPoint3 pos, rot;
	std::wstring name;
	bool destroyed = true, open = true, blockedActivation = true;
	std::shared_ptr<NiPoint3> editorPos, editorRot;

	bool isNative = false;
	bool waitingForDespawn = false;
	SpawnStage spawnStage;
	std::vector<ObjectTask> simpleTasks;
	std::map<OverrideChannel, ObjectTask> overridableTasks;

	std::function<void(Object *, bool)> onActivate;

	std::unique_ptr<ActivateEventSink> activateEventSink;
};

class ActivateEventSink : public BSTEventSink<TESActivateEvent>
{
public:
	ActivateEventSink(ci::Object *owner) : owner(owner)
	{
	}

	virtual ~ActivateEventSink() override
	{
	}

	virtual	EventResult	ReceiveEvent(TESActivateEvent *evn, BSTEventSource<TESActivateEvent> * source) override
	{
		if (evn->caster == g_thePlayer)
		{
			std::lock_guard<std::recursive_mutex> l(gMutex);
			if (allObjects.find(owner) == allObjects.end())
				return EventResult::kEvent_Continue;
			if (owner->pimpl->refID != evn->target->formID)
				return EventResult::kEvent_Continue;
			std::lock_guard<std::recursive_mutex> l1(owner->pimpl->mutex);
			switch (evn->target->baseForm->formType)
			{
			case FormType::Door:
			{
				bool isOpening = false;
				const int32_t state = sd::GetOpenState(evn->target);
				if (state == 3)
					isOpening = true;
				switch (evn->target->baseForm->formID)
				{
				case 0x00031897:
				case 0x000351EB:
				case 0x000180D8:
					isOpening = true;
					break;
				}
				if (owner->pimpl->onActivate != nullptr)
				{
					owner->pimpl->onActivate(owner, isOpening);
				}
				break;
			}
			}
		}
		return EventResult::kEvent_Continue;
	}

private:
	ci::Object *const owner;
};

ci::Object::Object(uint32_t existingReferenceID, uint32_t baseID, uint32_t locationID, NiPoint3 pos, NiPoint3 rot, std::function<void(Object *, bool)> onActivate) : pimpl(new Impl)
{
	std::lock_guard<std::recursive_mutex> l(gMutex);
	allObjects.insert(this);

	std::lock_guard<std::recursive_mutex> l1(pimpl->mutex);
	pimpl->refID = existingReferenceID;
	pimpl->baseID = baseID;
	pimpl->isNative = (existingReferenceID != 0);
	pimpl->locationID = locationID;
	pimpl->pos = pos;
	pimpl->rot = rot;
	pimpl->onActivate = onActivate;
	this->SetPosition(pos);
	this->SetAngle(rot);
	this->Lock(false);

	WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, true);

	this->SetDestroyed(false);
	this->SetOpen(false);
	this->BlockActivation(true);

	if (pimpl->isNative)
		pimpl->spawnStage = SpawnStage::Persistent;
	else
		pimpl->spawnStage = SpawnStage::NonSpawned;

	pimpl->activateEventSink = std::make_unique<ActivateEventSink>(this);
	g_activateEventSource.AddEventSink(pimpl->activateEventSink.get());
}

ci::Object::~Object()
{
	std::lock_guard<std::recursive_mutex> l(gMutex);
	allObjects.erase(this);
	
	if (pimpl->isNative && pimpl->editorPos != nullptr && pimpl->editorRot != nullptr)
	{
		const auto refID = pimpl->refID;
		const auto editorPos = *pimpl->editorPos;
		const auto editorRot = *pimpl->editorRot;
		globalTasks.push_back(GlobalTask{ [refID, editorPos, editorRot] {
			auto ref = (TESObjectREFR *)LookupFormByID(refID);
			if (ref && ref->formType == FormType::Reference && sd::GetParentCell(ref) != nullptr)
			{
				SetPositionSoftly(ref, editorPos);
				SetAngleSoftly(ref, editorRot);
				return true;
			}
			return false;
		} });
	}

	g_activateEventSource.RemoveEventSink(pimpl->activateEventSink.get());
		
	delete pimpl;

	WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, false);
}

void ci::Object::SetDestroyed(bool destroyed)
{
	std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

	auto task = ObjectTask{[=](TESObjectREFR *ref) {
		sd::SetDestroyed(ref, destroyed);
	}};
	pimpl->overridableTasks[OverrideChannel::SetDestroyed] = task;
	pimpl->destroyed = destroyed;
}

void ci::Object::SetOpen(bool open)
{
	std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		const bool openNow = sd::GetOpenState(ref) == 1 || sd::GetOpenState(ref) == 2;
		if (openNow != open)
			sd::SetOpen(ref, open);
	} };
	pimpl->overridableTasks[OverrideChannel::SetOpen] = task;
	pimpl->open = open;
}

void ci::Object::BlockActivation(bool block)
{
	std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		sd::BlockActivation(ref, block);
	} };
	pimpl->overridableTasks[OverrideChannel::BlockActivation] = task;
	pimpl->blockedActivation = block;
}

void ci::Object::SetPosition(NiPoint3 pos)
{
	std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		SetPositionSoftly(ref, pos);
	} };
	pimpl->overridableTasks[OverrideChannel::SetPosition] = task;
	pimpl->pos = pos;
}

void ci::Object::SetAngle(NiPoint3 rot)
{
	std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		SetAngleSoftly(ref, rot);
	} };
	pimpl->overridableTasks[OverrideChannel::SetAngle] = task;
	pimpl->rot = rot;
}

void ci::Object::SetName(const std::wstring &name)
{
	std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		BSFixedString fsName = WstringToString(name).data();
		ref->SetDisplayName(fsName);
	} };
	pimpl->overridableTasks[OverrideChannel::SetName] = task;
	pimpl->name = name;
}

bool ci::Object::SetLocation(uint32_t locationID)
{
	if (pimpl->isNative)
		return false;
	pimpl->locationID = locationID;
	return true;
}

void ci::Object::Lock(bool lock)
{
	std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		sd::Lock(ref, lock, false);
	} };
	pimpl->overridableTasks[OverrideChannel::Lock] = task;
}

void ci::Object::Update()
{
	std::lock_guard<std::recursive_mutex> l1(pimpl->mutex);

	TESObjectCELL *const localPlCell = sd::GetParentCell(g_thePlayer);
	const bool isInterior = localPlCell ? sd::IsInterior(localPlCell) : false;
	const FormID worldSpaceID = sd::GetWorldSpace(g_thePlayer) ? sd::GetWorldSpace(g_thePlayer)->formID : NULL;

	switch (pimpl->spawnStage)
	{
	case SpawnStage::NonSpawned:
	{
		if (markerFormID && localPlCell
			&& NiPoint3{ pimpl->pos - cd::GetPosition(g_thePlayer) }.Length() < GetRespawnRadius(isInterior)
			&& (pimpl->locationID == worldSpaceID || pimpl->locationID == localPlCell->formID))
		{
			this->ForceSpawn();
		}
		break;
	}
	case SpawnStage::Spawned:
	{
		auto ref = (TESObjectREFR *)LookupFormByID(pimpl->refID);
		if (!ref)
			return this->ForceDespawn("Unloaded by game");

		if (NiPoint3{ pimpl->pos - cd::GetPosition(g_thePlayer) }.Length() >= GetRespawnRadius(isInterior))
			return this->ForceDespawn("Object is too far");

		if (pimpl->locationID != worldSpaceID && pimpl->locationID != localPlCell->formID)
			return this->ForceDespawn("Wrong Cell/WorldSpace");
	}
	case SpawnStage::Persistent:
	{
		pimpl->waitingForDespawn = false;
		auto ref = (TESObjectREFR *)LookupFormByID(pimpl->refID);
		if (ref && ref->formType == FormType::Reference)
		{
			if (abs(pimpl->rot.z - sd::GetAngleZ(ref)) > 7.5)
				sd::SetAngle(ref, pimpl->rot.x, pimpl->rot.y, pimpl->rot.z);

			if (pimpl->editorPos == nullptr || pimpl->editorRot == nullptr)
			{
				pimpl->editorPos = std::make_shared<NiPoint3>(cd::GetPosition(ref));
				pimpl->editorRot = std::make_shared<NiPoint3>(NiPoint3{sd::GetAngleX(ref), sd::GetAngleY(ref), sd::GetAngleZ(ref)});
			}

			std::for_each(pimpl->simpleTasks.begin(), pimpl->simpleTasks.end(), [=](ObjectTask &task) {
				task.func(ref);
			});
			pimpl->simpleTasks = {};

			for (auto it = pimpl->overridableTasks.begin(); it != pimpl->overridableTasks.end(); ++it)
			{
				const auto &task = it->second;
				task.func(ref);
			}
			pimpl->overridableTasks.clear();
		}
		break;
	}
	}
}

void ci::Object::Update_OT()
{
	std::lock_guard<std::recursive_mutex> l1(pimpl->mutex);

	if (MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
	{
		if (!pimpl->waitingForDespawn)
		{
			pimpl->waitingForDespawn = true;
			Timer::Set(0, [=] {
				Timer::Set(3000, [=] {
					std::lock_guard<std::recursive_mutex> l(gMutex);
					if (allObjects.find(this) == allObjects.end())
						return;
					this->ForceDespawn("Geme reinit");
				});
			});
		}
	}
}

void ci::Object::UpdateAll()
{
	std::lock_guard<std::recursive_mutex> l(gMutex);
	std::for_each(allObjects.begin(), allObjects.end(), [] (ci::Object *object){
		object->Update();
	});

	for (auto it = globalTasks.begin(); it != globalTasks.end(); ++it)
	{
		if (it->func != nullptr && it->func())
			it->func = nullptr;
	}
	auto pred = [](const GlobalTask &task) {
		return task.func == nullptr;
	};
	globalTasks.erase(
		std::remove_if(globalTasks.begin(), globalTasks.end(), pred),
		globalTasks.end()
	);

}

void ci::Object::UpdateAll_OT()
{
	std::lock_guard<std::recursive_mutex> l(gMutex);
	std::for_each(allObjects.begin(), allObjects.end(), [](ci::Object *object) {
		object->Update_OT();
	});
}

void ci::Object::ForceSpawn()
{
	std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
	if (pimpl->spawnStage == SpawnStage::NonSpawned)
	{
		pimpl->spawnStage = SpawnStage::Spawning;
		Timer::Set(100, [=] {
			std::lock_guard<std::recursive_mutex> l(gMutex);
			if (allObjects.find(this) == allObjects.end())
				return;

			auto at = (TESObjectREFR *)LookupFormByID(markerFormID);
			if (!at)
				at = g_thePlayer;
			auto baseForm = LookupFormByID(pimpl->baseID);
			if (!baseForm)
			{
				this->Warning("Incorrect base form ID");
				baseForm = LookupFormByID(0x00031897);
			}
			auto ref = sd::PlaceAtMe(at, baseForm, 1, true, false);
			if (!ref)
			{
				this->Warning("Unable to place");
				return;
			}
			pimpl->refID = ref->formID;
			WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, true);

			//sd::SetPosition(ref, pimpl->pos.x, pimpl->pos.y, pimpl->pos.z);
			//sd::SetAngle(ref, pimpl->rot.x, pimpl->rot.y, pimpl->rot.z);

			this->SetPosition(pimpl->pos);
			this->SetAngle(pimpl->rot);
			this->SetDestroyed(pimpl->destroyed);
			if (pimpl->name.size() > 0)
				this->SetName(pimpl->name);
			this->SetOpen(pimpl->open);
			this->BlockActivation(pimpl->blockedActivation);

			pimpl->spawnStage = SpawnStage::Spawned;
		});
	}
}

void ci::Object::ForceDespawn(const std::string &r)
{
	std::lock_guard<std::recursive_mutex> l(pimpl->mutex);
	if (pimpl->spawnStage == SpawnStage::Spawned)
	{
		WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, false);
		pimpl->refID = 0;
		pimpl->spawnStage = SpawnStage::NonSpawned;
	}
}

void ci::Object::Warning(const char *warning)
{
	const std::string txt = warning;
	Timer::Set(0, [txt] {
		sd::PrintNote("%s", txt.data());
	});
}