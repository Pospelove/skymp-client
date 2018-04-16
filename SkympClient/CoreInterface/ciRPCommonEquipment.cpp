#include "../stdafx.h"
#include "../Sync/SyncEssential.h"
#include "CoreInterface.h"

#include "ciRPEngineBase.h"

void ci::RemotePlayer::ManageMagicEquipment(Actor *actor)
{
	SAFE_CALL("RemotePlayer", [&] {

		auto handsMagicProxy = pimpl->handsMagicProxy;

		if (handsMagicProxy[0] != handsMagicProxy[1] && handsMagicProxy[0] && handsMagicProxy[1])
			handsMagicProxy[1] = handsMagicProxy[0];

		for (int32_t i = 0; i <= 1; ++i)
		{
			if (pimpl->eq.handsMagic[i] != handsMagicProxy[i])
			{
				if (clock() - pimpl->lastMagicEquipmentChange > 750)
				{
					pimpl->eq.handsMagic[i] = handsMagicProxy[i];
					pimpl->lastMagicEquipmentChange = clock();
				}
			}
		}
	});
}

void ci::RemotePlayer::ApplyEquipmentHands(Actor *actor)
{
	SAFE_CALL("RemotePlayer", [&] {

		auto isAiming = [=] {
			std::lock_guard<dlf_mutex> l(gMutex);
			if (allRemotePlayers.find(this) == allRemotePlayers.end())
				return false;

			auto md = this->GetMovementData();

			auto getIsAiming = [](const ci::MovementData &md) {
				enum AttackState {
					BowClick = 8,
					BowDrawing = 9,
					BowHoldingAShot = 10,
					BowRelease = 11,
					BowReleased = 12,
				};
				return md.attackState == BowClick
					|| md.attackState == BowDrawing
					|| md.attackState == BowHoldingAShot
					|| md.attackState == BowRelease;
			};

			return getIsAiming(md);
		};

		if ((pimpl->eq.hands != pimpl->eqLast.hands && clock() - pimpl->lastWeaponsUpdate > 300)
			|| (pimpl->eq.handsMagic != pimpl->eqLast.handsMagic))
		{
			Equipment_::Equipment eq;
			for (int32_t i = 0; i <= 1; ++i)
			{
				TESForm *form = nullptr;
				if (pimpl->eq.hands[i])
				{
					form = LookupFormByID(pimpl->eq.hands[i]->GetFormID());
				}
				else if (pimpl->eq.handsMagic[i])
				{
					form = LookupFormByID(pimpl->eq.handsMagic[i]->GetFormID());
				}
				eq.hands[i] = form;
			}

			Equipment_::ApplyHands(actor, eq, isAiming);
			pimpl->eqLast.hands = pimpl->eq.hands;
			pimpl->eqLast.handsMagic = pimpl->eq.handsMagic;
			pimpl->lastWeaponsUpdate = clock();
		}
	});
}

void ci::RemotePlayer::ApplyEquipmentOther(Actor *actor)
{
	SAFE_CALL("RemotePlayer", [&] {
		//if (sd::HasLOS(g_thePlayer, actor))
		{
			Equipment_::Equipment eq;
			if (pimpl->eq.ammo != nullptr)
				eq.other.insert(LookupFormByID(pimpl->eq.ammo->GetFormID()));
			for (auto &item : pimpl->eq.armor)
			{
				const bool emptyLeftHand = pimpl->eq.hands[1] == nullptr &&
					(!pimpl->eq.hands[0]
						|| pimpl->eq.hands[0]->GetSubclass() < ci::ItemType::Subclass::WEAP_Greatsword
						|| pimpl->eq.hands[0]->GetSubclass() > ci::ItemType::Subclass::WEAP_Crossbow);

				const bool dontEquipPlease = item->GetSubclass() == ci::ItemType::Subclass::ARMO_Shield && !emptyLeftHand;

				if (!dontEquipPlease)
				{
					eq.other.insert(LookupFormByID(item->GetFormID()));
				}
				else
				{
					static bool reported = false;
					if (reported == false)
					{
						reported = true;
						ErrorHandling::SendError("WARN:RemotePlayer Incorrect equipment");
					}
				}
			}

			// Prevent breaking SetWerewolf 
			static auto wwSkin = LookupFormByID(0x000F6002);
			if (sd::IsEquipped(actor, wwSkin))
			{
				eq.other.insert(wwSkin);
			}

			Equipment_::ApplyOther(actor, eq);
			pimpl->eqLast.armor = pimpl->eq.armor;
			pimpl->eqLast.ammo = pimpl->eq.ammo;
		}
	});
}