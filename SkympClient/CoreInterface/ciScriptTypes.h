#pragma once
// To be included into ciScript.cpp

namespace Convert
{
	using RunMode = ci::MovementData::RunMode;
	using JumpStage = ci::MovementData::JumpStage;
	using CastStage = ci::MovementData::CastStage;
	using MountStage = ci::MovementData::MountStage;

	template<class T>
	std::string ToString(const T &source)
	{
		wtf();
	}

	template<class T>
	T FromString(const std::string &source)
	{
		wtf();
	}

	template <>
	std::string ToString<RunMode>(const RunMode &source)
	{
		switch (source)
		{
		case RunMode::Running:
			return "Running";
		case RunMode::Standing:
			return "Standing";
		case RunMode::Walking:
			return "Walking";
		default:
			return "";
		}
	}

	template <>
	RunMode FromString(const std::string &src)
	{
		if (src == "Running")
			return RunMode::Running;
		if (src == "Standing")
			return RunMode::Standing;
		if (src == "Walking")
			return RunMode::Walking;
		return RunMode::Standing;
	}

	template <>
	std::string ToString(const JumpStage &source)
	{
		switch (source)
		{
		case JumpStage::Falling:
			return "Falling";
		case JumpStage::Jumping:
			return "Jumping";
		case JumpStage::Landed:
			return "Landed";
		default:
			return "";
		}
	}

	template <>
	JumpStage FromString(const std::string &source)
	{
		if (source == "Falling")
			return JumpStage::Falling;
		if (source == "Jumping")
			return JumpStage::Jumping;
		if (source == "Landed")
			return JumpStage::Landed;
		return JumpStage::Landed;
	}

	template <>
	std::string ToString(const CastStage &source)
	{
		switch (source)
		{
		case CastStage::Casting:
			return "Casting";
		case CastStage::Fire:
			return "Fire";
		case CastStage::None:
			return "None";
		case CastStage::Release:
			return "Release";
		default:
			return "";
		}
	}

	template <>
	CastStage FromString(const std::string &source)
	{
		if (source == "Casting")
			return CastStage::Casting;
		if (source == "Fire")
			return CastStage::Fire;
		if (source == "None")
			return CastStage::None;
		if (source == "Release")
			return CastStage::Release;
		return CastStage::None;
	}

	template <>
	std::string ToString(const MountStage &source)
	{
		switch (source)
		{
		case MountStage::Dismounting:
			return "Dismounting";
		case MountStage::Mounting:
			return "Mounting";
		case MountStage::None:
			return "None";
		case MountStage::OnMount:
			return "OnMount";
		default:
			return "";
		}
	}

	template <>
	MountStage FromString(const std::string &source)
	{
		if (source == "Dismounting")
			return MountStage::Dismounting;
		if (source == "Mounting")
			return MountStage::Mounting;
		if (source == "OnMount")
			return MountStage::OnMount;
		return MountStage::None;
	}

	template <class T>
	luabridge::LuaRef ToTable(lua_State *, const T &)
	{
		wtf();
	}

	template <>
	luabridge::LuaRef ToTable(lua_State *L, const ci::MovementData &movement)
	{
		luabridge::LuaRef result{ L };

		result = luabridge::newTable(L);

		result["x"] = movement.pos.x;
		result["y"] = movement.pos.y;
		result["z"] = movement.pos.z;

		result["angleZ"] = movement.angleZ;
		result["direction"] = movement.direction;
		result["speedSampled"] = movement.speedSampled;
		result["aimingAngle"] = movement.aimingAngle;
		result["attackState"] = movement.attackState;
		result["runMode"] = ToString(movement.runMode);
		result["jumpStage"] = ToString(movement.jumpStage);
		result["castStageR"] = ToString(movement.castStage[0]);
		result["castStageL"] = ToString(movement.castStage[1]);

		result["mountStage"] = ToString(movement.mountStage);
		
		result["isInJumpState"] = movement.isInJumpState;
		result["isJumping"] = movement.isJumping;
		result["isSprinting"] = movement.isSprinting;
		result["isSneaking"] = movement.isSneaking;
		result["isWeapDrawn"] = movement.isWeapDrawn;
		result["isBlocking"] = movement.isBlocking;
		result["isFirstPerson"] = movement.isFirstPerson;
		result["isSwimming"] = movement.isSwimming;
		result["isRPressed"] = movement.isRPressed;

		return result;
	}

	template <>
	luabridge::LuaRef ToTable(lua_State *L, const ci::LookData &look)
	{
		luabridge::LuaRef result{ L };

		result = luabridge::newTable(L);

		static_assert(sizeof ci::LookData::Color == sizeof uint32_t, "");

		result["hairColor"] = (uint32_t &)look.hairColor;
		result["headTextureSetID"] = look.headTextureSetID;
		result["isFemale"] = (bool)look.isFemale;
		result["raceID"] = look.raceID;
		result["skinColor"] = (uint32_t &)look.skinColor;
		result["weight"] = look.weight;

		result["headpartIDs"] = luabridge::newTable(L);
		for (auto &value : look.headpartIDs)
			result["headpartIDs"].append(value);

		result["options"] = luabridge::newTable(L);
		for (auto &value : look.options)
			result["options"].append(value);

		result["presets"] = luabridge::newTable(L);
		for (auto &value : look.presets)
			result["presets"].append(value);

		result["tintmasks"] = luabridge::newTable(L);
		for (auto &value : look.tintmasks)
		{
			std::stringstream ss;
			ss << "return {tintType = " << (uint32_t)value.tintType << ", tintMaskTextureID = " << (uint32_t)value.tintMaskTextureID << ", color = " << (uint32_t &)value.color << ", alpha = " << (uint32_t)(100 * value.alpha) << " / 100 }";
			bool suc = !luaL_dostring(L, ss.str().data());
			if (!suc)
			{
				ErrorHandling::SendError("FATAL:Lua luaL_dostring failed %s", ss.str().data());
				std::exit(0);
			}
			auto ref = luabridge::LuaRef::fromStack(L, -1);
			if (ref.isTable() == false)
			{
				ErrorHandling::SendError("FATAL:Lua wtf");
				std::exit(0);
			}
			result["tintmasks"].append(ref);
		}

		return result;
	}

	template <class T>
	T FromTable(const luabridge::LuaRef &table)
	{
		wtf();
	}

	template <>
	ci::MovementData FromTable(const luabridge::LuaRef &table)
	{
		ci::MovementData mov;

		if (table.isTable())
		{
			mov.pos.x = table["x"].isNumber() ? (float)table["x"] : 0;
			mov.pos.y = table["y"].isNumber() ? (float)table["y"] : 0;
			mov.pos.z = table["z"].isNumber() ? (float)table["z"] : 0;

			mov.angleZ = table["angleZ"].isNumber() ? (uint16_t)table["angleZ"] : 0;
			mov.direction = table["direction"].isNumber() ? (uint16_t)table["direction"] : 0;
			mov.speedSampled = table["speedSampled"].isNumber() ? (uint16_t)table["speedSampled"] : 0;
			mov.aimingAngle = table["aimingAngle"].isNumber() ? (uint16_t)table["aimingAngle"] : 0;
			mov.attackState = table["attackState"].isNumber() ? (uint16_t)table["attackState"] : 0;

			mov.runMode = table["runMode"].isString() ? FromString<RunMode>(table["runMode"]) : RunMode::Standing;
			mov.jumpStage = table["jumpStage"].isString() ? FromString<JumpStage>(table["jumpStage"]) : JumpStage::Landed;
			mov.mountStage = table["mountStage"].isString() ? FromString<MountStage>(table["mountStage"]) : MountStage::None;
			mov.castStage[0] = table["castStageR"].isString() ? FromString<CastStage>(table["castStageR"]) : CastStage::None;
			mov.castStage[1] = table["castStageL"].isString() ? FromString<CastStage>(table["castStageL"]) : CastStage::None;

			mov.isInJumpState = (bool)table["isInJumpState"];
			mov.isJumping = (bool)table["isJumping"];
			mov.isSprinting = (bool)table["isSprinting"];
			mov.isSneaking = (bool)table["isSneaking"];
			mov.isWeapDrawn = (bool)table["isWeapDrawn"];
			mov.isBlocking = (bool)table["isBlocking"];
			mov.isFirstPerson = (bool)table["isFirstPerson"];
			mov.isSwimming = (bool)table["isSwimming"];
			mov.isRPressed = (bool)table["isRPressed"];
		}
		return mov;
	}

	template <>
	ci::LookData FromTable(const luabridge::LuaRef &table)
	{
		ci::LookData look;

		if (table.isTable())
		{
			class ColorUnion 
			{
			public:
				ColorUnion(uint32_t uint) {
					this->uint = uint;
				}

				operator ci::LookData::Color() const {
					return this->c;
				}

			private:

				union {
					uint32_t uint;
					ci::LookData::Color c;
				};
			};

			look.hairColor = ColorUnion(table["hairColor"].isNumber() ? table["hairColor"] : 0);
			look.skinColor = ColorUnion(table["skinColor"].isNumber() ? table["skinColor"] : 0);
			look.headTextureSetID = table["headTextureSetID"].isNumber() ? table["headTextureSetID"] : 0;
			look.isFemale = (bool)table["isFemale"];
			look.raceID = table["raceID"].isNumber() ? table["raceID"] : 0;
			look.weight = table["weight"].isNumber() ? table["weight"] : 0;

			{
				const luabridge::LuaRef tableTintmasks = table["tintmasks"];
				if (tableTintmasks.isTable())
				{
					for (uint32_t n = 1; n <= tableTintmasks.length(); ++n)
					{
						const luabridge::LuaRef entry = tableTintmasks[n];
						if (entry.isTable())
						{
							ci::LookData::TintMask tintmask;
							tintmask.alpha = entry["alpha"].isNumber() ? entry["alpha"].cast<float>() : 0.0f;
							tintmask.color = (entry["color"].isNumber()) ? (ColorUnion((uint32_t)entry["color"])) : (0);
							tintmask.tintMaskTextureID = entry["tintMaskTextureID"].isNumber() ? entry["tintMaskTextureID"] : 0;
							tintmask.tintType = entry["tintType"].isNumber() ? entry["tintType"] : 0;
							look.tintmasks.push_back(tintmask);
						}
					}
				}
			}
			{
				const luabridge::LuaRef tableHeadpartIDs = table["headpartIDs"];
				if (tableHeadpartIDs.isTable())
				{
					for (uint32_t n = 1; n <= tableHeadpartIDs.length(); ++n)
					{
						const auto value = tableHeadpartIDs[n];
						if (value.isNumber())
							look.headpartIDs.push_back(value);
					}
				}
			}


#define EXTRACT_NUMBER_ARRAY(dest, source) [&] { const luabridge::LuaRef array = source;\
				if (array.isTable())\
				{\
					for (uint32_t n = 1; n <= array.length(); ++n)\
					{\
						const auto value = array[n];\
						if (value.isNumber())\
						{\
							try {\
								dest.at(n - 1) = value;\
							}\
							catch (...) {\
								break;\
							}\
						}\
					}\
				}\
			};

			EXTRACT_NUMBER_ARRAY(look.options, table["options"]);
			EXTRACT_NUMBER_ARRAY(look.presets, table["presets"]);
		}

		return look;
	}
}