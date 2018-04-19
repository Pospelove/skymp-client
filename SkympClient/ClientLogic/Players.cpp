#include "ClientLogic.h"

void ClientLogic::InitPlayersHandlers()
{
	this->SetPacketHandler(ID_PLAYER_CREATE, [this](RakNet::BitStream &bsIn) {
		uint32_t baseNpc = 0;
		ci::LookData look;
		uint16_t id = ~0;
		ci::MovementData movement;
		uint32_t locationID = 0;

		bsIn.Read(baseNpc);
		Deserialize(bsIn, look);
		bsIn.Read(baseNpc);
		bsIn.Read(id);
		Deserialize(bsIn, movement);
		{
			ci::LookData _;
			Deserialize(bsIn, _);
		}
		bsIn.Read(locationID);

		uint16_t characters;
		bsIn.Read(characters);

		std::string name;
		for (size_t i = 0; i != characters; ++i)
		{
			char ch;
			bsIn.Read(ch);
			name += ch;
		}

		uint32_t cellID = localPlayer->GetCell();
		uint32_t worldSpaceID = localPlayer->GetWorldSpace();

		auto onHit = [this, id](const ci::HitEventData &eventData) {
			try {
				auto pl = players.at(id);

				const bool h2h = eventData.spell == nullptr && eventData.weapon == nullptr;
				if (h2h)
				{
					const auto closestAc = FindClosestActor(pl->GetPos(), [=](ci::IActor *ac) { return ac != pl; });
					if ((closestAc->GetPos() - pl->GetPos()).Length() > 256)
					{
						auto rem = dynamic_cast<ci::RemotePlayer *>(pl);
						if (rem != nullptr)
						{
							ci::Log("ERROR:ClientLogic bad h2h hit (is it invisible gnome?)");
							rem->Respawn();
						}
					}
				}

				auto source = (ci::IActor *)nullptr;
				if (eventData.hitSrcMark.empty())
				{
					source = players.at(net.myID);
					if (tracehost)
						ci::Chat::AddMessage(L"#BEBEBEEmpty Hit Mark. Probably LocalPlayer.");
				}
				else
				{
					source = players.at((uint16_t)atoi(eventData.hitSrcMark.data()));
				}
				if (tracehost)
					ci::Chat::AddMessage(L"#BEBEBEHit Mark = " + StringToWstring(eventData.hitSrcMark));
				this->OnHit(pl, source, eventData);
			}
			catch (const std::exception &e) {
				ci::Log(L"ERROR:ClientLogic OnHit() " + StringToWstring("Something gone wrong " + (std::string)e.what()));
			}
			catch (...) {
				ci::Log(L"ERROR:ClientLogic OnHit() Something gone wrong");
			}
		};

		auto onActivate = [this, id]() {
			try {
				this->OnActivate(players.at(id));
			}
			catch (...) {
			}
		};

		if (players[id] != nullptr)
		{
			delete players[id];
			players[id] = nullptr;
		}

		auto newPl = new ci::RemotePlayer(decodeRu(StringToWstring(name)), look, movement.pos, cellID, worldSpaceID, onHit, "RPEngineInput", onActivate);
		newPl->SetMark(std::to_string(id));
		players[id] = newPl;
		lastFurniture[id] = 0;

		//uint32_t baseNpc = 0;
		//bsIn.Read(baseNpc);
		if (baseNpc != 0 && baseNpc != ~0)
		{
			baseNPCs[id] = baseNpc;
			newPl->SetBaseNPC(baseNpc);
			if (IsHorseBase(baseNpc)) // лошадиный позор
			{
				auto p = new ci::RemotePlayer(
					newPl->GetName() + L" ",
					localPlayer->GetLookData(),
					localPlayer->GetPos(),
					localPlayer->GetCell(),
					localPlayer->GetWorldSpace(),
					onHit, "",
					onActivate);
				p->SetBaseNPC(baseNpc);
				p->SetMark(std::to_string(id));
				delete players[id];
				players[id] = p;
			}
		}
		else
			baseNPCs.erase(id);
		if (tracehost)
		{
			ci::Chat::AddMessage((L"#BEBEBE" L"BaseNPC ") + std::to_wstring(baseNpc));
		}
	});

	this->SetPacketHandler(ID_PLAYER_DESTROY, [this](RakNet::BitStream &bsIn) {
		uint16_t id = ~0;
		bsIn.Read(id);
		auto it = players.find(id);
		if (it != players.end())
		{
			delete it->second;
			players.erase(it);
			werewolfs.erase(id);
			hostedPlayers.erase(id);
		}
	});

	this->SetPacketHandler(ID_PLAYER_PAUSE, [this](RakNet::BitStream &bsIn) {
		uint16_t id = ~0;
		uint8_t isPaused;

		bsIn.Read(id);
		bsIn.Read(isPaused);

		try {
			auto player = dynamic_cast<ci::RemotePlayer *>(players.at(id));
			if (player != nullptr)
				player->SetInAFK(!!isPaused);
		}
		catch (...) {
		}
	});

	this->SetPacketHandler(ID_PLAYER_LOOK, [this](RakNet::BitStream &bsIn) {
		uint16_t id = ~0;
		ci::LookData look;

		bsIn.Read(id);
		Deserialize(bsIn, look);

		uint8_t isWerewolf = 0;
		bsIn.Read(isWerewolf);
		if (isWerewolf)
			werewolfs.insert(id);
		else
			werewolfs.erase(id);

		if (isWerewolf == false)
		{
			try {
				players.at(id)->SetWerewolf(false);
				ci::SetTimer(1000, [=] {
					try {
						if (!look.isEmpty())
							players.at(id)->ApplyLookData(look);
					}
					catch (...) {
					}
				});
			}
			catch (...) {
			};
		}
	});

	this->SetPacketHandler(ID_PLAYER_FURNITURE, [this](RakNet::BitStream &bsIn) {
		uint16_t playerID = ~0;
		uint32_t furnitureID = 0;

		bsIn.Read(playerID);
		bsIn.Read(furnitureID);

		lastFurniture[playerID] = furnitureID;
		try {

			auto pl = players.at(playerID);
			auto obj = objects.at(furnitureID);

			switch (objectsInfo[furnitureID].type)
			{
				// Что происходит ниже? Я ни*** не понимаю..
			case Type::Furniture:
			{
				if (pl == localPlayer)
				{
					static std::map<uint16_t, uint32_t> lastFurnitureUsed;

					auto it = objects.find(furnitureID);
					if (it != objects.end())
					{
						pl->UseFurniture(it->second, true);
						lastFurnitureUsed[playerID] = furnitureID;
					}
					else
					{
						enum {
							mustBeTrue = true
						};
						pl->UseFurniture(objects.at(lastFurnitureUsed[playerID]), mustBeTrue); // 2nd activate to stop use furniture
						lastFurnitureUsed.erase(playerID);
					}
				}
				break;
			}

			case Type::Container:

				try {
					if (obj->GetLockLevel() != 0)
					{
						static std::set<uint32_t> already;
						if (already.insert(furnitureID).second)
						{
							pl->UseFurniture(obj, true);
						}
						else
						{
							already.erase(furnitureID);
						}
					}
					else
					{
						pl->UseFurniture(obj, true);
					}
				}
				catch (...) {
				}
				break;

			case Type::TeleportDoor:
			case Type::Door:
				pl->UseFurniture(obj, true);
				break;

			case (Type)NULL:
				break;

			default:
				ci::Log("ERROR:ClientLogic Unknown furniture type");
				break;
			}
		}
		catch (...) {
		}
	});

	this->SetPacketHandler(ID_PLAYER_MOVEMENT, [this](RakNet::BitStream &bsIn) {
		uint16_t playerid = ~0;
		bsIn.Read(playerid);
		if (playerid == net.myID)
			return;

		ci::MovementData movData;
		Deserialize(bsIn, movData);
		uint8_t enabled;
		uint32_t locationID;
		bsIn.Read(enabled);
		bsIn.Read(locationID);
		try {
			auto &player = this->players.at(playerid);
			if (player->GetName() == localPlayer->GetName())
				movData.pos += NiPoint3{ 128, 128, 0 };
			if (movData.jumpStage == ci::MovementData::JumpStage::Jumping)
				lastFurniture[playerid] = 0;
			if (lastFurniture[playerid] != 0)
				movData.runMode = ci::MovementData::RunMode::Standing;
			if (enabled)
			{
				if (hostedPlayers.count(playerid) == 0)
				{
					player->ApplyMovementData(movData);
				}
			}

			if (locationID == localPlayer->GetLocation())
			{
				player->SetCell(localPlayer->GetCell());
				player->SetWorldSpace(localPlayer->GetWorldSpace());
			}
			else
			{
				player->SetCell(~0);
				player->SetWorldSpace(~0);
			}

			auto rPlayer = dynamic_cast<ci::RemotePlayer *>(player);
			if (rPlayer != nullptr)
			{
				int32_t clearVisualEffect = 0;
				for (int32_t i = 1; i >= 0; --i)
					if ((int32_t)movData.castStage[i] != 0)
					{
						rPlayer->MagicAttackBegin(i);
						rPlayer->SetVisualMagicEffect(rPlayer->GetEquippedSpell(i));
					}
					else
					{
						rPlayer->MagicAttackEnd(i);
						++clearVisualEffect;
					}
				if (clearVisualEffect == 2)
					rPlayer->SetVisualMagicEffect(nullptr);

				if (rPlayer->IsBroken())
				{
					auto engine = rPlayer->GetEngine();
					if (engine == "RPEngineInput")
					{
						ci::Log("ERROR:ClientLogic RemotePlayer is broken " + engine + " isHosted=" + std::to_string(hostedPlayers.count(playerid)));
					}
					else
					{
						ci::Log("WARN:ClientLogic RemotePlayer is broken " + engine);
					}
				}

				if (movData.mountStage == ci::MovementData::MountStage::OnMount)
				{
					if (rPlayer->HasAttachedHorse() == false)
					{
						const auto horse = dynamic_cast<ci::RemotePlayer *>(
							FindClosestActor(movData.pos, [this, movData](ci::IActor *actor) {
							return actor != localPlayer
								&& actor->GetPos().z < movData.pos.z
								&& abs(actor->GetAngleZ() - movData.angleZ) < 15.0
								&& (actor->GetPos() - movData.pos).Length() < 256.0;
						})
							);
						if (horse->IsSpawned())
						{
							rPlayer->SetAttachedHorse(horse);
						}
					}
				}
				else
					rPlayer->SetAttachedHorse(nullptr);
			}
		}
		catch (...) {
			//ci::Log("ERROR:ClientLogic RemotePlayer not found %u", playerid);
			//this->StreamOut(playerid);
		}
	});

	this->SetPacketHandler(ID_PLAYER_NAME, [this](RakNet::BitStream &bsIn) {
		using T = uint16_t;
		std::string name;
		T characters;
		uint16_t id = ~0;
		bsIn.Read(id);
		bsIn.Read(characters);

		for (size_t i = 0; i != characters; ++i)
		{
			char ch;
			bsIn.Read(ch);
			name += ch;
		}
		try {
			if (ADD_PLAYER_ID_TO_NICKNAME_LABEL && id != net.myID)
				name += " [" + std::to_string(id) + "]";
			players.at(id)->SetName(decodeRu(StringToWstring(name)));
		}
		catch (...) {
		}
	});

	this->SetPacketHandler(ID_PLAYER_INVENTORY, [this](RakNet::BitStream &bsIn) {
		uint16_t playerID;
		bool add;
		uint32_t count1;

		bsIn.Read(playerID);
		bsIn.Read(add);
		bsIn.Read(count1);

		for (uint32_t i = 0; i != count1; ++i)
		{
			uint32_t itemTypeID;
			uint32_t count;
			bsIn.Read(itemTypeID);
			bsIn.Read(count);

			try {
				auto asd = itemTypes.at(itemTypeID);
			}
			catch (...) {
				ci::Log("WARN:Logic:AddItem ItemType %d not found", itemTypeID);
			}

			try {
				auto itemType = itemTypes.at(itemTypeID);
				const bool silent = this->silentInventoryChanges;
				if (add)
				{
					if (itemType->IsCustomPotion())
						itemType->GenPotionName();

					players.at(playerID)->AddItem(itemType, count, silent);
					if (playerID == net.myID)
						numItems[itemType] += count;
				}
				else
				{
					players.at(playerID)->RemoveItem(itemType, count, silent);
					if (playerID == net.myID)
						numItems[itemType] -= count;
				}


				for (auto it = objects.begin(); it != objects.end(); ++it)
				{
					if (it->second)
						it->second->AddItem(itemTypes.at(itemTypeID), 0);
				}
			}
			catch (...) {
			}
		}
	});

	this->SetPacketHandler(ID_PLAYER_SPELLLIST, [this](RakNet::BitStream &bsIn) {
		uint16_t playerID;
		bsIn.Read(playerID);

		ci::IActor *pl;
		try {
			pl = players.at(playerID);
		}
		catch (...) {
			pl = nullptr;
		}
		if (!pl)
			return;

		using SpellList = std::set<ci::Spell *>;
		using EnchList = std::set<ci::Enchantment *>;
		SpellList spellList;
		EnchList enchList;
		while (true)
		{
			uint32_t spellID;
			const bool read = bsIn.Read(spellID);
			if (!read)
				break;
			try {
				spellList.insert(dynamic_cast<ci::Spell *>(magic.at(spellID)));
				enchList.insert(dynamic_cast<ci::Enchantment *>(magic.at(spellID)));
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic Unknown spell in SpellList");
			}
		}
		spellList.erase(nullptr);
		enchList.erase(nullptr);

		static std::map<uint16_t, SpellList> lastSpellList;

		for (auto &pair : this->magic)
		{
			auto ench = dynamic_cast<ci::Enchantment *>(pair.second);
			if (ench != nullptr)
				ench->SetPlayerKnows(false);
		}
		for (auto entry : enchList)
			entry->SetPlayerKnows(true);

		{
			std::vector<ci::Spell *> toRemove;
			std::set_difference(lastSpellList[playerID].begin(), lastSpellList[playerID].end(), spellList.begin(), spellList.end(),
				std::inserter(toRemove, toRemove.begin()));
			for (auto sp : toRemove)
			{
				if (pl == localPlayer)
					pl->RemoveSpell(sp, this->silentInventoryChanges);
			}
		}

		{
			std::vector<ci::Spell *> toAdd;
			std::set_difference(spellList.begin(), spellList.end(), lastSpellList[playerID].begin(), lastSpellList[playerID].end(),
				std::inserter(toAdd, toAdd.begin()));
			for (auto sp : toAdd)
				pl->AddSpell(sp, this->silentInventoryChanges);
		}

		lastSpellList[playerID] = spellList;
	});

	this->SetPacketHandler(ID_PLAYER_ANIM, [this](RakNet::BitStream &bsIn) {
		uint16_t playerID;
		uint32_t animID;
		bsIn.Read(playerID);
		bsIn.Read(animID);

		static bool reged = false;
		if (!reged)
		{
			reged = true;
			ci::RegisterAnimation("IdleStopInstant", ~0);
		}

		if (hostedPlayers.count(playerID) != 0 && ci::IsCombatAnimID(animID)) // Prevent doubling attack animations
			return;

		try {
			players.at(playerID)->PlayAnimation(animID);
		}
		catch (...) {
		}
	});

	this->SetPacketHandler(ID_PLAYER_AV, [this](RakNet::BitStream &bsIn) {
		uint16_t playerID;
		bsIn.Read(playerID);
		while (true)
		{
			uint8_t avID;
			ci::AVData avData;
			bsIn.Read(avID);
			bsIn.Read(avData.base);
			bsIn.Read(avData.modifier);
			const bool read = bsIn.Read(avData.percentage);
			if (!read)
				break;
			try {
				currentAVsOnServer[avID] = avData.percentage * (avData.base + avData.modifier);
				try {
					auto avName = av.GetAVName(avID);
					if (avName.size() > 0 && avName[0] == '-')
					{
						if (playerID == net.myID)
						{
							const std::string skillName = { avName.begin() + 1, avName.end() - 3 };
							const uint8_t skillPoints = uint8_t(avData.base + avData.modifier);
							localPlayer->SetSkillPointsPercent(skillName, skillPoints);
						}
					}
					else
					{
						players.at(playerID)->UpdateAVData(avName, avData);
					}
				}
				catch (...) {
				}
				allowUpdateAVs = true;
				lastUpdateByServer[avID] = clock();
			}
			catch (...) {
			}
		}
	});

	this->SetPacketHandler(ID_PLAYER_EQUIPMENT, [this](RakNet::BitStream &bsIn) {
		uint16_t playerID;
		uint32_t ammo, hands[2];
		bsIn.Read(playerID);
		bsIn.Read(ammo);
		bsIn.Read(hands[0]);
		bsIn.Read(hands[1]);

		ci::IActor *pl = nullptr;

		try {
			pl = players.at(playerID);
		}
		catch (...) {
			ci::Log("ERROR:ClientLogic Equipment: Player not found");
			return;
		}

		auto armorWas = pl->GetEquippedArmor();
		std::set<ci::ItemType *> armorNow;
		uint32_t armor;
		while (bsIn.Read(armor))
		{
			try {
				auto itemType = itemTypes.at(armor);
				pl->EquipItem(itemType, this->silentInventoryChanges, false, false);
				armorNow.insert(itemType);
				if (itemType->GetClass() != ci::ItemType::Class::Armor)
					ci::Log("ERROR:ClientLogic Equipment: Armor is not armor");
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic Equipment: Armor not found");
				return;
			}
		}
		for (auto itemType : armorWas)
		{
			if (armorNow.find(itemType) == armorNow.end())
				pl->UnequipItem(itemType, this->silentInventoryChanges, false, false);
		}

		for (int32_t i = 0; i <= 1; ++i)
		{
			const bool isLeftHand = (i == 1);
			pl->UnequipItem(pl->GetEquippedWeapon(i), true, false, isLeftHand);
			if (hands[i] != ~0)
			{
				try {
					auto itemType = itemTypes.at(hands[i]);
					pl->EquipItem(itemType, this->silentInventoryChanges, false, isLeftHand);
					if (itemType->GetClass() != ci::ItemType::Class::Weapon)
						ci::Log("ERROR:ClientLogic Equipment: Weapon is not weapon");
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic Equipment: Weapon not found");
					return;
				}
			}
		}

		if (ammo != ~0)
		{
			try {
				auto itemType = itemTypes.at(ammo);
				pl->EquipItem(itemType, this->silentInventoryChanges, false, false);
				if (itemType->GetClass() != ci::ItemType::Class::Ammo)
					ci::Log("ERROR:ClientLogic Equipment: Ammo is not ammo");
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic Equipment: Ammo not found");
				return;
			}
		}
		else
		{
			pl->UnequipItem(pl->GetEquippedAmmo(), true, false, false);
		}
	});

	this->SetPacketHandler(ID_PLAYER_MAGIC_EQUIPMENT, [this](RakNet::BitStream &bsIn) {
		uint16_t playerID;
		uint32_t spellIDs[2];
		bsIn.Read(playerID);

		ci::IActor *pl;
		try {
			pl = players.at(playerID);
		}
		catch (...) {
			pl = nullptr;
		}
		if (!pl)
			return;

		for (int32_t i = 0; i <= 1; ++i)
		{
			const bool leftHand = (i == 1);

			bsIn.Read(spellIDs[i]);
			if (spellIDs[i] != ~0)
			{
				try {
					ci::Spell *sp;
					try {
						sp = dynamic_cast<ci::Spell *>(magic.at(spellIDs[i]));
					}
					catch (...) {
						sp = nullptr;
						ci::Log("ERROR:ClientLogic spell not found");
					}

					pl->EquipSpell(sp, leftHand);
				}
				catch (...) {
				}
			}
			else
				pl->UnequipSpell(pl->GetEquippedSpell(leftHand), leftHand);
		}
	});

	this->SetPacketHandler(ID_PLAYER_BOW_SHOT, [this](RakNet::BitStream &bsIn) {
		uint16_t playerID;
		uint32_t power;
		bsIn.Read(playerID);
		bsIn.Read(power);

		try {
			auto remotePlayer = dynamic_cast<ci::RemotePlayer *>(players.at(playerID));
			if (remotePlayer)
				remotePlayer->Fire(power * 0.01f);
		}
		catch (...) {
		}
	});

	this->SetPacketHandler(ID_PLAYER_SPELL_RELEASE, [this](RakNet::BitStream &bsIn) {
		uint16_t playerID;
		int32_t handID;
		bsIn.Read(playerID);
		bsIn.Read(handID);
		try {
			auto remotePlayer = dynamic_cast<ci::RemotePlayer *>(players.at(playerID));
			if (remotePlayer)
				remotePlayer->FireMagic(remotePlayer->GetEquippedSpell(handID), handID);
		}
		catch (...) {
		}
	});

	this->SetPacketHandler(ID_PLAYER_ACTIVE_EFFECTS, [this](RakNet::BitStream &bsIn) {
		static std::set<ci::MagicEffect *> knownEffects;

		struct ActiveEffectData
		{
			float duration;
			float magnitude;
			ci::IActor *caster;
		};
		using ActiveEffectList = std::map<ci::MagicEffect *, ActiveEffectData>;
		ActiveEffectList activeEffects;

		uint16_t playerID;
		bsIn.Read(playerID);

		ci::IActor *player;
		try {
			player = players.at(playerID);
		}
		catch (...) {
			player = nullptr;
		}
		if (player == nullptr)
		{
			ci::Log(L"ERROR:ClientLogic Magic target not found");
			return;
		}

		uint32_t numEffects;
		bsIn.Read(numEffects);
		for (uint32_t i = 0; i != numEffects; ++i)
		{
			uint32_t effectID;
			uint16_t casterID;
			float magnitude;
			int64_t durationMs;
			bsIn.Read(effectID);
			bsIn.Read(casterID);
			bsIn.Read(magnitude);
			bsIn.Read(durationMs);

			try {
				const auto effect = effects.at(effectID);
				const auto caster = players.at(casterID);

				activeEffects.insert({
					effect,{ durationMs / 1000.f, magnitude, caster }
				});
				knownEffects.insert(effect);
			}
			catch (...) {
				ci::Log(L"ERROR:ClientLogic Bad active effect");
			}
		}

		for (auto mgef : knownEffects)
		{
			try {
				auto activeEffectData = activeEffects.at(mgef);
			}
			catch (...) {
				player->RemoveActiveEffect(mgef);
			}
		}
		for (const auto &pair : activeEffects)
		{
			auto mgef = pair.first;
			auto &effectData = pair.second;
			player->AddActiveEffect(mgef, effectData.duration, effectData.magnitude);
		}
	});

	this->SetPacketHandler(ID_CHAT_BUBBLE, [this](RakNet::BitStream &bsIn) {
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
		uint16_t playerID;
		uint32_t ms;
		bsIn.Read(playerID);
		bsIn.Read(ms);

		if (message.empty())
			message = " ";


		auto str = decodeRu(StringToWstring(message));
		playerBubbles[playerID].reset(new ci::Text3D(str, { 0,0,0 }));
		playerBubbles[playerID]->SetFontHeight(25);
		playerBubbles[playerID]->SetDrawDistance(512.6667f);
		playerBubbles[playerID]->SetPosSource([=] {
			try {
				return players.at(playerID)->GetPos() += {0, 0, 166};
			}
			catch (...) {
				return NiPoint3{ -1000000000, 1000000000, -1000000000 };
			}
		});
		const auto addr = (size_t)playerBubbles[playerID].get();
		ci::SetTimer(ms, [=] {
			if ((size_t)playerBubbles[playerID].get() == addr)
				playerBubbles[playerID] = nullptr;
		});
	});

	this->SetPacketHandler(ID_PLAYER_HOST, [this](RakNet::BitStream &bsIn) {
		uint16_t playerID, hostID;
		uint8_t isNpc;
		uint16_t combatTarget;
		bsIn.Read(playerID);
		bsIn.Read(hostID);
		bsIn.Read(isNpc);
		if (bsIn.Read(combatTarget) == false)
			combatTarget = ~0;
		if (isNpc)
		{
			if (players.count(playerID) == 0)
				return;
			auto remPl = dynamic_cast<ci::RemotePlayer *>(players.at(playerID));
			if (remPl == nullptr)
				return;

			remPl->SetNicknameVisible(false);
			remPl->SetCombatTarget(combatTarget == 65535 ? nullptr : localPlayer); // shitfix of npc stuck

			std::string engine;

			if (hostID == net.myID)
			{
				engine = ("RPEngineIO");
				hostedPlayers.insert(playerID);
				if (tracehost)
				{
					ci::Chat::AddMessage(L"#BEBEBEHost start " + remPl->GetName() + L'[' + std::to_wstring(playerID) + L']');
				}
			}
			else
			{
				engine = ("RPEngineInput");
				hostedPlayers.erase(playerID);
				if (tracehost)
				{
					ci::Chat::AddMessage(L"#BEBEBEHost stop " + remPl->GetName() + L'[' + std::to_wstring(playerID) + L']');
				}
			}

			if (remPl->GetEngine() != engine)
				remPl->SetEngine(engine);
		}
	});
}