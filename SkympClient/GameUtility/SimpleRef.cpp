#include "../stdafx.h"
#include "SimpleRef.h"

std::set<SimpleRef *> allSimpleRefs;
dlf_mutex gMutex{"simpleref_cont"};

enum {
	NON_SPAWNED = 0,
	SPAWNED = 1,
};

struct SimpleRef::Impl 
{
	TESForm *base = nullptr;
	dlf_mutex m{"simpleref_impl"};
	int32_t spawnStage = NON_SPAWNED;
	float drawDistance = 0;
	uint32_t refID = 0;
	NiPoint3 pos = NiPoint3{ 0,0,0 };
	NiPoint3 rot = NiPoint3{ 0,0,0 };
	cd::Value<TESObjectREFR> spawner = nullptr;
	std::vector<std::function<void(TESObjectREFR *)>> tasks, tasksPersist;
};

SimpleRef::SimpleRef(TESForm *base, NiPoint3 pos, float drawDistance, cd::Value<TESObjectREFR> spawner) : pimpl(new Impl)
{

	pimpl->base = base;
	pimpl->drawDistance = drawDistance;
	pimpl->pos = pos;
	pimpl->spawner = spawner;

	std::lock_guard<dlf_mutex> l(gMutex);
	allSimpleRefs.insert(this);
}

SimpleRef::~SimpleRef()
{
	std::lock_guard<dlf_mutex> l(gMutex);
	allSimpleRefs.erase(this);

	std::lock_guard<dlf_mutex> l1(pimpl->m);
	switch (pimpl->spawnStage)
	{
	case SPAWNED:
		WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, false);
		break;
	}
}

void SimpleRef::UpdateAll()
{
	std::lock_guard<dlf_mutex> l(gMutex);
	SAFE_CALL("SimpleRef", [&] {
		for (auto ref : allSimpleRefs)
		{
			SAFE_CALL("SimpleRef", [&] {
				ref->Update();
			});
		}
	});
}

bool SimpleRef::IsSimpleRef(uint32_t formID)
{
	std::lock_guard<dlf_mutex> l(gMutex);
	for (auto ref : allSimpleRefs)
	{
		if (ref->GetFormID() == formID)
			return true;
	}
	return false;
}

void SimpleRef::Update()
{
	SAFE_CALL("SimpleRef", [&] {
		std::lock_guard<dlf_mutex> l(pimpl->m);

		auto spawner = pimpl->spawner.operator TESObjectREFR *();
		const bool isInRange = (cd::GetPosition(g_thePlayer) - this->GetPos()).Length() < pimpl->drawDistance;

		switch (pimpl->spawnStage)
		{
		case NON_SPAWNED:
			SAFE_CALL("SimpleRef", [&] {
				if (isInRange)
				{
					if (spawner != nullptr)
					{
						auto ref = sd::PlaceAtMe(spawner, pimpl->base, 1, true, false);
						if (ref != nullptr)
						{
							pimpl->spawnStage = SPAWNED;
							pimpl->refID = ref->formID;
							WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, true);

							for (auto task : pimpl->tasksPersist)
								pimpl->tasks.push_back(task);
						}
					}
				}
			});
			break;
		case SPAWNED:
			auto f = [&] {
				auto ref = (TESObjectREFR *)LookupFormByID(pimpl->refID);
				if (ref && ref->formType != FormType::Reference && ref->formType != FormType::Character)
					ref = nullptr;

				if (!isInRange || ref == nullptr)
				{
					WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, false);
					pimpl->spawnStage = NON_SPAWNED;
					pimpl->refID = NULL;
					return;
				}

				sd::TranslateTo(ref, pimpl->pos.x, pimpl->pos.y, pimpl->pos.z, pimpl->rot.x, pimpl->rot.y, pimpl->rot.z, 10000.f, 10000.f);

				const auto tasks = pimpl->tasks;
				pimpl->tasks = {};

				for (auto task : tasks)
				{
					SAFE_CALL("SimpleRef", [&] {
						task(ref);
					});
				}
			};
			SAFE_CALL("SimpleRef", f);
			break;
		}
	});
}

void SimpleRef::SetPos(const NiPoint3 &pos)
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	pimpl->pos = pos;
}

NiPoint3 SimpleRef::GetPos() const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return pimpl->pos;
}

void SimpleRef::SetRot(const NiPoint3 &rot)
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	pimpl->rot = rot;
}

NiPoint3 SimpleRef::GetRot() const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return pimpl->rot;
}

uint32_t SimpleRef::GetFormID() const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return pimpl->refID;
}

void SimpleRef::Task(std::function<void(TESObjectREFR *)> f, bool persist)
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	pimpl->tasks.push_back(f);
	if (persist)
		pimpl->tasksPersist.push_back(f);
}