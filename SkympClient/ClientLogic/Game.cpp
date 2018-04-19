#include "ClientLogic.h"

void ClientLogic::InitGameHandlers()
{
	this->SetPacketHandler(ID_MOVE_TO, [this](RakNet::BitStream &bsIn) {
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
	});

	this->SetPacketHandler(ID_SHOW_RACE_MENU, [this](RakNet::BitStream &bsIn) {
		ci::ShowRaceMenu();
	});

	this->SetPacketHandler(ID_SHOW_DIALOG, [this](RakNet::BitStream &bsIn) {
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
					return;
				}

				lastDialogID = dialogID;
				ci::Dialog::Show(decodeRu(StringToWstring(title)), style, decodeRu(StringToWstring(text)), defaultIndex, [=](ci::Dialog::Result result) {
					this->OnDialogResponse(dialogID, result);
				});
			}
		}
	}); 
	
	this->SetPacketHandler(ID_WEATHER, [this](RakNet::BitStream &bsIn) {
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
	});

	this->SetPacketHandler(ID_GLOBAL_VARIABLE, [this](RakNet::BitStream &bsIn) {
		uint32_t globalID;
		float val;
		bsIn.Read(globalID);
		bsIn.Read(val);
		ci::SetGlobalVariable(globalID, val);
	});

	this->SetPacketHandler(ID_SILENT, [this](RakNet::BitStream &bsIn) {
		bool silent;
		bsIn.Read(silent);
		silentInventoryChanges = silent;
	});
}