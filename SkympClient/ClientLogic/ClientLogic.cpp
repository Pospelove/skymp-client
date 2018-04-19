#include "ClientLogic.h"

auto clientLogic = new ClientLogic;

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

	ci::Chat::Init(
		atoi(g_config[CONFIG_CHAT_OFFSET_X].data()),
		atoi(g_config[CONFIG_CHAT_OFFSET_Y].data())
	);
	ci::Chat::AddMessage(L"#eeeeeeSkyMP " + StringToWstring(version) + L" Client Started");
	ci::LocalPlayer::GetSingleton()->SetName(L"Player");

	ci::SetUpdateRate(1);

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

void ClientLogic::OnActivate(ci::Object *self, bool isOpen, ci::IActor *source)
{
	RakNet::BitStream bsOut;
	bsOut.Write(ID_ACTIVATE);
	const auto id = GetID(self);
	const auto sourceId = GetID(source);
	bsOut.Write(id);
	bsOut.Write((uint8_t)isOpen);
	bsOut.Write(sourceId);
	net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
}

void ClientLogic::OnActivate(ci::IActor *self)
{
	RakNet::BitStream bsOut;
	bsOut.Write(ID_ACTIVATE);
	const auto id = GetID(self);
	bsOut.Write((uint32_t)0);
	bsOut.Write(id);
	net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
}

void ClientLogic::OnContainerChanged(ci::Object *self, const ci::ItemType *itemType, uint32_t count, bool isAdd)
{
	RakNet::BitStream bsOut;
	bsOut.Write(ID_CONTAINER_CHANGED);

	uint32_t itemTypeID;
	for (auto it = itemTypes.begin(); it != itemTypes.end(); ++it)
		if (it->second == itemType)
		{
			itemTypeID = it->first;
			break;
		}
	bsOut.Write(itemTypeID);
	bsOut.Write(count);
	bsOut.Write((uint32_t)isAdd);
	bsOut.Write(GetID(self));
	net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
}

void ClientLogic::OnHit(ci::IActor *hitTarget, ci::IActor *hitSource, const ci::HitEventData &eventData)
{
	const auto playerID = GetID(hitTarget);
	const auto weaponID = GetID(eventData.weapon);
	const auto spellID = GetID(eventData.spell);
	const auto sourceID = GetID(hitSource);

	RakNet::BitStream bsOut;
	bsOut.Write(ID_HIT_PLAYER);
	bsOut.Write(playerID);
	bsOut.Write(weaponID);
	bsOut.Write(eventData.powerAttack);
	bsOut.Write(spellID);
	bsOut.Write(sourceID);
	net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);

	if (tracehost)
	{
		std::wstringstream ss;
		ss << L"#BEBEBE" << L"OnHit Source = " << sourceID << L"OnHit Target = " << playerID;
		ci::Chat::AddMessage(ss.str());
	}
}

void ClientLogic::OnHit(ci::Object *hitTarget, const ci::HitEventData &eventData)
{
	const auto objectID = GetID(hitTarget);
	const auto weaponID = GetID(eventData.weapon);
	const auto spellID = GetID(eventData.spell);

	RakNet::BitStream bsOut;
	bsOut.Write(ID_HIT_OBJECT);
	bsOut.Write(objectID);
	bsOut.Write(weaponID);
	bsOut.Write(eventData.powerAttack);
	bsOut.Write(spellID);
	net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
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
				auto source = players.at((uint16_t)atoi(evn.hitSrcMark.data()));
				if (!source)
					ci::Log("ERROR:ClientLogic Invalid hit source");
				else
					this->OnHit(localPlayer, source, evn);
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic LocalPlayer OnHit()");
			}
		});
	}

	firstInit = false;
}

void ClientLogic::RespawnPlayers()
{
	if (ci::IsLoadScreenOpen())
	{
		ci::SetTimer(1000, [this] {
			this->RespawnPlayers();
		});
	}
	else
	{
		ci::SetTimer(2500, [this] {
			for (auto &pair : players)
			{
				auto pl = dynamic_cast<ci::RemotePlayer *>(pair.second);
				if (pl)
					pl->Respawn();
			}
		});
	}
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

	for (auto &f : fns)
	{
		f();
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
}

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

void ClientLogic::OnRaceMenuExit()
{
	ci::SetTimer(1300, [&] {
		auto newLook = localPlayer->GetLookData();
		ld = newLook;

		RakNet::BitStream bsOut;
		bsOut.Write(ID_UPDATE_LOOK);
		Serialize(bsOut, newLook);

		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	});
}

void ClientLogic::OnItemDropped(const ci::ItemType *itemType, uint32_t count)
{
	const uint32_t itemTypeID = GetID(itemType);
	RakNet::BitStream bsOut;
	bsOut.Write(ID_DROPITEM);
	bsOut.Write(itemTypeID);
	bsOut.Write(count);
	net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
}

void ClientLogic::OnItemUsed(const ci::ItemType *itemType)
{
	// shitfix doubling this callback:
	static bool send = true;
	if (send)
	{
		const uint32_t itemTypeID = GetID(itemType);
		RakNet::BitStream bsOut;
		bsOut.Write(ID_USEITEM);
		bsOut.Write(itemTypeID);
		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}
	send = !send;
}

void ClientLogic::OnItemUsedInCraft(const ci::ItemType *itemType, uint32_t count)
{
	const uint32_t itemTypeID = GetID(itemType);
	RakNet::BitStream bsOut;
	bsOut.Write(ID_CRAFT_INGREDIENT);
	bsOut.Write(itemTypeID);
	bsOut.Write(count);
	net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
}

void ClientLogic::OnCraftFinish(bool isPoison, const ci::ItemType *itemType, uint32_t numCraftedItems)
{
	RakNet::BitStream bsOut;

	bool isAlchemy = itemType == nullptr;
	if (isAlchemy)
	{
		bsOut.Write(ID_CRAFT_FINISH_ALCHEMY);
		bsOut.Write((uint64_t)isPoison);
	}
	else
	{
		const uint32_t itemTypeID = GetID(itemType);
		bsOut.Write(ID_CRAFT_FINISH);
		bsOut.Write(itemTypeID);
		bsOut.Write(numCraftedItems);
	}

	net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
}

void ClientLogic::OnLockpick()
{
	RakNet::BitStream bsOut;
	bsOut.Write(ID_LOCKPICK);
	net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
}

void ClientLogic::OnPoisonAttack()
{
	RakNet::BitStream bsOut;
	bsOut.Write(ID_POISON_ATTACK);
	net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
}

void ClientLogic::OnItemEnchanting(const ci::ItemType *itemType, const ci::Enchantment *ench)
{
	const uint32_t itemTypeID = GetID(itemType);
	const uint32_t enchID = GetID(ench);
	RakNet::BitStream bsOut;
	bsOut.Write(ID_ENCHANTING_ITEM);
	bsOut.Write(itemTypeID);
	bsOut.Write(enchID);
	net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
}

void ClientLogic::UpdateObjects()
{
	const uint32_t sendingInterval = 50;
	bool sendMovement = false;
	static clock_t lastSend = 0;
	if (clock() - lastSend > sendingInterval)
	{
		lastSend = clock();
		sendMovement = true;
	}

	for (auto &pair : objects)
	{
		const auto id = pair.first;
		const auto object = pair.second;

		auto &info = objectsInfo[id];
		if (info.type != Type::Item)
			continue;
		if (info.isHostable == false)
			continue;

		enum : uint16_t {
			INVALID_HOST_ID = (uint16_t)~0,
		};

		int32_t motionType = ci::Object::Motion_Keyframed;

		const bool hostedByOther =
			info.hostID != INVALID_HOST_ID && info.hostID != net.myID;
		if (!hostedByOther)
		{
			const float distance = (object->GetPos() - localPlayer->GetPos()).Length();
			if (object->IsCrosshairRef() || object->IsGrabbed() || distance < 256)
			{
				motionType = ci::Object::Motion_Dynamic;
				if (info.hostID == INVALID_HOST_ID)
				{
					RakNet::BitStream bsOut;
					bsOut.Write(ID_HOST_START);
					bsOut.Write(id);
					net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
				}
			}
		}

		const bool hostedByMe =
			info.hostID == net.myID;
		if (hostedByMe && sendMovement)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_HOSTED_OBJECT_MOVEMENT);
			bsOut.Write(id);
			const auto pos = object->GetPos();
			bsOut.Write(pos.x);
			bsOut.Write(pos.y);
			bsOut.Write(pos.z);
			const auto rot = object->GetRot();
			bsOut.Write(rot.x);
			bsOut.Write(rot.y);
			bsOut.Write(rot.z);
			const bool isGrabbed = object->IsGrabbed();
			bsOut.Write(isGrabbed);
			net.peer->Send(&bsOut, MEDIUM_PRIORITY, unreliable, NULL, net.remote, false);
		}

		info.lastSpeed = (object->GetSpeed() + info.lastSpeed) / 2;
		if (info.lastSpeed > 2.0)
			motionType = ci::Object::Motion_Dynamic;

		if (hostedByOther)
			motionType = ci::Object::Motion_Keyframed;

		if (object->GetMotionType() != motionType)
			object->SetMotionType(motionType);

		if (object->IsGrabbed())
		{
			static bool timerSet = false;
			if (!timerSet)
			{
				timerSet = true;
				ci::SetTimer(1333, [=] {
					try {
						auto object = objects.at(id);
						if (objectsInfo[id].hostID != net.myID)
						{
							auto pos = object->GetPos();
							object->Respawn();
							object->SetPosition(pos);
						}
					}
					catch (...) {
					}
					timerSet = false;
				});
			}
		}
	}
}

void ClientLogic::SendAnimation(uint32_t animID, uint16_t source)
{
	RakNet::BitStream bsOut;
	bsOut.Write(ID_ANIMATION_EVENT_HIT);
	bsOut.Write(animID);
	bsOut.Write(source);
	net.peer->Send(&bsOut, LOW_PRIORITY, unreliable, NULL, net.remote, false);
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

void ClientLogic::OnEffectLearned(uint32_t itemTypeID, EffectIndex i)
{
	const bool learned = true;

	RakNet::BitStream bsOut;
	bsOut.Write(ID_LEARN_EFFECT);
	bsOut.Write(itemTypeID);
	bsOut.Write((uint32_t)i);
	bsOut.Write(learned);
	net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE, NULL, net.remote, false);
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

void ClientLogic::OnDialogResponse(uint32_t dialogID, ci::Dialog::Result result)
{
	//ci::Chat::AddMessage(L"OnDialogResponse " + std::to_wstring(dialogID) + L' ' + result.inputText + L' ' + std::to_wstring(result.listItem));
	RakNet::BitStream bsOut;
	bsOut.Write(ID_DIALOG_RESPONSE);
	bsOut.Write((uint16_t)result.inputText.size());
	const auto inputText = WstringToString(result.inputText);
	for (auto it = inputText.begin(); it != inputText.end(); ++it)
		bsOut.Write(*it);
	bsOut.Write(result.listItem);

	net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
}