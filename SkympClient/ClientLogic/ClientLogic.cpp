#include "ClientLogic.h"

auto clientLogic = new ClientLogic;

std::vector<std::shared_ptr<ci::Script>> scripts;

ClientLogic::ClientLogic()
{
	this->InitItemTypesHandlers();
	this->InitObjectsHandlers();
	this->InitPlayersHandlers();
	this->InitConnectionHandlers();
	this->InitGameHandlers();
	this->InitDataSearchHandlers();
	this->Init3DTextsHandlers();
	this->InitMessagesHandlers();
	this->InitCommandsHandlers();
	this->InitRecipesHandlers();
	this->InitMagicHandlers();
}

void ClientLogic::PrintStartupInfo()
{
	ci::Log(L"-- LocalPlayer Name is " + localPlayer->GetName());
	ci::Log(L"-- Current Date is " + ci::GetDateString());
}

void ClientLogic::ConnectToServer(const std::string &host,
	uint16_t port,
	const std::string &hardcodedPassword,
	const std::string &password,
	const std::wstring &nickname)
{
	net = {};
	net.peer = RakNet::RakPeerInterface::GetInstance();
	net.peer->Startup(22, net.socket, 10);
	net.password = password;
	net.nickname = nickname;
	net.host = host;
	net.hardcodedPassword = hardcodedPassword;
	net.port = port;
	net.peer->Connect(net.host.data(), net.port, net.hardcodedPassword.data(), net.hardcodedPassword.size());

	net.connectingMsg = L"Connecting to " + StringToWstring(host) + L":" + std::to_wstring(port) + L"...";
	ci::Chat::AddMessage(net.connectingMsg);
}

void ClientLogic::ProcessPacket(RakNet::Packet *packet)
{
	RakNet::BitStream bsIn(&packet->data[1], packet->length, false);
	const auto messageID = (packet->data[0]);
	try {
		//ci::Chat::AddMessage(StringToWstring(GetPacketName(messageID)), false);
		this->packetHandlers.at(messageID)(bsIn, packet);
	}
	catch (...) {
		ci::Log(L"Unknown packet type " + std::to_wstring(messageID));
	}
}

void ClientLogic::UpdateNetworking()
{
	if (net.peer == nullptr)
		return;

	// SEND:
	try {
		auto movementData = localPlayer->GetMovementData();
		static clock_t cl = 0;
		auto clNow = clock();
		float delay = 40 + (float)players.size();
		if (delay > 60)
			delay = 60;
		if (players.empty() || (players.size() == 1 && players.begin()->second == localPlayer))
			delay = 200;
		if (cl + delay < clNow)
		{
			cl = clNow;
			if (net.remote != RakNet::UNASSIGNED_SYSTEM_ADDRESS && net.fullyConnected
				&& !movementData.isEmpty()
				&& !ci::IsInPause())
			{
				auto sendMovement = [this](uint16_t movementOwner, const ci::MovementData &movement) {
					RakNet::BitStream bsOut;
					bsOut.Write(ID_UPDATE_MOVEMENT);
					Serialize(bsOut, movement);
					bsOut.Write(movementOwner);
					if (movementOwner == net.myID)
						bsOut.Write(localPlayer->GetLocation());
					else
						bsOut.Write(uint32_t(-1));
					bsOut.Write((uint32_t)unreliable);
					net.peer->Send(&bsOut, HIGH_PRIORITY, unreliable, NULL, net.remote, false);
				};

				sendMovement(net.myID, movementData);

				for (auto hosted : hostedPlayers)
				{
					try {
						const auto hostedPl = players.at(hosted);
						const auto md = hostedPl->GetMovementData();
						sendMovement(hosted, md);

						auto asRemote = dynamic_cast<ci::RemotePlayer *>(hostedPl);
						if (asRemote != nullptr)
						{
							const auto hitAnimIDPtr = asRemote->GetNextHitAnim();
							if (hitAnimIDPtr != nullptr)
								this->SendAnimation(*hitAnimIDPtr, hosted);
						}

					}
					catch (...) {
					}
				}
			}
		}
	}
	catch (...) {
		ci::Log("WARN:ClientLogic UpdateNetworking() Send"); // TODO: Refactor
	}

	// RECEIVE:
	for (auto packet = net.peer->Receive(); packet; net.peer->DeallocatePacket(packet), packet = net.peer->Receive())
	{
		const auto packetID = (int32_t)packet->data[0];
		if (net.remote == RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			net.remote = packet->systemAddress;
		if (net.remoteGUID == RakNet::UNASSIGNED_RAKNET_GUID)
			net.remoteGUID = packet->guid;
		try {
			this->ProcessPacket(packet);

			if (tracePackets)
			{
				std::wstringstream ss;
				ss << StringToWstring(GetPacketName(packetID));
				ci::Chat::AddMessage(ss.str());
			}
		}
		catch (...) {
			ci::Log("WARN:ClientLogic UpdateNetworking() Receive " + std::to_string(packetID)); // TODO: Refactor
		}
	}
}

void ClientLogic::OnStartup()
{
	ci::SetVolume(-1);

	for (int32_t i = 0; i != 1024; ++i)
		this->lastUpdateByServer[i] = NULL;

	const int32_t cfgNumLines = read_cfg();
	for (auto badSymbol : { '\n', '\r' })
		for (auto &pair : g_config)
		{
			auto &v = pair.second;
			while (v.size() > 0 && v.back() == badSymbol)
				v.pop_back();
			while (v.size() > 0 && v.front() == badSymbol)
				v = { v.begin() + 1, v.end() };
		}

	ci::ImGui::Init();

	ci::Chat::Init(
		atoi(g_config[CONFIG_CHAT_OFFSET_X].data()),
		atoi(g_config[CONFIG_CHAT_OFFSET_Y].data())
	);
	ci::Chat::AddMessage(L"#eeeeeeSkyMP " + StringToWstring(version) + L" Client Started");
	ci::LocalPlayer::GetSingleton()->SetName(L"Player");

	ci::SetUpdateRate(1);

	std::shared_ptr<ci::Script> script{ new ci::Script("skymp.lua") };
	if (script->IsValid())
	{
		ci::Log(L"DBG:ClientLogic skymp.lua loaded");
	}
	else
	{
		std::wstringstream ss;
		ss << L"ERROR:ClientLogic skymp.lua failed with error: " << StringToWstring(script->GetLastError());
		ci::Log(ss.str());
	}
	scripts.push_back(script);

	if (cfgNumLines == 0)
	{
		ci::Chat::AddMessage(L"#eeeeee" + StringToWstring(cfgFile) + L" not found");
		return;
	}

	if (IsRussianTranslate())
		ci::Chat::SetRussianUser();

	const auto drawDistance = g_config[CONFIG_DRAW_DISTANCE];
	if (drawDistance.empty() == false && atoi(drawDistance.data()) != 0)
		ci::SetSyncOption("DRAW_DISTANCE", drawDistance);

	const auto fov = g_config[CONFIG_FOV];
	if (fov.empty() == false && atoi(fov.data()) != 0)
		ci::ExecuteCommand(ci::CommandType::Console, "fov " + fov);

	auto name = StringToWstring(g_config[CONFIG_NAME]);

	if (g_config[CONFIG_TCP_ONLY] == "true")
	{
		ci::Chat::AddMessage(L"Switch reliability...");
		Sleep(500);
		unreliable = PacketReliability::RELIABLE;
		ci::Chat::AddMessage(L"Done");
	}

	if (name != L"")
	{
		this->haveName = true;
		this->ConnectToServer(
			g_config[CONFIG_IP],
			std::atoi(g_config[CONFIG_PORT].data()),
			version,
			g_config[CONFIG_SERVER_PASSWORD],
			name);
		localPlayer->SetName(name);
		PrintStartupInfo();
	}
	else
	{
		if (IsRussianTranslate())
			ci::Chat::AddMessage(L"Нажмите T и введите Ваш никнейм");
		else
			ci::Chat::AddMessage(L"Press T and type your name");
	}
}

void ClientLogic::OnWorldInit()
{
	static bool firstInit = true;

	if (firstInit)
	{
		Tests::Run();

		localPlayer->onPerkSelect.Add([=](ci::LocalPlayer::_PerkID perkID) {
			RakNet::BitStream bsOut;
			bsOut.Write(ID_LEARN_PERK);
			bsOut.Write((uint32_t)perkID);
			net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
		});

		localPlayer->onLevelUp.Add([=](std::string increasedAv) {
			RakNet::BitStream bsOut;
			bsOut.Write(ID_LEVEL_UP);
			if (increasedAv == "Health")
				bsOut.Write((uint8_t)av.Health);
			else if (increasedAv == "Magicka")
				bsOut.Write((uint8_t)av.Magicka);
			else if (increasedAv == "Stamina")
				bsOut.Write((uint8_t)av.Stamina);
			net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
		});

		localPlayer->onPlayerBowShot.Add([=](float power) {
			const auto powerInt = int32_t(power * 100);
			RakNet::BitStream bsOut;
			bsOut.Write(ID_BOW_SHOT);
			bsOut.Write(powerInt);
			net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
		});

		localPlayer->onPlayerMagicRelease.Add([=](int32_t handID) {
			RakNet::BitStream bsOut;
			bsOut.Write(ID_SPELL_RELEASE);
			bsOut.Write(handID);
			net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
		});

		localPlayer->onHit.Add([=](ci::HitEventData evn) {
			try {
				const auto source = players.at((uint16_t)atoi(evn.hitSrcMark.data()));
				this->OnHit(localPlayer, source, evn);
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic LocalPlayer OnHit()");
			}
		});
	}

	firstInit = false;
}

void ClientLogic::OnUpdate()
{
	{
		const auto now = ci::IsKeyPressed(VK_ESCAPE);
		static bool was = false;
		if (was != now)
		{
			was = now;
			if (now)
				if (ci::Dialog::Hide())
				{
					this->OnDialogResponse(lastDialogID, { L"", -1 });
				}
		}
	}

	{
		const auto now = ci::IsKeyPressed(VK_F5);
		static bool was = false;
		if (was != now)
		{
			was = now;
			if (now)
			{
				for (auto &pair : players)
				{
					auto pl = dynamic_cast<ci::RemotePlayer *>(pair.second);
					if (pl && pl->GetEngine() != "RPEngineIO")
						pl->Respawn();
				}
			}
		}
	}

	// Freeze activators (fake items)
	static clock_t lastCheck = 0;
	if (clock() - lastCheck > 100)
	{
		lastCheck = clock();
		for (auto &p : objects)
		{
			//if (p.second->IsCrosshairRef())
			{
				try {
					if (objectsInfo.at(p.first).type != ClientLogic::Type::Item)
					{
						p.second->SetMotionType(ci::Object::Motion_Keyframed);
					}
				}
				catch (...) {
				}
				break;
			}
		}
	}

	if (testUpd)
		testUpd();

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
			try {
				UpdateNetworking();
			}
			catch (...) {
				ci::Log("WARN:ClientLogic UpdateNetworking()"); // TODO: Refactor
			}
			try {
				UpdateObjects();
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic UpdateObjects()");
			}
			try {
				UpdateCombat();
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic UpdateCombat()");
			}
			try {
				UpdateActorValues();
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic UpdateActorValues()");
			}
			try {
				UpdateEquippedArmor();
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic UpdateEquippedArmor()");
			}
			try {
				UpdateEquippedWeapon<0>();
				UpdateEquippedWeapon<1>();
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic UpdateEquippedWeapon<>()");
			}
			try {
				UpdateEquippedSpell<0>();
				UpdateEquippedSpell<1>();
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic UpdateEquippedSpell<>()");
			}
			try {
				UpdateEquippedAmmo();
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic UpdateEquippedAmmo()");
			}
			try {
				UpdateKnownEffects();
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic UpdateKnownEffects()");
			}
			try {
				UpdateWerewolfs();
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic UpdateWerewolfs()");
			}
		}
		catch (const std::exception &e) {
			ci::Log("ERROR:ClientLogic OnUpdate() %s", e.what());
		}
		catch (...) {
			ci::Log("ERROR:ClientLogic OnUpdate()");
		}
	}

	for (auto &script : scripts)
		script->TriggerEvent("update");
}

void ClientLogic::UpdateCombat()
{
	const auto hitAnimIDPtr = dynamic_cast<ci::LocalPlayer *>(localPlayer)->GetNextHitAnim();
	if (hitAnimIDPtr != nullptr)
	{
		dbg.hitAnims.push_back(hitAnimIDPtr);
		this->SendAnimation(*hitAnimIDPtr, net.myID);
	}
}

void ClientLogic::UpdateActorValues()
{
	if (allowUpdateAVs == false)
		return;
	static clock_t lastUpd = 0;
	if (lastUpd + 550 < clock())
	{
		lastUpd = clock();
		for (uint8_t avID = av.Health; avID <= av.Stamina; ++avID)
		{
			if (clock() - this->lastUpdateByServer[avID] < 333)
				continue;
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
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, unreliable, NULL, net.remote, false);
			}
		}
	}
}

void ClientLogic::UpdateEquippedArmor()
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
				bsOut.Write(GetID(itemType));
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
				bsOut.Write(GetID(itemType));
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}
		}


		armorLast = armor;
	}
}

template<int32_t handID>
void ClientLogic::UpdateEquippedWeapon()
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
			bsOut.Write(GetID(weapWas));
			bsOut.Write(handID);
			net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
		}

		if (weap != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_EQUIP_ITEM);
			bsOut.Write(GetID(weap));
			bsOut.Write(handID);
			net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
		}

		weapWas = weap;
	}
}

template<int32_t handID>
void ClientLogic::UpdateEquippedSpell()
{
	enum : bool {
		isLeftHand = handID != 0
	};
	const ci::Spell *const sp = localPlayer->GetEquippedSpell(isLeftHand);
	static const ci::Spell *spWas = nullptr;
	if (sp != spWas)
	{
		if (spWas != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_UNEQUIP_SPELL);
			bsOut.Write(GetID(spWas));
			bsOut.Write(handID);
			net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
		}

		if (sp != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_EQUIP_SPELL);
			bsOut.Write(GetID(sp));
			bsOut.Write(handID);
			net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
		}

		spWas = sp;
	}
}

void ClientLogic::UpdateEquippedAmmo()
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
			bsOut.Write(GetID(ammoWas));
			bsOut.Write(handID);
			net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
		}

		if (ammo != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_EQUIP_ITEM);
			bsOut.Write(GetID(ammo));
			bsOut.Write(handID);
			net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
		}

		ammoWas = ammo;
	}
}

void ClientLogic::UpdateKnownEffects()
{
	using KnownEffects = std::map<EffectIndex, bool>;
	using KnownEffectsMap = std::map<uint32_t, KnownEffects>;

	static KnownEffectsMap knownEffectsWas;
	KnownEffectsMap knownEffects;

	for (auto &pair : itemTypes)
	{
		auto id = pair.first;
		auto itemType = pair.second;
		for (EffectIndex i = 0; i != itemType->GetNumEffects(); ++i)
		{
			knownEffects[id][i] = itemType->IsNthEffectKnown(i);
		}
	}

	std::map<uint32_t, std::set<EffectIndex>> learned;

	for (auto &pair : knownEffects)
	{
		auto itemTypeID = pair.first;

		for (EffectIndex i = 0; i != 4; ++i)
		{
			if (pair.second[i] != knownEffectsWas[itemTypeID][i])
			{
				knownEffectsWas[itemTypeID][i] = pair.second[i];
				if (pair.second[i])
					learned[itemTypeID].insert(i);
			}
		}
	}

	for (auto &pair : learned)
	{
		auto &learnedThis = pair.second;
		auto &itemTypeID = pair.first;
		for (auto idx : learnedThis)
		{
			this->OnEffectLearned(itemTypeID, idx);
		}

	}
}

void ClientLogic::UpdateWerewolfs()
{
	std::set<uint16_t> lastWerewolfs;

	auto onWerewolfUpdate = [this](ci::IActor *actor) {
		auto remPl = dynamic_cast<ci::RemotePlayer *>(actor);
		const bool isSpawned = remPl == nullptr || remPl->IsSpawned();
		actor->SetWerewolf(true, !isSpawned);
	};

	if (lastWerewolfs != werewolfs)
	{
		for (auto id : werewolfs)
		{
			try {
				auto wwPl = players.at(id);
				onWerewolfUpdate(wwPl);
			}
			catch (...) {
			}
		}
		lastWerewolfs = werewolfs;
	}
}