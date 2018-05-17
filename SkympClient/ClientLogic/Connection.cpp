#include "ClientLogic.h"

void ClientLogic::InitConnectionHandlers()
{
	this->SetPacketHandlerEx(ID_CONNECTION_REQUEST_ACCEPTED, [this](RakNet::BitStream &bsIn, RakNet::Packet *packet) {
		RakNet::BitStream bsOut;
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
	});

	this->SetPacketHandler(ID_ALREADY_CONNECTED, [this](RakNet::BitStream &bsIn) {
		ci::Chat::AddMessage(L"Already connected");
		Sleep(50);
		this->ConnectToServer(g_config[CONFIG_IP], std::atoi(g_config[CONFIG_PORT].data()), version, g_config[CONFIG_SERVER_PASSWORD], StringToWstring(g_config[CONFIG_NAME]));
	});

	this->SetPacketHandler(ID_DISCONNECTION_NOTIFICATION, [this](RakNet::BitStream &bsIn) {
		if (net.fullyConnected)
			ci::Chat::AddMessage(L"Server closed the connection");
	});

	this->SetPacketHandler(ID_CONNECTION_LOST, [this](RakNet::BitStream &bsIn) {
		ci::Chat::AddMessage(L"Connection lost");
	});

	this->SetPacketHandler(ID_CONNECTION_BANNED, [this](RakNet::BitStream &bsIn) {
		ci::Chat::AddMessage(L"You are banned from the server");
	});

	this->SetPacketHandler(ID_SERVER_CLOSED_THE_CONNECTION, [this](RakNet::BitStream &bsIn) {
	});

	this->SetPacketHandler(ID_NO_FREE_INCOMING_CONNECTIONS, [this](RakNet::BitStream &bsIn) {
		ci::Chat::AddMessage(L"The server is full. Retrying ...");
		ci::Chat::AddMessage(net.connectingMsg);
		net.peer->Connect(net.host.data(), net.port, net.hardcodedPassword.data(), net.hardcodedPassword.size());
	});

	this->SetPacketHandler(ID_CONNECTION_ATTEMPT_FAILED, [this](RakNet::BitStream &bsIn) {
		ci::Chat::AddMessage(L"Connection failed. Retrying ...");
		ci::Chat::AddMessage(net.connectingMsg);
		net.peer->Connect(net.host.data(), net.port, net.hardcodedPassword.data(), net.hardcodedPassword.size());
	});

	this->SetPacketHandler(ID_INVALID_PASSWORD, [this](RakNet::BitStream &bsIn) {
		ci::Log(L"FATAL:ClientLogic bad version");
		std::exit(0);
	});

	this->SetPacketHandler(ID_WRONG_PASS, [this](RakNet::BitStream &bsIn) {
	});

	this->SetPacketHandler(ID_NAME_INVALID, [this](RakNet::BitStream &bsIn) {
	});

	this->SetPacketHandler(ID_NAME_ALREADY_USED, [this](RakNet::BitStream &bsIn) {
	});

	this->SetPacketHandler(ID_WELCOME, [this](RakNet::BitStream &bsIn) {
		//ci::Chat::AddMessage(L"Successful handshake.");
		Sleep(1);
		//ci::Chat::AddMessage(L"Connected.");
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
				//ci::Chat::AddMessage(L"Loading a plugin (" + std::to_wstring(pluginBinray.size()) + L" bytes)");
				ci::HotLoadPlugin(pluginBinray);
			}
		}
	});
}