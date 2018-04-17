#include "../stdafx.h"
#include "../Sync/SyncEssential.h"
#include "CoreInterface.h"

#include "ciRPEngineBase.h"

void ci::RemotePlayer::DespawnIfNeed()
{
	if (pimpl->model.IsSpawned() == false)
		return;

	const auto actor = pimpl->model.GetActor();

	if (!actor)
		return pimpl->model.Despawn(L"Despawned: Unloaded by the game");

	if (clock() - pimpl->model.GetSpawnMoment() > 2500
		&& sd::Is3DLoaded(actor) == false)
	{
		return pimpl->model.Despawn(L"Despawned: 3D isn't loaded (1.0.30)");
	}

	auto myCell = sd::GetParentCell(g_thePlayer);
	auto actorCell = sd::GetParentCell(actor);
	if (myCell && myCell->IsInterior() && myCell != actorCell)
	{
		return pimpl->model.Despawn(L"Despawned: Cell changed (1.0.28e)");
	}

	SAFE_CALL("RemotePlayer", [&] {
		if (this->IsDerived() == false && pimpl->baseNpc == nullptr && pimpl->height != ((TESNPC *)actor->baseForm)->height)
			pimpl->model.Despawn(L"Despawned: Height changed");
	});

	SAFE_CALL("RemotePlayer", [&] {
		const FormID worldSpaceID = sd::GetWorldSpace(g_thePlayer) ? sd::GetWorldSpace(g_thePlayer)->formID : NULL;
		if (pimpl->worldSpaceID != worldSpaceID && worldSpaceID != NULL && pimpl->worldSpaceID != NULL)
			pimpl->model.Despawn(std::wstring(L"Despawned: WorldSpace changed " + std::to_wstring(worldSpaceID) + L' ' + std::to_wstring(pimpl->worldSpaceID)).data());
	});

	SAFE_CALL("RemotePlayer", [&] {
		const NiPoint3 localPlPos = cd::GetPosition(g_thePlayer);
		const bool isInterior = sd::GetParentCell(g_thePlayer) ? sd::IsInterior(sd::GetParentCell(g_thePlayer)) : false;
		if (NiPoint3{ cd::GetPosition(actor) - localPlPos }.Length() >= SyncOptions::GetRespawnRadius(isInterior))
			pimpl->model.Despawn(L"Despawned: Player is too far");
	});

	SAFE_CALL("RemotePlayer", [&] {
		if (sd::IsDead(actor) && this->GetAVData("Health").percentage > 0.0)
			pimpl->model.Despawn(L"Despawned: Fake death");
	});

	SAFE_CALL("RemotePlayer", [&] {
		if (clock() - pimpl->model.GetSpawnMoment() > 3000)
		{
			const NiPoint3 offset = { 0, 0, 128 + 16 };
			auto nicknamePos = cd::GetPosition(actor) + offset;

			//ci::Chat::AddMessage(this->GetName() + L" " + std::to_wstring(nicknamePos.x) + L" " + std::to_wstring(this->GetPos().x));
			// ¬ыведет крайне различающиес€ числа, если человек исчез. 
			// Ќапример, в ¬айтране x позици€ ноды может быть 60, 228, а x добытый через интерфейс this->GetPos() нормальный
			// ≈сли человек не исчез, то числа мало различаютс€
			// Ќа основе этого делаетс€ следующий фикс

			if (std::abs(nicknamePos.x - this->GetPos().x) > 1000.0)
			{
				pimpl->model.Despawn(L"Bad Node pos (1.0.34d)");
				return;
			}
		}
	});
}