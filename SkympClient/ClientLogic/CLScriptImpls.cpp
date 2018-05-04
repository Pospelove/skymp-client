#include "ClientLogic.h"

ci::Script::Impls ClientLogic::GetScriptImpls() const
{
	auto sendServerEvent = [this](const char *jsonEvnNameAndData) {
		if (net.fullyConnected != true)
			return ci::Log("ERROR:ClientLogic unable to call sendServerEvent: no server");
		RakNet::BitStream bsOut;
		bsOut.Write(ID_EVENT);
		bsOut.Write(jsonEvnNameAndData);
		net.peer->Send(&bsOut, HIGH_PRIORITY, unreliable, NULL, net.remote, false);
	};
	return {
		sendServerEvent
	};
}