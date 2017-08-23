#include "../stdafx.h"
#include "CoreInterface.h"

namespace ci
{
	std::set<RemotePlayer *> allRemotePlayers;
	std::recursive_mutex gMutex;

	enum class SpawnStage
	{
		NonSpawned,
		Spawning,
		Spawned,
	};

	struct RemotePlayer::Impl
	{
		std::recursive_mutex mutex;
		UInt32 formID = 0;
		NiPoint3 pos;
		SpawnStage spawnStage = SpawnStage::NonSpawned;
	};

	RemotePlayer::RemotePlayer(const std::wstring &name, NiPoint3 spawnPoint, uint32_t cellID, uint32_t worldSpaceID) :
		pimpl(new Impl)
	{
		std::lock_guard<std::recursive_mutex> l(gMutex);
		std::lock_guard<std::recursive_mutex> l1(pimpl->mutex);

		allRemotePlayers.insert(this);
	}

	RemotePlayer::~RemotePlayer()
	{
		std::lock_guard<std::recursive_mutex> l(gMutex);
		std::lock_guard<std::recursive_mutex> l1(pimpl->mutex);

		allRemotePlayers.erase(this);

		if (pimpl->formID != NULL)
			WorldCleaner::GetSingletone()->SetFormProtected(pimpl->formID, false);
	}

	void RemotePlayer::Update()
	{
		std::lock_guard<std::recursive_mutex> l1(pimpl->mutex);

		switch (pimpl->spawnStage)
		{
		case SpawnStage::NonSpawned:
		{
			return this->ForceSpawn();
		}
		break;

		case SpawnStage::Spawning:
		{
		}
		break;

		case SpawnStage::Spawned:
		{
			auto actor = (Actor *)LookupFormByID(pimpl->formID);
			if (!actor)
				return this->ForceDespawn(L"Despawned: Unloaded by the game");
		}
		break;
		}
	}

	void RemotePlayer::Update_OT()
	{
	}

	void RemotePlayer::ForceSpawn()
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

		if (pimpl->spawnStage != SpawnStage::NonSpawned)
			return;

		pimpl->spawnStage = SpawnStage::Spawning;


		enum {
			AmaundMotierre = 0x0003B43A
		};


		WorldCleaner::GetSingletone()->SetFormProtected(AmaundMotierre, true);

		auto refToPlaceAt = g_thePlayer;

		//cd::PlaceAtMe(refToPlaceAt, (TESNPC *)LookupFormByID(AmaundMotierre), 1, true, false, [=](cd::Value<TESObjectREFR> ac) {
		cd::PlaceActorAtMe(refToPlaceAt, (TESNPC *)LookupFormByID(AmaundMotierre), 1, 0, [=](cd::Value<Actor> ac) {
			Timer::Set(0, [=] {
				if (LookupFormByID(ac.GetFormID()) == nullptr)
				{
					pimpl->spawnStage = SpawnStage::NonSpawned;
					return;
				}

				std::lock_guard<std::recursive_mutex> l(gMutex);
				if (allRemotePlayers.find(this) == allRemotePlayers.end())
					return;

				std::lock_guard<std::recursive_mutex> l1(pimpl->mutex);

				pimpl->formID = ac.GetFormID();
				WorldCleaner::GetSingletone()->SetFormProtected(pimpl->formID, true);
				WorldCleaner::GetSingletone()->SetFormProtected(AmaundMotierre, false);
				pimpl->spawnStage = SpawnStage::Spawned;

				static int count = 0;
				++count;
				if (count % 100 == 0)
					ci::Chat::AddMessage(std::to_wstring(count));
			});
		});
	}

	void RemotePlayer::ForceDespawn(const wchar_t *reason)
	{
		std::lock_guard<std::recursive_mutex> l(pimpl->mutex);

		if (pimpl->spawnStage == SpawnStage::Spawned ||
			(pimpl->spawnStage == SpawnStage::Spawning && MenuManager::GetSingleton()->IsMenuOpen("Main Menu")))
		{
			WorldCleaner::GetSingletone()->SetFormProtected(pimpl->formID, false);
			pimpl->formID = 0;
			pimpl->spawnStage = //SpawnStage::NonSpawned
				SpawnStage(1000000);
		}
	}

	void RemotePlayer::UpdateAll()
	{
		//UpdatePlaceAtMeMarker();

		std::lock_guard<std::recursive_mutex> l(gMutex);
		for (auto it = allRemotePlayers.begin(); it != allRemotePlayers.end(); ++it)
		{
			if (*it)
				(*it)->Update();
		}
	}

	void RemotePlayer::UpdateAll_OT()
	{
		std::lock_guard<std::recursive_mutex> l(gMutex);
		for (auto it = allRemotePlayers.begin(); it != allRemotePlayers.end(); ++it)
		{
			if (*it)
				(*it)->Update_OT();
		}
	}

	void RemotePlayer::SetName(const std::wstring &name)
	{
	}

	void RemotePlayer::SetPos(NiPoint3 pos)
	{
	}

	void RemotePlayer::SetCell(uint32_t cellID)
	{
	}

	void RemotePlayer::SetWorldSpace(uint32_t worldSpaceID)
	{
	}

	std::wstring RemotePlayer::GetName() const
	{
		return L"";
	}

	NiPoint3 RemotePlayer::GetPos() const
	{
		return { 0,0,0 };
	}

	uint32_t RemotePlayer::GetCell() const
	{
		return 0;
	}

	uint32_t RemotePlayer::GetWorldSpace() const
	{
		return 0;
	}
}