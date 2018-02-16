#pragma once

#ifndef CI_IMPL
#error "Only internal use of this header allowed"
#endif

inline std::string GetEffectAVName(uint32_t id)
{
	static const std::vector<std::string> names = {
		"kActorValue_Aggression",
		"kActorValue_Confidence",
		"kActorValue_Energy",
		"kActorValue_Morality",
		"kActorValue_Mood",
		"kActorValue_Assistance",
		"kActorValue_Onehanded",
		"kActorValue_Twohanded",
		"kActorValue_Marksman",
		"kActorValue_Block",
		"kActorValue_Smithing",
		"kActorValue_HeavyArmor",
		"kActorValue_LightArmor",
		"kActorValue_Pickpocket",
		"kActorValue_Lockpicking",
		"kActorValue_Sneak",
		"kActorValue_Alchemy",
		"kActorValue_Speechcraft",
		"kActorValue_Alteration",
		"kActorValue_Conjuration",
		"kActorValue_Destruction",
		"kActorValue_Illusion",
		"kActorValue_Restoration",
		"kActorValue_Enchanting",
		"kActorValue_Health",
		"kActorValue_Magicka",
		"kActorValue_Stamina",
		"kActorValue_Healrate",
		"kActorValue_MagickaRate",
		"kActorValue_StaminaRate",
		"kActorValue_SpeedMult",
		"kActorValue_InventoryWeight",
		"kActorValue_CarryWeight",
		"kActorValue_CritChance",
		"kActorValue_MeleeDamage",
		"kActorValue_UnarmedDamage",
		"kActorValue_Mass",
		"kActorValue_VoicePoints",
		"kActorValue_VoiceRate",
		"kActorValue_DamageResist",
		"kActorValue_PoisonResist",
		"kActorValue_FireResist",
		"kActorValue_ElectricResist",
		"kActorValue_FrostResist",
		"kActorValue_MagicResist",
		"kActorValue_DiseaseResist",
		"kActorValue_PerceptionCondition",
		"kActorValue_EnduranceCondition",
		"kActorValue_LeftAttackCondition",
		"kActorValue_RightAttackCondition",
		"kActorValue_LeftMobilityCondition",
		"kActorValue_RightMobilityCondition",
		"kActorValue_BrainCondition",
		"kActorValue_Paralysis",
		"kActorValue_Invisibility",
		"kActorValue_NightEye",
		"kActorValue_DetectLifeRange",
		"kActorValue_WaterBreathing",
		"kActorValue_WaterWalking",
		"kActorValue_IgnoreCrippledLimbs",
		"kActorValue_Fame",
		"kActorValue_Infamy",
		"kActorValue_JumpingBonus",
		"kActorValue_WardPower",
		"kActorValue_RightItemCharge",
		"kActorValue_ArmorPerks",
		"kActorValue_ShieldPerks",
		"kActorValue_WardDeflection",
		"kActorValue_Variable01",
		"kActorValue_Variable02",
		"kActorValue_Variable03",
		"kActorValue_Variable04",
		"kActorValue_Variable05",
		"kActorValue_Variable06",
		"kActorValue_Variable07",
		"kActorValue_Variable08",
		"kActorValue_Variable09",
		"kActorValue_Variable10",
		"kActorValue_BowSpeedBonus",
		"kActorValue_FavorActive",
		"kActorValue_FavorSperDay",
		"kActorValue_FavorSperDaytimer",
		"kActorValue_LeftItemCharge",
		"kActorValue_AbsorbChance",
		"kActorValue_Blindness",
		"kActorValue_WeaponSpeedMult",
		"kActorValue_ShoutRecoveryMult",
		"kActorValue_BowStaggerBonus",
		"kActorValue_Telekinesis",
		"kActorValue_FavorPointsBonus",
		"kActorValue_LastBribedIntimidated",
		"kActorValue_LastFlattered",
		"kActorValue_MovementNoiseMult",
		"kActorValue_BypassVendorStolenCheck",
		"kActorValue_BypassVendorKeywordCheck",
		"kActorValue_WaitingForPlayer",
		"kActorValue_OnehandedMod",
		"kActorValue_TwohandedMod",
		"kActorValue_MarksmanMod",
		"kActorValue_BlockMod",
		"kActorValue_SmithingMod",
		"kActorValue_HeavyArmorMod",
		"kActorValue_LightArmorMod",
		"kActorValue_PickpocketMod",
		"kActorValue_LockpickingMod",
		"kActorValue_SneakMod",
		"kActorValue_AlchemyMod",
		"kActorValue_SpeechcraftMod",
		"kActorValue_AlterationMod",
		"kActorValue_ConjurationMod",
		"kActorValue_DestructionMod",
		"kActorValue_IllusionMod",
		"kActorValue_RestorationMod",
		"kActorValue_EnchantingMod",
		"kActorValue_OnehandedSkillAdvance",
		"kActorValue_TwohandedSkillAdvance",
		"kActorValue_MarksmanSkillAdvance",
		"kActorValue_BlockSkillAdvance",
		"kActorValue_SmithingSkillAdvance",
		"kActorValue_HeavyArmorSkillAdvance",
		"kActorValue_LightArmorSkillAdvance",
		"kActorValue_PickpocketSkillAdvance",
		"kActorValue_LockpickingSkillAdvance",
		"kActorValue_SneakSkillAdvance",
		"kActorValue_AlchemySkillAdvance",
		"kActorValue_SpeechcraftSkillAdvance",
		"kActorValue_AlterationSkillAdvance",
		"kActorValue_ConjurationSkillAdvance",
		"kActorValue_DestructionSkillAdvance",
		"kActorValue_IllusionSkillAdvance",
		"kActorValue_RestorationSkillAdvance",
		"kActorValue_EnchantingSkillAdvance",
		"kActorValue_LeftWeaponSpeedMult",
		"kActorValue_DragonSouls",
		"kActorValue_CombatHealthRegenMult",
		"kActorValue_OnehandedPowerMod",
		"kActorValue_TwohandedPowerMod",
		"kActorValue_MarksmanPowerMod",
		"kActorValue_BlockPowerMod",
		"kActorValue_SmithingPowerMod",
		"kActorValue_HeavyarmorPowerMod",
		"kActorValue_LightarmorPowerMod",
		"kActorValue_PickpocketPowerMod",
		"kActorValue_LockpickingPowerMod",
		"kActorValue_SneakPowerMod",
		"kActorValue_AlchemyPowerMod",
		"kActorValue_SpeechcraftPowerMod",
		"kActorValue_AlterationPowerMod",
		"kActorValue_ConjurationPowerMod",
		"kActorValue_DestructionPowerMod",
		"kActorValue_IllusionPowerMod",
		"kActorValue_RestorationPowerMod",
		"kActorValue_EnchantingPowerMod",
		"kActorValue_Dragonrend",
		"kActorValue_AttackDamageMult",
		"kActorValue_HealRateMult",
		"kActorValue_MagickaRateMult",
		"kActorValue_StaminaRateMult",
		"kActorValue_WerewolfPerks",
		"kActorValue_VampirePerks",
		"kActorValue_GrabActorOffset",
		"kActorValue_Grabbed",
		"kActorValue_Deprecated05",
		"kActorValue_ReflectDamage"
	};
	try {
		auto str = names.at(id);
		str = { str.begin() + 12, str.end() };
		return str;
	}
	catch (...) {
		return "";
	}
}

class EffectArchetype
{
public:
	enum
	{
		ValueMod = 0,
		Script,
		Dispel,
		CureDisease,
		Absorb,
		DualValueMod,
		Calm,
		Demoralize,
		Frenzy,
		Disarm,
		CommandSummoned,
		Invisibility,
		Light,
		Lock = 15,
		Open,
		BoundWeapon,
		SummonCreature,
		DetectLife,
		Telekinesis,
		Paralysis,
		Reanimate,
		SoulTrap,
		TurnUndead,
		Guide,
		WerewolfFeed,
		CureParalysis,
		CureAddiction,
		CurePoison,
		Concussion,
		ValueAndParts,
		AccumulateMagnitude,
		Stagger,
		PeakValueMod,
		Cloak,
		Werewolf,
		SlowTime,
		Rally,
		EnhanceWeapon,
		SpawnHazard,
		Etherealize,
		Banish,
		Disguise = 44,
		GrabActor,
		VampireLord,
	};

	EffectArchetype() {
		Init();
	}

	EffectArchetype(uint8_t classID) {
		Init();
		this->typeID = classID;
	}

	EffectArchetype(const std::string &className) {
		Init();
		try {
			this->typeID = this->idByName.at(className);
		}
		catch (...) {
			this->typeID = ValueMod;
		};
	}

	uint8_t GetID() const {
		return this->typeID;
	}

	std::string GetName() const {
		try {
			return nameByID.at(this->typeID);
		}
		catch (...) {
			return EffectArchetype(ValueMod).GetName();
		}
	}

private:
	void Init() {
		const static std::map<std::string, uint8_t> idByName {
			{ "ValueMod", ValueMod },
			{ "Script", Script },
			{ "Dispel", Dispel },
			{ "CureDisease", CureDisease },
			{ "Absorb", Absorb },
			{ "DualValueMod", DualValueMod },
			{ "Calm", Calm },
			{ "Demoralize", Demoralize },
			{ "Frenzy", Frenzy },
			{ "Disarm", Disarm },
			{ "CommandSummoned", CommandSummoned },
			{ "Invisibility", Invisibility },
			{ "Light", Light },
			{ "Lock", Lock },
			{ "Open", Open },
			{ "BoundWeapon", BoundWeapon },
			{ "SummonCreature", SummonCreature },
			{ "DetectLife", DetectLife },
			{ "Telekinesis", Telekinesis },
			{ "Paralysis", Paralysis },
			{ "Reanimate", Reanimate },
			{ "SoulTrap", SoulTrap },
			{ "TurnUndead", TurnUndead },
			{ "Guide", Guide },
			{ "WerewolfFeed", WerewolfFeed },
			{ "CureParalysis", CureParalysis },
			{ "CureAddiction", CureAddiction },
			{ "CurePoison", CurePoison },
			{ "Concussion", Concussion },
			{ "ValueAndParts", ValueAndParts },
			{ "AccumulateMagnitude", AccumulateMagnitude },
			{ "Stagger", Stagger },
			{ "PeakValueMod", PeakValueMod },
			{ "Cloak", Cloak },
			{ "Werewolf", Werewolf },
			{ "SlowTime", SlowTime },
			{ "Rally", Rally },
			{ "EnhanceWeapon", EnhanceWeapon },
			{ "SpawnHazard", SpawnHazard },
			{ "Etherealize", Etherealize },
			{ "Banish", Banish },
			{ "Disguise", Disguise },
			{ "GrabActor", GrabActor },
			{ "VampireLord", VampireLord }
		};
		this->idByName = idByName;
		this->nameByID.clear();
		for (auto &pair : this->idByName)
			this->nameByID.insert({ pair.second, pair.first });
	}

	uint8_t typeID = ValueMod;

	std::map<std::string, uint8_t> idByName;
	std::map<uint8_t, std::string> nameByID;
};

class ItemClass
{
public:
	enum {
		Misc = 0x00,
		Weapon = 0x01,
		Armor = 0x02,
		Ammo = 0x03,
		Ingredient = 0x04,
		Potion = 0x05,
		SoulGem = 0x06,
		Key = 0x07,
	};

	ItemClass() {
		Init();
	}

	ItemClass(uint8_t classID) {
		Init();
		this->classID = classID;
	}

	ItemClass(const std::string &className) {
		Init();
		try {
			this->classID = this->idByName->at(className);
		}
		catch (...) {
			this->classID = Misc;
		};
	}

	uint8_t GetID() const {
		return this->classID;
	}

	std::string GetName() const {
		try {
			return nameByID->at(this->classID);
		}
		catch (...) {
			return ItemClass(Misc).GetName();
		}
	}

private:
	void Init() {
		static std::map<std::string, uint8_t> _idByName;
		static std::map<uint8_t, std::string> _nameByID;
		if (_idByName.empty())
		{
			const std::list<std::pair<std::string, int32_t>> idByNameList = {
				{ "Misc",			Misc },
				{ "Weapon",			Weapon },
				{ "Armor",			Armor },
				{ "Ammo",			Ammo },
				{ "Ingredient",		Ingredient },
				{ "Potion",			Potion },
				{ "SoulGem",		SoulGem },
				{ "Key",			Key }
			};
			for (auto &listItem : idByNameList)
				_idByName[listItem.first] = listItem.second;
			_nameByID.clear();
			for (auto &pair : _idByName)
				_nameByID.insert({ pair.second, pair.first });
		}
		this->idByName = &_idByName;
		this->nameByID = &_nameByID;
	}

	uint8_t classID = Misc;

	std::map<std::string, uint8_t> *idByName;
	std::map<uint8_t, std::string> *nameByID;
};

class ItemSubclass
{
public:
	enum : uint8_t {
		Misc = 0x01,
		Gold = 0x02,
		Lockpick = 0x03,
		Torch = 0x04, // Is it unused ? 

		Sword = 0x05,
		WarAxe = 0x06,
		Mace = 0x07,
		Dagger = 0x08,
		Greatsword = 0x09,
		Battleaxe = 0x0A,
		Warhammer = 0x0B,
		Bow = 0x0C,
		Crossbow = 0x0D,

		Armor = 0x0E,
		Boots = 0x0F,
		Gauntlets = 0x10,
		Helmet = 0x11,
		Amulet = 0x12,
		Ring = 0x13,
		Shield = 0x14,

		Potion = 0x15,
		Food = 0x16,
		Poison = 0x17,

		END = 0x18,
	};

	ItemSubclass() {
		Init();
		this->subclassID = NULL;
	}

	ItemSubclass(uint8_t subclassID) {
		Init();
		this->subclassID = subclassID;
	}

	ItemSubclass(std::string name) {
		Init();
		try {
			this->subclassID = this->idByName->at(name);
		}
		catch (...) {
			this->subclassID = NULL;
			//log(Log::Error, "Unknown item subclass ", name.data());
		};
	}

	uint8_t GetID() const {
		return this->subclassID;
	}

	std::string GetName() const {
		try {
			return nameByID->at(this->subclassID);
		}
		catch (...) {
			return "";
		}
	}

	ItemClass GetClass() const {
		auto id = this->GetID();
		if (id >= Misc && id <= Torch)
			return ItemClass::Misc;
		if (id >= Sword && id <= Crossbow)
			return ItemClass::Weapon;
		if (id >= Armor && id <= Shield)
			return ItemClass::Armor;
		if (id >= Potion && id <= Poison)
			return ItemClass::Potion;
		return {};
	}

private:
	void Init() {
		static std::map<std::string, uint8_t> _idByName;
		static std::vector<std::string> _nameByID;
		if (_idByName.empty())
		{
			const std::list<std::pair<std::string, int32_t>> list = {
				{ "Misc",			Misc },
				{ "Gold",			Gold },
				{ "Lockpick",		Lockpick },
				{ "Torch",			Torch },

				{ "Sword",			Sword },
				{ "WarAxe",			WarAxe },
				{ "Mace",			Mace },
				{ "Dagger",			Dagger },
				{ "Greatsword",		Greatsword },
				{ "Battleaxe",		Battleaxe },
				{ "Warhammer",		Warhammer },
				{ "Bow",			Bow },
				{ "Crossbow",		Crossbow },

				{ "Armor", 			Armor },
				{ "Boots", 			Boots },
				{ "Gauntlets", 		Gauntlets },
				{ "Helmet", 		Helmet },
				{ "Amulet", 		Amulet }, // 0.15.13
				{ "Ring", 			Ring }, // 0.15.13
				{ "Shield", 		Shield },

				{ "Potion",			Potion },
				{ "Food",			Food },
				{ "Poison",			Poison }
			};
			for (auto &listItem : list)
				_idByName[listItem.first] = listItem.second;
			for (uint8_t id = 0; id != ItemSubclass::END; ++id)
			{
				for (auto &pair : _idByName)
				{
					if (pair.second == id)
					{
						_nameByID.push_back(pair.first);
						break;
					}
				}
			}
		}
		this->idByName = &_idByName;
		this->nameByID = &_nameByID;
	}

	uint8_t subclassID;

	std::map<std::string, uint8_t> *idByName;
	std::vector<std::string> *nameByID;
};