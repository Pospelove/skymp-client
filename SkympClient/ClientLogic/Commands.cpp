#include "ClientLogic.h"

void ClientLogic::InitCommandsHandlers()
{
	this->SetPacketHandler(ID_COMMAND, [this](RakNet::BitStream &bsIn) {
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
	});
}

void ClientLogic::ExecuteCommand(std::string cmd)
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
		if (arguments.size() != 1)
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