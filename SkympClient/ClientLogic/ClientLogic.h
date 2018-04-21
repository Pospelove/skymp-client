#pragma once
#include "../CoreInterface/CoreInterface.h"

#include <set>
#include <map>
#include <vector>
#include <thread>
#include <sstream>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")
#include <RakPeer.h>

#include "Config.h"
#include "Serialization.h"
#include "MessageID.h"
#include "EncodeCyrillic.h"
#include "Tests.h"

#define MAX_NICKNAME							(24u)
#define MAX_PASSWORD							(32u)
#define ADD_PLAYER_ID_TO_NICKNAME_LABEL			FALSE

#define version "1.0.37"

class ClientLogic : public ci::IClientLogic
{
public:

	enum class Type : uint8_t
	{
		Static = 0x00,
		Door = 0x01,
		TeleportDoor = 0x02,
		Activator = 0x03,
		Furniture = 0x04,
		Item = 0x05,
		Container = 0x06,
	};

	struct ObjectInfo
	{
		Type type = Type::Static;
		uint16_t hostID = ~0;
		clock_t createMoment = 0;

		float lastSpeed = 0;

		bool isHostable = true;
	};

	struct Net
	{
		RakNet::RakPeerInterface *peer = nullptr;
		RakNet::SocketDescriptor socket[10];
		std::string password, host, hardcodedPassword;
		std::wstring nickname;
		uint16_t port = 0;
		std::wstring connectingMsg;

		RakNet::SystemAddress remote = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
		RakNet::RakNetGUID remoteGUID = RakNet::UNASSIGNED_RAKNET_GUID;

		bool fullyConnected = false;
		uint16_t myID = ~0;
	};

	struct AVNamesHandler
	{
		enum {
			InvalidAV, Health, Magicka, Stamina, HealRate, MagickaRate, StaminaRate, HealRateMult, MagickaRateMult, StaminaRateMult,
			_fSprintStaminaDrainMult, _fSprintStaminaWeightBase, _fSprintStaminaWeightMult, CarryWeight, UnarmedDamage, OneHanded,
			TwoHanded, Marksman, Block, Smithing, HeavyArmor, LightArmor, Pickpocket, Lockpicking, Sneak, Alchemy, Speechcraft, Alteration,
			Conjuration, Destruction, Illusion, Restoration, Enchanting, Level, PerkPoints, _fXPLevelUpBase, _fXPLevelUpMult, Experience,
			__OneHandedExp, __TwoHandedExp, __MarksmanExp, __BlockExp, __SmithingExp, __HeavyArmorExp, __LightArmorExp, __PickpocketExp,
			__LockpickingExp, __SneakExp, __AlchemyExp, __SpeechcraftExp, __AlterationExp, __ConjurationExp, __DestructionExp, __IllusionExp,
			__RestorationExp, __EnchantingExp, NUM_AVS,
		};
		const std::string &GetAVName(uint8_t id)
		{
			static const std::vector<std::string> names = {
				"", "Health", "Magicka", "Stamina", "HealRate", "MagickaRate", "StaminaRate", "HealRateMult", "MagickaRateMult", "StaminaRateMult",
				"_fSprintStaminaDrainMult", "_fSprintStaminaWeightBase", "_fSprintStaminaWeightMult", "CarryWeight", "UnarmedDamage", "OneHanded",
				"TwoHanded", "Marksman", "Block", "Smithing", "HeavyArmor", "LightArmor", "Pickpocket", "Lockpicking", "Sneak", "Alchemy", "Speechcraft",
				"Alteration", "Conjuration", "Destruction", "Illusion", "Restoration", "Enchanting", "Level", "PerkPoints", "_fXPLevelUpBase",
				"_fXPLevelUpMult", "Experience", "-OneHandedExp", "-TwoHandedExp", "-MarksmanExp", "-BlockExp", "-SmithingExp", "-HeavyArmorExp",
				"-LightArmorExp", "-PickpocketExp", "-LockpickingExp", "-SneakExp", "-AlchemyExp", "-SpeechcraftExp", "-AlterationExp", "-ConjurationExp",
				"-DestructionExp", "-IllusionExp", "-RestorationExp", "-EnchantingExp"
			};
			if (id < NUM_AVS)
				return names[id];
			else
				return names[InvalidAV];
		}
	};

	using EffectIndex = uint32_t;

	ClientLogic();

	void InitItemTypesHandlers();
	void InitObjectsHandlers();
	void InitPlayersHandlers();
	void InitConnectionHandlers();
	void InitGameHandlers();
	void InitDataSearchHandlers();
	void Init3DTextsHandlers();
	void InitMessagesHandlers();
	void InitCommandsHandlers();
	void InitMagicHandlers();
	void InitRecipesHandlers();

	std::map<RakNet::MessageID, std::function<void(RakNet::BitStream &, RakNet::Packet *)>> packetHandlers;

	void SetPacketHandlerEx(RakNet::MessageID packetID, std::function<void(RakNet::BitStream &, RakNet::Packet *)> handler)
	{
		packetHandlers[packetID] = handler;
	}

	void SetPacketHandler(RakNet::MessageID packetID, std::function<void(RakNet::BitStream &)> handler)
	{
		packetHandlers[packetID] = [=](RakNet::BitStream &bs, RakNet::Packet *p) {
			handler(bs);
		};
	}

	ci::LocalPlayer *const localPlayer = ci::LocalPlayer::GetSingleton();
	ci::LookData ld;
	std::map<uint16_t, ci::IActor *> players;
	std::set<uint16_t> werewolfs;
	std::map<uint16_t, std::shared_ptr<ci::Text3D>> playerBubbles;
	std::map<uint32_t, ci::Object *> objects;
	std::map<uint32_t, ci::ItemType *> itemTypes;
	std::set<ci::Object *> hostedJustNow;
	std::map<uint32_t, ci::MagicEffect *> effects;
	std::map<uint32_t, ci::Magic *> magic;
	std::map<uint32_t, ci::Text3D *> text3Ds;
	std::map<uint32_t, std::string> keywords;
	std::map<uint32_t, ci::Perk *> perks;
	std::map<uint32_t, ci::Recipe *> recipes;
	std::map<uint16_t, uint32_t> baseNPCs;
	std::set<uint16_t> hostedPlayers;
	struct {
		std::list<std::shared_ptr<uint32_t>> hitAnims;
	} dbg;
	bool silentInventoryChanges = false;
	bool dataSearchEnabled = false;
	std::function<void(ci::DataSearch::TeleportDoorsData)> tpdCallback;
	std::map<uint16_t, uint32_t> lastFurniture;
	bool tracehost = false;
	bool tracePackets = false;
	uint32_t lastDialogID = ~0;
	PacketReliability unreliable = PacketReliability::UNRELIABLE;
	std::map<uint32_t, ObjectInfo> objectsInfo;
	std::map<uint8_t, float> currentAVsOnServer; // percentage
	uint32_t lastLocation = 0;
	bool haveName = false;
	bool allowUpdateAVs = false;
	clock_t lastUpdateByServer[1024]; // index is AV ID
	Net net;
	AVNamesHandler av;
	ci::Signal<void()> testUpd;

	bool IsRussianTranslate();
	bool IsHorseBase(uint32_t baseNpc);

	uint16_t GetID(const ci::IActor *player);
	uint32_t GetID(const ci::Object *object);
	uint32_t GetID(const ci::ItemType *itemType);
	uint32_t GetID(const ci::Spell *spell);
	uint32_t GetID(const ci::Enchantment *ench);

	ci::IActor *FindClosestActor(NiPoint3 pos, std::function<bool(ci::IActor *)> pred = nullptr);

	void PrintStartupInfo();
	void ConnectToServer(const std::string &host, uint16_t port, const std::string &hardcodedPassword, const std::string &password, const std::wstring &nickname);
	void ProcessPacket(RakNet::Packet *packet);
	void SendAnimation(uint32_t animID, uint16_t source);
	void ExecuteCommand(std::string cmd);

	void OnStartup() override;
	void OnActivate(ci::Object *self, bool isOpen, ci::IActor *source);
	void OnActivate(ci::IActor *self);
	void OnContainerChanged(ci::Object *self, const ci::ItemType *itemType, uint32_t count, bool isAdd);
	void OnHit(ci::IActor *hitTarget, ci::IActor *hitSource, const ci::HitEventData &eventData);
	void OnHit(ci::Object *hitTarget, const ci::HitEventData &eventData);
	void OnWorldInit() override;
	void OnUpdate() override;
	void OnChatMessage(const std::wstring &text) override;
	void OnChatCommand(const std::wstring &cmdText, const std::vector<std::wstring> &arguments) override;
	void OnRaceMenuExit() override;
	void OnItemDropped(const ci::ItemType *itemType, uint32_t count) override;
	void OnItemUsed(const ci::ItemType *itemType) override;
	void OnItemUsedInCraft(const ci::ItemType *itemType, uint32_t count) override;
	void OnCraftFinish(bool isPoison, const ci::ItemType *itemType, uint32_t numCraftedItems) override;
	void OnLockpick() override;
	void OnPoisonAttack() override;
	void OnItemEnchanting(const ci::ItemType *itemType, const ci::Enchantment *ench) override;
	void OnEffectLearned(uint32_t itemTypeID, EffectIndex i);
	void OnDialogResponse(uint32_t dialogID, ci::Dialog::Result result);

	void UpdateNetworking();
	void UpdateObjects();
	void UpdateCombat();
	void UpdateActorValues();
	void UpdateEquippedArmor();
	template<int32_t handID> void UpdateEquippedWeapon();
	template<int32_t handID> void UpdateEquippedSpell();
	void UpdateEquippedAmmo();
	void UpdateKnownEffects();
	void UpdateWerewolfs();
};