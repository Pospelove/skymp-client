#include "../CoreInterface/CoreInterface.h"

#include <map>
#include <vector>
#include <thread>
#include <sstream>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")
#include <RakPeer.h>
#include <MessageIdentifiers.h>

#include "Serialization.h"
#include "Config.h"

enum : RakNet::MessageID {
	// client ----> server
	ID_HANDSHAKE = ID_USER_PACKET_ENUM,
	ID_UPDATE_MOVEMENT,
	ID_UPDATE_WORLDSPACE,
	ID_UPDATE_CELL,
	ID_UPDATE_LOOK,
	ID_LOADING_START,
	ID_LOADING_STOP,
	ID_PAUSE_START,
	ID_PAUSE_STOP,
	ID_ACTIVATE,

	// client <---> server
	ID_CHAT_MESSAGE,

	// client <---- server
	ID_WRONG_PASS,
	ID_NAME_ALREADY_USED,
	ID_NAME_INVALID,
	ID_WELCOME,
	ID_SERVER_CLOSED_THE_CONNECTION,
	ID_MOVE_TO,
	ID_OTHER_PLAYER_CREATE,
	ID_OTHER_PLAYER_DESTROY,
	ID_OTHER_PLAYER_MOVEMENT,
	ID_OTHER_PLAYER_NAME,
	ID_OTHER_PLAYER_PAUSE,
	ID_OTHER_PLAYER_LOOK,
	ID_SHOW_RACE_MENU,
	ID_OBJECT_CREATE,
	ID_OBJECT_DESTROY,
	ID_OBJECT_POS_ROT_LOCATION,
	ID_OBJECT_NAME,
	ID_OBJECT_BEHAVIOR,
};


#define MAX_NICKNAME			(24u)
#define MAX_PASSWORD			(32u)

ci::LookData ld;

class ClientLogic : public ci::IClientLogic
{
	ci::IActor *const localPlayer = ci::LocalPlayer::GetSingleton();
	//std::map<uint16_t, std::shared_ptr<ci::RemotePlayer>> remotePlayers;
	std::map<uint16_t, ci::IActor *> players;
	std::map<uint32_t, ci::Object *> objects;

	struct ObjectInfo
	{
		uint8_t type = 0;
	};
	std::map<uint32_t, ObjectInfo> objectsInfo;

	uint32_t lastLocation = 0;

	bool haveName = false;

	struct
	{
		RakNet::RakPeerInterface *peer = nullptr;
		RakNet::SocketDescriptor socket;
		std::string password, host, hardcodedPassword;
		std::wstring nickname;
		uint16_t port;
		std::wstring connectingMsg;

		RakNet::SystemAddress remote = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
		RakNet::RakNetGUID remoteGUID = RakNet::UNASSIGNED_RAKNET_GUID;

		bool fullyConnected = false;
		uint16_t myID = ~0;

	} net;

	void ConnectToServer(const std::string &host,
		uint16_t port,
		const std::string &hardcodedPassword,
		const std::string &password,
		const std::wstring &nickname)
	{
		net.peer = RakNet::RakPeerInterface::GetInstance();
		net.peer->Startup(1, &net.socket, 1);
		net.password = password;
		net.nickname = nickname;
		net.host = host;
		net.hardcodedPassword = hardcodedPassword;
		net.port = port;
		net.peer->Connect(net.host.data(), net.port, net.hardcodedPassword.data(), net.hardcodedPassword.size());

		net.connectingMsg = L"Connecting to " + StringToWstring(host) + L":" + std::to_wstring(port) + L"...";
		ci::Chat::AddMessage(net.connectingMsg);
	}

	void UpdateNetworking()
	{
		if (net.peer == nullptr)
			return;

		// SEND:
		auto movementData = localPlayer->GetMovementData();
		static clock_t cl = 0;
		auto clNow = clock();
		const int32_t delay = 
			movementData.runMode == ci::MovementData::RunMode::Standing ? 50 : 40;
		if (cl + delay < clNow)
		{
			cl = clNow;
			if (net.remote != RakNet::UNASSIGNED_SYSTEM_ADDRESS && net.fullyConnected
				&& !movementData.isEmpty()
				&& !ci::IsInPause())
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_UPDATE_MOVEMENT);
				Serialize(bsOut, movementData);
				net.peer->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE, NULL, net.remote, false);
			}
		}

		// RECEIVE:
		RakNet::Packet *packet;
		for (packet = net.peer->Receive(); packet; net.peer->DeallocatePacket(packet), packet = net.peer->Receive())
		{
			if (net.remote == RakNet::UNASSIGNED_SYSTEM_ADDRESS)
				net.remote = packet->systemAddress;
			if (net.remoteGUID == RakNet::UNASSIGNED_RAKNET_GUID)
				net.remoteGUID = packet->guid;

			RakNet::BitStream bsOut;
			RakNet::BitStream bsIn(&packet->data[1], packet->length, false);

			switch (packet->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				bsOut.Write(ID_HANDSHAKE);

				wchar_t nicknameCStr[MAX_NICKNAME];
				if (net.nickname.size() > MAX_NICKNAME - 1)
					net.nickname.resize(MAX_NICKNAME - 1);
				std::memcpy(nicknameCStr, net.nickname.data(), sizeof(wchar_t) * net.nickname.size());
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

				ci::Chat::AddMessage(L"Connection request sent. Waiting for verification...");

				net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, NULL, packet->systemAddress, false);
				break;
			case ID_ALREADY_CONNECTED:
				ci::Chat::AddMessage(L"Already connected");
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

			case ID_WRONG_PASS:
				ci::Chat::AddMessage(L"Wrong password (" + StringToWstring(net.password) + L")");
				break;
			case ID_NAME_INVALID:
				ci::Chat::AddMessage(L"Name can only contain A-Z, a-z, 0-9 and _");
				break;
			case ID_NAME_ALREADY_USED:
				ci::Chat::AddMessage(L"Player " + net.nickname + L" is already connected to the server");
				break;
			case ID_WELCOME:
				ci::Chat::AddMessage(L"Connected.");
				bsIn.Read((uint16_t &)net.myID);
				players[net.myID] = ci::LocalPlayer::GetSingleton();
				net.fullyConnected = true;
				break;
			case ID_CHAT_MESSAGE:
			{
				using T = uint16_t;
				if (packet->length > (2 * sizeof(RakNet::MessageID)) + sizeof(T))
				{
					std::wstring message;
					T characters;
					bsIn.Read(characters);

					if (packet->length == (2 * sizeof(RakNet::MessageID)) + sizeof(T) + sizeof(wchar_t) * characters)
					{
						for (size_t i = 0; i != characters; ++i)
						{
							wchar_t ch;
							bsIn.Read(ch);
							message += ch;
						}
						bool isNotification;
						bsIn.Read((uint8_t &)isNotification);
						ci::Chat::AddMessage(message, isNotification);
					}
				}
				break;
			}
			case ID_MOVE_TO:
			{
				//remotePlayers.clear();


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

				lastLocation = in.cellOrWorldspace;

				ci::SetTimer(100, [=] {
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
			case ID_OTHER_PLAYER_MOVEMENT:
			{
				uint16_t playerid = ~0;
				bsIn.Read(playerid);
				ci::MovementData movData;
				Deserialize(bsIn, movData);
				uint32_t locationID;
				bsIn.Read(locationID);
				try {
					auto &player = this->players.at(playerid);
					if (player->GetName() == localPlayer->GetName())
						movData.pos += NiPoint3{ 128, 128, 0 };
					player->ApplyMovementData(movData);
					player->SetCell(localPlayer->GetCell());
					player->SetWorldSpace(localPlayer->GetWorldSpace());
				}
				catch (...) {
				}
				break;
			}
			case ID_OTHER_PLAYER_NAME:
			{
				using T = uint16_t;
				if (packet->length > (3 + sizeof(T)))
				{
					std::wstring name;
					T characters;
					uint16_t id = ~0;
					bsIn.Read(id);
					bsIn.Read(characters);

					if (packet->length == (3 + sizeof(T) + sizeof(wchar_t) * characters))
					{
						for (size_t i = 0; i != characters; ++i)
						{
							wchar_t ch;
							bsIn.Read(ch);
							name += ch;
						}
						try {
							players.at(id)->SetName(name);
						}
						catch (...) {
						}
					}
				}

				break;
			}
			case ID_OTHER_PLAYER_CREATE:
			{
				uint16_t id = ~0;
				ci::MovementData movement;
				uint32_t locationID = 0;
				ci::LookData lookData = localPlayer->GetLookData();

				bsIn.Read(id);
				Deserialize(bsIn, movement);
				bsIn.Read(locationID);

				uint16_t characters;
				bsIn.Read(characters);

				std::wstring name;
				for (size_t i = 0; i != characters; ++i)
				{
					wchar_t ch;
					bsIn.Read(ch);
					name += ch;
				}

				uint32_t cellID = localPlayer->GetCell();
				uint32_t worldSpaceID = localPlayer->GetWorldSpace();

				players[id] = new ci::RemotePlayer(name, lookData, movement.pos, cellID, worldSpaceID);
				break;
			}
			case ID_OTHER_PLAYER_DESTROY:
			{
				uint16_t id = ~0;
				bsIn.Read(id);
				auto it = players.find(id);
				if (it != players.end())
				{
					delete it->second;
					players.erase(it);
				}
				break;
			}
			case ID_OTHER_PLAYER_PAUSE:
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
			case ID_OTHER_PLAYER_LOOK:
			{
				uint16_t id = ~0;
				ci::LookData look;

				bsIn.Read(id);
				Deserialize(bsIn, look);
				ci::SetTimer(1000, [=] {
					try {
						if (!look.isEmpty())
							players.at(id)->ApplyLookData(look);
					}
					catch (...) {
					}
				});
				break;
			}
			case ID_SHOW_RACE_MENU:
				ci::ShowRaceMenu();
				break;
			case ID_OBJECT_CREATE:
			{
				//ci::Chat::AddMessage(L"ID_OBJECT_CREATE");
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

				auto onActivate = [&](ci::Object *self, bool isOpen) {
					std::lock_guard<std::mutex> l(IClientLogic::callbacksMutex);
					this->OnActivate(self, isOpen); 
				};
				objects[id] = new ci::Object(isNative ? id : 0, baseFormID, locationID, pos, rot, onActivate);
				objectsInfo[id] = {};
				break;
			}
			case ID_OBJECT_DESTROY:
			{
				//ci::Chat::AddMessage(L"ID_OBJECT_DESTROY");
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
				//ci::Chat::AddMessage(L"ID_OBJECT_POS_ROT_LOCATION");
				uint32_t id = 0;
				NiPoint3 pos, rot;
				uint32_t locationID;
				bsIn.Read(id);
				bsIn.Read(pos.x);
				bsIn.Read(pos.y);
				bsIn.Read(pos.z);
				bsIn.Read(rot.x);
				bsIn.Read(rot.y);
				bsIn.Read(rot.z);
				bsIn.Read(locationID);
				try {
					auto obj = objects.at(id);
					obj->SetPosition(pos);
					obj->SetAngle(rot);
					obj->SetLocation(locationID);
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

				std::wstring name;
				for (size_t i = 0; i != characters; ++i)
				{
					wchar_t ch;
					bsIn.Read(ch);
					name += ch;
				}

				try {
					objects.at(id)->SetName(name);
				}
				catch (...) {
				}
				break;
			}
			case ID_OBJECT_BEHAVIOR:
			{
				enum class Type : uint8_t
				{
					Static = 0x00,
					Door = 0x01,
					TeleportDoor = 0x02,
				};

				uint32_t id = 0;
				Type type;
				bool isOpen;
				uint32_t teleportTargetID = 0;

				bsIn.Read(id);
				bsIn.Read(type);
				bsIn.Read(isOpen);
				bsIn.Read(teleportTargetID);

				try {
					auto object = objects.at(id);

					objectsInfo[id].type = static_cast<uint8_t>(type);

					switch (type)
					{
					case Type::Door:
						object->SetOpen(isOpen);
						object->BlockActivation(false);
						break;
					case Type::TeleportDoor:
						object->SetOpen(isOpen);
						object->BlockActivation(false);
						break;
					}
				}
				catch (...) {
				}

				break;
			}
			default:
				ci::Chat::AddMessage(L"Unknown packet type " + std::to_wstring(packet->data[0]));
				break;
				
			}
		}
	}

	void OnStartup() override
	{
		if (ci::IsInDebug())
		{
			ci::Chat::AddMessage(L"#eeeeeeSkyMP Debug");
			return;
		}

		ci::Chat::Init();
		ci::Chat::AddMessage(L"#eeeeeeSkyMP Client Started");
		ci::LocalPlayer::GetSingleton()->SetName(L"Player");
		
		ci::SetUpdateRate(1);

		if (read_cfg() == 0)
		{
			ci::Chat::AddMessage(L"#eeeeee" + StringToWstring(cfgFile) + L" not found");
			return;
		}

		auto name = StringToWstring(g_config[CONFIG_NAME]);

		if (name != L"")
		{
			this->haveName = true;
			this->ConnectToServer(
				g_config[CONFIG_IP],
				std::atoi(g_config[CONFIG_PORT].data()),
				"A1",
				g_config[CONFIG_SERVER_PASSWORD],
				name);
			localPlayer->SetName(name);
		}
		else
		{
			ci::Chat::AddMessage(L"Press T and type your name");
		}
	}

	void OnActivate(ci::Object *self, bool isOpen)
	{
		RakNet::BitStream bsOut;
		bsOut.Write(ID_ACTIVATE);
		uint32_t id = 0;
		for (auto it = objects.begin(); it != objects.end(); ++it)
		{
			if (it->second == self)
			{
				id = it->first;
				break;
			}
		}
		bsOut.Write(id);
		bsOut.Write(isOpen);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnWorldInit() override
	{
	}

	void OnUpdate() override
	{
		if (ci::IsInDebug())
			return;
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
			try
			{
				cl = clNow;
				UpdateNetworking();
			}
			catch (...)
			{
			}
		}
	}

	void OnChatMessage(const std::wstring &text) override
	{
		if (ci::IsInDebug())
			return;
		if (!this->haveName)
		{
			this->haveName = true;
			this->ConnectToServer(
				g_config[CONFIG_IP],
				std::atoi(g_config[CONFIG_PORT].data()),
				"A1",
				g_config[CONFIG_SERVER_PASSWORD],
				text);
			localPlayer->SetName(text);
			return;
		}

		if (net.peer == nullptr || net.remote == RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			return;

		RakNet::BitStream bsOut;
		bsOut.Write(ID_CHAT_MESSAGE);
		bsOut.Write((uint16_t)text.size());
		for (auto it = text.begin(); it != text.end(); ++it)
			bsOut.Write(*it);

		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnChatCommand(const std::wstring &cmdText, const std::vector<std::wstring> &arguments) override
	{
		if (ci::IsInDebug())
			return;
		static ci::LookData testLookData;
		static ci::Object *obj = nullptr;

		if (cmdText == L"/q")
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
		else if (cmdText == L"//obj")
		{
			if (obj)
			{
				delete obj;
				obj = nullptr;
			}
			else
			{
				obj = new ci::Object(0x0005815F, 0x00024E26, 0x000580A2, { 3420, 1021, 7298 }, { 0,0,0 }, nullptr);
			}
		}
		else if (cmdText == L"//qnnu")
		{
			ci::Chat::AddMessage(L"#bebebe>> QueueNiNodeUpdate()");
			ci::ExecuteConsoleCommand(L"skymp qnnu");
		}
		else if (cmdText == L"//ld")
		{
			ci::Chat::AddMessage(L"#bebebe>> ApplyLookData()");
			localPlayer->ApplyLookData(ld);
		}
		else if (cmdText == L"//save")
		{
			static const auto file = "Debug_SavedWorldPoints.txt";
			const auto movData = localPlayer->GetMovementData();
			std::ofstream ofs(file, std::ios::app);

			ofs << "\nSkyMP Debug." << std::endl;
			
			static size_t i = 0;
			ofs << ++i << ") ";
			ofs << "X=" << movData.pos.x << ' ';
			ofs << "Y=" << movData.pos.y << ' ';
			ofs << "Z=" << movData.pos.z << ' ';
			ofs << "Angle=" << movData.angleZ << ' ';
			ofs << "Cell=" << std::hex << localPlayer->GetCell() << ' ';
			ofs << "WorldSpace=" << std::hex << localPlayer->GetWorldSpace() << ' ';
			ofs << std::endl;

			ci::Chat::AddMessage(L"#bebebe>> World point saved to " + StringToWstring(file));
		}
		else if (cmdText == L"//worldspaces")
		{
			ci::ExecuteConsoleCommand(L"skymp worldspaces");
		}
		else if (cmdText == L"//testld_save")
		{
			testLookData = localPlayer->GetLookData();
			ci::Chat::AddMessage(L"testld_save");
		}
		else if (cmdText == L"//testld_spawn")
		{
			new ci::RemotePlayer(L"TestPlayer", testLookData, localPlayer->GetPos(), localPlayer->GetCell(), localPlayer->GetWorldSpace());
			ci::Chat::AddMessage(L"testld_spawn");
			ci::Chat::AddMessage(L"[Test] TintMasks count = " + std::to_wstring(testLookData.tintmasks.size()));
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
		if (ci::IsInDebug())
		{

		}
		else
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
				localPlayer->ApplyLookData(newLook);

			});
		}
	}
};

auto clientLogic = new ClientLogic;