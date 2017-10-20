#include "stdafx.h"
#include <ClientLogic\Config.h>

namespace ci
{
	namespace Chat
	{
		void AddMessage(const std::wstring &str, bool)
		{
			wprintf(L"[Chat] %s\n", str.data());
		}
	}
}

struct ConnectionData
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
};
ConnectionData net;

enum {
	MAX_NICKNAME = (24u),
	MAX_PASSWORD = (32u),
};

struct
{
	ci::MovementData movement;
} my;

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
	wprintf(L"%s\n", net.connectingMsg.data());
}

void PrepareConfig()
{
	// IP:
	if (g_config[CONFIG_IP] == "")
	{
		printf("server ip: ");
		std::cin >> g_config[CONFIG_IP];
	}

	// Port:
	const auto delPos = g_config[CONFIG_IP].find(':');
	if (delPos != std::string::npos)
	{
		auto cfgIp = g_config[CONFIG_IP];
		g_config[CONFIG_PORT] = std::string(cfgIp.begin() + delPos + 1, cfgIp.end());
		g_config[CONFIG_IP] = std::string(cfgIp.begin(), cfgIp.begin() + delPos);
	}
	if (g_config[CONFIG_PORT] == "")
	{
		printf("server port: ");
		std::cin >> g_config[CONFIG_PORT];
	}

	// Pass:
	if (g_config[CONFIG_SERVER_PASSWORD] == "")
	{
		printf("server needs password? (y/n): ");
		char ch;
		std::cin >> ch;
		if (ch == 'y')
		{
			printf("server password: ");
			std::cin >> g_config[CONFIG_SERVER_PASSWORD];
		}
	}

	// Name:
	if (g_config[CONFIG_NAME] == "")
	{
		printf("your name: ");
		std::cin >> g_config[CONFIG_NAME];
	}
}

void Update()
{
	if (net.peer == nullptr)
		return;
}

int main()
{
	PrepareConfig();

	auto hardcodedPassword = "A1";
	ConnectToServer(
		g_config[CONFIG_IP],
		std::atoi(g_config[CONFIG_PORT].data()),
		hardcodedPassword,
		g_config[CONFIG_SERVER_PASSWORD],
		StringToWstring(g_config[CONFIG_NAME]));

	while (1)
	{
		Update();
		Sleep(1);
	}
}