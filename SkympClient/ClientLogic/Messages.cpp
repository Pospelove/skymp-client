#include "ClientLogic.h"

void ClientLogic::OnChatMessage(const std::wstring &text)
{
	if (!this->haveName)
	{
		this->haveName = true;
		g_config[CONFIG_NAME] = WstringToString(text);
		this->ConnectToServer(
			g_config[CONFIG_IP],
			std::atoi(g_config[CONFIG_PORT].data()),
			version,
			g_config[CONFIG_SERVER_PASSWORD],
			StringToWstring(g_config[CONFIG_NAME]));
		localPlayer->SetName(text);
		PrintStartupInfo();
		return;
	}

	if (net.peer == nullptr || net.remote == RakNet::UNASSIGNED_SYSTEM_ADDRESS)
		return;

	RakNet::BitStream bsOut;
	bsOut.Write(ID_MESSAGE);
	auto text_ = encodeRu(text);
	bsOut.Write((uint16_t)text_.size());
	for (auto it = text_.begin(); it != text_.end(); ++it)
		bsOut.Write(*it);

	net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
}

void ClientLogic::OnChatCommand(const std::wstring &cmdText, const std::vector<std::wstring> &arguments)
{
	static ci::LookData testLookData;
	static ci::Object *obj = nullptr;
	static std::vector<ci::RemotePlayer *> ps;
	static std::map<ci::RemotePlayer *, NiPoint3> offsets;
	static bool traceHorseMovement = false;

	if (cmdText == L"/q" || cmdText == L"//q")
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
	else if (cmdText == L"//mute")
	{
		static bool mute = true;
		ci::SetVolume(mute ? 0 : -1);
		mute = !mute;
	}
	else if (cmdText == L"//fov")
	{
		static int32_t fov = 0;
		const int32_t fovNow = _wtoi(arguments[0].data());
		if (fov != fovNow)
		{
			ci::ExecuteCommand(ci::CommandType::Console, "fov " + std::to_string(fovNow));
		}
	}
	else if (cmdText == L"//syncopt" || cmdText == L"//so")
	{
		ci::SetSyncOption(WstringToString(arguments[0]), WstringToString(arguments[1]));
		ci::Chat::AddMessage((L"#BEBEBE" L"Set ") + arguments[0] + L" to " + arguments[1]);
	}
	else if (cmdText == L"//stat")
	{
		static bool tr = true;
		ci::RemotePlayer::SetTracing(tr);
		ci::Object::SetTracing(tr);
		tr = !tr;
	}
	else if (cmdText == L"//eq")
	{
		for (auto p : ps)
		{
			while (true)
			{
				auto anim = p->GetNextHitAnim();
				if (anim == nullptr)
					break;
				localPlayer->PlayAnimation(*anim);
			}

			while (dbg.hitAnims.empty() == false)
			{
				auto anim = dbg.hitAnims.front();
				dbg.hitAnims.pop_front();
				p->PlayAnimation(*anim);
			}

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
			}

			{
				const ci::Spell *hands[2] = {
					localPlayer->GetEquippedSpell(0),
					localPlayer->GetEquippedSpell(1)
				};
				for (int i = 0; i <= 1; ++i)
				{
					if (hands[i] != nullptr)
					{
						p->AddSpell(hands[i], true);
						p->EquipSpell(hands[i], i);
					}
					else
						p->UnequipSpell(p->GetEquippedSpell(i), i);
				}
			}

			auto armorWas = p->GetEquippedArmor();
			for (auto item : armorWas)
			{
				p->UnequipItem(item, true, false, false);
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
				p->EquipItem(ammo, true, false, false);
			}
			else
				p->UnequipItem(p->GetEquippedAmmo(), true, false, false);

			auto m = localPlayer->GetMovementData();
			m.pos += offsets[p];
			p->ApplyMovementData(m);
			p->SetCell(localPlayer->GetCell());
			p->SetWorldSpace(localPlayer->GetWorldSpace());

			int32_t clearVisualEffect = 0;
			for (int32_t i = 1; i >= 0; --i)
				if ((int32_t)m.castStage[i] != 0)
				{
					p->MagicAttackBegin(i);
					p->SetVisualMagicEffect(p->GetEquippedSpell(i));
				}
				else
				{
					p->MagicAttackEnd(i);
					++clearVisualEffect;
				}
			if (clearVisualEffect == 2)
				p->SetVisualMagicEffect(nullptr);

		}
	}
	else if (cmdText == L"//tpd")
	{
		if (dataSearchEnabled)
		{
			ci::DataSearch::RequestTeleportDoorsManual([this](ci::DataSearch::TeleportDoorsData res) {
				if (this->tpdCallback != nullptr)
					this->tpdCallback(res);
			});
			ci::Chat::AddMessage(L"#BEBEBE" L"DataSearch TPD module is ready");
		}
		else
			ci::Chat::AddMessage(L"#BEBEBE" L"DataSearch is disabled on your client");
	}
	else if (cmdText == L"//s" || cmdText == L"//setspeedmult")
	{
		if (arguments.empty())
			return;
		if (dataSearchEnabled)
		{
			const auto str = WstringToString(arguments[0]);
			const auto sm = atoi(str.data());
			ci::LocalPlayer::GetSingleton()->SetSpeedmult((float)sm);
			ci::Chat::AddMessage(L"#BEBEBE" L"Set Speedmult to " + std::to_wstring(sm));
		}
		else
			ci::Chat::AddMessage(L"#BEBEBE" L"DataSearch is disabled on your client");
	}
	else if (cmdText == L"//players")
	{
		static auto text3d = new ci::Text3D(L"", { 0,0,0 });
		static bool enabled = false;
		enabled = !enabled;

		if (enabled)
		{
			text3d->SetTextSource([this] {
				std::wstring text;
				for (auto &pair : players)
				{
					text += pair.second->GetName();
					text += L"\n";
				}
				return text;
			});
			text3d->SetDrawDistance(8192.0);
			text3d->SetPosSource([this] {
				return localPlayer->GetPos();
			});
		}
		else
		{
			delete text3d;
			text3d = new ci::Text3D(L"", { 0,0,0 });
		}
	}
	else if (cmdText == L"//dump")
	{
		ci::Chat::AddMessage(L"#BEBEBE" L"Running Lua Codegen");
		ci::DataSearch::LuaCodegenStart([] {
			ci::Chat::AddMessage(L"#BEBEBE" L"Done");
		});
	}
	else if (cmdText == L"//testperk")
	{
		static auto Alchemist80 = new ci::Perk(0x000C07CD);
		Alchemist80->SetRequiredSkillLevel(1000);
		if (arguments.size() == 0 || arguments[0] == L"add")
		{
			ci::Chat::AddMessage(L"#BEBEBE" L"Perk added");
			localPlayer->AddPerk(Alchemist80);
		}
		else
		{
			ci::Chat::AddMessage(L"#BEBEBE" L"Perk removed");
			localPlayer->RemovePerk(Alchemist80);
		}
	}
	else if (cmdText == L"//tracehost")
	{
		tracehost = !tracehost;
		ci::Chat::AddMessage((std::wstring)L"#BEBEBE" L">> tracehost " + (tracehost ? L"On" : L"Off"));
	}
	else if (cmdText == L"//tracepackets")
	{
		tracePackets = !tracePackets;
		ci::Chat::AddMessage((std::wstring)L"#BEBEBE" L">> tracepackets " + (tracePackets ? L"On" : L"Off"));
	}
	else if (cmdText == L"//ping")
	{
		const int32_t ping = net.peer ? net.peer->GetAveragePing(net.remoteGUID) : 0;
		ci::Chat::AddMessage(L"#BEBEBE" L"Average ping is " + std::to_wstring(ping));
	}
	else if (cmdText == L"//tracehorsemd")
	{
		traceHorseMovement = !traceHorseMovement;
		ci::Chat::AddMessage((std::wstring)L"#BEBEBE" L">> tracehorsemd " + (traceHorseMovement ? L"On" : L"Off"));
	}
	else if (cmdText == L"//testhorse")
	{
		static std::vector<ci::RemotePlayer *> horses;
		horses.clear();
		for (int32_t i = 0; i != 2; ++i)
		{
			auto onActivate = [=] {
				if (localPlayer->IsOnMount() == false)
				{
					localPlayer->EnterHorse(horses[i]);
				}
				else
				{
					localPlayer->ExitHorse();
				}
			};

			auto p = new ci::RemotePlayer(
				localPlayer->GetName(),
				localPlayer->GetLookData(),
				localPlayer->GetPos(),
				localPlayer->GetCell(),
				localPlayer->GetWorldSpace(),
				{}, "",
				onActivate);
			p->SetBaseNPC(0x00109E3D);
			horses.push_back(p);
		}
		if (horses.size() == 2)
		{
			auto horseOut = horses.back();
			auto horseIn = horses.front();

			horseOut->SetEngine("RPEngineIO");
			horseIn->SetEngine("RPEngineInput");

			testUpd.Add([=] {
				auto md = horseOut->GetMovementData();
				if (traceHorseMovement)
				{
					std::wstringstream ss;
					ss << L"#BEBEBE" << L"SpeedSampled " << md.speedSampled << L" RunMode " << (int32_t)md.runMode;
					ci::Chat::AddMessage(ss.str());
				}
				md.pos += {128, 128, 0};
				horseIn->ApplyMovementData(md);
				if (ps.size() > 0)
				{
					if (ps.front()->GetMovementData().mountStage == ci::MovementData::MountStage::OnMount)
						ps.front()->SetAttachedHorse(horseIn);
					else
						ps.front()->SetAttachedHorse(nullptr);
				}
			});
		}
	}
	else if (cmdText == L"//imgui")
	{
		ci::ImGui::Init();
	}
	else if (cmdText == L"//clone")
	{
		static uint32_t counter = 0;
		const auto id = counter++;
		static std::map<uint32_t, ci::IActor *> testPlayers;

		auto onHit = [this, id](const ci::HitEventData &eventData) {
			std::wstring msg;
			if (eventData.spell != nullptr)
				msg = (L"#BEBEBE" L"Debug: Magic Hit");
			else if (eventData.weapon != nullptr)
				msg = (L"#BEBEBE" L"Debug: Weap Hit");
			else
				msg = (L"#BEBEBE" L"Debug: H2H Hit");
			ci::Chat::AddMessage(msg + L" " + StringToWstring(eventData.hitSrcMark));

			try {
				const bool h2h = eventData.spell == nullptr && eventData.weapon == nullptr;
				if (h2h)
				{
					const auto closestAc = FindClosestActor(testPlayers.at(id)->GetPos(), [=](ci::IActor *ac) { return ac != testPlayers.at(id); });
					if ((closestAc->GetPos() - testPlayers.at(id)->GetPos()).Length() > 256)
					{
						auto rem = dynamic_cast<ci::RemotePlayer *>(testPlayers.at(id));
						if (rem != nullptr)
						{
							ci::Log("ERROR:ClientLogic TEST bad h2h hit (is it invisible gnome?)");
							rem->Respawn();
						}
					}
				}
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic TEST player not found");
			}
		};

		auto p = new ci::RemotePlayer(
			localPlayer->GetName(),
			localPlayer->GetLookData(),
			localPlayer->GetPos(),
			localPlayer->GetCell(),
			localPlayer->GetWorldSpace(),
			onHit, "RPEngineInput", nullptr);
		testPlayers[id] = p;
		if (arguments.size() == 1 && arguments[0] == L"horse")
			p->SetBaseNPC(0x00109E3D);

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

		localPlayer->onPlayerBowShot.Add([=](float power) {
			p->Fire(power);
		});

		localPlayer->onPlayerMagicRelease.Add([=](int32_t handID) {
			p->FireMagic(p->GetEquippedSpell(handID), handID);
		});

		ps.push_back(p);
		offsets[p] = NiPoint3{ 128.f * ps.size(), 128.f * ps.size(), 0 };

		static bool added = false;
		if (!added)
		{
			added = true;
			testUpd.Add([=] {
				this->OnChatCommand(L"//eq", {});
				if (p->GetEngine() != "RPEngineInput")
				{
					ci::Log("ERROR:ClientLogic Bad engine (test)");
				}
			});
		}
	}
	else if (cmdText == L"//cddbg" || cmdText == L"//cdtrace")
	{
		static bool tr = true;
		ci::TraceCDCalls(tr);
		tr = !tr;
	}
	else
	{
		std::wstring str = cmdText;
		for (auto it = arguments.begin(); it != arguments.end(); ++it)
			str += L' ' + *it;
		this->OnChatMessage(str);
	}
}

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