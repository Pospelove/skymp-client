#include "../stdafx.h"
#include "CoreInterface.h"
#include "ciRPGhostAxe.h"

#include "ciRPEngineBase.h"
#include "../Sync/SyncEssential.h"

namespace ci
{
	class GhostAxe : public RemotePlayer
	{
	public:
		GhostAxe() : RemotePlayer(L"Ghost Axe", {}, {}, NULL, NULL, nullptr, "RPEngineInput", nullptr)
		{
		}

		virtual ~GhostAxe() override
		{
		}

	private:
		void ApplyMovementDataImpl() override {
			auto actor = (Actor *)LookupFormByID(pimpl->formID);
			if (!actor)
				return;
			auto movData = pimpl->movementData;
			if (this->timer < clock())
			{
				timer = clock() + SyncOptions::GetSingleton()->GetInt("GHOST_AXE_UPDATE_RATE");
				cd::TranslateTo(actor, movData.pos.x, movData.pos.y, movData.pos.z, 0, 0, 0, 100000.0, 4.0);
				sd::ForceActorValue(actor, "Confidence", 0);
				if (sd::GetBaseActorValue(actor, "Invisibility") == 0)
				{
					this->aiEnabled = true;
					sd::SetActorValue(actor, "Invisibility", 100);
				}
			}
			if (this->aiEnabled)
			{
				this->aiEnabled = false;
				sd::EnableAI(actor, false);
			}
		}

		TESNPC *AllocateNPC() const override {
			enum {
				InvisibleNPC = 0x00071E6B,
			};
			auto npc = (TESNPC *)LookupFormByID(InvisibleNPC);
			npc->height = 0.333;
			npc->headparts = nullptr;
			npc->numHeadParts = 0;
			return npc;
		}

		clock_t timer = 0;
		bool aiEnabled = true;
	};

	struct GhostAxeManager::Impl
	{
		RemotePlayer *ghostAxe = nullptr;
		uint32_t ghostAxeID = 0;
	};

	ci::GhostAxeManager &ci::GhostAxeManager::GetSingleton()
	{
		static ci::GhostAxeManager m;
		return m;
	}

	ci::GhostAxeManager::GhostAxeManager() : pimpl(new Impl)
	{
	}

	ci::GhostAxeManager::~GhostAxeManager()
	{
		delete pimpl;
	}

	uint32_t ci::GhostAxeManager::GetGhostAxeFormID() const
	{
		return pimpl->ghostAxeID;
	}

	void ci::GhostAxeManager::Update()
	{
		if (pimpl->ghostAxe == nullptr)
		{
			pimpl->ghostAxe = CreateGhostAxe();
		}
		if (pimpl->ghostAxe != nullptr)
		{
			auto movData = MovementData_::GetFromPlayer();
			movData.pos.z += SyncOptions::GetSingleton()->GetFloat("GHOST_AXE_OFFSET_Z");
			movData.pos.x += SyncOptions::GetRespawnRadius(false) * 0.85;
			pimpl->ghostAxe->ApplyMovementData(movData);
			static auto localPl = ci::LocalPlayer::GetSingleton();
			pimpl->ghostAxe->SetCell(localPl->GetCell());
			pimpl->ghostAxe->SetWorldSpace(localPl->GetWorldSpace());
			pimpl->ghostAxe->Update();
			pimpl->ghostAxeID = pimpl->ghostAxe->GetFormID();
		}
	}

	RemotePlayer *ci::GhostAxeManager::CreateGhostAxe()
	{
		auto axe = new GhostAxe;
		axe->SetBaseNPC(ID_TESNPC::InvisibleNPC);
		axe->UpdateAVData("invisibility", AVData(100.0, 0.0, 1.0));
		return axe;
	}
}