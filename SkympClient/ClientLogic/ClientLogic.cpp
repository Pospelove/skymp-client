#include "ClientLogic.h"

auto version = "1.0.37";

class ClientLogic : public ci::IClientLogic
{
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
	std::map<const ci::ItemType *, int32_t> numItems; // localPlayer
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

	bool IsRussianTranslate()
	{
		return g_config[CONFIG_TRANSLATE] == "RU" 
			|| g_config[CONFIG_TRANSLATE] == "Ru" 
			|| g_config[CONFIG_TRANSLATE] == "ru";
	}

	bool IsHorseBase(uint32_t baseNpc)
	{
		static const std::set<uint32_t> horseIds = {
			0x00109E3D,
			0x00109AB1,
			0x00109E41,
			0x00109E40,
			0x00109E3E,
			0x00097E1E,
			0x0009CCD7,
			0x0010BF90
		};
		return horseIds.count(baseNpc) != 0;
	}

	uint16_t GetID(const ci::IActor *player)
	{
		for (auto it = players.begin(); it != players.end(); ++it)
		{
			if (it->second == player)
				return it->first;
		}
		return -1;
	}
	
	uint32_t GetID(const ci::Object *object)
	{
		for (auto it = objects.begin(); it != objects.end(); ++it)
		{
			if (it->second == object)
				return it->first;
		}
		return -1;
	}

	uint32_t GetID(const ci::ItemType *itemType)
	{
		for (auto it = itemTypes.begin(); it != itemTypes.end(); ++it)
		{
			if (it->second == itemType)
				return it->first;
		}
		return -1;
	}

	uint32_t GetID(const ci::Spell *spell)
	{
		for (auto it = magic.begin(); it != magic.end(); ++it)
		{
			if (it->second == spell)
				return it->first;
		}
		return -1;
	}

	uint32_t GetID(const ci::Enchantment *ench)
	{
		return GetID(
			reinterpret_cast<const ci::Spell *>(ench)
		);
	}

	ci::IActor *FindClosestActor(NiPoint3 pos, std::function<bool(ci::IActor *)> pred = nullptr)
	{
		float minDistance = std::numeric_limits<float>::infinity();
		ci::IActor *res = nullptr;
		for (auto &pair : players)
		{
			const auto actor = pair.second;
			const auto acPos = actor->GetPos();
			const auto distance = (pos - acPos).Length();
			if (distance < minDistance && (pred == nullptr || pred(actor)))
			{
				minDistance = distance;
				res = actor;
			}
		}
		return res;
	}

	enum class Type : uint8_t
	{
		Static =			0x00,
		Door =				0x01,
		TeleportDoor =		0x02,
		Activator =			0x03,
		Furniture =			0x04,
		Item =				0x05,
		Container =			0x06,
	};

	struct ObjectInfo
	{
		Type type = Type::Static;
		uint16_t hostID = ~0;
		clock_t createMoment = 0;

		float lastSpeed = 0;

		bool isHostable = true;
	};
	std::map<uint32_t, ObjectInfo> objectsInfo;

	std::map<uint8_t, float> currentAVsOnServer; // percentage

	uint32_t lastLocation = 0;

	bool haveName = false;
	bool allowUpdateAVs = false;
	clock_t lastUpdateByServer[1024]; // index is AV ID

	struct
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

	} net;

	struct {
		enum {
			InvalidAV,
			Health,
			Magicka,
			Stamina,
			HealRate,
			MagickaRate,
			StaminaRate,
			HealRateMult,
			MagickaRateMult,
			StaminaRateMult,
			_fSprintStaminaDrainMult,
			_fSprintStaminaWeightBase,
			_fSprintStaminaWeightMult,
			CarryWeight,
			UnarmedDamage,
			OneHanded,
			TwoHanded,
			Marksman,
			Block,
			Smithing,
			HeavyArmor,
			LightArmor,
			Pickpocket,
			Lockpicking,
			Sneak,
			Alchemy,
			Speechcraft,
			Alteration,
			Conjuration,
			Destruction,
			Illusion,
			Restoration,
			Enchanting,
			Level,
			PerkPoints,
			_fXPLevelUpBase,
			_fXPLevelUpMult,
			Experience,
			__OneHandedExp,
			__TwoHandedExp,
			__MarksmanExp,
			__BlockExp,
			__SmithingExp,
			__HeavyArmorExp,
			__LightArmorExp,
			__PickpocketExp,
			__LockpickingExp,
			__SneakExp,
			__AlchemyExp,
			__SpeechcraftExp,
			__AlterationExp,
			__ConjurationExp,
			__DestructionExp,
			__IllusionExp,
			__RestorationExp,
			__EnchantingExp,
			NUM_AVS,
		};
		const std::string &GetAVName(uint8_t id) 
		{
			static const std::vector<std::string> names = {
				"",
				"Health",
				"Magicka",
				"Stamina",
				"HealRate",
				"MagickaRate",
				"StaminaRate",
				"HealRateMult",
				"MagickaRateMult",
				"StaminaRateMult",
				"_fSprintStaminaDrainMult",
				"_fSprintStaminaWeightBase",
				"_fSprintStaminaWeightMult",
				"CarryWeight",
				"UnarmedDamage",
				"OneHanded",
				"TwoHanded",
				"Marksman",
				"Block",
				"Smithing",
				"HeavyArmor",
				"LightArmor",
				"Pickpocket",
				"Lockpicking",
				"Sneak",
				"Alchemy",
				"Speechcraft",
				"Alteration",
				"Conjuration",
				"Destruction",
				"Illusion",
				"Restoration",
				"Enchanting",
				"Level",
				"PerkPoints",
				"_fXPLevelUpBase",
				"_fXPLevelUpMult",
				"Experience",
				"-OneHandedExp",
				"-TwoHandedExp",
				"-MarksmanExp",
				"-BlockExp",
				"-SmithingExp",
				"-HeavyArmorExp",
				"-LightArmorExp",
				"-PickpocketExp",
				"-LockpickingExp",
				"-SneakExp",
				"-AlchemyExp",
				"-SpeechcraftExp",
				"-AlterationExp",
				"-ConjurationExp",
				"-DestructionExp",
				"-IllusionExp",
				"-RestorationExp",
				"-EnchantingExp"
			};
			if (id < NUM_AVS)
				return names[id];
			else
				return names[InvalidAV];
		}
	} av;

	void PrintStartupInfo()
	{
		ci::Log(L"-- LocalPlayer Name is " + localPlayer->GetName());
		ci::Log(L"-- Current Date is " + ci::GetDateString());
	}

	void ConnectToServer(const std::string &host,
		uint16_t port,
		const std::string &hardcodedPassword,
		const std::string &password,
		const std::wstring &nickname)
	{
		net = {};
		net.peer = RakNet::RakPeerInterface::GetInstance();
		net.peer->Startup(22, net.socket, 10);
		net.password = password;
		net.nickname = nickname;
		net.host = host;
		net.hardcodedPassword = hardcodedPassword;
		net.port = port;
		net.peer->Connect(net.host.data(), net.port, net.hardcodedPassword.data(), net.hardcodedPassword.size());

		net.connectingMsg = L"Connecting to " + StringToWstring(host) + L":" + std::to_wstring(port) + L"...";
		ci::Chat::AddMessage(net.connectingMsg);
	}

	void ProcessPacket(RakNet::Packet *packet)
	{
		RakNet::BitStream bsOut;
		RakNet::BitStream bsIn(&packet->data[1], packet->length, false);

		switch (packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			bsOut.Write(ID_HANDSHAKE);

			char nicknameCStr[MAX_NICKNAME];
			if (net.nickname.size() > MAX_NICKNAME - 1)
				net.nickname.resize(MAX_NICKNAME - 1);
			{
				auto nicknameStr = WstringToString(net.nickname.data());
				std::memcpy(nicknameCStr, nicknameStr.data(), sizeof(char) * net.nickname.size());
			}
			nicknameCStr[net.nickname.size()] = NULL;
			nicknameCStr[MAX_NICKNAME - 1] = NULL;

			char passwordCStr[MAX_PASSWORD];
			if (net.password.size() > MAX_PASSWORD - 1)
				net.password.resize(MAX_PASSWORD - 1);
			std::memcpy(passwordCStr, net.password.data(), net.password.size());
			passwordCStr[net.password.size()] = NULL;
			passwordCStr[MAX_PASSWORD - 1] = NULL;

			for (size_t i = 0; i != MAX_NICKNAME + 1; ++i)
				bsOut.Write(nicknameCStr[i]);
			bsOut.Write(passwordCStr, MAX_PASSWORD + 1);

			Sleep(1);
			ci::Chat::AddMessage(L"Loading...");

			net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, NULL, packet->systemAddress, false);
			break;
		case ID_ALREADY_CONNECTED:
			ci::Chat::AddMessage(L"Already connected");
			Sleep(50);
			this->ConnectToServer(g_config[CONFIG_IP], std::atoi(g_config[CONFIG_PORT].data()), version, g_config[CONFIG_SERVER_PASSWORD], StringToWstring(g_config[CONFIG_NAME]));
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			if (net.fullyConnected)
				ci::Chat::AddMessage(L"Server closed the connection");
			break;
		case ID_CONNECTION_LOST:
			ci::Chat::AddMessage(L"Connection lost");
			break;
		case ID_CONNECTION_BANNED:
			ci::Chat::AddMessage(L"You are banned from the server");
			break;
		case ID_SERVER_CLOSED_THE_CONNECTION:
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			ci::Chat::AddMessage(L"The server is full. Retrying ...");
			ci::Chat::AddMessage(net.connectingMsg);
			net.peer->Connect(net.host.data(), net.port, net.hardcodedPassword.data(), net.hardcodedPassword.size());
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			ci::Chat::AddMessage(L"Connection failed. Retrying ...");
			ci::Chat::AddMessage(net.connectingMsg);
			net.peer->Connect(net.host.data(), net.port, net.hardcodedPassword.data(), net.hardcodedPassword.size());
			break;
		case ID_INVALID_PASSWORD:
			if (IsRussianTranslate())
			{
				ci::Chat::AddMessage(L"��� ������ �������.");
				ci::Chat::AddMessage(L"����������, ���������� ����� ������ �������.");
				ci::Chat::AddMessage(L"����������� /q, ����� �����.");
			}
			else
			{
				ci::Chat::AddMessage(L"Incompatible client version");
				ci::Chat::AddMessage(L"/q to exit");
			}
			break;
		case ID_WRONG_PASS:
			if (IsRussianTranslate())
			{
				ci::Chat::AddMessage(L"������������ ������ (" + StringToWstring(net.password) + L")");
			}
			else
			{
				ci::Chat::AddMessage(L"Wrong password (" + StringToWstring(net.password) + L")");
			}
			break;
		case ID_NAME_INVALID:
			if (IsRussianTranslate())
			{
				ci::Chat::AddMessage(L"��� ����� ��������� ������ ������� A-Z, a-z, 0-9 � ��������� ����. �������");
			}
			else
			{
				ci::Chat::AddMessage(L"Name can only contain A-Z, a-z, 0-9");
			}
			break;
		case ID_NAME_ALREADY_USED:
			if (IsRussianTranslate())
			{
				ci::Chat::AddMessage(L"����� " + net.nickname + L" ��� ���� �� �������.");
			}
			else
			{
				ci::Chat::AddMessage(L"Player " + net.nickname + L" is already connected to the server");
			}
			break;
		case ID_WELCOME:
			//ci::Chat::AddMessage(L"Successful handshake.");
			Sleep(1);
			ci::Chat::AddMessage(L"Connected.");
			bsIn.Read((uint16_t &)net.myID);
			players[net.myID] = ci::LocalPlayer::GetSingleton();
			net.fullyConnected = true;
			{
				uint64_t numPlugins = 0;
				bsIn.Read(numPlugins);
				for (auto i = decltype(numPlugins)(0); i != numPlugins; ++i)
				{
					uint64_t pluginSize = 0;
					bsIn.Read(pluginSize);
					std::vector<char> pluginBinray;
					for (auto j = decltype(pluginSize)(0); j != pluginSize; ++j)
					{
						char ch = ' ';
						bsIn.Read(ch);
						pluginBinray.push_back(ch);
					}
					ci::Chat::AddMessage(L"Loading a plugin (" + std::to_wstring(pluginBinray.size()) + L" bytes)");
					ci::HotLoadPlugin(pluginBinray);
				}
			}
			break;
		case ID_MESSAGE:
		{
			using T = uint16_t;
			if (packet->length > (2 * sizeof(RakNet::MessageID)) + sizeof(T))
			{
				std::string message;
				T characters;
				bsIn.Read(characters);

				if (packet->length == 2 + sizeof(T) + sizeof(char) * characters)
				{
					for (size_t i = 0; i != characters; ++i)
					{
						char ch;
						bsIn.Read(ch);
						message += ch;
					}
					bool isNotification;
					bsIn.Read((uint8_t &)isNotification);

					if (!memcmp(message.data(), "<RegisterAnim>", -(1) + (int32_t)sizeof "<RegisterAnim>"))
					{
						std::istringstream ss(message.data());
						std::string aeName, cmdStr;
						uint32_t aeID;
						ss >> cmdStr;
						ss >> aeName;
						ss >> aeID;
						ci::RegisterAnimation((aeName), aeID);
						ci::Log("RegisterAnim added");
					}
					else if (!memcmp(message.data(), "<RegisterKeyword>", -(1) + (int32_t)sizeof "<RegisterKeyword>"))
					{
						std::istringstream ss(message.data());
						std::string str, cmdStr;
						uint32_t id;
						ss >> cmdStr;
						ss >> str;
						ss >> id;
						keywords[id] = (str);
						ci::Log("RegisterKeyword %s", str.data());
					}
					else
					{
						auto abcMsg = decodeRu
							(StringToWstring(message));
						if (message.find("Learn perk") == 0)
						{
							ci::Log(L"A new perk learned");
						}
						else
						{
							ci::Chat::AddMessage((abcMsg), isNotification);
							//new ci::Text3D(abcMsg, ci::LocalPlayer::GetSingleton()->GetPos());
							ci::Log(L"Message: " + abcMsg);
						}
					}
				}
			}
			break;
		}
		case ID_LEARN_EFFECT:
		{
			uint32_t itemTypeID;
			uint32_t effectIdx;
			bool learn;
			bsIn.Read(itemTypeID);
			bsIn.Read((uint32_t &)effectIdx);
			bsIn.Read(learn);
			try {
				auto itemType = itemTypes.at(itemTypeID);
				itemType->SetNthEffectKnown(effectIdx, learn);
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic Unable to learn effect");
			}
			break;
		}
		case ID_MOVE_TO:
		{
			struct {
				NiPoint3 pos;
				uint16_t angleZ;
				uint32_t cellOrWorldspace;
			} in;
			bsIn.Read(in.pos.x);
			bsIn.Read(in.pos.y);
			bsIn.Read(in.pos.z);
			bsIn.Read(in.angleZ);
			bsIn.Read(in.cellOrWorldspace);

			std::stringstream ss;
			ss << in.pos.x << " " << in.pos.y << " " << in.pos.z << " " << in.angleZ << " " << in.cellOrWorldspace;
			//ci::Chat::AddMessage(StringToWstring(ss.str()));

			lastLocation = in.cellOrWorldspace;

			localPlayer->Resurrect();

			ci::SetTimer(180, [=] {
				if (ci::IsWorldSpace(in.cellOrWorldspace))
					localPlayer->SetWorldSpace(in.cellOrWorldspace);
				else if (ci::IsCell(in.cellOrWorldspace))
					localPlayer->SetCell(in.cellOrWorldspace);

				if (in.pos.x == in.pos.x && in.pos.y == in.pos.y && in.pos.z == in.pos.z
					&& in.pos.x != std::numeric_limits<float>::infinity() && in.pos.y != std::numeric_limits<float>::infinity() && in.pos.z != std::numeric_limits<float>::infinity()
					&& in.pos.x != -std::numeric_limits<float>::infinity() && in.pos.y != -std::numeric_limits<float>::infinity() && in.pos.z != -std::numeric_limits<float>::infinity())
				{
					auto distance = (localPlayer->GetPos() - in.pos).Length();
					if (distance > 32.0)
						localPlayer->SetPos(in.pos);
				}

				if (in.angleZ <= 360)
					localPlayer->SetAngleZ(in.angleZ);
			});
			break;
		}
		case ID_PLAYER_MOVEMENT:
		{
			uint16_t playerid = ~0;
			bsIn.Read(playerid);
			if (playerid == net.myID)
				return;

			ci::MovementData movData;
			Deserialize(bsIn, movData);
			uint8_t enabled;
			uint32_t locationID;
			bsIn.Read(enabled);
			bsIn.Read(locationID);
			try {
				auto &player = this->players.at(playerid);
				if (player->GetName() == localPlayer->GetName())
					movData.pos += NiPoint3{ 128, 128, 0 };
				if (movData.jumpStage == ci::MovementData::JumpStage::Jumping)
					lastFurniture[playerid] = 0;
				if (lastFurniture[playerid] != 0)
					movData.runMode = ci::MovementData::RunMode::Standing;
				if (enabled)
				{
					if (hostedPlayers.count(playerid) == 0)
					{
						player->ApplyMovementData(movData);
					}
				}

				if (locationID == localPlayer->GetLocation())
				{
					player->SetCell(localPlayer->GetCell());
					player->SetWorldSpace(localPlayer->GetWorldSpace());
				}
				else
				{
					player->SetCell(~0);
					player->SetWorldSpace(~0);
				}

				auto rPlayer = dynamic_cast<ci::RemotePlayer *>(player);
				if (rPlayer != nullptr)
				{
					int32_t clearVisualEffect = 0;
					for (int32_t i = 1; i >= 0; --i)
						if ((int32_t)movData.castStage[i] != 0)
						{
							rPlayer->MagicAttackBegin(i);
							rPlayer->SetVisualMagicEffect(rPlayer->GetEquippedSpell(i));
						}
						else
						{
							rPlayer->MagicAttackEnd(i);
							++clearVisualEffect;
						}
					if (clearVisualEffect == 2)
						rPlayer->SetVisualMagicEffect(nullptr);

					if (rPlayer->IsBroken())
					{
						auto engine = rPlayer->GetEngine();
						if (engine == "RPEngineInput")
						{
							ci::Log("ERROR:ClientLogic RemotePlayer is broken " + engine + " isHosted=" + std::to_string(hostedPlayers.count(playerid)));
						}
						else
						{
							ci::Log("WARN:ClientLogic RemotePlayer is broken " + engine);
						}
					}

					if (movData.mountStage == ci::MovementData::MountStage::OnMount)
					{
						if (rPlayer->HasAttachedHorse() == false)
						{
							const auto horse = dynamic_cast<ci::RemotePlayer *>(
								FindClosestActor(movData.pos, [this, movData](ci::IActor *actor) {
								return actor != localPlayer
									&& actor->GetPos().z < movData.pos.z
									&& abs(actor->GetAngleZ() - movData.angleZ) < 15.0
									&& (actor->GetPos() - movData.pos).Length() < 256.0;
							})
								);
							if (horse->IsSpawned())
							{
								rPlayer->SetAttachedHorse(horse);
							}
						}
					}
					else
						rPlayer->SetAttachedHorse(nullptr);
				}
			}
			catch (...) {
				//ci::Log("ERROR:ClientLogic RemotePlayer not found %u", playerid);
				//this->StreamOut(playerid);
			}
			break;
		}
		case ID_PLAYER_NAME:
		{
			using T = uint16_t;
			if (packet->length > (3 + sizeof(T)))
			{
				std::string name;
				T characters;
				uint16_t id = ~0;
				bsIn.Read(id);
				bsIn.Read(characters);

				if (packet->length == (3 + sizeof(T) + sizeof(char) * characters))
				{
					for (size_t i = 0; i != characters; ++i)
					{
						char ch;
						bsIn.Read(ch);
						name += ch;
					}
					try {
						if (ADD_PLAYER_ID_TO_NICKNAME_LABEL && id != net.myID)
							name += " [" + std::to_string(id) + "]";
						players.at(id)->SetName(decodeRu(StringToWstring(name)));
					}
					catch (...) {
					}
				}
			}

			break;
		}
		case ID_PLAYER_CREATE:
		{
			uint32_t baseNpc = 0;
			ci::LookData look;
			uint16_t id = ~0;
			ci::MovementData movement;
			uint32_t locationID = 0;

			bsIn.Read(baseNpc);
			Deserialize(bsIn, look);
			bsIn.Read(baseNpc);
			bsIn.Read(id);
			Deserialize(bsIn, movement);
			{
				ci::LookData _;
				Deserialize(bsIn, _);
			}
			bsIn.Read(locationID);

			uint16_t characters;
			bsIn.Read(characters); 

			std::string name;
			for (size_t i = 0; i != characters; ++i)
			{
				char ch;
				bsIn.Read(ch);
				name += ch;
			}

			uint32_t cellID = localPlayer->GetCell();
			uint32_t worldSpaceID = localPlayer->GetWorldSpace();

			auto onHit = [this, id](const ci::HitEventData &eventData) {
				try {
					auto pl = players.at(id);

					const bool h2h = eventData.spell == nullptr && eventData.weapon == nullptr;
					if (h2h)
					{
						const auto closestAc = FindClosestActor(pl->GetPos(), [=](ci::IActor *ac) { return ac != pl; });
						if((closestAc->GetPos() - pl->GetPos()).Length() > 256)
						{
							auto rem = dynamic_cast<ci::RemotePlayer *>(pl);
							if (rem != nullptr)
							{
								ci::Log("ERROR:ClientLogic bad h2h hit (is it invisible gnome?)");
								rem->Respawn();
							}
						}
					}

					auto source = (ci::IActor *)nullptr;
					if (eventData.hitSrcMark.empty())
					{
						source = players.at(net.myID);
						if (tracehost)
							ci::Chat::AddMessage(L"#BEBEBEEmpty Hit Mark. Probably LocalPlayer.");
					}
					else
					{
						source = players.at((uint16_t)atoi(eventData.hitSrcMark.data()));
					}
					if (tracehost)
						ci::Chat::AddMessage(L"#BEBEBEHit Mark = " + StringToWstring(eventData.hitSrcMark));
					this->OnHit(pl, source, eventData);
				}
				catch (const std::exception &e) {
					ci::Log(L"ERROR:ClientLogic OnHit() " + StringToWstring("Something gone wrong " + (std::string)e.what()));
				}
				catch (...) {
					ci::Log(L"ERROR:ClientLogic OnHit() Something gone wrong");
				}
			};

			auto onActivate = [this, id]() {
				try {
					this->OnActivate(players.at(id));
				}
				catch (...) {
				}
			};

			if (players[id] != nullptr)
			{
				delete players[id];
				players[id] = nullptr;
			}

			auto newPl = new ci::RemotePlayer(decodeRu(StringToWstring(name)), look, movement.pos, cellID, worldSpaceID, onHit, "RPEngineInput", onActivate);
			newPl->SetMark(std::to_string(id));
			players[id] = newPl;
			lastFurniture[id] = 0;

			//uint32_t baseNpc = 0;
			//bsIn.Read(baseNpc);
			if (baseNpc != 0 && baseNpc != ~0)
			{
				baseNPCs[id] = baseNpc;
				newPl->SetBaseNPC(baseNpc);
				if (IsHorseBase(baseNpc)) // ��������� �����
				{
					auto p = new ci::RemotePlayer(
						newPl->GetName() + L" ",
						localPlayer->GetLookData(),
						localPlayer->GetPos(),
						localPlayer->GetCell(),
						localPlayer->GetWorldSpace(),
						onHit, "",
						onActivate);
					p->SetBaseNPC(baseNpc);
					p->SetMark(std::to_string(id));
					delete players[id];
					players[id] = p;
				}
			}
			else
				baseNPCs.erase(id);
			if (tracehost)
			{
				ci::Chat::AddMessage((L"#BEBEBE" L"BaseNPC ") + std::to_wstring(baseNpc));
			}
			break;
		}
		case ID_PLAYER_DESTROY:
		{
			uint16_t id = ~0;
			bsIn.Read(id);
			auto it = players.find(id);
			if (it != players.end())
			{
				delete it->second;
				players.erase(it);
				werewolfs.erase(id);
				hostedPlayers.erase(id);
			}
			break;
		}
		case ID_PLAYER_PAUSE:
		{
			uint16_t id = ~0;
			uint8_t isPaused;

			bsIn.Read(id);
			bsIn.Read(isPaused);

			try {
				auto player = dynamic_cast<ci::RemotePlayer *>(players.at(id));
				if (player != nullptr)
					player->SetInAFK(!!isPaused);
			}
			catch (...) {
			}
			break;
		}
		case ID_PLAYER_LOOK:
		{
			uint16_t id = ~0;
			ci::LookData look;

			bsIn.Read(id);
			Deserialize(bsIn, look);
			
			uint8_t isWerewolf = 0;
			bsIn.Read(isWerewolf);
			if (isWerewolf)
				werewolfs.insert(id);
			else
				werewolfs.erase(id);

			if (isWerewolf == false)
			{
				try {
					players.at(id)->SetWerewolf(false);
					ci::SetTimer(1000, [=] {
						try {
							if (!look.isEmpty())
								players.at(id)->ApplyLookData(look);
						}
						catch (...) {
						}
					});
				}
				catch (...) {
				};
			}
			break;
		}
		case ID_PLAYER_FURNITURE:
		{
			uint16_t playerID = ~0;
			uint32_t furnitureID = 0;

			bsIn.Read(playerID);
			bsIn.Read(furnitureID);

			lastFurniture[playerID] = furnitureID;
			try {

				auto pl = players.at(playerID);
				auto obj = objects.at(furnitureID);

				switch (objectsInfo[furnitureID].type)
				{
					// ��� ���������� ����? � ��*** �� �������..
				case Type::Furniture:
				{
					if (pl == localPlayer)
					{
						static std::map<uint16_t, uint32_t> lastFurnitureUsed;

						auto it = objects.find(furnitureID);
						if (it != objects.end())
						{
							pl->UseFurniture(it->second, true);
							lastFurnitureUsed[playerID] = furnitureID;
						}
						else
						{
							enum {
								mustBeTrue = true
							};
							pl->UseFurniture(objects.at(lastFurnitureUsed[playerID]), mustBeTrue); // 2nd activate to stop use furniture
							lastFurnitureUsed.erase(playerID);
						}
					}
					break;
				}

				case Type::Container:

						try {
							if(obj->GetLockLevel() != 0)
							{
								static std::set<uint32_t> already;
								if (already.insert(furnitureID).second)
								{
									pl->UseFurniture(obj, true);
								}
								else
								{
									already.erase(furnitureID);
								}
							}
							else
							{
								pl->UseFurniture(obj, true);
							}
						}
						catch (...) {
						}
					break;

				case Type::TeleportDoor:
				case Type::Door:
					pl->UseFurniture(obj, true);
					break;

				case (Type)NULL:
					break;

				default:
					ci::Log("ERROR:ClientLogic Unknown furniture type");
					break;
				}
			}
			catch (...) {
			}

			break;
		}
		case ID_SHOW_RACE_MENU:
			ci::ShowRaceMenu();
			break;
		case ID_SHOW_DIALOG:
			using T = uint16_t;
			using Index = int32_t;
			using DialogID = uint32_t;
			//if (packet->length > (1 * sizeof(RakNet::MessageID)) + sizeof(T))
			{
				std::string title;
				std::string text;
				T characters;
				T characters2;
				bsIn.Read(characters);
				bsIn.Read(characters2);


				//if (packet->length == sizeof(DialogID) + sizeof(Index) + sizeof(ci::Dialog::Style) + (1 * sizeof(RakNet::MessageID)) + 2 * sizeof(T) + sizeof(wchar_t) * characters + sizeof(wchar_t) * characters2)
				{
					for (size_t i = 0; i != characters; ++i)
					{
						char ch;
						bsIn.Read(ch);
						title += ch;
					}
					for (size_t i = 0; i != characters2; ++i)
					{
						char ch;
						bsIn.Read(ch);
						text += ch;
					}
					DialogID dialogID;
					Index defaultIndex;
					ci::Dialog::Style style;
					bsIn.Read(dialogID);
					bsIn.Read(defaultIndex);
					bsIn.Read(style);

					if (dialogID == ~0)
					{
						ci::Dialog::Hide();
						break;
					}

					lastDialogID = dialogID;
					ci::Dialog::Show(decodeRu(StringToWstring(title)), style, decodeRu(StringToWstring(text)), defaultIndex, [=](ci::Dialog::Result result) {
						this->OnDialogResponse(dialogID, result);
					});
				}
			}
			break;
		case ID_OBJECT_CREATE:
		{
			uint32_t id = 0;
			bool isNative = true;
			uint32_t baseFormID;
			uint32_t locationID;
			NiPoint3 pos, rot;

			bsIn.Read(id);
			bsIn.Read(isNative);
			bsIn.Read(baseFormID);
			bsIn.Read(locationID);
			bsIn.Read(pos.x);
			bsIn.Read(pos.y);
			bsIn.Read(pos.z);
			bsIn.Read(rot.x);
			bsIn.Read(rot.y);
			bsIn.Read(rot.z);

			auto onActivate = [this, id](bool isOpen, ci::IActor *source) {
				try {
					this->OnActivate(objects.at(id), isOpen, source);
				}
				catch (...) {
				}
			};

			auto onContainerChanged = [this, id](const ci::ItemType *itemType, uint32_t count, bool isAdd) {
				try {
					this->OnContainerChanged(objects.at(id), itemType, count, isAdd);
				}
				catch (...) {
				}
			};

			auto onHit = [this, id](const ci::HitEventData &eventData) {
				try {
					this->OnHit(objects.at(id), eventData);
				}
				catch (...) {
				}
			};

			objects[id] = new ci::Object(isNative ? id : 0, baseFormID, /*locationID*/localPlayer->GetLocation(), pos, rot, onActivate, onContainerChanged, onHit);
			objects[id]->SetDestroyed(true);
			objects[id]->SetMotionType(ci::Object::Motion_Keyframed); 
			objectsInfo[id] = {};
			objectsInfo[id].createMoment = clock();
			break;
		}
		case ID_OBJECT_DESTROY:
		{
			uint32_t id = 0;
			bsIn.Read(id);
			try {
				delete objects.at(id);
				objects.erase(id);
			}
			catch (...) {
			}
			break;
		}
		case ID_OBJECT_POS_ROT_LOCATION:
		{
			uint32_t id = 0;
			NiPoint3 pos, rot;
			uint32_t locationID;
			uint8_t isHostable;
			bsIn.Read(id);
			bsIn.Read(pos.x);
			bsIn.Read(pos.y);
			bsIn.Read(pos.z);
			bsIn.Read(rot.x);
			bsIn.Read(rot.y);
			bsIn.Read(rot.z);
			bsIn.Read(locationID);
			bsIn.Read(isHostable);
			try {
				auto obj = objects.at(id);
				if (objectsInfo[id].hostID != net.myID)
				{
					enum : uint16_t {
						INVALID_HOST_ID = (uint16_t)~0,
					};
					if (objectsInfo[id].hostID != INVALID_HOST_ID)
					{
						if (obj->GetMotionType() != ci::Object::Motion_Keyframed)
							obj->SetMotionType(ci::Object::Motion_Keyframed);
						const float S = (pos - obj->GetPos()).Length();
						const float t = 0.100f;
						obj->TranslateTo(pos, rot, S / t, 300);
					}
					else
					{
						obj->SetPosition(pos);
						obj->SetAngle(rot);
					}
				}
				//obj->SetLocation(locationID);
				obj->SetLocation(localPlayer->GetLocation());

				objectsInfo[id].isHostable = isHostable;
				if (isHostable == false)
				{
					obj->SetMotionType(ci::Object::Motion_Keyframed);
					for (auto ms : { 100, 200, 300, 500 })
					{
						ci::SetTimer(ms, [=] {
							try {
								objects.at(id)->SetMotionType(ci::Object::Motion_Keyframed);
							}
							catch (...) {
							}
						});
					}
				}
			}
			catch (...) {
			}
			break;
		}
		case ID_OBJECT_NAME:
		{
			uint32_t id = 0;
			bsIn.Read(id);

			uint16_t characters;
			bsIn.Read(characters);

			std::string name;
			for (size_t i = 0; i != characters; ++i)
			{
				char ch;
				bsIn.Read(ch);
				name += ch;
			}

			try {
				auto name1 = decodeRu(StringToWstring(name));
				if (name1.find(L'<') != name1.npos)
				{
					ci::Log("ERROR:ClientLogic bad object name");
					name1 = L" ";
				}
				objects.at(id)->SetName(name1);
			}
			catch (...) {
			}
			break;
		}
		case ID_OBJECT_BEHAVIOR:
		{
			uint32_t id = 0;
			Type type;
			bool isOpen;
			uint32_t teleportTargetID = 0;
			bool isDisabled;
			uint32_t itemsCount;
			uint32_t itemTypeID;
			uint16_t hostPlayerID;
			uint8_t lockLevel;
			bool destroyed = false;

			bsIn.Read(id);
			bsIn.Read(type);
			bsIn.Read(isOpen);
			bsIn.Read(teleportTargetID);
			bsIn.Read(isDisabled);
			bsIn.Read(itemsCount);
			bsIn.Read(itemTypeID);
			bsIn.Read(hostPlayerID);
			bsIn.Read(lockLevel);
			bsIn.Read(destroyed);

			try {
				auto object = objects.at(id);

				if (type == Type::Container)
				{
					uint32_t size;
					bsIn.Read(size);
					for (uint32_t i = 0; i != size; ++i)
					{
						uint32_t itemTypeID;
						uint32_t count;
						bsIn.Read(itemTypeID);
						const bool read = bsIn.Read(count);
						if (!read)
							break;
						auto type = itemTypes.at(itemTypeID);
						object->AddItem(itemTypes[itemTypeID], count);
					}

					bsIn.Read(size);
					for (uint32_t i = 0; i != size; ++i)
					{
						uint32_t itemTypeID;
						uint32_t count;
						bsIn.Read(itemTypeID);
						const bool read = bsIn.Read(count);
						if (!read)
							break;
						auto type = itemTypes.at(itemTypeID);
						object->RemoveItem(itemTypes[itemTypeID], count);
					}
				}


				objectsInfo[id].type = type;
				objectsInfo[id].hostID = hostPlayerID;

				if (hostPlayerID == net.myID)
				{
					ci::SetTimer(200, [=] {
						hostedJustNow.insert(object);
						ci::SetTimer(1800, [=] {
							ci::SetTimer(1, [=] {
								hostedJustNow.erase(object);
							});
						});
					});
				}

				object->SetDisabled(isDisabled);
				object->SetLockLevel(lockLevel);

				switch (type)
				{
				case Type::Static:
					break;
				case Type::Door:
					object->SetOpen(isOpen);
					object->SetDestroyed(destroyed);
					object->BlockActivation(FALSE);
					break;
				case Type::TeleportDoor:
					object->SetOpen(isOpen);
					switch (object->GetBase())
					{
					case 0x31897:
					case 0x351EB:
					case 0x180D8:
						object->SetDestroyed(FALSE);
						object->BlockActivation(FALSE);
						break;
					default:
						object->SetDestroyed(destroyed);
						object->BlockActivation(false);
						break;
					}
					break;
				case Type::Activator:
				case Type::Furniture:
					object->SetDestroyed(destroyed);
					object->BlockActivation(true);
					object->SetMotionType(ci::Object::Motion_Keyframed);
					break;
				case Type::Container:
					object->SetDestroyed(destroyed);
					object->BlockActivation(true);
					object->SetOpen(isOpen);
					break;
				case Type::Item:
					object->SetDestroyed(destroyed);
					object->BlockActivation(true);
					object->SetBase(itemTypes[itemTypeID]);
					object->SetCount(itemsCount);
				}
			}
			catch (...) {
			}

			break;
		}
		case ID_OBJECT_KEYWORDS:
		{
			uint32_t id, numKs;
			std::list<uint32_t> keywords;
			bsIn.Read(id);
			bsIn.Read(numKs);

			uint32_t temp;
			for (size_t i = 0; i != numKs; ++i)
			{
				bsIn.Read(temp);
				keywords.push_back(temp);
			}

			try {
				for (auto kID : keywords)
				{
					ci::RemoveAllKeywords(objects.at(id)->GetBase());
					const auto keywordStr = this->keywords[kID];
					ci::AddKeyword(objects.at(id)->GetBase(), keywordStr);

					if (this->keywords[kID].empty())
						ci::Log("ERROR:ClientLogic id_object_keywords keyword not found");
				}
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic id_object_keywords object not found");
			}

			break;
		}
		case ID_PLAYER_INVENTORY:
		{
			uint16_t playerID;
			bool add;
			uint32_t count1;

			bsIn.Read(playerID);
			bsIn.Read(add);
			bsIn.Read(count1);

			for (uint32_t i = 0; i != count1; ++i)
			{
				uint32_t itemTypeID;
				uint32_t count;
				bsIn.Read(itemTypeID);
				bsIn.Read(count);

				try {
					auto asd = itemTypes.at(itemTypeID);
				}
				catch (...) {
					ci::Log("WARN:Logic:AddItem ItemType %d not found", itemTypeID);
				}

				try {
					auto itemType = itemTypes.at(itemTypeID);
					const bool silent = this->silentInventoryChanges;
					if (add)
					{
						if (itemType->IsCustomPotion())
							itemType->GenPotionName();

						players.at(playerID)->AddItem(itemType, count, silent);
						if (playerID == net.myID)
							numItems[itemType] += count;
					}
					else
					{
						players.at(playerID)->RemoveItem(itemType, count, silent);
						if (playerID == net.myID)
							numItems[itemType] -= count;
					}


					for (auto it = objects.begin(); it != objects.end(); ++it)
					{
						if (it->second)
							it->second->AddItem(itemTypes.at(itemTypeID), 0);
					}
				}
				catch (...) {
				}
			}

			break;
		}
		case ID_PLAYER_SPELLLIST:
		{
			uint16_t playerID;
			bsIn.Read(playerID);

			ci::IActor *pl;
			try {
				pl = players.at(playerID);
			}
			catch (...) {
				pl = nullptr;
			}
			if (!pl)
				break;

			using SpellList = std::set<ci::Spell *>;
			using EnchList = std::set<ci::Enchantment *>;
			SpellList spellList;
			EnchList enchList;
			while (true)
			{
				uint32_t spellID;
				const bool read = bsIn.Read(spellID);
				if (!read)
					break;
				try {
					spellList.insert(dynamic_cast<ci::Spell *>(magic.at(spellID)));
					enchList.insert(dynamic_cast<ci::Enchantment *>(magic.at(spellID)));
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic Unknown spell in SpellList");
				}
			}
			spellList.erase(nullptr);
			enchList.erase(nullptr);

			static std::map<uint16_t, SpellList> lastSpellList;

			for (auto &pair : this->magic)
			{
				auto ench = dynamic_cast<ci::Enchantment *>(pair.second);
				if (ench != nullptr)
					ench->SetPlayerKnows(false);
			}
			for (auto entry : enchList)
				entry->SetPlayerKnows(true);

			{
				std::vector<ci::Spell *> toRemove;
				std::set_difference(lastSpellList[playerID].begin(), lastSpellList[playerID].end(), spellList.begin(), spellList.end(),
					std::inserter(toRemove, toRemove.begin()));
				for (auto sp : toRemove)
				{
					if (pl == localPlayer)
						pl->RemoveSpell(sp, this->silentInventoryChanges);
				}
			}

			{
				std::vector<ci::Spell *> toAdd;
				std::set_difference(spellList.begin(), spellList.end(), lastSpellList[playerID].begin(), lastSpellList[playerID].end(),
					std::inserter(toAdd, toAdd.begin()));
				for (auto sp : toAdd)
					pl->AddSpell(sp, this->silentInventoryChanges);
			}

			lastSpellList[playerID] = spellList;

			break;
		}
		case ID_PLAYER_ANIM:
		{
			uint16_t playerID;
			uint32_t animID;
			bsIn.Read(playerID);
			bsIn.Read(animID);

			static bool reged = false;
			if (!reged)
			{
				reged = true;
				ci::RegisterAnimation("IdleStopInstant", ~0);
			}

			if (hostedPlayers.count(playerID) != 0 && ci::IsCombatAnimID(animID)) // Prevent doubling attack animations
				break;

			try {
				players.at(playerID)->PlayAnimation(animID);
			}
			catch (...) {
			}
			break;
		}
		case ID_PLAYER_AV:
		{
			uint16_t playerID;
			bsIn.Read(playerID);
			while (true)
			{
				uint8_t avID;
				ci::AVData avData;
				bsIn.Read(avID);
				bsIn.Read(avData.base);
				bsIn.Read(avData.modifier);
				const bool read = bsIn.Read(avData.percentage);
				if (!read)
					break;
				try {
					currentAVsOnServer[avID] = avData.percentage * (avData.base + avData.modifier);
					try {
						auto avName = av.GetAVName(avID);
						if (avName.size() > 0 && avName[0] == '-')
						{
							if (playerID == net.myID)
							{
								const std::string skillName = { avName.begin() + 1, avName.end() - 3 };
								const uint8_t skillPoints = uint8_t(avData.base + avData.modifier);
								localPlayer->SetSkillPointsPercent(skillName, skillPoints);
							}
						}
						else
						{
							players.at(playerID)->UpdateAVData(avName, avData);
						}
					}
					catch (...) {
					}
					allowUpdateAVs = true;
					lastUpdateByServer[avID] = clock();
				}
				catch (...) {
				}
			}
			break;
		}
		case ID_PLAYER_EQUIPMENT:
		{
			uint16_t playerID;
			uint32_t ammo, hands[2];
			bsIn.Read(playerID);
			bsIn.Read(ammo);
			bsIn.Read(hands[0]);
			bsIn.Read(hands[1]);

			ci::IActor *pl = nullptr;

			try {
				pl = players.at(playerID);
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic Equipment: Player not found");
				return;
			}

			auto armorWas = pl->GetEquippedArmor();
			std::set<ci::ItemType *> armorNow;
			uint32_t armor;
			while (bsIn.Read(armor))
			{
				try {
					auto itemType = itemTypes.at(armor);
					pl->EquipItem(itemType, this->silentInventoryChanges, false, false);
					armorNow.insert(itemType);
					if(itemType->GetClass() != ci::ItemType::Class::Armor)
						ci::Log("ERROR:ClientLogic Equipment: Armor is not armor");
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic Equipment: Armor not found");
					return;
				}
			}
			for (auto itemType : armorWas)
			{
				if (armorNow.find(itemType) == armorNow.end())
					pl->UnequipItem(itemType, this->silentInventoryChanges, false, false);
			}

			for (int32_t i = 0; i <= 1; ++i)
			{
				const bool isLeftHand = (i == 1);
				pl->UnequipItem(pl->GetEquippedWeapon(i), true, false, isLeftHand);
				if (hands[i] != ~0)
				{
					try {
						auto itemType = itemTypes.at(hands[i]);
						pl->EquipItem(itemType, this->silentInventoryChanges, false, isLeftHand);
						if (itemType->GetClass() != ci::ItemType::Class::Weapon)
							ci::Log("ERROR:ClientLogic Equipment: Weapon is not weapon");
					}
					catch (...) {
						ci::Log("ERROR:ClientLogic Equipment: Weapon not found");
						return;
					}
				}
			}

			if (ammo != ~0)
			{
				try {
					auto itemType = itemTypes.at(ammo);
					pl->EquipItem(itemType, this->silentInventoryChanges, false, false);
					if (itemType->GetClass() != ci::ItemType::Class::Ammo)
						ci::Log("ERROR:ClientLogic Equipment: Ammo is not ammo");
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic Equipment: Ammo not found");
					return;
				}
			}
			else
			{
				pl->UnequipItem(pl->GetEquippedAmmo(), true, false, false);
			}
			break;
		}
		case ID_PLAYER_MAGIC_EQUIPMENT:
		{
			uint16_t playerID;
			uint32_t spellIDs[2];
			bsIn.Read(playerID);

			ci::IActor *pl;
			try {
				pl = players.at(playerID);
			}
			catch (...) {
				pl = nullptr;
			}
			if (!pl)
				break;

			for (int32_t i = 0; i <= 1; ++i)
			{
				const bool leftHand = (i == 1);

				bsIn.Read(spellIDs[i]);
				if (spellIDs[i] != ~0)
				{
					try {
						ci::Spell *sp;
						try {
							sp = dynamic_cast<ci::Spell *>(magic.at(spellIDs[i]));
						}
						catch (...) {
							sp = nullptr;
							ci::Log("ERROR:ClientLogic spell not found");
						}

						pl->EquipSpell(sp, leftHand);
					}
					catch (...) {
					}
				}
				else
					pl->UnequipSpell(pl->GetEquippedSpell(leftHand), leftHand);
			}
			break;
		}
		case ID_PLAYER_BOW_SHOT:
		{
			uint16_t playerID;
			uint32_t power;
			bsIn.Read(playerID);
			bsIn.Read(power);

			try {
				auto remotePlayer = dynamic_cast<ci::RemotePlayer *>(players.at(playerID));
				if (remotePlayer)
					remotePlayer->Fire(power * 0.01f);
			}
			catch (...) {
			}
			break;
		}
		case ID_PLAYER_SPELL_RELEASE:
		{
			uint16_t playerID;
			int32_t handID;
			bsIn.Read(playerID);
			bsIn.Read(handID);
			try {
				auto remotePlayer = dynamic_cast<ci::RemotePlayer *>(players.at(playerID));
				if (remotePlayer)
					remotePlayer->FireMagic(remotePlayer->GetEquippedSpell(handID), handID);
			}
			catch (...) {
			}
			break;
		}
		case ID_PLAYER_ACTIVE_EFFECTS:
		{
			static std::set<ci::MagicEffect *> knownEffects;

			struct ActiveEffectData
			{
				float duration;
				float magnitude;
				ci::IActor *caster;
			};
			using ActiveEffectList = std::map<ci::MagicEffect *, ActiveEffectData>;
			ActiveEffectList activeEffects;
			
			uint16_t playerID;
			bsIn.Read(playerID);

			ci::IActor *player;
			try {
				player = players.at(playerID);
			}
			catch (...) {
				player = nullptr;
			}
			if (player == nullptr)
			{
				ci::Log(L"ERROR:ClientLogic Magic target not found");
				break;
			}

			uint32_t numEffects;
			bsIn.Read(numEffects);
			for (uint32_t i = 0; i != numEffects; ++i)
			{
				uint32_t effectID;
				uint16_t casterID;
				float magnitude;
				int64_t durationMs;
				bsIn.Read(effectID);
				bsIn.Read(casterID);
				bsIn.Read(magnitude);
				bsIn.Read(durationMs);

				try {
					const auto effect = effects.at(effectID);
					const auto caster = players.at(casterID);

					activeEffects.insert({
						effect, { durationMs / 1000.f, magnitude, caster }
					});
					knownEffects.insert(effect);
				}
				catch (...) {
					ci::Log(L"ERROR:ClientLogic Bad active effect");
				}
			}

			for (auto mgef : knownEffects)
			{
				try {
					auto activeEffectData = activeEffects.at(mgef);
				}
				catch (...) {
					player->RemoveActiveEffect(mgef);
				}
			}
			for (const auto &pair : activeEffects)
			{
				auto mgef = pair.first;
				auto &effectData = pair.second;
				player->AddActiveEffect(mgef, effectData.duration, effectData.magnitude);
			}
			break;
		}
		case ID_ITEMTYPES:
		{
			struct
			{
				uint32_t id;
				uint8_t classID;
				uint8_t subclassID;
				uint32_t existingFormID;
				float weight;
				uint32_t goldVal;
				float armorRating;
				float damage;
				uint32_t equipSlot;
				int32_t soulSize, capacity;
				float health;
			} itemType;
			bsIn.Read(itemType.id);
			bsIn.Read(itemType.classID);
			bsIn.Read(itemType.subclassID);
			bsIn.Read(itemType.existingFormID);
			bsIn.Read(itemType.weight);
			bsIn.Read(itemType.goldVal);
			bsIn.Read(itemType.armorRating);
			bsIn.Read(itemType.damage);
			bsIn.Read(itemType.equipSlot);

			if (itemTypes[itemType.id] == nullptr)
				itemTypes[itemType.id] = new ci::ItemType(
					static_cast<ci::ItemType::Class>(itemType.classID),
					static_cast<ci::ItemType::Subclass>(itemType.subclassID),
					itemType.existingFormID);

			itemTypes[itemType.id]->SetGoldValue(itemType.goldVal);
			itemTypes[itemType.id]->SetWeight(itemType.weight);
			itemTypes[itemType.id]->SetArmorRating(itemType.armorRating);
			itemTypes[itemType.id]->SetDamage(itemType.damage);

			uint32_t numEffects = 0;
			if (!bsIn.Read(numEffects))
				numEffects = 0;
			for (uint32_t i = 0; i != numEffects; ++i)
			{
				enum {
					MAX_DECREMENTS = 1,
				};
				uint64_t minusMinus = 0;
				uint32_t effectID;
				float mag, dur, area;
				bsIn.Read(effectID);
				bsIn.Read(mag);
				bsIn.Read(dur);
				bsIn.Read(area);
				pizdaLabel:
				ci::MagicEffect *effect;
				try {
					effect = effects.at(effectID);
				}
				catch (...) {
					effect = nullptr;
					std::string data;
					for (auto &pair : effects)
						data += std::to_string(pair.first) + ' ';
					auto data2 = std::to_string(effectID);
					if (minusMinus < MAX_DECREMENTS)
					{
						minusMinus++;
						effectID--;
						goto pizdaLabel;
					}
					else
						ci::Log("ERROR:ClientLogic effect not found (ItemType) (" + data + ") " + data2);
				}
				if (effect)
					itemTypes[itemType.id]->AddEffect(effect, abs(mag), dur);
			}

			uint32_t enchID = ~0;
			bsIn.Read(enchID);
			if (enchID != ~0)
			{
				try {
					auto ench = dynamic_cast<ci::Enchantment *>(magic.at(enchID));
					if (ench == nullptr)
						ci::Log("ERROR:ClientLogic Magic - Bad dynamic cast");
					else
						itemTypes[itemType.id]->SetEnchantment(ench);
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic Magic not found");
				}
			}

			bsIn.Read(itemType.soulSize);
			bsIn.Read(itemType.capacity);
			bsIn.Read(itemType.health);
			itemTypes[itemType.id]->SetSoulSize(itemType.soulSize);
			itemTypes[itemType.id]->SetCapacity(itemType.capacity);
			itemTypes[itemType.id]->SetItemHealth(itemType.health);
			break;
		}
		case ID_WEATHER:
		{
			uint32_t wTypeOrID;
			bsIn.Read(wTypeOrID);
			switch (wTypeOrID)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				ci::SetWeatherType(wTypeOrID);
				break;
			default:
				ci::SetWeather(wTypeOrID);
				break;
			}
			break;
		}
		case ID_GLOBAL_VARIABLE:
		{
			uint32_t globalID;
			float val;
			bsIn.Read(globalID);
			bsIn.Read(val);
			ci::SetGlobalVariable(globalID, val);
			break;
		}
		case ID_SILENT:
		{
			bool silent;
			bsIn.Read(silent);
			silentInventoryChanges = silent;
			break;
		}
		case ID_COMMAND:
		{
			std::string str;
			ci::CommandType t = ci::CommandType::CDScript;
			bsIn.Read(t);
			char ch;
			while (bsIn.Read(ch))
				str += ch;
			if (str.size() > 0)
				str.erase(str.end() - 1);

			enum {
				CommandTypeSkymp = 2,
			};

			if ((int32_t)t == CommandTypeSkymp)
				this->ExecuteCommand(str);
			else
				ci::ExecuteCommand(t, str);

			break;
		}
		case ID_CHAT_BUBBLE:
		{
			using T = uint16_t;
			if (packet->length > 2 + sizeof(T))
			{
				std::string message;
				T characters;
				bsIn.Read(characters);

				if (packet->length == 7 + sizeof(T) + sizeof(char) * characters)
				{
					for (size_t i = 0; i != characters; ++i)
					{
						char ch;
						bsIn.Read(ch);
						message += ch;
					}
					uint16_t playerID;
					uint32_t ms;
					bsIn.Read(playerID);
					bsIn.Read(ms);

					if (message.empty())
						message = " ";


					auto str = decodeRu(StringToWstring(message));
					playerBubbles[playerID].reset(new ci::Text3D(str, { 0,0,0 }));
					playerBubbles[playerID]->SetFontHeight(25);
					playerBubbles[playerID]->SetDrawDistance(512.6667f);
					playerBubbles[playerID]->SetPosSource([=] {
						try {
							return players.at(playerID)->GetPos() += {0, 0, 166};
						}
						catch (...) {
							return NiPoint3{ -1000000000, 1000000000, -1000000000 };
						}
					});
					const auto addr = (size_t)playerBubbles[playerID].get();
					ci::SetTimer(ms, [=] {
						if ((size_t)playerBubbles[playerID].get() == addr)
							playerBubbles[playerID] = nullptr;
					});
				}
			}
			break;
		}
		case ID_SPELL:
		{
			uint8_t isEnch = false;
			uint8_t isCustomEnch = false;
			uint32_t id;
			uint32_t formID;
			uint32_t numEffects;
			bsIn.Read(isEnch);
			bsIn.Read(isCustomEnch);
			bsIn.Read(id);
			bsIn.Read(formID);
			bsIn.Read(numEffects);

			if (magic[id] == nullptr)
			{
				if (isEnch)
				{
					auto newEnch = new ci::Enchantment(formID, isCustomEnch);
					newEnch->SetPlayerKnows(false);
					magic[id] = newEnch;
				}
				else
					magic[id] = new ci::Spell(formID);
				for (uint32_t i = 0; i != numEffects; ++i)
				{
					uint32_t effectID;
					float mag, dur, area;
					bsIn.Read(effectID);
					bsIn.Read(mag);
					bsIn.Read(dur);
					bsIn.Read(area);
					try {
						magic[id]->AddEffect(effects.at(effectID), abs(mag), dur);
					}
					catch (...) {
						ci::Log("ERROR:ClientLogic effect not found");
					}
				}
			}
			else
				bsIn.IgnoreBytes(16 * numEffects);

			const auto asSpell = dynamic_cast<ci::Spell *>(magic[id]);
			if (asSpell != nullptr)
			{
				float cost = 0;
				bsIn.Read(cost);
				asSpell->SetCost(cost);
			}
			break;
		}
		case ID_EFFECT:
		{
			uint32_t id = 0;
			uint32_t formID = 0;
			uint8_t archetype = 0;
			uint8_t castingType = 0;
			uint8_t delivery = 0;
			bsIn.Read(id);
			bsIn.Read(formID);
			bsIn.Read(archetype);
			bsIn.Read(castingType);
			bsIn.Read(delivery);

			using FormID = uint32_t;
			static std::map<FormID, ci::MagicEffect *> effectPool;

			if (effectPool[formID] == nullptr)
			{
				if (effects[id] == nullptr)
				{
					effects[id] = new ci::MagicEffect(ci::MagicEffect::Archetype(archetype), formID, ci::MagicEffect::CastingType(castingType), ci::MagicEffect::Delivery(delivery));
				}
				effectPool[formID] = effects[id];
			}
			else
				effects[id] = effectPool[formID];

			static size_t count = 0;
			++count;
			if (count != effects.size())
			{
				//ci::Log("ERROR:ClientLogic wat");
				// TODO: Explain how should it work and refactor
			}
			break;
		}
		case ID_DATASEARCH_INIT:
		{
			static bool inited = false;
			if (inited == false)
			{
				inited = true;
				this->dataSearchEnabled = true;
				ci::Chat::AddMessage(L"DataSearch process activated", true);

				enum class Opcode : uint8_t {
					NavMesh = 0,
					TeleportDoors = 1,
					Container = 2,
					Door = 3,
					Item = 4,
					Actor = 5,
					Acti = 6,
				};

				this->tpdCallback = [this](ci::DataSearch::TeleportDoorsData res) {
					RakNet::BitStream bsOut;
					bsOut.Write(ID_DATASEARCH_RESULT);
					bsOut.Write(Opcode::TeleportDoors);
					for (int32_t i = 0; i != 2; ++i)
					{
						bsOut.Write(res.doors[i].refID);
						bsOut.Write(res.doors[i].baseID);
						bsOut.Write(res.doors[i].locationID);
						bsOut.Write(res.doors[i].pos.x);
						bsOut.Write(res.doors[i].pos.y);
						bsOut.Write(res.doors[i].pos.z);
						bsOut.Write(res.doors[i].rot.x);
						bsOut.Write(res.doors[i].rot.y);
						bsOut.Write(res.doors[i].rot.z);
					}
					net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
					ci::Chat::AddMessage(L"#BEBEBE" L"TPDData sent");
				};

				ci::DataSearch::RequestNavMesh([this](ci::DataSearch::NavMeshData res) {
					RakNet::BitStream bsOut;
					bsOut.Write(ID_DATASEARCH_RESULT);
					bsOut.Write(Opcode::NavMesh);
					bsOut.Write(res.formID);
					bsOut.Write((uint32_t)res.vertices.size());
					for (size_t i = 0; i != res.vertices.size(); ++i)
					{
						bsOut.Write(res.vertices[i].x);
						bsOut.Write(res.vertices[i].y);
						bsOut.Write(res.vertices[i].z);
					}
					bsOut.Write((uint32_t)res.triangles.size());
					for (size_t i = 0; i != res.triangles.size(); ++i)
					{
						bsOut.Write(res.triangles[i].verticeIDs[0]);
						bsOut.Write(res.triangles[i].verticeIDs[1]);
						bsOut.Write(res.triangles[i].verticeIDs[2]);
					}
					bsOut.Write((uint32_t)res.externalConnections.size());
					for (size_t i = 0; i != res.externalConnections.size(); ++i)
					{
						bsOut.Write(res.externalConnections[i]);
					}
					net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
				});

				ci::DataSearch::RequestContainers([this](ci::DataSearch::ContainerData res) {
					RakNet::BitStream bsOut;
					bsOut.Write(ID_DATASEARCH_RESULT);
					bsOut.Write(Opcode::Container);
					bsOut.Write(res.refID);
					bsOut.Write(res.baseID);
					bsOut.Write(res.locationID);
					bsOut.Write(res.pos.x);
					bsOut.Write(res.pos.y);
					bsOut.Write(res.pos.z);
					bsOut.Write(res.rot.x);
					bsOut.Write(res.rot.y);
					bsOut.Write(res.rot.z);
					net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
				});

				ci::DataSearch::RequestDoors([this](ci::DataSearch::DoorData res) {
					RakNet::BitStream bsOut;
					bsOut.Write(ID_DATASEARCH_RESULT);
					bsOut.Write(Opcode::Door);
					bsOut.Write(res.refID);
					bsOut.Write(res.baseID);
					bsOut.Write(res.locationID);
					bsOut.Write(res.pos.x);
					bsOut.Write(res.pos.y);
					bsOut.Write(res.pos.z);
					bsOut.Write(res.rot.x);
					bsOut.Write(res.rot.y);
					bsOut.Write(res.rot.z);
					net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
				});

				ci::DataSearch::RequestActivators([this](ci::DataSearch::ActivatorData res) {
					RakNet::BitStream bsOut;
					bsOut.Write(ID_DATASEARCH_RESULT);
					bsOut.Write(Opcode::Acti);
					bsOut.Write(res.refID);
					bsOut.Write(res.baseID);
					bsOut.Write(res.locationID);
					bsOut.Write(res.pos.x);
					bsOut.Write(res.pos.y);
					bsOut.Write(res.pos.z);
					bsOut.Write(res.rot.x);
					bsOut.Write(res.rot.y);
					bsOut.Write(res.rot.z);
					net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
				});

				ci::DataSearch::RequestActors([this](ci::DataSearch::ActorData res) {
					RakNet::BitStream bsOut;
					bsOut.Write(ID_DATASEARCH_RESULT);
					bsOut.Write(Opcode::Actor);
					bsOut.Write(res.refID);
					bsOut.Write(res.baseID);
					bsOut.Write(res.locationID);
					bsOut.Write(res.pos.x);
					bsOut.Write(res.pos.y);
					bsOut.Write(res.pos.z);
					bsOut.Write(res.rot.x);
					bsOut.Write(res.rot.y);
					bsOut.Write(res.rot.z);
					bsOut.Write(res.race);
					net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE, NULL, net.remote, false);
				});

				ci::DataSearch::RequestItems([this](ci::DataSearch::ItemData res) {
					RakNet::BitStream bsOut;
					bsOut.Write(ID_DATASEARCH_RESULT);
					bsOut.Write(Opcode::Item);
					bsOut.Write(res.refID);
					bsOut.Write(res.baseID);
					bsOut.Write(res.locationID);
					bsOut.Write(res.pos.x);
					bsOut.Write(res.pos.y);
					bsOut.Write(res.pos.z);
					bsOut.Write(res.rot.x);
					bsOut.Write(res.rot.y);
					bsOut.Write(res.rot.z);
					bsOut.Write(res.value);
					bsOut.Write(res.damage);
					bsOut.Write(res.cl);
					bsOut.Write(res.subCl);
					net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE, NULL, net.remote, false);
				});

			}
			break;
		}
		case ID_TEXT_CREATE:
		{
			uint32_t id;
			NiPoint3 pos;
			uint32_t txtSize;
			std::string txt;
			bsIn.Read(id);
			bsIn.Read(pos.x);
			bsIn.Read(pos.y);
			bsIn.Read(pos.z);
			bsIn.Read(txtSize);
			for (uint32_t i = 0; i != txtSize; ++i)
			{
				char wch;
				bsIn.Read(wch);
				txt += wch;
			}
			if (text3Ds.find(id) != text3Ds.end())
				delete text3Ds[id];
			text3Ds[id] = new ci::Text3D(L" ", { 1000000000,1000000000,1000000000 });
			text3Ds[id]->SetPos(pos);
			text3Ds[id]->SetText(decodeRu(StringToWstring(txt)));
			break;
		}
		case ID_TEXT_DESTROY:
		{
			uint32_t id;
			bsIn.Read(id);
			auto it = text3Ds.find(id);
			if (it != text3Ds.end())
			{
				delete it->second;
				text3Ds.erase(it);
			}
			break;
		}
		case ID_REGISTER_ANIMATION:
		case ID_REGISTER_KEYWORD:
		{
			// Deprecated
			break;
		}
		case ID_RECIPE:
		{
			uint32_t id;
			uint32_t prodID;
			uint32_t numProd;
			uint32_t kwrdID;

			bsIn.Read(id);
			bsIn.Read(prodID);
			bsIn.Read(numProd);
			bsIn.Read(kwrdID);

			try {
				auto kwrdTxt = keywords.at(kwrdID);
				try {
					auto prod = itemTypes.at(prodID);

					if (recipes[id] == nullptr)
					{
						recipes[id] = new ci::Recipe(kwrdTxt, prod, numProd);
					}

					//recipes[id]->SetPlayerKnows(true);
					recipes[id]->RemoveAllRecipeComponents();

					try {
						uint32_t contsize;
						bsIn.Read(contsize);
						for (uint32_t i = 0; i != contsize; ++i)
						{
							uint32_t itemTypeID;
							uint32_t count;
							bsIn.Read(itemTypeID);
							const bool read = bsIn.Read(count);
							if (!read)
								break;
							auto type = itemTypes.at(itemTypeID);
							recipes[id]->AddRecipeComponent(itemTypes[itemTypeID], count);
						}
					}
					catch (...) {
						ci::Log("ERROR:ClientLogic Bad recipe container");
					}
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic Bad recipe produce " + std::to_string((int32_t &)prodID));
				}
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic Bad recipe keyword " + std::to_string((int32_t &)kwrdID));
			}

			break;
		}
		case ID_PLAYER_HOST:
		{
			uint16_t playerID, hostID;
			uint8_t isNpc;
			uint16_t combatTarget;
			bsIn.Read(playerID);
			bsIn.Read(hostID);
			bsIn.Read(isNpc);
			if (bsIn.Read(combatTarget) == false)
				combatTarget = ~0;
			if (isNpc)
			{
				if (players.count(playerID) == 0)
					return;
				auto remPl = dynamic_cast<ci::RemotePlayer *>(players.at(playerID));
				if (remPl == nullptr)
					return;

				remPl->SetNicknameVisible(false);
				remPl->SetCombatTarget(combatTarget == 65535 ? nullptr : localPlayer); // shitfix of npc stuck
				
				std::string engine;

				if (hostID == net.myID)
				{
					engine = ("RPEngineIO");
					hostedPlayers.insert(playerID);
					if (tracehost)
					{
						ci::Chat::AddMessage(L"#BEBEBEHost start " + remPl->GetName() + L'[' + std::to_wstring(playerID) + L']');
					}
				}
				else
				{
					engine = ("RPEngineInput");
					hostedPlayers.erase(playerID);
					if (tracehost)
					{
						ci::Chat::AddMessage(L"#BEBEBEHost stop " + remPl->GetName() + L'[' + std::to_wstring(playerID) + L']');
					}
				}

				if (remPl->GetEngine() != engine)
					remPl->SetEngine(engine);
			}
			break;
		}
		default:
			ci::Chat::AddMessage(L"Unknown packet type " + std::to_wstring(packet->data[0]));
			break;

		}
	}

	void UpdateNetworking()
	{
		if (net.peer == nullptr)
			return;

		// SEND:
		try {
			auto movementData = localPlayer->GetMovementData();
			static clock_t cl = 0;
			auto clNow = clock();
			float delay = 40 + (float)players.size();
			if (delay > 60)
				delay = 60;
			if (players.empty() || (players.size() == 1 && players.begin()->second == localPlayer))
				delay = 200;
			if (cl + delay < clNow)
			{
				cl = clNow;
				if (net.remote != RakNet::UNASSIGNED_SYSTEM_ADDRESS && net.fullyConnected
					&& !movementData.isEmpty()
					&& !ci::IsInPause())
				{
					auto sendMovement = [this](uint16_t movementOwner, const ci::MovementData &movement) {
						RakNet::BitStream bsOut;
						bsOut.Write(ID_UPDATE_MOVEMENT);
						Serialize(bsOut, movement);
						bsOut.Write(movementOwner);
						if (movementOwner == net.myID)
							bsOut.Write(localPlayer->GetLocation());
						else
							bsOut.Write(uint32_t(-1));
						bsOut.Write((uint32_t)unreliable);
						net.peer->Send(&bsOut, HIGH_PRIORITY, unreliable, NULL, net.remote, false);
					};

					sendMovement(net.myID, movementData);

					for (auto hosted : hostedPlayers)
					{
						try {
							const auto hostedPl = players.at(hosted);
							const auto md = hostedPl->GetMovementData();
							sendMovement(hosted, md);

							auto asRemote = dynamic_cast<ci::RemotePlayer *>(hostedPl);
							if (asRemote != nullptr)
							{
								const auto hitAnimIDPtr = asRemote->GetNextHitAnim();
								if (hitAnimIDPtr != nullptr)
									this->SendAnimation(*hitAnimIDPtr, hosted);
							}

						}
						catch (...) {
						}
					}
				}
			}
		}
		catch (...) {
			ci::Log("WARN:ClientLogic UpdateNetworking() Send"); // TODO: Refactor
		}

		// RECEIVE:
		for (auto packet = net.peer->Receive(); packet; net.peer->DeallocatePacket(packet), packet = net.peer->Receive())
		{
			const auto packetID = (int32_t)packet->data[0];
			if (net.remote == RakNet::UNASSIGNED_SYSTEM_ADDRESS)
				net.remote = packet->systemAddress;
			if (net.remoteGUID == RakNet::UNASSIGNED_RAKNET_GUID)
				net.remoteGUID = packet->guid;
			try {
				this->ProcessPacket(packet);

				if (tracePackets)
				{
					std::wstringstream ss;
					ss << StringToWstring(GetPacketName(packetID));
					ci::Chat::AddMessage(ss.str());
				}
			}
			catch (...) {
				ci::Log("WARN:ClientLogic UpdateNetworking() Receive " + std::to_string(packetID)); // TODO: Refactor
			}
		}
	}

	void OnStartup() override
	{
		ci::SetVolume(-1);

		for (int32_t i = 0; i != 1024; ++i)
			this->lastUpdateByServer[i] = NULL;

		const int32_t cfgNumLines = read_cfg();
		for(auto badSymbol : {'\n', '\r'})
			for (auto &pair : g_config)
			{
				auto &v = pair.second;
				while (v.size() > 0 && v.back() == badSymbol)
					v.pop_back();
				while (v.size() > 0 && v.front() == badSymbol)
					v = { v.begin() + 1, v.end() };
			}

		ci::Chat::Init(
			atoi(g_config[CONFIG_CHAT_OFFSET_X].data()), 
			atoi(g_config[CONFIG_CHAT_OFFSET_Y].data())
		);
		ci::Chat::AddMessage(L"#eeeeeeSkyMP " + StringToWstring(version) + L" Client Started");
		ci::LocalPlayer::GetSingleton()->SetName(L"Player");

		ci::SetUpdateRate(1);

		if (cfgNumLines == 0)
		{
			ci::Chat::AddMessage(L"#eeeeee" + StringToWstring(cfgFile) + L" not found");
			return;
		}

		if (IsRussianTranslate())
			ci::Chat::SetRussianUser();

		const auto drawDistance = g_config[CONFIG_DRAW_DISTANCE];
		if (drawDistance.empty() == false && atoi(drawDistance.data()) != 0)
			ci::SetSyncOption("DRAW_DISTANCE", drawDistance);

		const auto fov = g_config[CONFIG_FOV];
		if (fov.empty() == false && atoi(fov.data()) != 0)
			ci::ExecuteCommand(ci::CommandType::Console, "fov " + fov);

		auto name = StringToWstring(g_config[CONFIG_NAME]);

		if (g_config[CONFIG_TCP_ONLY] == "true")
		{
			ci::Chat::AddMessage(L"Switch reliability...");
			Sleep(500);
			unreliable = PacketReliability::RELIABLE;
			ci::Chat::AddMessage(L"Done");
		}

		if (name != L"")
		{
			this->haveName = true;
			this->ConnectToServer(
				g_config[CONFIG_IP],
				std::atoi(g_config[CONFIG_PORT].data()),
				version,
				g_config[CONFIG_SERVER_PASSWORD],
				name);
			localPlayer->SetName(name);
			PrintStartupInfo();
		}
		else
		{
			if (IsRussianTranslate())
				ci::Chat::AddMessage(L"������� T � ������� ��� �������");
			else
				ci::Chat::AddMessage(L"Press T and type your name");
		}
	}

	void OnActivate(ci::Object *self, bool isOpen, ci::IActor *source)
	{
		RakNet::BitStream bsOut;
		bsOut.Write(ID_ACTIVATE);
		const auto id = GetID(self);
		const auto sourceId = GetID(source);
		bsOut.Write(id);
		bsOut.Write((uint8_t)isOpen);
		bsOut.Write(sourceId);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnActivate(ci::IActor *self)
	{
		RakNet::BitStream bsOut;
		bsOut.Write(ID_ACTIVATE);
		const auto id = GetID(self);
		bsOut.Write((uint32_t)0);
		bsOut.Write(id);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnContainerChanged(ci::Object *self, const ci::ItemType *itemType, uint32_t count, bool isAdd)
	{
		RakNet::BitStream bsOut;
		bsOut.Write(ID_CONTAINER_CHANGED);

		uint32_t itemTypeID;
		for (auto it = itemTypes.begin(); it != itemTypes.end(); ++it)
			if (it->second == itemType)
			{
				itemTypeID = it->first;
				break;
			}
		bsOut.Write(itemTypeID);
		bsOut.Write(count);
		bsOut.Write((uint32_t)isAdd);
		bsOut.Write(GetID(self));
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnHit(ci::IActor *hitTarget, ci::IActor *hitSource, const ci::HitEventData &eventData)
	{
		const auto playerID = GetID(hitTarget);
		const auto weaponID = GetID(eventData.weapon);
		const auto spellID = GetID(eventData.spell);
		const auto sourceID = GetID(hitSource);

		RakNet::BitStream bsOut;
		bsOut.Write(ID_HIT_PLAYER);
		bsOut.Write(playerID);
		bsOut.Write(weaponID);
		bsOut.Write(eventData.powerAttack);
		bsOut.Write(spellID);
		bsOut.Write(sourceID);
		net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);

		if (tracehost)
		{
			std::wstringstream ss;
			ss << L"#BEBEBE" << L"OnHit Source = " << sourceID << L"OnHit Target = " << playerID;
			ci::Chat::AddMessage(ss.str());
		}
	}

	void OnHit(ci::Object *hitTarget, const ci::HitEventData &eventData)
	{
		const auto objectID = GetID(hitTarget);
		const auto weaponID = GetID(eventData.weapon);
		const auto spellID = GetID(eventData.spell);

		RakNet::BitStream bsOut;
		bsOut.Write(ID_HIT_OBJECT);
		bsOut.Write(objectID);
		bsOut.Write(weaponID);
		bsOut.Write(eventData.powerAttack);
		bsOut.Write(spellID);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnWorldInit() override
	{
		static bool firstInit = true;

		if (firstInit)
		{
			Tests::Run();

			localPlayer->onPerkSelect.Add([=](ci::LocalPlayer::_PerkID perkID) {
				RakNet::BitStream bsOut;
				bsOut.Write(ID_LEARN_PERK);
				bsOut.Write((uint32_t)perkID);
				net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
			});

			localPlayer->onLevelUp.Add([=](std::string increasedAv) {
				RakNet::BitStream bsOut;
				bsOut.Write(ID_LEVEL_UP);
				if (increasedAv == "Health")
					bsOut.Write((uint8_t)av.Health);
				else if (increasedAv == "Magicka")
					bsOut.Write((uint8_t)av.Magicka);
				else if(increasedAv == "Stamina")
					bsOut.Write((uint8_t)av.Stamina);
				net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
			});

			localPlayer->onPlayerBowShot.Add([=](float power) {
				const auto powerInt = int32_t(power * 100);
				RakNet::BitStream bsOut;
				bsOut.Write(ID_BOW_SHOT);
				bsOut.Write(powerInt);
				net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
			});

			localPlayer->onPlayerMagicRelease.Add([=](int32_t handID) {
				RakNet::BitStream bsOut;
				bsOut.Write(ID_SPELL_RELEASE);
				bsOut.Write(handID);
				net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
			});

			localPlayer->onHit.Add([=](ci::HitEventData evn) {
				try {
					auto source = players.at((uint16_t)atoi(evn.hitSrcMark.data()));
					if (!source)
						ci::Log("ERROR:ClientLogic Invalid hit source");
					else
						this->OnHit(localPlayer, source, evn);
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic LocalPlayer OnHit()");
				}
			});
		}

		firstInit = false;
	}

	ci::Signal<void()> testUpd;
	std::list<std::function<void()>> fns;

	void RespawnPlayers()
	{
		if (ci::IsLoadScreenOpen())
		{
			ci::SetTimer(1000, [this] {
				this->RespawnPlayers();
			});
		}
		else
		{
			ci::SetTimer(2500, [this] {
				for (auto &pair : players)
				{
					auto pl = dynamic_cast<ci::RemotePlayer *>(pair.second);
					if (pl)
						pl->Respawn();
				}
			});
		}
	}

	void OnUpdate() override
	{
		/*static uint32_t loc = 0;
		const uint32_t newLoc = localPlayer->GetCell();
		if (loc != newLoc)
		{
			this->RespawnPlayers();
			loc = newLoc;
		}*/

		{
			const auto now = ci::IsKeyPressed(VK_ESCAPE);
			static bool was = false;
			if (was != now)
			{
				was = now;
				if (now)
					if (ci::Dialog::Hide())
					{
						this->OnDialogResponse(lastDialogID, { L"", -1 });
					}
			}
		}

		{
			const auto now = ci::IsKeyPressed(VK_F5);
			static bool was = false;
			if (was != now)
			{
				was = now;
				if (now)
				{
					for (auto &pair : players)
					{
						auto pl = dynamic_cast<ci::RemotePlayer *>(pair.second);
						if (pl && pl->GetEngine() != "RPEngineIO")
							pl->Respawn();
					}
				}
			}
		}

		// Freeze activators (fake items)
		static clock_t lastCheck = 0;
		if (clock() - lastCheck > 100)
		{
			lastCheck = clock();
			for (auto &p : objects)
			{
				//if (p.second->IsCrosshairRef())
				{
					try {
						if (objectsInfo.at(p.first).type != ClientLogic::Type::Item)
						{
							p.second->SetMotionType(ci::Object::Motion_Keyframed);
						}
					}
					catch (...) {
					}
					break;
				}
			}
		}

		for (auto &f : fns)
		{
			f();
		}

		if (testUpd)
			testUpd();

		try {
			static bool was = false;
			bool nowOpen = ci::IsLoadScreenOpen();
			if (net.fullyConnected && was != nowOpen)
			{
				const RakNet::MessageID msgid = nowOpen ? ID_LOADING_START : ID_LOADING_STOP;
				RakNet::BitStream bsOut;
				bsOut.Write(msgid);
				was = nowOpen;
				net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
			}

			static bool wasPaused = true;
			bool nowPaused = ci::IsInPause() || nowOpen;
			if (net.fullyConnected && wasPaused != nowPaused)
			{
				const RakNet::MessageID msgid = nowPaused ? ID_PAUSE_START : ID_PAUSE_STOP;
				RakNet::BitStream bsOut;
				bsOut.Write(msgid);
				wasPaused = nowPaused;
				net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
			}
		}
		catch (...) {
		}

		static clock_t cl = 0;
		auto clNow = clock();
		if (cl + 10 < clNow)
		{
			try {
				cl = clNow;
				try {
					UpdateNetworking();
				}
				catch (...) {
					ci::Log("WARN:ClientLogic UpdateNetworking()"); // TODO: Refactor
				}
				try {
					UpdateObjects();
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic UpdateObjects()");
				}
				try {
					UpdateCombat();
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic UpdateCombat()");
				}
				try {
					UpdateActorValues();
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic UpdateActorValues()");
				}
				try {
					UpdateEquippedArmor();
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic UpdateEquippedArmor()");
				}
				try {
					UpdateEquippedWeapon<0>();
					UpdateEquippedWeapon<1>();
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic UpdateEquippedWeapon<>()");
				}
				try {
					UpdateEquippedSpell<0>();
					UpdateEquippedSpell<1>();
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic UpdateEquippedSpell<>()");
				}
				try {
					UpdateEquippedAmmo();
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic UpdateEquippedAmmo()");
				}
				try {
					UpdateKnownEffects();
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic UpdateKnownEffects()");
				}
				try {
					UpdateWerewolfs();
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic UpdateWerewolfs()");
				}
			}
			catch (const std::exception &e) {
				ci::Log("ERROR:ClientLogic OnUpdate() %s", e.what());
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic OnUpdate()");
			}
		}
	}

	void OnChatMessage(const std::wstring &text) override
	{
		if (!this->haveName)
		{
			this->haveName = true;
			g_config[CONFIG_NAME] = WstringToString(text);
			this->ConnectToServer(
				g_config[CONFIG_IP],
				std::atoi(g_config[CONFIG_PORT].data()),
				version,
				g_config[CONFIG_SERVER_PASSWORD],
				StringToWstring(g_config[CONFIG_NAME]));
			localPlayer->SetName(text);
			PrintStartupInfo();
			return;
		}

		if (net.peer == nullptr || net.remote == RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			return;

		RakNet::BitStream bsOut;
		bsOut.Write(ID_MESSAGE);
		auto text_ = encodeRu(text);
		bsOut.Write((uint16_t)text_.size());
		for (auto it = text_.begin(); it != text_.end(); ++it)
			bsOut.Write(*it);

		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnChatCommand(const std::wstring &cmdText, const std::vector<std::wstring> &arguments) override
	{
		static ci::LookData testLookData;
		static ci::Object *obj = nullptr;
		static std::vector<ci::RemotePlayer *> ps;
		static std::map<ci::RemotePlayer *, NiPoint3> offsets;
		static bool traceHorseMovement = false;

		if (cmdText == L"/q" || cmdText == L"//q")
		{
			if (net.peer != nullptr)
			{
				net.fullyConnected = false;
				net.peer->CloseConnection(net.remoteGUID, true);
			}
			ci::SetTimer(300, [] {
				std::exit(0);
			});
		}
		else if (cmdText == L"//mute")
		{
			static bool mute = true;
			ci::SetVolume(mute ? 0 : -1);
			mute = !mute;
		}
		else if (cmdText == L"//fov")
		{
			static int32_t fov = 0;
			const int32_t fovNow = _wtoi(arguments[0].data());
			if (fov != fovNow)
			{
				ci::ExecuteCommand(ci::CommandType::Console, "fov " + std::to_string(fovNow));
			}
		}
		else if (cmdText == L"//syncopt" || cmdText == L"//so")
		{
			ci::SetSyncOption(WstringToString(arguments[0]), WstringToString(arguments[1]));
			ci::Chat::AddMessage((L"#BEBEBE" L"Set ") + arguments[0] + L" to " + arguments[1]);
		}
		else if (cmdText == L"//stat")
		{
			static bool tr = true;
			ci::RemotePlayer::SetTracing(tr);
			ci::Object::SetTracing(tr);
			tr = !tr;
		}
		else if (cmdText == L"//eq")
		{
			for(auto p : ps)
			{
				while (true)
				{
					auto anim = p->GetNextHitAnim();
					if (anim == nullptr)
						break;
					localPlayer->PlayAnimation(*anim);
				}

				while (dbg.hitAnims.empty() == false)
				{
					auto anim = dbg.hitAnims.front();
					dbg.hitAnims.pop_front();
					p->PlayAnimation(*anim);
				}

				{
					auto handR = localPlayer->GetEquippedWeapon(),
						handL = localPlayer->GetEquippedWeapon(true);
					if (handR)
					{
						p->AddItem(handR, 1, true);
						p->EquipItem(handR, true, false, false);
					}
					else
						p->UnequipItem(p->GetEquippedWeapon(0), true, false, 0);
					if (handL)
					{
						p->AddItem(handL, 1, true);
						p->EquipItem(handL, true, false, true);
					}
					else
						p->UnequipItem(p->GetEquippedWeapon(1), true, false, 1);
				}

				{
					const ci::Spell *hands[2] = {
						localPlayer->GetEquippedSpell(0),
						localPlayer->GetEquippedSpell(1)
					};
					for (int i = 0; i <= 1; ++i)
					{
						if (hands[i] != nullptr)
						{
							p->AddSpell(hands[i], true);
							p->EquipSpell(hands[i], i);
						}
						else
							p->UnequipSpell(p->GetEquippedSpell(i), i);
					}
				}

				auto armorWas = p->GetEquippedArmor();
				for (auto item : armorWas)
				{
					p->UnequipItem(item, true, false, false);
					p->RemoveItem(item, 1, false);
				}
				auto armor = localPlayer->GetEquippedArmor();
				for (auto item : armor)
				{
					p->AddItem(item, 1, true);
					p->EquipItem(item, true, false, false);
				}

				auto ammo = localPlayer->GetEquippedAmmo();
				if (ammo)
				{
					p->AddItem(ammo, 1, true);
					p->EquipItem(ammo, true, false, false);
				}
				else
					p->UnequipItem(p->GetEquippedAmmo(), true, false, false);

				auto m = localPlayer->GetMovementData();
				m.pos += offsets[p];
				p->ApplyMovementData(m);
				p->SetCell(localPlayer->GetCell());
				p->SetWorldSpace(localPlayer->GetWorldSpace());

				int32_t clearVisualEffect = 0;
				for(int32_t i = 1; i >= 0; --i)
					if ((int32_t)m.castStage[i] != 0)
					{
						p->MagicAttackBegin(i);
						p->SetVisualMagicEffect(p->GetEquippedSpell(i));
					}
					else
					{
						p->MagicAttackEnd(i);
						++clearVisualEffect;
					}
				if(clearVisualEffect == 2)
					p->SetVisualMagicEffect(nullptr);
			
			}
		}
		else if (cmdText == L"//tpd")
		{
			if (dataSearchEnabled)
			{
				ci::DataSearch::RequestTeleportDoorsManual([this](ci::DataSearch::TeleportDoorsData res) {
					if (this->tpdCallback != nullptr)
						this->tpdCallback(res);
				});
				ci::Chat::AddMessage(L"#BEBEBE" L"DataSearch TPD module is ready");
			}
			else
				ci::Chat::AddMessage(L"#BEBEBE" L"DataSearch is disabled on your client");
		}
		else if (cmdText == L"//s" || cmdText == L"//setspeedmult")
		{
			if (arguments.empty())
				return;
			if (dataSearchEnabled)
			{
				const auto str = WstringToString(arguments[0]);
				const auto sm = atoi(str.data());
				ci::LocalPlayer::GetSingleton()->SetSpeedmult((float)sm);
				ci::Chat::AddMessage(L"#BEBEBE" L"Set Speedmult to " + std::to_wstring(sm));
			}
			else
				ci::Chat::AddMessage(L"#BEBEBE" L"DataSearch is disabled on your client");
		}
		else if (cmdText == L"//players")
		{
			static auto text3d = new ci::Text3D(L"", { 0,0,0 });
			static bool enabled = false;
			enabled = !enabled;

			if (enabled)
			{
				text3d->SetTextSource([this] {
					std::wstring text;
					for (auto &pair : players)
					{
						text += pair.second->GetName();
						text += L"\n";
					}
					return text;
				});
				text3d->SetDrawDistance(8192.0);
				text3d->SetPosSource([this] {
					return localPlayer->GetPos();
				});
			}
			else
			{
				delete text3d;
				text3d = new ci::Text3D(L"", { 0,0,0 });
			}
		}
		else if (cmdText == L"//dump")
		{
			ci::Chat::AddMessage(L"#BEBEBE" L"Running Lua Codegen");
			ci::DataSearch::LuaCodegenStart([] {
				ci::Chat::AddMessage(L"#BEBEBE" L"Done");
			});
		}
		else if (cmdText == L"//testperk")
		{
			static auto Alchemist80 = new ci::Perk(0x000C07CD);
			Alchemist80->SetRequiredSkillLevel(1000);
			if (arguments.size() == 0 || arguments[0] == L"add")
			{
				ci::Chat::AddMessage(L"#BEBEBE" L"Perk added");
				localPlayer->AddPerk(Alchemist80);
			}
			else
			{
				ci::Chat::AddMessage(L"#BEBEBE" L"Perk removed");
				localPlayer->RemovePerk(Alchemist80);
			}
		}
		else if (cmdText == L"//tracehost")
		{
			tracehost = !tracehost;
			ci::Chat::AddMessage((std::wstring)L"#BEBEBE" L">> tracehost " + (tracehost ? L"On" : L"Off"));
		}
		else if (cmdText == L"//tracepackets")
		{
			tracePackets = !tracePackets;
			ci::Chat::AddMessage((std::wstring)L"#BEBEBE" L">> tracepackets " + (tracePackets ? L"On" : L"Off"));
		}
		else if (cmdText == L"//ping")
		{
			const int32_t ping = net.peer ? net.peer->GetAveragePing(net.remoteGUID) : 0;
			ci::Chat::AddMessage(L"#BEBEBE" L"Average ping is " + std::to_wstring(ping));
		}
		else if (cmdText == L"//tracehorsemd")
		{
			traceHorseMovement = !traceHorseMovement;
			ci::Chat::AddMessage((std::wstring)L"#BEBEBE" L">> tracehorsemd " + (traceHorseMovement ? L"On" : L"Off"));
		}
		else if (cmdText == L"//testhorse")
		{
			static std::vector<ci::RemotePlayer *> horses;
			horses.clear();
			for (int32_t i = 0; i != 2; ++i)
			{
				auto onActivate = [=] {
					if (localPlayer->IsOnMount() == false)
					{
						localPlayer->EnterHorse(horses[i]);
					}
					else
					{
						localPlayer->ExitHorse();
					}
				};

				auto p = new ci::RemotePlayer(
					localPlayer->GetName(),
					localPlayer->GetLookData(),
					localPlayer->GetPos(),
					localPlayer->GetCell(),
					localPlayer->GetWorldSpace(),
					{}, "",
					onActivate);
				p->SetBaseNPC(0x00109E3D);
				horses.push_back(p);
			}
			if (horses.size() == 2)
			{
				auto horseOut = horses.back();
				auto horseIn = horses.front();

				horseOut->SetEngine("RPEngineIO");
				horseIn->SetEngine("RPEngineInput");

				testUpd.Add([=] {
					auto md = horseOut->GetMovementData();
					if (traceHorseMovement)
					{
						std::wstringstream ss;
						ss << L"#BEBEBE" << L"SpeedSampled " << md.speedSampled << L" RunMode " << (int32_t)md.runMode;
						ci::Chat::AddMessage(ss.str());
					}
					md.pos += {128, 128, 0};
					horseIn->ApplyMovementData(md);
					if (ps.size() > 0)
					{
						if (ps.front()->GetMovementData().mountStage == ci::MovementData::MountStage::OnMount)
							ps.front()->SetAttachedHorse(horseIn);
						else
							ps.front()->SetAttachedHorse(nullptr);
					}
				});
			}
		}
		else if (cmdText == L"//clone")
		{
			static uint32_t counter = 0;
			const auto id = counter++;
			static std::map<uint32_t, ci::IActor *> testPlayers;

			auto onHit = [this, id](const ci::HitEventData &eventData) {
				std::wstring msg;
				if (eventData.spell != nullptr)
					msg = (L"#BEBEBE" L"Debug: Magic Hit");
				else if (eventData.weapon != nullptr)
					msg = (L"#BEBEBE" L"Debug: Weap Hit");
				else
					msg = (L"#BEBEBE" L"Debug: H2H Hit");
				ci::Chat::AddMessage(msg + L" " + StringToWstring(eventData.hitSrcMark));

				try {
					const bool h2h = eventData.spell == nullptr && eventData.weapon == nullptr;
					if (h2h)
					{
						const auto closestAc = FindClosestActor(testPlayers.at(id)->GetPos(), [=](ci::IActor *ac) { return ac != testPlayers.at(id); });
						if ((closestAc->GetPos() - testPlayers.at(id)->GetPos()).Length() > 256)
						{
							auto rem = dynamic_cast<ci::RemotePlayer *>(testPlayers.at(id));
							if (rem != nullptr)
							{
								ci::Log("ERROR:ClientLogic TEST bad h2h hit (is it invisible gnome?)");
								rem->Respawn();
							}
						}
					}
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic TEST player not found");
				}
			};

			auto p = new ci::RemotePlayer(
				localPlayer->GetName(), 
				localPlayer->GetLookData(), 
				localPlayer->GetPos(), 
				localPlayer->GetCell(), 
				localPlayer->GetWorldSpace(), 
				onHit, "RPEngineInput", nullptr);
			testPlayers[id] = p;
			if (arguments.size() == 1 && arguments[0] == L"horse")
				p->SetBaseNPC(0x00109E3D);

			auto armor = localPlayer->GetEquippedArmor();
			for (auto item : armor)
			{
				p->AddItem(item, 1, true);
				p->EquipItem(item, true, false, false);
			}

			auto handR = localPlayer->GetEquippedWeapon(),
				handL = localPlayer->GetEquippedWeapon(true);
			if (handR)
			{
				p->AddItem(handR, 1, true);
				p->EquipItem(handR, true, false, false);
			}
			if (handL)
			{
				p->AddItem(handL, 1, true);
				p->EquipItem(handL, true, false, true);
			}

			localPlayer->onPlayerBowShot.Add([=](float power) {
				p->Fire(power);
			});

			localPlayer->onPlayerMagicRelease.Add([=](int32_t handID) {
				p->FireMagic(p->GetEquippedSpell(handID), handID);
			});

			ps.push_back(p);
			offsets[p] = NiPoint3{ 128.f * ps.size(), 128.f * ps.size(), 0 };

			static bool added = false;
			if (!added)
			{
				added = true;
				testUpd.Add([=] {
					this->OnChatCommand(L"//eq", {});
					if (p->GetEngine() != "RPEngineInput")
					{
						ci::Log("ERROR:ClientLogic Bad engine (test)");
					}
				});
			}
		}
		else if (cmdText == L"//cddbg" || cmdText == L"//cdtrace")
		{
			static bool tr = true;
			ci::TraceCDCalls(tr);
			tr = !tr;
		}
		else
		{
			std::wstring str = cmdText;
			for (auto it = arguments.begin(); it != arguments.end(); ++it)
				str += L' ' + *it;
			this->OnChatMessage(str);
		}
	}

	void OnRaceMenuExit() override
	{
		ci::SetTimer(1300, [&] {
			auto newLook = localPlayer->GetLookData();
			ld = newLook;

			RakNet::BitStream bsOut;
			bsOut.Write(ID_UPDATE_LOOK);
			Serialize(bsOut, newLook);

			net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);

			RakNet::BitStream bs;
			Serialize(bs, newLook);
			Deserialize(bs, newLook);
			//localPlayer->ApplyLookData(newLook);

		});
	}

	void OnItemDropped(const ci::ItemType *itemType, uint32_t count) override
	{
		const uint32_t itemTypeID = GetID(itemType);
		RakNet::BitStream bsOut;
		bsOut.Write(ID_DROPITEM);
		bsOut.Write(itemTypeID);
		bsOut.Write(count);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnItemUsed(const ci::ItemType *itemType) override
	{
		// shitfix doubling this callback:
		static bool send = true;
		if (send)
		{
			const uint32_t itemTypeID = GetID(itemType);
			RakNet::BitStream bsOut;
			bsOut.Write(ID_USEITEM);
			bsOut.Write(itemTypeID);
			net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
		}
		send = !send;
	}

	void OnItemUsedInCraft(const ci::ItemType *itemType, uint32_t count) override
	{
		const uint32_t itemTypeID = GetID(itemType);
		RakNet::BitStream bsOut;
		bsOut.Write(ID_CRAFT_INGREDIENT);
		bsOut.Write(itemTypeID);
		bsOut.Write(count);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnCraftFinish(bool isPoison, const ci::ItemType *itemType, uint32_t numCraftedItems) override
	{
		RakNet::BitStream bsOut;

		bool isAlchemy = itemType == nullptr;
		if (isAlchemy)
		{
			bsOut.Write(ID_CRAFT_FINISH_ALCHEMY);
			bsOut.Write((uint64_t)isPoison);
		}
		else
		{
			const uint32_t itemTypeID = GetID(itemType);
			bsOut.Write(ID_CRAFT_FINISH);
			bsOut.Write(itemTypeID);
			bsOut.Write(numCraftedItems);
		}

		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnLockpick() override
	{
		RakNet::BitStream bsOut;
		bsOut.Write(ID_LOCKPICK);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnPoisonAttack() override
	{
		RakNet::BitStream bsOut;
		bsOut.Write(ID_POISON_ATTACK);
		net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnItemEnchanting(const ci::ItemType *itemType, const ci::Enchantment *ench) override
	{
		const uint32_t itemTypeID = GetID(itemType);
		const uint32_t enchID = GetID(ench);
		RakNet::BitStream bsOut;
		bsOut.Write(ID_ENCHANTING_ITEM);
		bsOut.Write(itemTypeID);
		bsOut.Write(enchID);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void UpdateObjects()
	{
		const uint32_t sendingInterval = 50;
		bool sendMovement = false;
		static clock_t lastSend = 0;
		if (clock() - lastSend > sendingInterval)
		{
			lastSend = clock();
			sendMovement = true;
		}

		for (auto &pair : objects)
		{
			const auto id = pair.first;
			const auto object = pair.second;

			auto &info = objectsInfo[id];
			if (info.type != Type::Item)
				continue;
			if (info.isHostable == false)
				continue;

			enum : uint16_t {
				INVALID_HOST_ID = (uint16_t)~0,
			};

			int32_t motionType = ci::Object::Motion_Keyframed;

			const bool hostedByOther =
				info.hostID != INVALID_HOST_ID && info.hostID != net.myID;
			if (!hostedByOther)
			{
				const float distance = (object->GetPos() - localPlayer->GetPos()).Length();
				if (object->IsCrosshairRef() || object->IsGrabbed() || distance < 256)
				{
					motionType = ci::Object::Motion_Dynamic;
					if (info.hostID == INVALID_HOST_ID)
					{
						RakNet::BitStream bsOut;
						bsOut.Write(ID_HOST_START);
						bsOut.Write(id);
						net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
					}
				}
			}

			const bool hostedByMe =
				info.hostID == net.myID;
			if (hostedByMe && sendMovement)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_HOSTED_OBJECT_MOVEMENT);
				bsOut.Write(id);
				const auto pos = object->GetPos();
				bsOut.Write(pos.x);
				bsOut.Write(pos.y);
				bsOut.Write(pos.z);
				const auto rot = object->GetRot();
				bsOut.Write(rot.x);
				bsOut.Write(rot.y);
				bsOut.Write(rot.z);
				const bool isGrabbed = object->IsGrabbed();
				bsOut.Write(isGrabbed);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, unreliable, NULL, net.remote, false);
			}

			info.lastSpeed = (object->GetSpeed() + info.lastSpeed) / 2;
			if(info.lastSpeed > 2.0)
				motionType = ci::Object::Motion_Dynamic;

			if (hostedByOther)
				motionType = ci::Object::Motion_Keyframed;

			if (object->GetMotionType() != motionType)
				object->SetMotionType(motionType);

			if (object->IsGrabbed())
			{
				static bool timerSet = false;
				if (!timerSet)
				{
					timerSet = true;
					ci::SetTimer(1333, [=] {
						try {
							auto object = objects.at(id);
							if (objectsInfo[id].hostID != net.myID)
							{
								auto pos = object->GetPos();
								object->Respawn();
								object->SetPosition(pos);
							}
						}
						catch (...) {
						}
						timerSet = false;
					});
				}
			}
		}
	}

	void SendAnimation(uint32_t animID, uint16_t source)
	{
		RakNet::BitStream bsOut;
		bsOut.Write(ID_ANIMATION_EVENT_HIT);
		bsOut.Write(animID);
		bsOut.Write(source);
		net.peer->Send(&bsOut, LOW_PRIORITY, unreliable, NULL, net.remote, false);
	}

	void UpdateCombat()
	{
		const auto hitAnimIDPtr = dynamic_cast<ci::LocalPlayer *>(localPlayer)->GetNextHitAnim();
		if (hitAnimIDPtr != nullptr)
		{
			dbg.hitAnims.push_back(hitAnimIDPtr);
			this->SendAnimation(*hitAnimIDPtr, net.myID);
		}
	}

	void UpdateActorValues()
	{
		if (allowUpdateAVs == false)
			return;
		static clock_t lastUpd = 0;
		if (lastUpd + 550 < clock())
		{
			lastUpd = clock();
			for (uint8_t avID = av.Health; avID <= av.Stamina; ++avID)
			{
				if (clock() - this->lastUpdateByServer[avID] < 333)
					continue;
				RakNet::BitStream bsOut;
				bsOut.Write(ID_AV_CHANGED);
				const auto name = av.GetAVName(avID);
				const auto avData = localPlayer->GetAVData(name);
				auto p = avData.percentage;
				if (p < 0)
					p = 0;
				else if (p > 1)
					p = 1;
				if (p != currentAVsOnServer[avID])
				{
					static_assert(sizeof(avID) == 1, "");
					bsOut.Write(avID);
					bsOut.Write(p);
					currentAVsOnServer[avID] = p;
					net.peer->Send(&bsOut, MEDIUM_PRIORITY, unreliable, NULL, net.remote, false);
				}
			}
		}
	}

	void UpdateEquippedArmor()
	{
		enum {
			INVALID_HAND = -1,
		};
		const int32_t handID = INVALID_HAND;

		const std::vector<ci::ItemType *> armor = localPlayer->GetEquippedArmor();
		static std::vector<ci::ItemType *> armorLast;

		if (armor != armorLast)
		{
			for (auto itemType : armor)
			{
				const bool sendEquip = std::find(armorLast.begin(), armorLast.end(), itemType) == armorLast.end();
				if (sendEquip)
				{
					RakNet::BitStream bsOut;
					bsOut.Write(ID_EQUIP_ITEM);
					bsOut.Write(GetID(itemType));
					bsOut.Write(handID);
					net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
				}
			}
			for (auto itemType : armorLast)
			{
				const bool sendUnequip = std::find(armor.begin(), armor.end(), itemType) == armor.end();
				if (sendUnequip)
				{
					RakNet::BitStream bsOut;
					bsOut.Write(ID_UNEQUIP_ITEM);
					bsOut.Write(GetID(itemType));
					bsOut.Write(handID);
					net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
				}
			}


			armorLast = armor;
		}
	}

	template<int32_t handID>
	void UpdateEquippedWeapon()
	{
		enum : bool {
			isLeftHand = handID != 0
		};
		ci::ItemType *const weap = localPlayer->GetEquippedWeapon(isLeftHand);
		static ci::ItemType *weapWas = nullptr;
		if (weap != weapWas)
		{
			if (weapWas != nullptr)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_UNEQUIP_ITEM);
				bsOut.Write(GetID(weapWas));
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			if (weap != nullptr)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_EQUIP_ITEM);
				bsOut.Write(GetID(weap));
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			weapWas = weap;
		}
	}

	template<int32_t handID>
	void UpdateEquippedSpell()
	{
		enum : bool {
			isLeftHand = handID != 0
		};
		const ci::Spell *const sp = localPlayer->GetEquippedSpell(isLeftHand);
		static const ci::Spell *spWas = nullptr;
		if (sp != spWas)
		{
			if (spWas != nullptr)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_UNEQUIP_SPELL);
				bsOut.Write(GetID(spWas));
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			if (sp != nullptr)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_EQUIP_SPELL);
				bsOut.Write(GetID(sp));
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			spWas = sp;
		}
	}

	void UpdateEquippedAmmo()
	{
		ci::ItemType *const ammo = localPlayer->GetEquippedAmmo();
		static ci::ItemType *ammoWas = nullptr;
		if (ammo != ammoWas)
		{
			enum {
				INVALID_HAND = -1,
			};
			const int32_t handID = INVALID_HAND;

			if (ammoWas != nullptr)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_UNEQUIP_ITEM);
				bsOut.Write(GetID(ammoWas));
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			if (ammo != nullptr)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_EQUIP_ITEM);
				bsOut.Write(GetID(ammo));
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			ammoWas = ammo;
		}
	}

	using EffectIndex = uint32_t;

	void OnEffectLearned(uint32_t itemTypeID, EffectIndex i)
	{
		const bool learned = true;

		RakNet::BitStream bsOut;
		bsOut.Write(ID_LEARN_EFFECT);
		bsOut.Write(itemTypeID);
		bsOut.Write((uint32_t)i);
		bsOut.Write(learned);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE, NULL, net.remote, false);
	}

	void UpdateKnownEffects()
	{
		using KnownEffects = std::map<EffectIndex, bool>;
		using KnownEffectsMap = std::map<uint32_t, KnownEffects>;

		static KnownEffectsMap knownEffectsWas;
		KnownEffectsMap knownEffects;

		for (auto &pair : itemTypes)
		{
			auto id = pair.first;
			auto itemType = pair.second;
			for (EffectIndex i = 0; i != itemType->GetNumEffects(); ++i)
			{
				knownEffects[id][i] = itemType->IsNthEffectKnown(i);
			}
		}

		std::map<uint32_t, std::set<EffectIndex>> learned;

		for (auto &pair : knownEffects)
		{
			auto itemTypeID = pair.first;

			for (EffectIndex i = 0; i != 4; ++i)
			{
				if (pair.second[i] != knownEffectsWas[itemTypeID][i])
				{
					knownEffectsWas[itemTypeID][i] = pair.second[i];
					if(pair.second[i])
						learned[itemTypeID].insert(i);
				}
			}
		}

		for (auto &pair : learned)
		{
			auto &learnedThis = pair.second;
			auto &itemTypeID = pair.first;
			for (auto idx : learnedThis)
			{
				this->OnEffectLearned(itemTypeID, idx);
			}

		}
	}

	void UpdateWerewolfs()
	{
		std::set<uint16_t> lastWerewolfs;

		auto onWerewolfUpdate = [this](ci::IActor *actor) {
			auto remPl = dynamic_cast<ci::RemotePlayer *>(actor);
			const bool isSpawned = remPl == nullptr || remPl->IsSpawned();
			actor->SetWerewolf(true, !isSpawned);
		};

		if (lastWerewolfs != werewolfs)
		{
			for (auto id : werewolfs)
			{
				try {
					auto wwPl = players.at(id);
					onWerewolfUpdate(wwPl);
				}
				catch (...) {
				}
			}
			lastWerewolfs = werewolfs;
		}
	}

	void OnDialogResponse(uint32_t dialogID, ci::Dialog::Result result)
	{
		//ci::Chat::AddMessage(L"OnDialogResponse " + std::to_wstring(dialogID) + L' ' + result.inputText + L' ' + std::to_wstring(result.listItem));
		RakNet::BitStream bsOut;
		bsOut.Write(ID_DIALOG_RESPONSE);
		bsOut.Write((uint16_t)result.inputText.size());
		const auto inputText = WstringToString(result.inputText);
		for (auto it = inputText.begin(); it != inputText.end(); ++it)
			bsOut.Write(*it);
		bsOut.Write(result.listItem);

		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void ExecuteCommand(std::string cmd)
	{
		ci::Log("Command: " + cmd);
		enum {
			ReadFuncName,
			ReadArguments,
			Finish,
		};
		int32_t act = ReadFuncName;
		std::string funcName;
		std::vector<std::string> arguments;
		for (size_t i = 0; i != cmd.size(); ++i)
		{
			switch (act)
			{
			case ReadFuncName:
				if (cmd[i] == '(')
				{
					act = ReadArguments;
					arguments.push_back("");
				}
				else
				{
					if (!::isspace(cmd[i]))
						funcName += cmd[i];
				}
				break;
			case ReadArguments:
				if (cmd[i] == ',')
				{
					arguments.push_back("");
				}
				else if (cmd[i] == ')')
				{
					act = Finish;
				}
				else
				{
					if (!::isspace(cmd[i]))
						arguments.back() += cmd[i];
				}
				break;
			case Finish:
				break;
			}
		}

		if (funcName == "SetDisplayGold")
		{
			if (arguments.size() > 0)
			{
				localPlayer->SetDisplayGold(atoi(arguments[0].data()));
			}
		}
		else if (funcName == "Dismount")
		{
			localPlayer->ExitHorse();
		}
		else if (funcName == "Mount")
		{
			if (arguments.size() > 0)
			{
				const auto horseID = (uint16_t)atoi(arguments[0].data());
				try {
					auto horse = players.at(horseID);
					localPlayer->EnterHorse(horse);
				}
				catch (...) {
				}
			}
		}
		else if (funcName == "IncrementSkill")
		{
			if (arguments.size() > 0)
				localPlayer->IncrementSkill(arguments[0]);
		}
		else if (funcName == "AddPerk")
		{
			if (arguments.size() != 2)
				ci::Log("ERROR:ClientLogic AddPerk bad argc %d", (int32_t)arguments.size());
			else
			{
				const auto perkID = (uint32_t)atoll(arguments[0].data());
				const auto requiredSkillLevel = (uint32_t)atoll(arguments[1].data());
				ci::Perk *perk = nullptr;
				try {
					perk = perks.at(perkID);
				}
				catch (...) {
					try {
						perks[perkID] = new ci::Perk(perkID);
						perk = perks[perkID];
					}
					catch (const std::exception &e) {
						return ci::Log("ERROR:ClientLogic error while creating perk " + (std::string)e.what());
					}
					catch (...) {
						return ci::Log("ERROR:ClientLogic unknown error while creating perk");
					}
				}
				if (perk == nullptr)
					ci::Log("ERROR:ClientLogic AddPerk nullptr perk");
				else
				{
					perk->SetRequiredSkillLevel((float)requiredSkillLevel);
					localPlayer->AddPerk(perk);
				}
			}
		}
		else if (funcName == "RemovePerk")
		{
			if (arguments.size() != 1)
				ci::Log("ERROR:ClientLogic RemovePerk bad argc %d", (int32_t)arguments.size());
			else
			{
				const auto perkID = (uint32_t)atoll(arguments[0].data());
				try {
					localPlayer->RemovePerk(perks.at(perkID));
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic RemovePerk nullptr perk");
				}
			}
		}
		else if (funcName == "ShowSkympLogo")
		{
			ci::ShowSkympLogo();
		}
		else if (funcName == "HideSkympLogo")
		{
			ci::HideSkympLogo();
		}
		else if (funcName == "Activate")
		{
			if(arguments.size() != 1)
				ci::Log("ERROR:ClientLogic Activate bad argc %d", (int32_t)arguments.size());
			else
			{
				auto id = (uint32_t)atoll(arguments[0].data());
				try {
					objects.at(id)->Activate();
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic Activate failed");
				}
			}
		}
		else if (funcName == "SetControlEnabled")
		{
			if (arguments.size() != 2)
				ci::Log("ERROR:ClientLogic SetControlEnabled bad argc %d", (int32_t)arguments.size());
			else
			{
				const auto controlName = arguments[0];
				const bool enable = atoi(arguments[1].data()) != 0;
				ci::SetControlEnabled(controlName, enable);
			}
		}
	}
};

auto clientLogic = new ClientLogic;