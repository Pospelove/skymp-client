#include "../stdafx.h"
#include "HitData.h"
#include "MovementData.h"
#include "../ScriptDragon/obscript.h"
#include "Skyrim/Events/ScriptEvent.h"
#include <queue>

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
	void SendAnimationEvent(Actor *actor, const char *aeName, bool unsafe);
}
using MovementData_::SendAnimationEvent;

struct EquipData {
	bool leftHand;
	bool rightHand;
};

std::queue<uint8_t> pcAttacks;

const char *anims[] = {
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
	"AttackPowerStartDualWield"
};
const size_t numAnims = sizeof anims / sizeof anims[0];

const char *GetAnimName(uint8_t animID)
{
	if (animID < numAnims)
		return anims[animID];
	ErrorHandling::SendError("ERROR:HitData Unknown anim ID");
	return "";
}

uint8_t GetAnimID(const std::string &name)
{
	for (size_t i = 0; i != numAnims; ++i)
	{
		if (anims[i] == name)
			return i;
	}
	ErrorHandling::SendError("ERROR:HitData Unknown anim name");
	return 0;
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

void HitData_::Apply(Actor *actor, uint8_t hitAnimID, bool unsafe)
{
	if (!actor)
		return;
	actor->race->data.unarmedDamage = 0;

	//unsafe = true; // PVS

	auto anim = GetAnimName(hitAnimID);
	//ci::Chat::AddMessage(L"in >> " + StringToWstring(anim));
	SendAnimationEvent(actor, anim, unsafe);
	//sd::StartCombat(actor, g_thePlayer);
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

void HitData_OnAnimationEvent(TESObjectREFR *source, std::string animEventName)
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
		}
	});
}


std::shared_ptr<uint8_t> HitData_::UpdatePlayer()
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

	std::shared_ptr<uint8_t> result;
	if (!pcAttacks.empty())
	{
		result = std::make_shared<uint8_t>(pcAttacks.front());
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

bool HitData_::IsPowerAttack(uint8_t hitAnimID)
{
	const std::string name = GetAnimName(hitAnimID);
	return name.find("Power") != name.npos || name.find("Bash") != name.npos;
}