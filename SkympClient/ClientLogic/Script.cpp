#include "ClientLogic.h"

ci::Script::Impls ClientLogic::GetScriptImpls() const
{
	auto sendServerEvent = [this](const char *arg) {
		if (net.fullyConnected != true)
			return ci::Log("ERROR:ClientLogic unable to call sendServerEvent: no server");
		RakNet::BitStream bsOut;
		bsOut.Write(ID_EVENT);
		std::string jsonEvnNameAndData = arg;
		for (auto ch : jsonEvnNameAndData)
			bsOut.Write(ch);
		net.peer->Send(&bsOut, HIGH_PRIORITY, unreliable, NULL, net.remote, false);
	};
	return {
		sendServerEvent
	};
}

void ClientLogic::TriggerEvent(const char *eventName, const std::string &argumentsLuaTable)
{
	for (auto &script : scripts)
		script->TriggerEvent(eventName, argumentsLuaTable);
}

void ClientLogic::InitScriptHandlers()
{
	this->SetPacketHandler(ID_SCRIPT, [this](RakNet::BitStream &bsIn) {
		uint32_t size;
		std::string str;

		bsIn.Read(size);
		for (size_t i = 0; i != size; ++i)
		{
			char ch;
			bsIn.Read(ch);
			str.push_back(ch);
		}

		auto script = new ci::Script(str, this->GetScriptImpls());
		this->scripts.emplace_back(script);
	});
}