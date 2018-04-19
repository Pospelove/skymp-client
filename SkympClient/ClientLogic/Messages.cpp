#include "ClientLogic.h"

void ClientLogic::InitMessagesHandlers()
{
	this->SetPacketHandler(ID_MESSAGE, [this](RakNet::BitStream &bsIn) {
		using T = uint16_t;
		std::string message;
		T characters;
		bsIn.Read(characters);

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
	});
}