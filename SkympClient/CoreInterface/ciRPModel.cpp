#include "../stdafx.h"
#include "CoreInterface.h"
#include "ciRPModel.h"

#include "../Sync/SyncEssential.h"

#include "ciRPEngineBase.h" // ci::lookSync

namespace ci
{
	clock_t lastTintMaskUse = 0;
}

struct ci::Model::Impl
{
	bool isSpawned = false;
	NiPoint3 spawnPoint = { 0,0,0 };
	TintMasks tints;
	NPCSource npcSource;
	dlf_mutex m;
	uint32_t refID = 0;
	std::wstring name = L" ";
	bool despawnFlag = false;
	clock_t spawnMoment = 0;
};

ci::Model::Model()
{
	pimpl.reset(new Impl);
}

ci::Model::~Model()
{
	this->ForgetActor();
}

bool ci::Model::IsSpawned() const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return pimpl->isSpawned;
}

Actor *ci::Model::GetActor() const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	auto ac = (Actor *)LookupFormByID(pimpl->refID);
	return ac;
}

clock_t ci::Model::GetSpawnMoment() const
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	return pimpl->spawnMoment;
}

void ci::Model::SetSpawnPoint(const NiPoint3 &spawnPoint)
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	pimpl->spawnPoint = spawnPoint;
}

void ci::Model::SetNPCSource(NPCSource f)
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	pimpl->npcSource = f;
}

void ci::Model::SetTints(const TintMasks &tints)
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	pimpl->tints = tints;
}

void ci::Model::SetName(const std::wstring &name)
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	pimpl->name = name;
}

void ci::Model::Despawn(const std::wstring &reason)
{
	std::lock_guard<dlf_mutex> l(pimpl->m);
	pimpl->despawnFlag = true;
	ci::Log(L"ERROR:Model Forced despawn " + reason);
}

void ci::Model::Update()
{
	std::lock_guard<dlf_mutex> l(pimpl->m);

	const float distance = (pimpl->spawnPoint - ci::LocalPlayer::GetSingleton()->GetPos()).Length();
	const bool isInterior = false; //TODO

	const auto actor = this->GetActor();

	const bool despawnFlag = pimpl->despawnFlag;
	pimpl->despawnFlag = false;

	if (pimpl->isSpawned)
	{
		if (despawnFlag
			|| actor == nullptr 
			|| distance > SyncOptions::GetRespawnRadius(isInterior))
		{
			this->ForgetActor();
		}
	}
	else
	{
		if (pimpl->spawnPoint.Length() > 0 && pimpl->npcSource != nullptr)
		{
			if (distance <= SyncOptions::GetRespawnRadius(isInterior))
			{
				this->SpawnActor();
			}
		}
	}
}

void ci::Model::SpawnActor()
{
	auto actor = sd::PlaceActorAtMe(g_thePlayer, pimpl->npcSource(), 0, nullptr);
	pimpl->refID = actor->formID;
	WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, true);
	sd::TranslateTo(actor, pimpl->spawnPoint.x, pimpl->spawnPoint.y, pimpl->spawnPoint.z, 0.0f, 0.0f, 0.0f, 10000.0f, 0.0f);
	pimpl->isSpawned = true;
	pimpl->spawnMoment = clock();

	auto setAVsToDefault = [](Actor *actor) {
		sd::SetActorValue(actor, "Agression", 0.0);
		sd::SetActorValue(actor, "attackdamagemult", 0.0);
		sd::SetActorValue(actor, "Variable01", rand());
		sd::SetActorValue(actor, "MagicResist", 99);
	};

	setAVsToDefault(actor);
	sd::AddItem(actor, LookupFormByID(ID_TESSoulGem::SoulGemBlack), 100, true);
	sd::Enable(actor, true);

	cd::Value<Actor> ac = actor;

	auto tints = pimpl->tints;
	TaskRunner<TASK_RUNNER_1000_MS>::AddTask([ac, tints] {
		const auto actor = ac.operator Actor *();
		if (actor != nullptr)
		{
			ci::lookSync->ApplyTintMasks((TESNPC *)actor->baseForm, tints);
			actor->baseForm->formID = g_thePlayer->baseForm->formID;
			actor->QueueNiNodeUpdate(true);
		}
	}, (int64_t)(pimpl->spawnPoint - ci::LocalPlayer::GetSingleton()->GetPos()).Length());

	SET_TIMER_LIGHT(0, [=] {
		const auto id = ac.GetFormID();
		if (LookupFormByID(id) == nullptr)
			return;

		auto actor = (Actor *)LookupFormByID(id);
		if (actor != nullptr)
		{
			setAVsToDefault(actor);

			BSFixedString name = WstringToString(pimpl->name).c_str();
			actor->SetDisplayName(name, true);

			cd::SetPosition(ac, pimpl->spawnPoint);
			sd::BlockActivation(actor, true);
			sd::AllowPCDialogue(actor, false);
			//sd::AllowPCDialogue(actor, true);

			AnimData_::Register(actor);
		}

		cd::KeepOffsetFromActor(cd::Value<Actor>(ac.GetFormID()), cd::Value<Actor>(ac.GetFormID()), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0);
	});
}

void ci::Model::ForgetActor()
{
	WorldCleaner::GetSingleton()->SetFormProtected(pimpl->refID, false);
	pimpl->refID = 0;
	pimpl->isSpawned = false;
}