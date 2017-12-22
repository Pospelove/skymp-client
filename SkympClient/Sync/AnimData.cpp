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

struct EquipData {
	bool leftHand;
	bool rightHand;
};

std::queue<uint8_t> pcAttacks;

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

const char *GetAEName(const EquipData &equipData, Direction dir, bool isLeftHand, bool isPowerAttack, bool isBashing, bool isDualAttack)
{
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

bool AnimData_::Apply(Actor *actor, AnimID hitAnimID, bool unsafe)
{
	try {
		if (!actor)
			return false;
		actor->race->data.unarmedDamage = 0;

		auto anim = GetAnimName(hitAnimID);
		if (!anim || strlen(anim) == 0)
			return false;
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

uint32_t GetDelay()
{
	if (lastWeapDrawnChange + 1000 >= clock())
	{
		float speed;
		g_thePlayer->GetAnimationVariableFloat(BSFixedString("speedsampled"), speed);
		if (speed <= 0.1f)
			return 0;
	}
	return 133;
}

bool IsRightHandOnly()
{
	return sd::GetEquippedWeapon(g_thePlayer, false) && !sd::GetEquippedWeapon(g_thePlayer, true);
}

bool IsDualWield()
{
	return sd::GetEquippedWeapon(g_thePlayer, false) && sd::GetEquippedWeapon(g_thePlayer, true);
}

void OnWeapSwing(bool isLeftHand, bool calledFrom_OnAnimationEvent = false, bool isDualAttack = false)
{
	auto pc = g_thePlayer;
	EquipData equipData{ sd::GetEquippedWeapon(pc, true) != 0, sd::GetEquippedWeapon(pc, false) != 0};

	bool isBashing;
	pc->GetAnimationVariableBool(BSFixedString("IsBashing"), isBashing);

	int32_t ms = GetDelay();
	if (sd::GetActorValue(pc, "Stamina") == 0
		|| calledFrom_OnAnimationEvent)
		ms = 0;

	float speed;
	pc->GetAnimationVariableFloat(BSFixedString("speedsampled"), speed);

	auto f = [=] {

		float speed2;
		pc->GetAnimationVariableFloat(BSFixedString("speedsampled"), speed2);

		auto IsPowerAttacking = [&] {
			const bool powerAttack = sd::Obscript::IsPowerAttacking(pc) != 0.0
				|| (speed2 != speed && abs(speed - speed2) < 20 && lastMovementChange + 300 < clock());
			return powerAttack;
		};
		const bool isPowerAttack = IsPowerAttacking() || isBashing;

		const auto direction = (Direction)(uint32_t)sd::Obscript::GetMovementDirection(g_thePlayer);

		auto ae = GetAEName(equipData, direction, isLeftHand, isPowerAttack, isBashing, isDualAttack);
		//ci::Chat::AddMessage(L"out >> " + StringToWstring(ae));

		pcAttacks.push(GetAnimID(ae));
	};
	if (ms > 0)
		SET_TIMER(ms, f);
	else
		f();
}

clock_t lastLRButtonsPressed = 0;

void AnimData_OnAnimationEvent(TESObjectREFR *source, std::string animEventName)
{
	enum {
		CalledFromOnAnimationEvent = TRUE
	};
	SET_TIMER(0, [=] {
		if (source == g_thePlayer)
		{
			if (animEventName == "weaponswing")
			{
				if (numSwings > 0)
				{
					numSwings--;
					if (!IsRightHandOnly())
						OnWeapSwing(false, CalledFromOnAnimationEvent);
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
						OnWeapSwing(true, CalledFromOnAnimationEvent, dualAttack);
					}
				}
			}
			else if (animEventName == "idlechairenterstart")
			{
				ci::Chat::AddMessage(L"Hey! Detected");
			}
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
			OnWeapSwing(true);
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
					SET_TIMER(min(5, GetDelay()), [=] {
						OnWeapSwing(isLeft, false, dualAttack);
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

bool AnimData_::IsPowerAttack(AnimID hitAnimID)
{
	const std::string name = GetAnimName(hitAnimID);
	return name.find("Power") != name.npos || name.find("Bash") != name.npos;
}

void AnimData_::Register()
{
	// Legacy (< 0.10)
	cd::SendAnimationEvent(g_thePlayer, "Skymp_Register"); 
	// Calls RegisterForAnimationEvent() for combat animation events and RegisterForActorAction(1..4)
	// Implemented in Costile.psc

	sd::RegisterForAnimationEvent(g_thePlayer, g_thePlayer, "IdleChairEnterStart");
}