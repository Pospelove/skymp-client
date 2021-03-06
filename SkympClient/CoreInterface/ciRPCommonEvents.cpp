#include "../stdafx.h"
#include "CoreInterface.h"
#include "ciRPCommonEvents.h"

#include "ciRPEngineBase.h"

class ci::RemotePlayer::Impl::ActivateEventSinkGlobal : public BSTEventSink<TESActivateEvent>
{
public:
	ActivateEventSinkGlobal() {
		g_activateEventSource.AddEventSink(this);
	}

	virtual ~ActivateEventSinkGlobal() override {
		g_activateEventSource.RemoveEventSink(this);
	}

	virtual	EventResult	ReceiveEvent(TESActivateEvent *evn, BSTEventSource<TESActivateEvent> * source) override
	{
		if (evn->caster == g_thePlayer)
		{
			std::lock_guard<RPGMUTEX> l(gMutex);

			for (auto pl : allRemotePlayers)
			{
				std::lock_guard<dlf_mutex> l1(pl->pimpl->mutex);
				if (pl->pimpl->formID == evn->target->formID)
				{
					auto onActivate = pl->pimpl->onActivate;
					if (onActivate != nullptr)
					{
						ci::IClientLogic::QueueCallback([=] {
							onActivate();
						});
					}
					return EventResult::kEvent_Continue;
				}
			}
		}
		return EventResult::kEvent_Continue;
	}
};

class ci::RemotePlayer::Impl::HitEventSinkGlobal : public BSTEventSink<TESHitEvent>
{
public:
	HitEventSinkGlobal() {
		g_hitEventSource.AddEventSink(this);
	}

	virtual ~HitEventSinkGlobal() override {
		g_hitEventSource.RemoveEventSink(this);
	}

private:
	std::map<uint32_t, clock_t> lastReceive;
	dlf_mutex m{"ci_remoteplayer_hiteventsink"};

	virtual	EventResult	ReceiveEvent(TESHitEvent *evn, BSTEventSource<TESHitEvent> * source) override
	{
		if (LookupFormByID(evn->sourceFormID) != nullptr && LookupFormByID(evn->sourceFormID)->formType == FormType::Enchantment)
			return EventResult::kEvent_Continue;

		if (evn->target == nullptr)
			return EventResult::kEvent_Continue;

		auto getRefMark = [](uint32_t refID)->std::string {
			if (refID == g_thePlayer->GetFormID())
				return "";

			for (auto it = allRemotePlayers.begin(); it != allRemotePlayers.end(); ++it)
			{
				auto pl = *it;
				if (pl == nullptr)
					continue;
				std::lock_guard<dlf_mutex> l1(pl->pimpl->mutex);
				if (pl->pimpl->formID == refID)
				{
					return pl->pimpl->mark;
				}
			}
			return "";
		};

		auto caster = evn->caster;
		if (caster == nullptr)
			caster = g_thePlayer;

		const auto casterID = caster->GetFormID();
		const auto targetID = evn->target->GetFormID();
		const bool isTargetLocal = (evn->target == g_thePlayer);
		const bool isCasterLocal = (caster == g_thePlayer);

		if (isCasterLocal)
		{
			bool doRet = false;

			std::lock_guard<dlf_mutex> l(this->m);
			if (clock() - lastReceive[evn->sourceFormID] < 25)
			{
				SET_TIMER_LIGHT(167, [] {
					CIAccess::OnPoisonAttack();
				});
				doRet = true;
			}
			lastReceive[evn->sourceFormID] = clock();

			if (doRet)
				return EventResult::kEvent_Continue;
		}

		HitEventData hitEventData_;
		hitEventData_.powerAttack = evn->flags.powerAttack;

		auto sourceForm = LookupFormByID(evn->sourceFormID);

		SET_TIMER_LIGHT(1, [=] {
			auto hitEventData = hitEventData_;

			std::lock_guard<RPGMUTEX> l(gMutex);

			const ci::ItemType *weapon = nullptr;
			const ci::Spell *spell = nullptr;
			if (sourceForm != nullptr)
			{
				try {
					std::lock_guard<RPGMUTEX> l(gMutex);
					weapon = RemotePlayerKnownItems()->at(sourceForm);
				}
				catch (...) {
				}
				try {
					std::lock_guard<RPGMUTEX> l(gMutex);
					spell = RemotePlayerKnownSpells()->at(sourceForm);
				}
				catch (...) {
				}
			}
			hitEventData.weapon = weapon;
			hitEventData.spell = spell;
			hitEventData.hitSrcMark = getRefMark(casterID);

			if (isTargetLocal)
			{
				auto localPl = ci::LocalPlayer::GetSingleton();
				ci::IClientLogic::QueueCallback([=] {
					localPl->onHit(hitEventData);
				});
			}
			else
			{
				for (auto it = allRemotePlayers.begin(); it != allRemotePlayers.end(); ++it)
				{
					auto pl = *it;
					if (pl == nullptr)
						continue;
					std::lock_guard<dlf_mutex> l1(pl->pimpl->mutex);
					auto ref = (TESObjectREFR *)LookupFormByID(pl->pimpl->formID);
					if (!ref)
						continue;
					if (ref->formID != targetID
						&& (pl->pimpl->gnomeHost.IsGnomeExist(0) == false || pl->pimpl->gnomeHost.GetGnomeFormID(0) != targetID) // requires sd thread
						&& (pl->pimpl->gnomeHost.IsGnomeExist(1) == false || pl->pimpl->gnomeHost.GetGnomeFormID(1) != targetID))
						continue;
					auto onHit = pl->pimpl->onHit;
					if (onHit)
					{
						ci::IClientLogic::QueueCallback([=] {
							onHit(hitEventData);
						});
					}
					else
						pl->pimpl->broken = true;
					break;
				}
			}
		});

		return EventResult::kEvent_Continue;
	}
};

void ci::CommonEvents::Hook()
{
	static void *hitEventSink = new RemotePlayer::Impl::HitEventSinkGlobal,
		*activateEventSink = new RemotePlayer::Impl::ActivateEventSinkGlobal;
}