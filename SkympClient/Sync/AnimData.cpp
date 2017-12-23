#include "../stdafx.h"
#include "AnimData.h"
#include "MovementData.h"
#include "../ScriptDragon/obscript.h"
#include "Skyrim/Events/ScriptEvent.h"
#include <queue>

using AnimID = AnimData_::AnimID;

enum class Direction : uint8_t
{
	Standing = 0,
	Forward = 1,
	Right = 2,
	Backward = 3,
	Left = 4
};

namespace MovementData_
{
	void SendAnimationEvent(cd::Value<Actor>, const char *aeName, bool unsafe);
}
using MovementData_::SendAnimationEvent;

bool IsRaceFlagSet(TESRace *thisRace, uint32_t flag) {
	return (thisRace) ? (thisRace->data.raceFlags & flag) == flag : false;
}

bool IsPlayable(TESRace *race) {
	return IsRaceFlagSet(race, TESRace::kRace_Playable);
}

std::string GetRaceRandomAnim(TESRace *race);

struct EquipData {
	bool leftHand;
	bool rightHand;
};

struct NPCAttacks 
{
	std::list<AnimID> aeIDs;
	clock_t lastPush = 0;
};

std::queue<AnimID> pcAttacks;
std::map<uint32_t, NPCAttacks> npcAttacks;

const char *combatAnims[] = {
	"BashStart",
	"attackPowerStartForwardH2HRightHand",
	"AttackStartH2HRight",
	"AttackPowerStartInPlace",
	"AttackPowerStartForward",
	"AttackPowerStartRight",
	"AttackPowerStartBackward",
	"AttackPowerStartLeft",
	"AttackStart",
	"AttackPowerStartForwatdH2HRightHand",
	"AttackStartH2HRight",
	"AttackPowerStartInPlaceLeftHand",
	"AttackPowerStartForwardLeftHand",
	"AttackPowerStartRightLeftHand",
	"AttackPowerStartBackLeftHand",
	"AttackPowerStartLeftLeftHand",
	"AttackStartLeftHand",
	"AttackStartH2HLeft",
	"AttackStartDualWield",
	"AttackPowerStartH2HCombo",
	"AttackPowerStartDualWield",
	"IdleChairEnterInstant",
};
const size_t numCombatAnims = sizeof combatAnims / sizeof combatAnims[0];

std::map<AnimID, const char *> allAnims;

auto addCombatAnims = []() {
	static bool combatAnimsAdded = false;
	if (combatAnimsAdded == false)
	{
		for (AnimID i = 0; i != numCombatAnims; ++i)
			allAnims[i] = combatAnims[i];
		combatAnimsAdded = true;
	}
};

const char *GetAnimName(AnimID animID)
{
	try {
		addCombatAnims();
		return allAnims.at(animID);
	}
	catch (...) {
		ErrorHandling::SendError("ERROR:HitData Unknown anim ID");
		return "";
	}
}

AnimID GetAnimID(const std::string &name)
{
	addCombatAnims();
	for (auto &pair : allAnims)
	{
		if (pair.second == name)
			return pair.first;
	}
	ErrorHandling::SendError("ERROR:HitData Unknown anim name");
	return NULL;
}

const char *GetAEName(bool isRacePlayable, const EquipData &equipData, Direction dir, bool isLeftHand, bool isPowerAttack, bool isBashing, bool isDualAttack)
{
	if (!isRacePlayable)
		return "AttackStart";

	if (isBashing)
		return "BashStart";

	if (isDualAttack)
	{
		if (equipData.rightHand == 0)
			return "AttackPowerStartH2HCombo";
		return isPowerAttack ? "AttackPowerStartDualWield" : "AttackStartDualWield";
	}

	if (equipData.rightHand == 0 && equipData.leftHand == 0)
	{
		if (isPowerAttack)
			return "attackPowerStartForwardH2HRightHand";
		else
			return "AttackStartH2HRight";
	}

	if (!isLeftHand)
	{
		if (equipData.rightHand != 0)
		{
			if (isPowerAttack)
			{
				switch (dir)
				{
				case Direction::Standing:
					return "AttackPowerStartInPlace";
				case Direction::Forward:
					return "AttackPowerStartForward";
				case Direction::Right:
					return "AttackPowerStartRight";
				case Direction::Backward:
					return "AttackPowerStartBackward";
				case Direction::Left:
					return "AttackPowerStartLeft";
				}
			}
			return "AttackStart";
		}
		else
		{
			if (isPowerAttack)
				return "AttackPowerStartForwatdH2HRightHand";
			else
				return "AttackStartH2HRight";
		}
	}
	if (equipData.leftHand != 0)
	{
		if (isPowerAttack)
		{
			switch (dir)
			{
			case Direction::Standing:
				return "AttackPowerStartInPlaceLeftHand";
			case Direction::Forward:
				return "AttackPowerStartForwardLeftHand";
			case Direction::Right:
				return "AttackPowerStartRightLeftHand";
			case Direction::Backward:
				return "AttackPowerStartBackLeftHand";
			case Direction::Left:
				return "AttackPowerStartLeftLeftHand";
			}
		}
		return "AttackStartLeftHand";
	}
	return "AttackStartH2HLeft";
}

bool AnimData_::Apply(Actor *actor, AnimID animID, bool unsafe)
{
	try {
		if (!actor)
			return false;
		actor->race->data.unarmedDamage = 0;

		auto anim = GetAnimName(animID);
		if (!anim || strlen(anim) == 0)
			return false;
		if (animID == 8 && IsPlayable(actor->GetRace()) == false)
		{
			const auto str = GetRaceRandomAnim(actor->GetRace());
			SendAnimationEvent(actor, str.data(), unsafe);
		}
		else
			SendAnimationEvent(actor, anim, unsafe);
		return true;
	}
	catch (...) {
		return false;
	}
}

void AnimData_::RegisterAnimation(const std::string &animationEvent, AnimID animID)
{
	const size_t chars = animationEvent.size();
	auto str = new std::string(animationEvent);
	allAnims[animID] = str->data();
}

int32_t numSwings = 0;
clock_t lastMovementChange = NULL, lastWeapDrawnChange = NULL;

uint32_t GetDelayFor(Actor *actor)
{
	if (sd::GetActorValue(actor, "Stamina") == 0)
		return 0;

	if (lastWeapDrawnChange + 1000 >= clock())
	{
		float speed;
		static auto fsSpeedMult = BSFixedString("speedsampled");
		actor->GetAnimationVariableFloat(fsSpeedMult, speed);
		if (speed <= 0.1f)
			return 0;
	}
	return 133;
}

bool IsRightHandOnly() {
	return sd::GetEquippedWeapon(g_thePlayer, false) 
		&& !sd::GetEquippedWeapon(g_thePlayer, true);
}

bool IsDualWield() {
	return sd::GetEquippedWeapon(g_thePlayer, false) 
		&& sd::GetEquippedWeapon(g_thePlayer, true);
}

void PushAttackAnim(Actor *actor, AnimID animID)
{
	if (actor == nullptr)
		return;
	if (actor == g_thePlayer)
	{
		//ci::Chat::AddMessage(L"Push attack anim player");
		return pcAttacks.push(animID);
	}

	if (clock() - npcAttacks[actor->formID].lastPush > 300)
	{
		npcAttacks[actor->formID].lastPush = clock();
		if (npcAttacks[actor->formID].aeIDs.back() != animID)
		{
			//ci::Chat::AddMessage(L"Push attack anim");
			return npcAttacks[actor->formID].aeIDs.push_back(animID);
		}
	}
}

void OnWeapSwing(Actor *actor, bool isLeftHand, bool calledFrom_OnAnimationEvent = false, bool isDualAttack = false)
{
	if (actor == nullptr)
		return;

	EquipData equipData{ sd::GetEquippedWeapon(actor, true) != 0, sd::GetEquippedWeapon(actor, false) != 0};

	bool isBashing;
	static auto fsIsBashing = BSFixedString("IsBashing");
	actor->GetAnimationVariableBool(fsIsBashing, isBashing);

	const int32_t ms = calledFrom_OnAnimationEvent ? 0 : GetDelayFor(actor);

	float speed;
	static auto fsSpeedSampled = BSFixedString("speedsampled");
	actor->GetAnimationVariableFloat(fsSpeedSampled, speed);

	const uint32_t refID = actor->formID;
	auto f = [=] {
		auto actor = (Actor *)LookupFormByID(refID);
		if (actor == nullptr)
			return;

		float speed2;
		actor->GetAnimationVariableFloat(fsSpeedSampled, speed2);

		auto IsPowerAttacking = [&] {
			const bool powerAttack = sd::Obscript::IsPowerAttacking(actor) != 0.0
				|| (g_thePlayer == actor && speed2 != speed && abs(speed - speed2) < 20 && lastMovementChange + 300 < clock());
			return powerAttack;
		};
		const bool isPowerAttack = IsPowerAttacking() || isBashing;

		const auto direction = (Direction)(uint32_t)sd::Obscript::GetMovementDirection(g_thePlayer);

		const auto ae = GetAEName(IsPlayable(actor->GetRace()), equipData, direction, isLeftHand, isPowerAttack, isBashing, isDualAttack);
		const auto aeID = GetAnimID(ae);
		PushAttackAnim(actor, aeID);
	};
	if (ms > 0)
		SET_TIMER(ms, f);
	else
		SET_TIMER_LIGHT(1, f);
}

clock_t lastLRButtonsPressed = 0;

std::string GetRaceRandomAnim(TESRace *race)
{
	static bool sranded = false;
	if (!sranded)
	{
		sranded = true;
		srand(time(0));
	}

	if (race != nullptr)
	{
		auto mapBgs = dynamic_cast<BGSAttackDataForm *>(race);
		if (mapBgs != nullptr)
		{
			auto &map = mapBgs->attackDataMap->map;
			auto pairIter = map.begin();
			const float n = rand() % map.size();
			for (size_t i = 0; i != n; ++i)
				pairIter++;
			return pairIter->key.c_str();
		}
	}
	return "";
}

void AnimData_OnAnimationEvent(TESObjectREFR *source, std::string animEventName)
{
	SAFE_CALL("AnimData", [&] {
		enum {
			CalledFromOnAnimationEvent = TRUE
		};

		if (source == g_thePlayer)
		{
			SET_TIMER(0, [=] {
				if (animEventName == "weaponswing")
				{
					if (numSwings > 0)
					{
						numSwings--;
						if (!IsRightHandOnly())
							OnWeapSwing(g_thePlayer, false, CalledFromOnAnimationEvent);
					}
				}
				else if (animEventName == "weaponleftswing")
				{
					if (numSwings > 0)
					{
						numSwings--;
						if (!IsRightHandOnly())
						{
							const bool dualAttack = IsDualWield() && lastLRButtonsPressed + 500 > clock();
							OnWeapSwing(g_thePlayer, true, CalledFromOnAnimationEvent, dualAttack);
						}
					}
				}
			});
		}
		else
		{
			const auto refID = source->formID;
			SET_TIMER_LIGHT(1, [refID, animEventName] {
				auto actor = (Actor *)LookupFormByID(refID);
				if (actor->baseForm->formType == FormType::NPC)
				{
					static dlf_mutex m;
					std::lock_guard<dlf_mutex> l(m);
					static std::map<uint32_t, clock_t> last;

					auto gc = [] {
						if (last.size() > 1000)
							last.clear();
					};
					if (rand() % 10 == 0)
						gc();

					auto race = actor->GetRace();
					if (IsPlayable(race))
					{
						const bool isLeft = animEventName == "weaponleftswing";
						if (clock() - last[actor->formID] > 400)
						{
							last[actor->formID] = clock();
							OnWeapSwing(actor, isLeft, true, false);
						}
					}
					else
					{
						if (animEventName != "weaponswing")
							return;
						if (clock() - last[actor->formID] > 400)
						{
							last[actor->formID] = clock();
							OnWeapSwing(actor, false, true, false);
						}
					}
				}
			});
		}
	});
}


std::shared_ptr<AnimID> AnimData_::UpdatePlayer()
{
	auto pc = g_thePlayer;
	static bool isBashingWas = false;
	bool isBashing = false;
	pc->GetAnimationVariableBool("IsBashing", isBashing);
	if (isBashingWas != isBashing)
	{
		isBashingWas = isBashing;
		if(isBashing)
			OnWeapSwing(g_thePlayer, true);
	}

	std::shared_ptr<AnimID> result;
	if (!pcAttacks.empty())
	{
		result = std::make_shared<AnimID>(pcAttacks.front());
		pcAttacks.pop();
	}

	static UInt32 attackState = 0;
	const auto newAttackState = (UInt32)sd::Obscript::GetAttackState(pc);
	if (attackState != newAttackState)
	{
		const bool dualAttack = sd::GetKeyPressed(VK_LBUTTON) && sd::GetKeyPressed(VK_RBUTTON);
		if (newAttackState == 1)
		{
			numSwings++;
		}
		if (IsRightHandOnly())
		{
			enum {
				HoldingSwing = 1
			};
			if (newAttackState == HoldingSwing)
			{
				numSwings++;
				SET_TIMER(0, [=] {
					bool isLeft = !!sd::GetEquippedWeapon(g_thePlayer, true);
					SET_TIMER(min(5, GetDelayFor(g_thePlayer)), [=] {
						OnWeapSwing(g_thePlayer, isLeft, false, dualAttack);
					});
				});
			}
		}
		attackState = newAttackState;
	}

	static ci::MovementData mdWas;
	ci::MovementData md = MovementData_::GetFromPlayer();
	if (md.direction != mdWas.direction
		|| md.isFirstPerson != mdWas.isFirstPerson
		|| md.isBlocking != mdWas.isBlocking
		|| md.isSneaking != mdWas.isSneaking
		|| md.runMode != mdWas.runMode)
	{
		lastMovementChange = clock();
	}
	if (md.isWeapDrawn != mdWas.isWeapDrawn)
	{
		lastWeapDrawnChange = clock();
	}
	mdWas = md;

	if (sd::GetKeyPressed(VK_LBUTTON) && sd::GetKeyPressed(VK_RBUTTON))
		lastLRButtonsPressed = clock();
	return result;
}

std::shared_ptr<AnimID> AnimData_::UpdateActor(uint32_t actorID)
{
	std::shared_ptr<AnimID> result;
	try {
		auto &aeIDs = npcAttacks[actorID].aeIDs;
		if (aeIDs.empty())
			throw std::logic_error("empty aeIDs");
		const auto aeID = aeIDs.front();
		aeIDs.pop_front();
		result.reset(new AnimID(aeID));
	}
	catch (...) {
		result = nullptr;
	}
	return result;
}

void CollectGarbage()
{
	SET_TIMER_LIGHT(1, [] {
		std::vector<uint32_t> keysToErase;
		for (auto &pair : npcAttacks)
		{
			if (LookupFormByID(pair.first) == nullptr)
				keysToErase.push_back(pair.first);
		}
		for (auto key : keysToErase)
			npcAttacks.erase(key);
	});
}

bool AnimData_::IsPowerAttack(AnimID hitAnimID)
{
	const std::string name = GetAnimName(hitAnimID);
	return name.find("Power") != name.npos || name.find("Bash") != name.npos;
}

AnimData_::AnimSource AnimData_::GetAnimSource(AnimID animID)
{
	enum {
		MaxPlayable = 21,
	};
	if (animID <= MaxPlayable)
		return AnimSource::PlayableRace;
	return AnimSource::Server;
}

void AnimData_::Register(Actor *actor)
{
	CollectGarbage(); // should be called sometimes

	if (actor)
	{
		// Legacy (< 0.10)
		cd::SendAnimationEvent(actor, "Skymp_Register");
		// Calls RegisterForAnimationEvent() for combat animation events and RegisterForActorAction(1..4)
		// Implemented in Costile.psc
	}
}