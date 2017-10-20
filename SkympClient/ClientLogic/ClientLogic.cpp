#include "../CoreInterface/CoreInterface.h"

#include <set>
#include <map>
#include <vector>
#include <thread>
#include <sstream>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")
#include <RakPeer.h>

#include "Serialization.h"
#include "Config.h"
#include "MessageID.h"


#define MAX_NICKNAME			(24u)
#define MAX_PASSWORD			(32u)

ci::LookData ld;

class ClientLogic : public ci::IClientLogic
{
	ci::IActor *const localPlayer = ci::LocalPlayer::GetSingleton();
	std::map<uint16_t, ci::IActor *> players;
	std::map<uint32_t, ci::Object *> objects;
	std::map<uint32_t, ci::ItemType *> itemTypes;
	std::set<ci::Object *> hostedJustNow;
	bool silentInventoryChanges = false;

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
		bool isDynamic = false;
		clock_t createMoment = 0;
	};
	std::map<uint32_t, ObjectInfo> objectsInfo;
	std::map<uint32_t, NiPoint3> pos;

	std::map<uint8_t, float> currentAVsOnServer; // percentage

	uint32_t lastLocation = 0;

	bool haveName = false;
	bool allowUpdateAVs = false;

	struct
	{
		RakNet::RakPeerInterface *peer = nullptr;
		RakNet::SocketDescriptor socket;
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
				"UnarmedDamage"
			};
			if (id < NUM_AVS)
				return names[id];
			else
				return names[InvalidAV];
		}
	} av;

	void ConnectToServer(const std::string &host,
		uint16_t port,
		const std::string &hardcodedPassword,
		const std::string &password,
		const std::wstring &nickname)
	{
		net = {};
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
		for (auto it = objects.begin(); it != objects.end(); ++it)
		{
			const auto id = it->first;
			const auto object = it->second;
			if (objectsInfo[id].hostID == net.myID)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_HOSTED_OBJECT_MOVEMENT);
				bsOut.Write(id);
				bsOut.Write(object->GetPos().x);
				bsOut.Write(object->GetPos().y);
				bsOut.Write(object->GetPos().z);
				bsOut.Write(object->GetRot().x);
				bsOut.Write(object->GetRot().y);
				bsOut.Write(object->GetRot().z);
				bsOut.Write(object->IsGrabbed());
				pos[id] = object->GetPos();
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, UNRELIABLE, NULL, net.remote, false);
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
				Sleep(250);
				this->ConnectToServer(net.host, net.port, net.hardcodedPassword, net.password, net.nickname);
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
			case ID_MESSAGE:
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
			case ID_PLAYER_MOVEMENT:
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
			case ID_PLAYER_NAME:
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
			case ID_PLAYER_CREATE:
			{
				uint16_t id = ~0;
				ci::MovementData movement;
				ci::LookData look;
				uint32_t locationID = 0;

				bsIn.Read(id);
				Deserialize(bsIn, movement);
				Deserialize(bsIn, look);
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

				auto onHit = [this, id](const ci::HitEventData &eventData) {
					std::lock_guard<ci::Mutex> l(IClientLogic::callbacksMutex);
					try {
						this->OnHit(players.at(id), eventData);
					}
					catch (...) {
					}
				};

				players[id] = new ci::RemotePlayer(name, look, movement.pos, cellID, worldSpaceID, onHit);
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
			case ID_PLAYER_FURNITURE:
			{
				uint16_t playerID = ~0;
				uint32_t furnitureID = 0;

				bsIn.Read(playerID);
				bsIn.Read(furnitureID);
				try {

					switch (objectsInfo[furnitureID].type)
					{
						// ��� ���������� ����? � ��*** �� �������..
					case Type::Furniture:
					{
						static std::map<uint16_t, uint32_t> lastFurnitureUsed;
						auto pl = players.at(playerID);

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
						break;
					}

					case Type::Container:
						players.at(playerID)->UseFurniture(objects.at(furnitureID), true);
						break;

					case Type::TeleportDoor:
						players.at(playerID)->UseFurniture(objects.at(furnitureID), true);
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
				if (packet->length > (1 * sizeof(RakNet::MessageID)) + sizeof(T))
				{
					std::wstring title;
					std::wstring text;
					T characters;
					T characters2;
					bsIn.Read(characters);
					bsIn.Read(characters2);


					if (packet->length == sizeof(DialogID) + sizeof(Index) + sizeof(ci::Dialog::Style) + (1 * sizeof(RakNet::MessageID)) + 2 * sizeof(T) + sizeof(wchar_t) * characters + sizeof(wchar_t) * characters2)
					{
						for (size_t i = 0; i != characters; ++i)
						{
							wchar_t ch;
							bsIn.Read(ch);
							title += ch;
						}
						for (size_t i = 0; i != characters2; ++i)
						{
							wchar_t ch;
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

						ci::Dialog::Show(title, style, text, defaultIndex, [=](ci::Dialog::Result result) {
							std::lock_guard<ci::Mutex> l(IClientLogic::callbacksMutex);
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

				auto onActivate = [this, id](bool isOpen) {
					std::lock_guard<ci::Mutex> l(IClientLogic::callbacksMutex);
					try {
						this->OnActivate(objects.at(id), isOpen);
					}
					catch (...) {
					}
				};

				auto onContainerChanged = [this, id](const ci::ItemType *itemType, uint32_t count, bool isAdd) {
					std::lock_guard<ci::Mutex> l(IClientLogic::callbacksMutex);
					try {
						this->OnContainerChanged(objects.at(id), itemType, count, isAdd);
					}
					catch (...) {
					}
				};

				auto onHit = [this, id](const ci::HitEventData &eventData) {
					std::lock_guard<ci::Mutex> l(IClientLogic::callbacksMutex);
					try {
						this->OnHit(objects.at(id), eventData);
					}
					catch (...) {
					}
				};

				objects[id] = new ci::Object(isNative ? id : 0, baseFormID, locationID, pos, rot, onActivate, onContainerChanged, onHit);
				objects[id]->SetDestroyed(true);
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
					if (objectsInfo[id].hostID != net.myID)
					{
						if (objectsInfo[id].hostID != 65535)
						{
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
				uint32_t id = 0;
				Type type;
				bool isOpen;
				uint32_t teleportTargetID = 0;
				bool isDisabled;
				uint32_t itemsCount;
				uint32_t itemTypeID;
				uint16_t hostPlayerID;

				bsIn.Read(id);
				bsIn.Read(type);
				bsIn.Read(isOpen);
				bsIn.Read(teleportTargetID);
				bsIn.Read(isDisabled);
				bsIn.Read(itemsCount);
				bsIn.Read(itemTypeID);
				bsIn.Read(hostPlayerID);

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
								std::thread([=] {
									std::lock_guard<ci::Mutex> l(IClientLogic::callbacksMutex);
									hostedJustNow.erase(object);
								}).detach();
							});
						});
					}

					object->SetDisabled(isDisabled);

					switch (type)
					{
					case Type::Static:
						break;
					case Type::Door:
						object->SetOpen(isOpen);
						object->SetDestroyed(false);
						object->BlockActivation(true);
						break;
					case Type::TeleportDoor:
						object->SetOpen(isOpen);
						switch (object->GetBase())
						{
						case 0x31897:
						case 0x351EB:
						case 0x180D8:
							object->SetDestroyed(true);
							object->BlockActivation(true);
							break;
						default:
							object->SetDestroyed(false);
							object->BlockActivation(false);
							break;
						}
						break;
					case Type::Activator:
					case Type::Furniture:
						object->SetDestroyed(false);
						object->BlockActivation(true);
						break;
					case Type::Container:
						object->SetDestroyed(false);
						object->BlockActivation(true);
						object->SetOpen(isOpen);
						break;
					case Type::Item:
						object->SetDestroyed(false);
						object->BlockActivation(true);
						object->SetBase(itemTypes[itemTypeID]);
						object->SetCount(itemsCount);
					}
				}
				catch (...) {
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
						if (add)
						{
							players.at(playerID)->AddItem(itemTypes.at(itemTypeID), count, silentInventoryChanges);
						}
						else
							players.at(playerID)->RemoveItem(itemTypes.at(itemTypeID), count, silentInventoryChanges);
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
			case ID_PLAYER_HIT:
			{
				uint16_t playerID;
				uint8_t hitAnimID;
				bsIn.Read(playerID);
				bsIn.Read(hitAnimID);
				try {
					players.at(playerID)->PlayHitAnimation(hitAnimID);
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
						ci::SetTimer(avID == av.Health ? 200 : 0, [=] {
							std::lock_guard<ci::Mutex> l(IClientLogic::callbacksMutex);
							players.at(playerID)->UpdateAVData(av.GetAVName(avID), avData);
						});
						allowUpdateAVs = true;
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
						pl->EquipItem(itemType, this->silentInventoryChanges, false);
						armorNow.insert(itemType);
					}
					catch (...) {
						ci::Log("ERROR:ClientLogic Equipment: Armor not found");
						return;
					}
				}
				for (auto itemType : armorWas)
				{
					if (armorNow.find(itemType) == armorNow.end())
						pl->UnequipItem(itemType, this->silentInventoryChanges, false);
				}

				for (int32_t i = 0; i <= 1; ++i)
				{
					if (hands[i] != ~0)
					{
						try {
							auto itemType = itemTypes.at(hands[i]);
							pl->EquipItem(itemType, this->silentInventoryChanges, false);
						}
						catch (...) {
							ci::Log("ERROR:ClientLogic Equipment: Weapon not found");
							return;
						}
					}
					else
					{
						pl->UnequipItem(pl->GetEquippedWeapon(i), true, false, i);
					}
				}

				if (ammo != ~0)
				{
					try {
						auto itemType = itemTypes.at(ammo);
						pl->EquipItem(itemType, this->silentInventoryChanges, false);
					}
					catch (...) {
						ci::Log("ERROR:ClientLogic Equipment: Ammo not found");
						return;
					}
				}
				else
				{
					pl->UnequipItem(pl->GetEquippedAmmo(), true, false);
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
				ci::ExecuteCommand(t, str);

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

		/*if (!isOpen && objectsInfo[id].type == Type::Container)
			self->Respawn();*/
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
		bsOut.Write(isAdd);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnHit(ci::IActor *self, const ci::HitEventData &eventData)
	{
		const auto playerID = GetID(self);
		const auto weaponID = GetID(eventData.weapon);

		RakNet::BitStream bsOut;
		bsOut.Write(ID_HIT_PLAYER);
		bsOut.Write(playerID);
		bsOut.Write(weaponID);
		bsOut.Write(eventData.powerAttack);
		net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnHit(ci::Object *self, const ci::HitEventData &eventData)
	{
		const auto objectID = GetID(self);
		const auto weaponID = GetID(eventData.weapon);

		RakNet::BitStream bsOut;
		bsOut.Write(ID_HIT_OBJECT);
		bsOut.Write(objectID);
		bsOut.Write(weaponID);
		bsOut.Write(eventData.powerAttack);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void OnWorldInit() override
	{
	}

	std::function<void()> testUpd;

	void OnUpdate() override
	{
		if (testUpd)
			testUpd();

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
			try {
				cl = clNow;
				UpdateNetworking();
				UpdateObjects();
				UpdateCombat();
				UpdateActorValues();
				UpdateEquippedArmor();
				UpdateEquippedWeapon<0>();
				UpdateEquippedWeapon<1>();
				UpdateEquippedAmmo();
			}
			catch (const std::exception &e) {
				ci::Log("ERROR:ClientLogic OnUpdate() %s", e.what());
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic OnUpdate() unk");
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
		bsOut.Write(ID_MESSAGE);
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
		static ci::IActor *p = nullptr;

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
				obj = new ci::Object(0x0005815F, 0x00024E26, 0x000580A2, { 3420, 1021, 7298 }, { 0,0,0 });
			}
		}
		else if (cmdText == L"//qnnu")
		{
			ci::Chat::AddMessage(L"#bebebe>> QueueNiNodeUpdate()");
			ci::ExecuteCommand(ci::CommandType::Console, L"skymp qnnu");
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
			ci::ExecuteCommand(ci::CommandType::Console, L"skymp worldspaces");
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
		else if (cmdText == L"//eq")
		{
			if (p)
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

				auto armorWas = p->GetEquippedArmor();
				for (auto item : armorWas)
				{
					p->UnequipItem(item, true, false);
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
					p->EquipItem(ammo, true, false);
				}
				else
					p->UnequipItem(p->GetEquippedAmmo(), true, false);

				auto m = localPlayer->GetMovementData();
				m.pos = p->GetPos();
				m.angleZ = p->GetAngleZ();
				p->ApplyMovementData(m);
			
			}
		}
		else if (cmdText == L"//clone")
		{
			p = new ci::RemotePlayer(
				localPlayer->GetName(), 
				localPlayer->GetLookData(), 
				localPlayer->GetPos(), 
				localPlayer->GetCell(), 
				localPlayer->GetWorldSpace());

			p->ApplyMovementData(localPlayer->GetMovementData());

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

			testUpd = [=] {
				this->OnChatCommand(L"//eq", {});
			};
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

	void OnItemDropped(const ci::ItemType *itemType, uint32_t count) override
	{
		uint32_t itemTypeID = ~0;
		for (auto it = itemTypes.begin(); it != itemTypes.end(); ++it)
			if (it->second == itemType)
			{
				itemTypeID = it->first;
				break;
			}

		RakNet::BitStream bsOut;
		bsOut.Write(ID_DROPITEM);
		bsOut.Write(itemTypeID);
		bsOut.Write(count);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
		//ci::Chat::AddMessage(L"OnItemDropped " + std::to_wstring(count));
	}

	uint32_t GetItemTypeID(const ci::ItemType *itemType)
	{
		for (auto it = itemTypes.begin(); it != itemTypes.end(); ++it)
			if (it->second == itemType)
				return it->first;
		return ~0;
	}

	void UpdateObjects()
	{
		for (auto it = objects.begin(); it != objects.end(); ++it)
		{
			const uint32_t id = it->first;
			const auto object = it->second;
			if (objectsInfo[id].isDynamic)
			{
				OnObjectMove(object);
			}

			if (object->IsGrabbed())
			{
				static bool timerSet = false;
				if (!timerSet)
				{
					timerSet = true;
					ci::SetTimer(1333, [=] {
						try {
							std::lock_guard<ci::Mutex> l(IClientLogic::callbacksMutex);
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

			const auto hostPlayerID = objectsInfo[id].hostID;
			if ((hostedJustNow.find(object) != hostedJustNow.end() || object->IsCrosshairRef() || (localPlayer->GetPos() - object->GetPos()).Length() < 128)
				&& (hostPlayerID == 65535 || hostPlayerID == net.myID))
			{
				if (!objectsInfo[id].isDynamic)
				{
					object->SetMotionType(ci::Object::Motion_Dynamic);
					objectsInfo[id].isDynamic = true;
				}
			}

			else
			{
				if (object->GetSpeed() == 0)
				{
					if (objectsInfo[id].isDynamic)
					{
						object->SetMotionType(ci::Object::Motion_Keyframed);
						objectsInfo[id].isDynamic = false;
					}
				}
			}
		}
	}

	void UpdateCombat()
	{
		const auto hitAnimIDPtr = dynamic_cast<ci::LocalPlayer *>(localPlayer)->GetNextHitAnim();
		if (hitAnimIDPtr != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_ANIMATION_EVENT_HIT);
			bsOut.Write(*hitAnimIDPtr);
			net.peer->Send(&bsOut, LOW_PRIORITY, UNRELIABLE, NULL, net.remote, false);
		}
	}

	void UpdateActorValues()
	{
		if (allowUpdateAVs == false)
			return;
		static clock_t lastUpd = 0;
		if (lastUpd + 333 < clock())
		{
			lastUpd = clock();
			for (uint8_t avID = av.Health; avID <= av.Stamina; ++avID)
			{
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
				}
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, UNRELIABLE, NULL, net.remote, false);
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
					bsOut.Write(GetItemTypeID(itemType));
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
					bsOut.Write(GetItemTypeID(itemType));
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
				bsOut.Write(GetItemTypeID(weapWas));
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			if (weap != nullptr)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_EQUIP_ITEM);
				bsOut.Write(GetItemTypeID(weap));
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			weapWas = weap;
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
				bsOut.Write(GetItemTypeID(ammoWas));
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			if (ammo != nullptr)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_EQUIP_ITEM);
				bsOut.Write(GetItemTypeID(ammo));
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			ammoWas = ammo;
		}
	}

	void OnObjectMove(ci::Object *object)
	{
		uint32_t id = 0;
		for (auto it = objects.begin(); it != objects.end(); it++)
		{
			if (it->second == object)
			{
				id = it->first;
				break;
			}
		}
		if (!id)
			return;

		static clock_t lastPausedMoment = 0;
		if (ci::IsInPause())
		{
			lastPausedMoment = clock();
		}

		static clock_t lastHostReq = 0;
		if (objectsInfo[id].hostID == 65535)
		{
			if(lastPausedMoment + 2000 < clock())
				if ((object->GetPos() - pos[id]).Length() > 8.0f && lastHostReq + 0 <= clock())
				{
					lastHostReq = clock();
					RakNet::BitStream bsOut;
					bsOut.Write(ID_HOST_START);
					bsOut.Write(id);
					net.peer->Send(&bsOut, MEDIUM_PRIORITY, UNRELIABLE, NULL, net.remote, false);
					pos[id] = object->GetPos();
				}
		}
	}

	void OnDialogResponse(uint32_t dialogID, ci::Dialog::Result result)
	{
		//ci::Chat::AddMessage(L"OnDialogResponse " + std::to_wstring(dialogID) + L' ' + result.inputText + L' ' + std::to_wstring(result.listItem));
		RakNet::BitStream bsOut;
		bsOut.Write(ID_DIALOG_RESPONSE);
		bsOut.Write((uint16_t)result.inputText.size());
		for (auto it = result.inputText.begin(); it != result.inputText.end(); ++it)
			bsOut.Write(*it);
		bsOut.Write(result.listItem);

		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}
};

auto clientLogic = new ClientLogic;