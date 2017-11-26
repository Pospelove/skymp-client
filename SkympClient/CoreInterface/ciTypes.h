#pragma once

#include "..\Skyrim\include\Skyrim\NetImmerse\NiPoint3.h"
#include <array>
#include <vector>

namespace ci
{
	struct LookData
	{
		enum {
			NumOptions = 19,
			NumPresets = 4
		};

		struct Color
		{
			uint8_t r; 
			uint8_t g;
			uint8_t b; 
			uint8_t a;
		};

		struct TintMask
		{
			uint8_t tintMaskTextureID;
			uint32_t tintType;
			Color color;
			float alpha;
		};

		uint8_t isFemale = 0;
		uint32_t raceID = 0;
		uint8_t weight = 0;
		Color skinColor;
		Color hairColor;
		std::vector<uint32_t> headpartIDs;
		std::vector<TintMask> tintmasks;
		std::array<float, NumOptions> options;
		std::array<uint32_t, NumPresets> presets;
		uint32_t headTextureSetID = 0;

		bool isEmpty() const {
			return !raceID;
		}
	};

	bool operator==(const LookData &l, const LookData &r);
	bool operator!=(const LookData &l, const LookData &r);

	bool operator==(const LookData::TintMask &l, const LookData::TintMask &r);
	bool operator!=(const LookData::TintMask &l, const LookData::TintMask &r);

	struct MovementData
	{
		MovementData() {
			this->castStage = { CastStage::None, CastStage::None };
		}

		enum class RunMode : uint8_t
		{
			Standing = 0,
			Walking = 1,
			Running = 2
		};

		enum class JumpStage : uint8_t
		{
			Landed = 0,
			Jumping = 1,
			Falling = 2
		};

		enum class CastStage : uint8_t
		{
			None = 0,
			Casting = 1,
			Fire = 2,
			Release = 3,
		};

		NiPoint3 pos = { 0,0,0 };
		uint16_t angleZ = 0;
		uint16_t direction = 0;
		uint16_t speedSampled = 0;
		int16_t aimingAngle = 0;
		uint16_t attackState = 0;
		RunMode runMode = RunMode::Standing;
		JumpStage jumpStage = JumpStage::Landed;
		std::array<CastStage, 2> castStage;
		bool isInJumpState = false;
		bool isJumping = false;
		bool isSprinting = false;
		bool isSneaking = false;
		bool isWeapDrawn = false;
		bool isBlocking = false;
		bool isFirstPerson = false;
		bool isSwimming = false;

		bool isEmpty() {
			return pos == NiPoint3{0, 0, 0};
		}
	};
}