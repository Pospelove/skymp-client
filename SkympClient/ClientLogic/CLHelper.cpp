#include "ClientLogic.h"

bool ClientLogic::IsRussianTranslate()
{
	return g_config[CONFIG_TRANSLATE] == "RU"
		|| g_config[CONFIG_TRANSLATE] == "Ru"
		|| g_config[CONFIG_TRANSLATE] == "ru";
}

bool ClientLogic::IsHorseBase(uint32_t baseNpc)
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

uint16_t ClientLogic::GetID(const ci::IActor *player)
{
	for (auto it = players.begin(); it != players.end(); ++it)
	{
		if (it->second == player)
			return it->first;
	}
	return -1;
}

uint32_t ClientLogic::GetID(const ci::Object *object)
{
	for (auto it = objects.begin(); it != objects.end(); ++it)
	{
		if (it->second == object)
			return it->first;
	}
	return -1;
}

uint32_t ClientLogic::GetID(const ci::ItemType *itemType)
{
	for (auto it = itemTypes.begin(); it != itemTypes.end(); ++it)
	{
		if (it->second == itemType)
			return it->first;
	}
	return -1;
}

uint32_t ClientLogic::GetID(const ci::Spell *spell)
{
	for (auto it = magic.begin(); it != magic.end(); ++it)
	{
		if (it->second == spell)
			return it->first;
	}
	return -1;
}

uint32_t ClientLogic::GetID(const ci::Enchantment *ench)
{
	return GetID(
		reinterpret_cast<const ci::Spell *>(ench)
	);
}

ci::IActor *ClientLogic::FindClosestActor(NiPoint3 pos, std::function<bool(ci::IActor *)> pred)
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

void ClientLogic::SendAnimation(uint32_t animID, uint16_t source)
{
	RakNet::BitStream bsOut;
	bsOut.Write(ID_ANIMATION_EVENT_HIT);
	bsOut.Write(animID);
	bsOut.Write(source);
	net.peer->Send(&bsOut, LOW_PRIORITY, unreliable, NULL, net.remote, false);
}

void ClientLogic::SendEvent(const std::string &eventName, const std::string &eventData)
{
	try {
		RakNet::BitStream bsOut;
		bsOut.Write(ID_EVENT);

		const std::string str = "{ \"eventName\":\"" + eventName + "\", \"eventDataStr\":\"" + eventData + "\" }";

		for (auto ch : str)
			bsOut.Write(ch);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE, NULL, net.remote, false);

		ci::Chat::AddMessage((StringToWstring(str.data())));
	}
	catch (const std::exception &e) {
		ci::Log("ERROR:ClientLogic SendEvent(%s, %s) %s", eventName.data(), eventData.data(), e.what());
	}
}