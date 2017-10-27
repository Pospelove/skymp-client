#include "../stdafx.h"
#include "CoreInterface.h"
#include "Skyrim/Events/ScriptEvent.h"
#include "SKSE/SKSEEvents.h"
#include "SKSE/PluginAPI.h"

#define SAFE_ITEM_REMOVE				FALSE

dlf_mutex gMutex;
std::set<ci::Object *> allObjects;

extern std::map<const ci::ItemType *, uint32_t> inventory;
extern std::map<TESForm *, const ci::ItemType *> knownItems;
extern ErrorHandling::DeadlockFreeMutex<75U> localPlMutex;

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
	Lock,
	SetMotionType,
	SetCount,
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

struct ci::Object::Impl
{
	class ActivateEventSink;
	class ContainerChangedEventSink;

	dlf_mutex mutex;
	uint32_t refID = 0;
	uint32_t baseID = 0;

	uint32_t locationID = 0;
	NiPoint3 pos, rot, posData;
	std::wstring name;
	bool destroyed = true, open = true, blockedActivation = true, grabbed = false;
	int32_t motionType = Motion_Keyframed;
	bool isDisabled = false;
	uint32_t count = 1;
	uint32_t lastCount = 1;
	std::shared_ptr<NiPoint3> editorPos, editorRot;
	clock_t lastUpdate = 0;
	uint32_t updateRate = 100;
	float speed = 0;
	clock_t every1000ms = 0;
	NiPoint3 lastPos = { 0,0,0 };

	bool isNative = false;
	bool waitingForDespawn = false;
	SpawnStage spawnStage;
	std::vector<ObjectTask> simpleTasks;
	std::vector<ObjectTask> inventoryTasks;
	std::map<OverrideChannel, ObjectTask> overridableTasks;

	OnActivate onActivate;
	OnContainerChanged onConatinerChanged;
	OnHit onHit;

	std::unique_ptr<ActivateEventSink> activateEventSink;
	std::unique_ptr<ContainerChangedEventSink> containerChangedEventSink;

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
				std::lock_guard<dlf_mutex> l(gMutex);
				if (allObjects.find(owner) == allObjects.end())
					return EventResult::kEvent_Continue;
				if (owner->pimpl->refID != evn->target->formID)
					return EventResult::kEvent_Continue;
				std::lock_guard<dlf_mutex> l1(owner->pimpl->mutex);

				bool isOpening = false;

				switch (evn->target->baseForm->formType)
				{
				case FormType::Door:
				{
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
					break;
				}
				case FormType::Container:
				{
					isOpening = true;
					auto onActivate = owner->pimpl->onActivate;
					auto resendOnActivate = [=] {
						SET_TIMER(0, [=] {
							onActivate(false);
						});
					};

					std::thread([=] {
						Sleep(1000);
						while (MenuManager::GetSingleton()->IsMenuOpen("ContainerMenu"))
							Sleep(100);
						resendOnActivate();
					}).detach();
					break;
				}
				}

				if (owner->pimpl->onActivate != nullptr)
				{
					auto onActivate = owner->pimpl->onActivate;
					auto refID = owner->pimpl->refID;
					owner->UpdateContainer();
					std::thread([=] {
						onActivate(isOpening);
					}).detach();
				}
			}
			return EventResult::kEvent_Continue;
		}

	private:
		ci::Object *const owner;
	};

	class ContainerChangedEventSink : public BSTEventSink<TESContainerChangedEvent>
	{
	public:
		ContainerChangedEventSink(ci::Object *owner) : owner(owner)
		{
		}

		virtual ~ContainerChangedEventSink() override
		{
		}

		virtual	EventResult	ReceiveEvent(TESContainerChangedEvent *evn, BSTEventSource<TESContainerChangedEvent> * source) override
		{
			std::lock_guard<dlf_mutex> l(gMutex);
			auto it = allObjects.find(owner);
			if (it != allObjects.end())
			{
				std::lock_guard<dlf_mutex> l1(owner->pimpl->mutex);

				auto form = LookupFormByID(evn->item);
				auto count = evn->count;
				if (form)
				{
					if (evn->from == g_thePlayer->formID && evn->to == owner->pimpl->refID) //put
					{
						auto onConatinerChanged = owner->pimpl->onConatinerChanged;
						SET_TIMER(0, [=] {
							try {
								auto itemType = knownItems.at(form);
								std::thread([=] {
									onConatinerChanged(itemType, count, true);
									std::lock_guard<dlf_mutex> l(localPlMutex);
									if (inventory[itemType] > count)
										inventory[itemType] -= count;
									else
										inventory.erase(itemType);
								}).detach();
							}
							catch (...) {
							}
						});

						ObjectTask task{ [=](TESObjectREFR *ref) {
							if (form)
							{
								cd::AddItem(ref, form, count, true);
							}
						} };
						owner->pimpl->inventoryTasks.push_back(task);
					}

					if (evn->from == owner->pimpl->refID && evn->to == g_thePlayer->formID) //take
					{
						auto onConatinerChanged = owner->pimpl->onConatinerChanged;
						SET_TIMER(0, [=] {
							try {
								auto itemType = knownItems.at(form);
								std::thread([=] {
									onConatinerChanged(itemType, count, false);
									std::lock_guard<dlf_mutex> l(localPlMutex);
									inventory[itemType] += count;
								}).detach();
							}
							catch (...) {
							}
						});

						ObjectTask task{ [=](TESObjectREFR *ref) {
							if (form)
							{
								if (SAFE_ITEM_REMOVE != FALSE)
									cd::RemoveItem(ref, form, count, true, nullptr);
								else
									sd::RemoveItem(ref, form, count, true, nullptr);
							}
						} };
						owner->pimpl->inventoryTasks.push_back(task);
					}
				}
			}
			return EventResult::kEvent_Continue;
		}
	private:
		ci::Object *const owner;
	};

	class HitEventSinkGlobal : public BSTEventSink<TESHitEvent>
	{
	public:
		HitEventSinkGlobal()
		{
			g_hitEventSource.AddEventSink(this);
		}

		virtual ~HitEventSinkGlobal() override
		{
			g_hitEventSource.RemoveEventSink(this);
		}

		virtual	EventResult	ReceiveEvent(TESHitEvent *evn, BSTEventSource<TESHitEvent> * source) override
		{
			if (evn->caster != g_thePlayer || evn->target == nullptr || evn->projectileFormID != NULL)
				return EventResult::kEvent_Continue;
			
			const ci::ItemType *weapon = nullptr;
			auto sourceForm = LookupFormByID(evn->sourceFormID);
			if (sourceForm != nullptr)
			{
				try {
					weapon = knownItems.at(sourceForm);
				}
				catch (...) {
				}
			}

			const uint32_t targetID = evn->target->formID;
			const HitEventData hitEventData{ evn->flags.powerAttack, weapon};
			std::thread([=] {
				std::lock_guard<dlf_mutex> l(gMutex);
				for (auto it = allObjects.begin(); it != allObjects.end(); ++it)
				{
					auto obj = *it;
					if (obj == nullptr)
						continue;
					std::lock_guard<dlf_mutex> l1(obj->pimpl->mutex);
					auto ref = (TESObjectREFR *)LookupFormByID(obj->pimpl->refID);
					if (!ref || ref->formType != FormType::Reference)
						continue;
					if (ref->formID != targetID)
						continue;

					auto onHit = obj->pimpl->onHit;
					if (onHit)
					{
						std::thread([=] {
							onHit(hitEventData);
						}).detach();
					}
				}
			}).detach();
			return EventResult::kEvent_Continue;
		}
	};
};

ci::Object::Object(uint32_t existingReferenceID, 
	uint32_t baseID, 
	uint32_t locationID, 
	NiPoint3 pos, 
	NiPoint3 rot, 
	OnActivate onActivate, 
	OnContainerChanged onContainerChanged, 
	OnHit onHit) : 
	pimpl(new Impl)
{
	static auto hitEventSink = new Impl::HitEventSinkGlobal;

	std::lock_guard<dlf_mutex> l(gMutex);
	allObjects.insert(this);

	std::lock_guard<dlf_mutex> l1(pimpl->mutex);
	pimpl->refID = existingReferenceID;
	pimpl->baseID = baseID;
	pimpl->isNative = (existingReferenceID != 0);
	pimpl->locationID = locationID;
	pimpl->pos = pos;
	pimpl->rot = rot;

	pimpl->onActivate = onActivate;
	pimpl->onConatinerChanged = onContainerChanged;
	pimpl->onHit = onHit;

	this->SetPosition(pos);
	this->SetAngle(rot);
	this->Lock(false);
	this->SetMotionType(Motion_Keyframed);

	WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, true);

	this->SetDestroyed(false);
	this->SetOpen(false);
	this->BlockActivation(true);

	if (pimpl->isNative)
		pimpl->spawnStage = SpawnStage::Persistent;
	else
		pimpl->spawnStage = SpawnStage::NonSpawned;

	pimpl->activateEventSink = std::make_unique<Impl::ActivateEventSink>(this);
	g_activateEventSource.AddEventSink(pimpl->activateEventSink.get());

	pimpl->containerChangedEventSink = std::make_unique<Impl::ContainerChangedEventSink>(this);
	g_containerChangedEventSource.AddEventSink(pimpl->containerChangedEventSink.get());
}

ci::Object::~Object()
{
	std::lock_guard<dlf_mutex> l(gMutex);
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
	g_containerChangedEventSource.RemoveEventSink(pimpl->containerChangedEventSink.get());

	delete pimpl;

	WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, false);
}

void ci::Object::SetDestroyed(bool destroyed)
{
	std::lock_guard<dlf_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		sd::SetDestroyed(ref, destroyed);
	} };
	pimpl->overridableTasks[OverrideChannel::SetDestroyed] = task;
	pimpl->destroyed = destroyed;
}

void ci::Object::SetOpen(bool open)
{
	std::lock_guard<dlf_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		const bool openNow = sd::GetOpenState(ref) == 1 || sd::GetOpenState(ref) == 2;
		if (ref->formType != FormType::Door || openNow != open)
			sd::SetOpen(ref, open);
	} };
	pimpl->overridableTasks[OverrideChannel::SetOpen] = task;
	pimpl->open = open;
}

void ci::Object::BlockActivation(bool block)
{
	std::lock_guard<dlf_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		sd::BlockActivation(ref, block);
	} };
	pimpl->overridableTasks[OverrideChannel::BlockActivation] = task;
	pimpl->blockedActivation = block;
}

void ci::Object::SetPosition(NiPoint3 pos)
{
	std::lock_guard<dlf_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		SetPositionSoftly(ref, pos);
	} };
	pimpl->overridableTasks[OverrideChannel::SetPosition] = task;
	pimpl->pos = pos;
}

void ci::Object::SetAngle(NiPoint3 rot)
{
	std::lock_guard<dlf_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		SetAngleSoftly(ref, rot);
	} };
	pimpl->overridableTasks[OverrideChannel::SetAngle] = task;
	pimpl->rot = rot;
}

void ci::Object::TranslateTo(NiPoint3 pos, NiPoint3 rot, float speed, float rotSpeed)
{
	std::lock_guard<dlf_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		sd::TranslateTo(ref, pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, speed, rotSpeed);
	} };
	pimpl->overridableTasks[OverrideChannel::SetPosition] = task;
	pimpl->rot = rot;
	pimpl->pos = pos;
}

void ci::Object::SetName(const std::wstring &name)
{
	std::lock_guard<dlf_mutex> l(pimpl->mutex);

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
	std::lock_guard<dlf_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		sd::Lock(ref, lock, false);
	} };
	pimpl->overridableTasks[OverrideChannel::Lock] = task;
}

void ci::Object::SetMotionType(int32_t type)
{
	std::lock_guard<dlf_mutex> l(pimpl->mutex);

	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		auto refID = ref->formID;
		SET_TIMER(100, [=] {
			auto ref = (TESObjectREFR *)LookupFormByID(refID);
			if (ref != nullptr)
			{
				sd::SetMotionType(ref, type, true);
				if (type == Motion_Dynamic)
					sd::ApplyHavokImpulse(ref, 0.0, 0.0, -1.0, 0.75);
			}
		});
	} };
	pimpl->overridableTasks[OverrideChannel::SetMotionType] = task;
	pimpl->motionType = type;
}

void ci::Object::SetDisabled(bool disabled)
{
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	auto task = ObjectTask{ [=](TESObjectREFR *ref) {
		if (disabled)
			sd::DisableNoWait(ref, true);
		else
			sd::EnableNoWait(ref, true);
	} };
	pimpl->simpleTasks.push_back(task);
	pimpl->isDisabled = disabled;
}

void ci::Object::SetBase(const ci::ItemType *itemType)
{
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	pimpl->baseID = itemType->GetFormID();
}

void ci::Object::SetCount(uint32_t count)
{
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	pimpl->count = count;
	if (pimpl->count != pimpl->lastCount)
	{
		pimpl->lastCount = pimpl->count;
		auto task = ObjectTask{ [=](TESObjectREFR *ref) {
			const auto refID = ref->formID;
			cd::GetDisplayName(ref, [count, refID](std::string displayName) {
				SET_TIMER(0, [=] {
					auto name = displayName;

					auto pos = name.find('(');
					if (pos != name.npos)
						name = std::string(name.begin(), name.begin() + pos - 1);

					if (count != 1)
						name += " (" + std::to_string(count) + ")";
					const BSFixedString fsName = name.data();
					auto ref = (TESObjectREFR *)LookupFormByID(refID);
					if (ref != nullptr)
						ref->SetDisplayName(fsName);
				});
			});
		} };
		pimpl->overridableTasks[OverrideChannel::SetCount] = task;
	}
}

void ci::Object::AddItem(const ItemType *item, uint32_t count)
{
	std::lock_guard<dlf_mutex> l1(pimpl->mutex);

	if (item && count)
	{
		auto form = LookupFormByID(item->GetFormID());

		SET_TIMER(0, [=] {
			knownItems[form] = item;
		});

		ObjectTask task{ [=](TESObjectREFR *ref) {
			if (form)
			{
				cd::AddItem(ref, form, count, true);
			}
		} };
		if (!MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
			pimpl->simpleTasks.push_back(task);
		pimpl->inventoryTasks.push_back(task);
	}
}

void ci::Object::RemoveItem(const ItemType *item, uint32_t count)
{
	std::lock_guard<dlf_mutex> l1(pimpl->mutex);

	if (item && count)
	{
		auto form = LookupFormByID(item->GetFormID());

		ObjectTask task{ [=](TESObjectREFR *ref) {
			if (form)
			{
				if (SAFE_ITEM_REMOVE != FALSE)
					cd::RemoveItem(ref, form, count, true, nullptr);
				else
					sd::RemoveItem(ref, form, count, true, nullptr);
			}
		} };
		if (!MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
			pimpl->simpleTasks.push_back(task);
		pimpl->inventoryTasks.push_back(task);
	}
}

NiPoint3 ci::Object::GetPos() const
{
	std::lock_guard<dlf_mutex> l1(pimpl->mutex);
	if (pimpl->posData.Length() > 0)
		return pimpl->posData;
	return pimpl->pos;
}

NiPoint3 ci::Object::GetRot() const
{
	std::lock_guard<dlf_mutex> l1(pimpl->mutex);
	return pimpl->rot;
}

bool ci::Object::IsGrabbed() const
{
	std::lock_guard<dlf_mutex> l1(pimpl->mutex);
	return pimpl->grabbed;
}

extern uint32_t localPlCrosshairRef;
bool ci::Object::IsCrosshairRef() const
{
	std::lock_guard<dlf_mutex> l1(pimpl->mutex);
	return (pimpl->spawnStage == SpawnStage::Spawned || pimpl->spawnStage == SpawnStage::Persistent)
		&& pimpl->refID == localPlCrosshairRef;
}

float ci::Object::GetSpeed() const
{
	std::lock_guard<dlf_mutex> l1(pimpl->mutex);
	if (pimpl->spawnStage == SpawnStage::Spawned || pimpl->spawnStage == SpawnStage::Persistent)
		return pimpl->speed;
	else
		return 0;
}

uint32_t ci::Object::GetBase() const
{
	std::lock_guard<dlf_mutex> l1(pimpl->mutex);
	return pimpl->baseID;
}

int32_t ci::Object::GetMotionType() const
{
	std::lock_guard<dlf_mutex> l1(pimpl->mutex);
	return pimpl->motionType;
}

void ci::Object::Update()
{
	auto calcUpdateRate = [](const ci::Object::Impl *pimpl) {
		return pimpl->isNative ? 150 : 50;
	};

	std::lock_guard<dlf_mutex> l1(pimpl->mutex);

	SAFE_CALL("Object", [&] {
		if (pimpl->lastUpdate + pimpl->updateRate >= clock())
			return;
		pimpl->updateRate = calcUpdateRate(pimpl);
		pimpl->lastUpdate = clock();
	});

	TESObjectCELL *const localPlCell = sd::GetParentCell(g_thePlayer);
	const bool isInterior = localPlCell ? sd::IsInterior(localPlCell) : false;
	const FormID worldSpaceID = sd::GetWorldSpace(g_thePlayer) ? sd::GetWorldSpace(g_thePlayer)->formID : NULL;

	switch (pimpl->spawnStage)
	{
	case SpawnStage::NonSpawned:
	{
		SAFE_CALL("Object", [&] {
			if (markerFormID && localPlCell
				&& NiPoint3{ pimpl->pos - cd::GetPosition(g_thePlayer) }.Length() < GetRespawnRadius(isInterior)
				&& (pimpl->locationID == worldSpaceID || pimpl->locationID == localPlCell->formID))
			{
				this->ForceSpawn();
			}
		});
		break;
	}
	case SpawnStage::Spawned:
	{
		TESObjectREFR *ref = nullptr;
		SAFE_CALL("Object", [&] {
			ref = (TESObjectREFR *)LookupFormByID(pimpl->refID);
			if (ref && ref->formType != FormType::Reference)
				ref = nullptr;
			if (!ref)
				this->ForceDespawn("Unloaded by game");
		});

		if (ref != nullptr)
		{
			SAFE_CALL("Object", [&] {
				if (!ref->baseForm || ref->baseForm->formID != pimpl->baseID)
					return this->ForceDespawn("Base form changed");
			});

			SAFE_CALL("Object", [&] {
				if (NiPoint3{ pimpl->pos - cd::GetPosition(g_thePlayer) }.Length() >= GetRespawnRadius(isInterior))
					return this->ForceDespawn("Object is too far");
			});

			SAFE_CALL("Object", [&] {
				if (pimpl->locationID != worldSpaceID && pimpl->locationID != localPlCell->formID)
					return this->ForceDespawn("Wrong Cell/WorldSpace");
			});
		}
	}
	// no break
	case SpawnStage::Persistent:
	{
		pimpl->waitingForDespawn = false;
		auto ref = (TESObjectREFR *)LookupFormByID(pimpl->refID);
		if (ref && ref->formType == FormType::Reference)
		{
			auto f = [&] {
				const NiPoint3 pos = { sd::GetPositionX(ref), sd::GetPositionY(ref), sd::GetPositionZ(ref) },
					rot = { sd::GetAngleX(ref), sd::GetAngleY(ref), sd::GetAngleZ(ref) };


				//if (pimpl->every1000ms + 1000 < clock())
				{
					//pimpl->every1000ms = clock();
					if (pimpl->lastPos != NiPoint3{ 0, 0, 0 })
						pimpl->speed = (pos - pimpl->lastPos).Length() / pimpl->updateRate * 1000.0;
					pimpl->lastPos = pos;
				}

				switch (ref->baseForm->formType)
				{
				case FormType::Static:
				case FormType::Container:
				case FormType::Furniture:
				case FormType::Activator:
				case FormType::Flora:
				case FormType::Tree:
				case FormType::Door:
					SAFE_CALL("Object", [&] {
						if ((pos - pimpl->pos).Length() > 64)
							this->SetPosition(pimpl->pos);
						if (abs(rot.z - pimpl->rot.z) > 8)
							this->SetAngle(pimpl->rot);
					});
					break;
				default:
					break;
				}

				SAFE_CALL("Object", [&] {
					pimpl->grabbed = sd::GetPlayerGrabbedRef() == ref;
				});

				SAFE_CALL("Object", [&] {
					if (pimpl->motionType == Motion_Dynamic)
					{
						pimpl->posData = pos;
						pimpl->rot = rot;
					}
				});

				SAFE_CALL("Object", [&] {
					if (pimpl->editorPos == nullptr || pimpl->editorRot == nullptr)
					{
						pimpl->editorPos = std::make_shared<NiPoint3>(cd::GetPosition(ref));
						pimpl->editorRot = std::make_shared<NiPoint3>(NiPoint3{ sd::GetAngleX(ref), sd::GetAngleY(ref), sd::GetAngleZ(ref) });
					}
				});

				SAFE_CALL("Object", [&] {
					std::for_each(pimpl->simpleTasks.begin(), pimpl->simpleTasks.end(), [=](ObjectTask &task) {
						task.func(ref);
					});
				});
				pimpl->simpleTasks = {};

				SAFE_CALL("Object", [&] {
					for (auto it = pimpl->overridableTasks.begin(); it != pimpl->overridableTasks.end(); ++it)
					{
						const auto &task = it->second;
						task.func(ref);
					}
				});

				SAFE_CALL("Object", [&] {
					auto taskBackup = pimpl->overridableTasks[OverrideChannel::SetOpen];
					if (ref->IsDisabled() == false)
						pimpl->overridableTasks.clear();
					pimpl->overridableTasks[OverrideChannel::SetOpen] = taskBackup;
				});
			};
			SAFE_CALL("Object", f);

		}
		break;
	}
	}
}

void ci::Object::Update_OT()
{
	SAFE_CALL("Object", [&] {
		std::lock_guard<dlf_mutex> l1(pimpl->mutex);

		if (MenuManager::GetSingleton()->IsMenuOpen("Main Menu"))
		{
			if (!pimpl->waitingForDespawn)
			{
				pimpl->waitingForDespawn = true;
				std::thread([=] {
					SET_TIMER(0, [=] {
						SET_TIMER(3000, [=] {
							std::lock_guard<dlf_mutex> l1(gMutex);
							if (allObjects.find(this) == allObjects.end())
								return;
							std::lock_guard<dlf_mutex> l(pimpl->mutex);
							this->ForceDespawn("Geme reinit");
						});
					});
				}).detach();
			}
		}
	});
}

void ci::Object::UpdateAll()
{
	std::lock_guard<dlf_mutex> l(gMutex);

	SAFE_CALL("Object", [&] {
		std::for_each(allObjects.begin(), allObjects.end(), [](ci::Object *object) {
			object->Update();
		});
	});

	SAFE_CALL("Object", [&] {
		for (auto it = globalTasks.begin(); it != globalTasks.end(); ++it)
		{
			if (it->func != nullptr && it->func())
				it->func = nullptr;
		}
	});

	SAFE_CALL("Object", [&] {
		auto pred = [](const GlobalTask &task) {
			return task.func == nullptr;
		};
		globalTasks.erase(
			std::remove_if(globalTasks.begin(), globalTasks.end(), pred),
			globalTasks.end()
		);
	});
}

void ci::Object::UpdateAll_OT()
{
	enum {
		UpdateAll_OT_Delay = 333
	};
	static clock_t lastUpdateAllOT = 0;

	if (lastUpdateAllOT + UpdateAll_OT_Delay < clock())
	{
		lastUpdateAllOT = clock();
		SAFE_CALL("Object", [&] {
			std::lock_guard<dlf_mutex> l(gMutex);
			std::for_each(allObjects.begin(), allObjects.end(), [](ci::Object *object) {
				object->Update_OT();
			});
		});
	}
}

void ci::Object::ForceSpawn()
{
	std::lock_guard<dlf_mutex> l(pimpl->mutex);
	if (pimpl->spawnStage == SpawnStage::NonSpawned)
	{
		pimpl->spawnStage = SpawnStage::Spawning;
		SET_TIMER(100, [=] {
			std::lock_guard<dlf_mutex> l(gMutex);
			if (allObjects.find(this) == allObjects.end())
				return;

			auto at = (TESObjectREFR *)LookupFormByID(markerFormID);
			if (!at)
				at = g_thePlayer;
			auto baseForm = LookupFormByID(pimpl->baseID);
			if (!baseForm)
			{
				ErrorHandling::SendError("ERROR:Object Incorrect base form ID");
				baseForm = LookupFormByID(0x00031897);
			}
			auto ref = sd::PlaceAtMe(at, baseForm, 1, true, false);
			if (!ref)
			{
				ErrorHandling::SendError("ERROR:Object Unable to place");
				return;
			}
			pimpl->refID = ref->formID;
			WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, true);

			this->SetPosition(pimpl->pos);
			this->SetAngle(pimpl->rot);
			this->SetDestroyed(pimpl->destroyed);
			if (pimpl->name.size() > 0)
				this->SetName(pimpl->name);
			this->SetOpen(pimpl->open);
			this->BlockActivation(pimpl->blockedActivation);
			this->SetMotionType(pimpl->motionType);
			this->SetDisabled(pimpl->isDisabled);
			
			pimpl->lastCount = 1;
			this->SetCount(pimpl->count);

			this->UpdateContainer();

			pimpl->spawnStage = SpawnStage::Spawned;
		});
	}
}

void ci::Object::UpdateContainer()
{
	SAFE_CALL("Object", [&] {
		auto refID = pimpl->refID;
		SET_TIMER(0, [refID] {
			auto ref = (TESObjectREFR *)LookupFormByID(refID);
			if (ref)
				sd::RemoveAllItems(ref, nullptr, false, true);
		});

		for (auto it = pimpl->inventoryTasks.begin(); it != pimpl->inventoryTasks.end(); ++it)
		{
			pimpl->simpleTasks.push_back(*it);
		}
	});
}

void ci::Object::ForceDespawn(const std::string &r)
{
	SAFE_CALL("Object", [&] {
		std::lock_guard<dlf_mutex> l(pimpl->mutex);
		if (pimpl->spawnStage == SpawnStage::Spawned)
		{
			WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, false);
			pimpl->refID = 0;
			pimpl->spawnStage = SpawnStage::NonSpawned;
		}
	});
}

uint32_t ci::Object::GetRefID() const
{
	return pimpl->refID;
}