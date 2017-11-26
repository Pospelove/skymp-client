#include "../CoreInterface/CoreInterface.h"

#include <set>
#include <map>
#include <vector>
#include <thread>
#include <sstream>
#include <fstream>

#include "SkympAgent.h"

#pragma comment(lib, "Ws2_32.lib")
#include <RakPeer.h>

#include "Serialization.h"
#include "Config.h"
#include "MessageID.h"

class ClientLogic :
	public ci::Plugin,
	public SkympAgent
{
	// Standard CI callbacks:

	void OnStartup() override
	{
		localPlayer = ci::LocalPlayer::GetSingleton();

		ci::Chat::Init();
		ci::Chat::AddMessage(L"#eeeeeeSkyMP Client Started");
		localPlayer->SetName(L"Player");

		ci::SetUpdateRate(1);

		if (read_cfg() == 0)
		{
			ci::Chat::AddMessage(L"#eeeeee" + StringToWstring(cfgFile) + L" not found");
			return;
		}

		auto name = StringToWstring(g_config[CONFIG_NAME]);

		if (name != L"")
		{
			this->haveName = true;
			this->ConnectToServer(
				g_config[CONFIG_IP],
				std::atoi(g_config[CONFIG_PORT].data()),
				"A1",
				g_config[CONFIG_SERVER_PASSWORD],
				name);
			localPlayer->SetName(name);
		}
		else
		{
			ci::Chat::AddMessage(L"Press T and type your name");
		}
	}

	void OnUpdate() override
	{
		this->SkympAgent::Tick();

		if (this->testUpd != nullptr)
			this->testUpd();
	}

	void OnWorldInit() override
	{
	}

	void OnChatMessage(const std::wstring &text) override
	{
		if (!this->haveName)
		{
			this->haveName = true;
			this->ConnectToServer(
				g_config[CONFIG_IP],
				std::atoi(g_config[CONFIG_PORT].data()),
				"A1",
				g_config[CONFIG_SERVER_PASSWORD],
				text);
			localPlayer->SetName(text);
			return;
		}

		this->SkympAgent::SendChatInput(text);
	}

	void OnChatCommand(const std::wstring &cmdText, const std::vector<std::wstring> &arguments) override
	{
		static ci::LookData testLookData;
		static ci::Object *obj = nullptr;
		static std::vector<ci::RemotePlayer *> ps;
		static std::map<ci::RemotePlayer *, NiPoint3> offsets;

		if (cmdText == L"/q")
		{
			this->SkympAgent::CloseConnection();
			ci::SetTimer(300, [] {
				std::exit(0);
			});
		}
		else if (cmdText == L"//save")
		{
			static const auto file = "Debug_SavedWorldPoints.txt";
			const auto movData = localPlayer->GetMovementData();
			std::ofstream ofs(file, std::ios::app);

			ofs << "\nSkyMP Debug." << std::endl;

			static size_t i = 0;
			ofs << ++i << ") ";
			ofs << "X=" << movData.pos.x << ' ';
			ofs << "Y=" << movData.pos.y << ' ';
			ofs << "Z=" << movData.pos.z << ' ';
			ofs << "Angle=" << movData.angleZ << ' ';
			ofs << "Cell=" << std::hex << localPlayer->GetCell() << ' ';
			ofs << "WorldSpace=" << std::hex << localPlayer->GetWorldSpace() << ' ';
			ofs << std::endl;

			ci::Chat::AddMessage(L"#bebebe>> World point saved to " + StringToWstring(file));
		}
		else if (cmdText == L"//eq")
		{
			for (auto p : ps)
			{
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
		else if (cmdText == L"//clone")
		{
			auto onHit = [](const ci::HitEventData &eventData) {
				if (eventData.spell != nullptr)
					ci::Chat::AddMessage(L"Magic Hit");
				else if (eventData.weapon != nullptr)
					ci::Chat::AddMessage(L"Weap Hit");
				else
					ci::Chat::AddMessage(L"H2H Hit");
			};

			auto p = new ci::RemotePlayer(
				localPlayer->GetName(),
				localPlayer->GetLookData(),
				localPlayer->GetPos(),
				localPlayer->GetCell(),
				localPlayer->GetWorldSpace(),
				onHit);

			p->ApplyMovementData(localPlayer->GetMovementData());

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


			testUpd = [=] {
				this->OnChatCommand(L"//eq", {});
			};
		}
		else if (cmdText == L"//cddbg" || cmdText == L"//cdtrace")
		{
			static bool tr = true;
			ci::TraceCDCalls(tr);
			tr = !tr;
		}
		else if (cmdText == L"//error")
		{
			for (auto &pair : players)
			{
				auto rPl = dynamic_cast<ci::RemotePlayer *>(pair.second.get());
				if (rPl != nullptr)
				{
					rPl->TestMakeBroken();
					ci::Chat::AddMessage(L"#BEBEBE" L"Done");
				}
			}
		}
		else
		{
			std::wstring str = cmdText;
			for (auto it = arguments.begin(); it != arguments.end(); ++it)
				str += L' ' + *it;
			this->OnChatMessage(str);
		}
	}

	void OnRaceMenuExit() override
	{
		ci::SetTimer(1300, [&] {
			auto newLook = localPlayer->GetLookData();

			this->SkympAgent::SendLook(newLook);

			RakNet::BitStream bs;
			Serialize(bs, newLook);
			Deserialize(bs, newLook);
			localPlayer->ApplyLookData(newLook);
		});
	}

	void OnItemDropped(const ci::ItemType *itemType, uint32_t count) override
	{
		this->SkympAgent::DropItem(this->GetID(itemType), count);
	}

	// SkympAgent:

	ci::MovementData GetMyMovement() const override
	{
		return localPlayer->GetMovementData();
	}

	bool AmIOnPause() const override
	{
		return ci::IsInPause();
	}

	std::shared_ptr<uint8_t> GetMyNextAnimation() const override
	{
		return localPlayer->GetNextHitAnim();
	}

	ci::AVData GetMyAVData(std::string avName) const override
	{
		return localPlayer->GetAVData(avName);
	}

	std::vector<uint32_t> GetMyEquippedArmor() const override
	{
		std::vector<uint32_t> result;
		const auto armor = localPlayer->GetEquippedArmor();
		for (auto element : armor)
			result.push_back(this->GetID(element));
		return result;
	}

	uint32_t GetMyEquippedWeapon(int32_t handID) const override
	{
		return this->GetID(localPlayer->GetEquippedWeapon(handID));
	}

	uint32_t GetMyEquippedAmmo() const override
	{
		return this->GetID(localPlayer->GetEquippedAmmo());
	}

	uint32_t GetMyEquippedSpell(int32_t handID) const override
	{
		return this->GetID(localPlayer->GetEquippedSpell(handID));
	}

	bool GetObjectMustBeHostedByMe(uint32_t objectID) const override
	{
		try {
			auto &info = this->objectsInfo.at(objectID);
			auto &object = this->objects.at(objectID);

			const bool hostedByOther =
				info.hostID != (uint16_t)~0 && info.hostID != this->SkympAgent::GetPlayerID();
			if (!hostedByOther)
			{
				const float distance = (object->GetPos() - localPlayer->GetPos()).Length();
				if (object->IsCrosshairRef() || object->IsGrabbed() || distance < 256)
				{
					if (info.hostID == (uint16_t)~0)
					{
						return true;
					}
				}
			}
		}
		catch (...) {
			return false;
		}
		return false;
	}

	uint16_t GetObjectHost(uint32_t objectID) const
	{
		try {
			return objectsInfo.at(objectID).hostID;
		}
		catch (...) {
			return ~0;
		}
	}

	NiPoint3 GetObjectLocalPos(uint32_t objectID) const
	{
		try {
			return objects.at(objectID)->GetPos();
		}
		catch (...) {
			return { 0,0,0 };
		}
	}

	NiPoint3 GetObjectLocalRot(uint32_t objectID) const
	{
		try {
			return objects.at(objectID)->GetRot();
		}
		catch (...) {
			return { 0,0,0 };
		}
	}

	bool IsObjectGrabbedByMe(uint32_t objectID) const
	{
		try {
			return objects.at(objectID)->IsGrabbed();
		}
		catch (...) {
			return false;
		}
	}

	float GetObjectLocalSpeed(uint32_t objectID) const
	{
		try {
			return objects.at(objectID)->GetSpeed();
		}
		catch (...) {
			return 0;
		}
	}

	// SkympAgent implementation:

	void Output(std::wstring message, bool isNotification)
	{
		return ci::Chat::AddMessage(message, isNotification);
	}

	virtual void SendError(std::string message)
	{
		return ci::Log(message);
	}

	virtual void MoveTo(NiPoint3 pos, float angle, uint32_t locationID)
	{
		ci::SetTimer(100, [=] {
			if (ci::IsWorldSpace(locationID))
				localPlayer->SetWorldSpace(locationID);
			else if (ci::IsCell(locationID))
				localPlayer->SetCell(locationID);

			if (pos.x == pos.x && pos.y == pos.y && pos.z == pos.z
				&& pos.x != std::numeric_limits<float>::infinity() && pos.y != std::numeric_limits<float>::infinity() && pos.z != std::numeric_limits<float>::infinity()
				&& pos.x != -std::numeric_limits<float>::infinity() && pos.y != -std::numeric_limits<float>::infinity() && pos.z != -std::numeric_limits<float>::infinity())
			{
				auto distance = (localPlayer->GetPos() - pos).Length();
				if (distance > 32.0)
					localPlayer->SetPos(pos);
			}

			if (angle <= 360)
				localPlayer->SetAngleZ(angle);
		});
	}

	virtual void SetPlayerMovement(uint16_t playerID, ci::MovementData movement)
	{
		try {
			auto player = this->players.at(playerID).get();
			if (player->GetName() == localPlayer->GetName())
				movement.pos += NiPoint3{ 128, 128, 0 };
			player->ApplyMovementData(movement);
			player->SetCell(localPlayer->GetCell());
			player->SetWorldSpace(localPlayer->GetWorldSpace());

			auto rPlayer = dynamic_cast<ci::RemotePlayer *>(player);
			if (rPlayer != nullptr)
			{
				int32_t clearVisualEffect = 0;
				for (int32_t i = 1; i >= 0; --i)
					if ((int32_t)movement.castStage[i] != 0)
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
			}
		}
		catch (...) {
		}
	}

	virtual void SetPlayerName(uint16_t playerID, std::wstring name)
	{
		try {
			players.at(playerID)->SetName(name);
		}
		catch (...) {
		}
	}

	virtual void CreatePlayer(uint16_t playerID, std::wstring name, ci::LookData look, NiPoint3 pos)
	{
		uint32_t cellID = localPlayer->GetCell();
		uint32_t worldSpaceID = localPlayer->GetWorldSpace();

		auto onHit = [this, playerID](const ci::HitEventData &eventData) {
			try {
				this->HitPlayer(playerID, this->GetID(eventData.weapon), this->GetID(eventData.spell), eventData.powerAttack);
			}
			catch (...) {
			}
		};

		players[playerID].reset(new ci::RemotePlayer(name, look, pos, cellID, worldSpaceID, onHit));
	}

	virtual void DestroyPlayer(uint16_t playerID)
	{
		auto it = players.find(playerID);
		if (it != players.end())
		{
			it->second.reset();
			players.erase(it);
		}
	}

	virtual void SetPlayerPause(uint16_t playerID, bool pause)
	{
		try {
			auto player = dynamic_cast<ci::RemotePlayer *>(players.at(playerID).get());
			if (player != nullptr)
				player->SetInAFK(!!pause);
		}
		catch (...) {
		}
	}

	virtual void SetPlayerLook(uint16_t playerID, ci::LookData look)
	{
		ci::SetTimer(1000, [=] {
			try {
				if (!look.isEmpty())
					players.at(playerID)->ApplyLookData(look);
			}
			catch (...) {
			}
		});
	}

	virtual void SetPlayerFurniture(uint16_t playerID, uint32_t objectID)
	{
		try {

			switch (objectsInfo[objectID].type)
			{
				// Что происходит ниже? Я ни*** не понимаю..
			case Type::Furniture:
			{
				static std::map<uint16_t, uint32_t> lastFurnitureUsed;
				auto pl = players.at(playerID).get();

				auto it = objects.find(objectID);
				if (it != objects.end())
				{
					pl->UseFurniture(it->second, true);
					lastFurnitureUsed[playerID] = objectID;
				}
				else
				{
					enum {
						mustBeTrue = true
					};
					pl->UseFurniture(objects.at(lastFurnitureUsed[playerID]), mustBeTrue); // 2nd activate to stop use furniture
					lastFurnitureUsed.erase(playerID);
				}
				break;
			}

			case Type::Container:
				players.at(playerID)->UseFurniture(objects.at(objectID), true);
				break;

			case Type::TeleportDoor:
				players.at(playerID)->UseFurniture(objects.at(objectID), true);
				break;
			}
		}
		catch (...) {
		}
	}

	virtual void ShowRaceMenu()
	{
		ci::ShowRaceMenu();
	}

	virtual void HideDialog()
	{
		ci::Dialog::Hide();
	}

	virtual void ShowDialog(std::wstring title, ci::Dialog::Style style, std::wstring text, int32_t defaultIndex, std::function<void(ci::Dialog::Result)> sendResult)
	{
		ci::Dialog::Show(title, style, text, defaultIndex, sendResult);
	}

	virtual void CreateObject(uint32_t objectID, bool isNative, uint32_t baseFormID, uint32_t locationID, NiPoint3 pos, NiPoint3 rot)
	{
		auto onActivate = [this, objectID](bool isOpen) {
			try {
				this->SkympAgent::ActivateObject(objectID, isOpen);
			}
			catch (...) {
			}
		};

		auto onContainerChanged = [this, objectID](const ci::ItemType *itemType, uint32_t count, bool isAdd) {
			try {
				this->SkympAgent::ChangeContainer(objectID, this->GetID(itemType), count, isAdd);
			}
			catch (...) {
			}
		};

		auto onHit = [this, objectID](const ci::HitEventData &eventData) {
			try {
				this->SkympAgent::HitObject(objectID, this->GetID(eventData.weapon), this->GetID(eventData.spell), eventData.powerAttack);
			}
			catch (...) {
			}
		};

		objects[objectID] = new ci::Object(isNative ? objectID : 0, baseFormID, locationID, pos, rot, onActivate, onContainerChanged, onHit);
		objects[objectID]->SetDestroyed(true);
		objectsInfo[objectID] = {};
		objectsInfo[objectID].createMoment = clock();
	}

	virtual void DestroyObject(uint32_t objectID)
	{
		try {
			delete objects.at(objectID);
			objects.erase(objectID);
		}
		catch (...) {
		}
	}

	virtual void SetObjectState(uint32_t objectID, NiPoint3 pos, NiPoint3 rot, uint32_t locationID)
	{
		try {
			auto obj = objects.at(objectID);
			if (objectsInfo[objectID].hostID != this->SkympAgent::GetPlayerID())
			{
				enum : uint16_t {
					INVALID_HOST_ID = (uint16_t)~0,
				};
				if (objectsInfo[objectID].hostID != INVALID_HOST_ID)
				{
					if (obj->GetMotionType() != ci::Object::Motion_Keyframed)
						obj->SetMotionType(ci::Object::Motion_Keyframed);
					const float S = (pos - obj->GetPos()).Length();
					const float t = 0.100f;
					obj->TranslateTo(pos, rot, S / t, 300);
				}
				else
				{
					obj->SetPosition(pos);
					obj->SetAngle(rot);
				}
			}
			obj->SetLocation(locationID);
		}
		catch (...) {
		}
	}

	virtual void SetObjectName(uint32_t objectID, std::wstring name)
	{
		try {
			objects.at(objectID)->SetName(name);
		}
		catch (...) {
		}
	}

	virtual void SetObjectBehavior(uint32_t objectID, int32_t typeInt, bool isOpen, uint32_t teleportTargetID, bool isDisabled, uint32_t itemsCount, uint32_t itemTypeID, uint16_t hostPlayerID, std::vector<std::pair<uint32_t, uint32_t>> toAdd, std::vector<std::pair<uint32_t, uint32_t>> toRemove)
	{
		const Type type = (Type)typeInt;

		try {
			auto object = objects.at(objectID);


			objectsInfo[objectID].type = type;
			objectsInfo[objectID].hostID = hostPlayerID;

			if (hostPlayerID == this->SkympAgent::GetPlayerID())
			{
				ci::SetTimer(200, [=] {
					hostedJustNow.insert(object);
					ci::SetTimer(1800, [=] {
						ci::SetTimer(1, [=] {
							hostedJustNow.erase(object);
						});
					});
				});
			}

			object->SetDisabled(isDisabled);

			try {
				for (auto pair : toAdd)
					object->AddItem(this->itemTypes.at(pair.first), pair.second);

				for (auto pair : toRemove)
					object->RemoveItem(this->itemTypes.at(pair.first), pair.second);
			}
			catch (...) {
			}

			switch (type)
			{
			case Type::Static:
				break;
			case Type::Door:
				object->SetOpen(isOpen);
				object->SetDestroyed(false);
				object->BlockActivation(true);
				break;
			case Type::TeleportDoor:
				object->SetOpen(isOpen);
				switch (object->GetBase())
				{
				case 0x31897:
				case 0x351EB:
				case 0x180D8:
					object->SetDestroyed(true);
					object->BlockActivation(true);
					break;
				default:
					object->SetDestroyed(false);
					object->BlockActivation(false);
					break;
				}
				break;
			case Type::Activator:
			case Type::Furniture:
				object->SetDestroyed(false);
				object->BlockActivation(true);
				break;
			case Type::Container:
				object->SetDestroyed(false);
				object->BlockActivation(true);
				object->SetOpen(isOpen);
				break;
			case Type::Item:
				object->SetDestroyed(false);
				object->BlockActivation(true);
				object->SetBase(itemTypes[itemTypeID]);
				object->SetCount(itemsCount);
			}
		}
		catch (...) {
		}
	}

	virtual void SetObjectMotionType(uint32_t objectID, int32_t motionTypeID)
	{
		try {
			auto obj = objects.at(objectID);
			if (obj->GetMotionType() != motionTypeID)
			{
				obj->SetMotionType(motionTypeID);
			}
		}
		catch (...) {
		}
	}

	virtual void AddToPlayerInventory(uint16_t playerID, std::vector<std::pair<uint32_t, uint32_t>> pairsOfTypeAndCount, bool add)
	{
		for (size_t i = 0; i != pairsOfTypeAndCount.size(); ++i)
		{
			const auto itemTypeID = pairsOfTypeAndCount[i].first;
			const auto count = pairsOfTypeAndCount[i].second;
			try {
				auto itemType = itemTypes.at(itemTypeID);
				if (add)
					players.at(playerID)->AddItem(itemType, count, silentInventoryChanges);
				else
					players.at(playerID)->RemoveItem(itemType, count, silentInventoryChanges);
				for (auto it = objects.begin(); it != objects.end(); ++it)
				{
					if (it->second)
						it->second->AddItem(itemTypes.at(itemTypeID), 0);
				}
			}
			catch (...) {
			}
		}
	}

	virtual void SetPlayerSpellList(uint16_t playerID, std::vector<uint32_t> spellList)
	{
		static std::map<uint16_t, std::vector<uint32_t>> lastSpellList;

		ci::IActor *pl;
		try {
			pl = players.at(playerID).get();
		}
		catch (...) {
			return; 
		}

		{
			std::vector<uint32_t> toRemove;
			std::set_difference(lastSpellList[playerID].begin(), lastSpellList[playerID].end(), spellList.begin(), spellList.end(),
				std::inserter(toRemove, toRemove.begin()));
			for (auto sp : toRemove)
			{
				if (pl == localPlayer)
					pl->RemoveSpell(this->spells.at(sp), this->silentInventoryChanges);
			}
		}

		{
			std::vector<uint32_t> toAdd;
			std::set_difference(spellList.begin(), spellList.end(), lastSpellList[playerID].begin(), lastSpellList[playerID].end(),
				std::inserter(toAdd, toAdd.begin()));
			for (auto sp : toAdd)
				pl->AddSpell(this->spells.at(sp), this->silentInventoryChanges);
		}

		lastSpellList[playerID] = spellList;
	}

	virtual void PlayAnimation(uint16_t playerID, uint8_t animID)
	{
		try {
			auto pl = players.at(playerID).get();
			auto rPl = dynamic_cast<ci::RemotePlayer *>(pl);
			if (rPl != nullptr)
				rPl->PlayHitAnimation(animID);
		}
		catch (...) {
		}
	}

	virtual void SetPlayerAVs(uint16_t playerID, std::vector<std::pair<uint8_t, ci::AVData>> avs)
	{
		try {
			for (auto pair : avs)
			{
				players.at(playerID)->UpdateAVData(this->SkympAgent::GetAVLegend().GetAVName(pair.first), pair.second);
			}
		}
		catch (...) {
		}
	}

	virtual void SetPlayerEquipment(uint16_t playerID, uint32_t rightHand, uint32_t leftHand, std::vector<uint32_t> armor, uint32_t ammo)
	{
		uint32_t hands[2];
		hands[0] = rightHand;
		hands[1] = leftHand;
		ci::IActor *pl = nullptr;

		try {
			pl = players.at(playerID).get();
		}
		catch (...) {
			ci::Log("ERROR:ClientLogic Equipment: Player not found");
			return;
		}

		auto armorWas = pl->GetEquippedArmor();
		std::set<ci::ItemType *> armorNow;
		for(auto armorID : armor)
		{
			try {
				auto itemType = itemTypes.at(armorID);
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
	}

	virtual void SetPlayerMagicEquipment(uint16_t playerID, uint32_t rightHandSpell, uint32_t leftHandSpell)
	{
		uint32_t spellIDs[2];
		spellIDs[0] = rightHandSpell;
		spellIDs[1] = leftHandSpell;

		ci::IActor *pl;
		try {
			pl = players.at(playerID).get();
		}
		catch (...) {
			pl = nullptr;
		}
		if (!pl)
			return;

		for (int32_t i = 0; i <= 1; ++i)
		{
			const bool leftHand = (i == 1);

			if (spellIDs[i] != ~0)
			{
				try {
					ci::Spell *sp;
					try {
						sp = spells.at(spellIDs[i]);
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
	}

	virtual void FireBow(uint16_t playerID, uint32_t power)
	{
		try {
			auto remotePlayer = dynamic_cast<ci::RemotePlayer *>(players.at(playerID).get());
			if (remotePlayer)
				remotePlayer->Fire(power * 0.01f);
		}
		catch (...) {
		}
	}

	virtual void FireMagic(uint16_t playerID, int32_t handID)
	{
		try {
			auto remotePlayer = dynamic_cast<ci::RemotePlayer *>(players.at(playerID).get());
			if (remotePlayer)
				remotePlayer->FireMagic(remotePlayer->GetEquippedSpell(handID), handID);
		}
		catch (...) {
		}
	}

	virtual void SetItemTypeData(uint32_t itemTypeID, ItemTypeData itemType)
	{
		if (itemTypes[itemTypeID] == nullptr)
			itemTypes[itemTypeID] = new ci::ItemType(
				static_cast<ci::ItemType::Class>(itemType.classID),
				static_cast<ci::ItemType::Subclass>(itemType.subclassID),
				itemType.existingFormID);

		itemTypes[itemTypeID]->SetGoldValue(itemType.goldVal);
		itemTypes[itemTypeID]->SetWeight(itemType.weight);
		itemTypes[itemTypeID]->SetArmorRating(itemType.armorRating);
		itemTypes[itemTypeID]->SetDamage(itemType.damage);
	}

	virtual void SetWeather(uint32_t wTypeOrID)
	{
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
	}

	virtual void SetGlobal(uint32_t globalID, float value)
	{
		ci::SetGlobalVariable(globalID, value);
	}

	virtual void SetSilentInventoryChanges(bool silent)
	{
		this->silentInventoryChanges = silent;
	}

	virtual void ExecuteCommand(std::string cmdText, int32_t cmdEngine)
	{
		enum {
			CommandTypeSkymp = 2,
		};

		if (cmdEngine == CommandTypeSkymp)
			this->ParseCommand(cmdText);
		else
			ci::ExecuteCommand((ci::CommandType)cmdEngine, cmdText);
	}

	virtual void SetPlayerChatBubble(uint16_t playerID, std::wstring text, uint32_t ms)
	{
		playerBubbles[playerID].reset(new ci::Text3D(text, { 0,0,0 }));
		playerBubbles[playerID]->SetFontHeight(25);
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
	}

	virtual void SetSpellData(uint32_t id, uint32_t formID, std::vector<EffectItemData> effectItems)
	{
		if (spells[id] == nullptr)
		{
			spells[id] = new ci::Spell(formID);
			for (uint32_t i = 0; i != effectItems.size(); ++i)
			{
				try {
					spells[id]->AddEffect(effects.at(effectItems[i].effectID), abs(effectItems[i].mag), effectItems[i].dur);
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic effect not found");
				}
			}
		}
	}

	virtual void SetEffectData(uint32_t id, uint32_t formID, uint8_t archetype, uint8_t castingType, uint8_t delivery)
	{
		if (effects[id] == nullptr)
		{
			effects[id] = new ci::MagicEffect(ci::MagicEffect::Archetype(archetype), formID, ci::MagicEffect::CastingType(castingType), ci::MagicEffect::Delivery(delivery));
		}
	}

	virtual void InitDataSearch()
	{
		static bool inited = false;
		if (inited == false)
		{
			inited = true;
			this->dataSearchEnabled = true;
			ci::Chat::AddMessage(L"DataSearch process activated", true);

			this->tpdCallback = [this](ci::DataSearch::TeleportDoorsData res) {
				this->SkympAgent::SendDataSearchResult(res);
			};

			ci::DataSearch::RequestNavMesh([this](ci::DataSearch::NavMeshData res) {
				this->SkympAgent::SendDataSearchResult(res);
			});

			ci::DataSearch::RequestContainers([this](ci::DataSearch::ContainerData res) {
				this->SkympAgent::SendDataSearchResult(res);
			});

			ci::DataSearch::RequestDoors([this](ci::DataSearch::DoorData res) {
				this->SkympAgent::SendDataSearchResult(res);
			});

		}
	}

	virtual void CreateText3D(uint32_t id, NiPoint3 pos, std::wstring text)
	{
		if (text3Ds.find(id) != text3Ds.end())
			delete text3Ds[id];
		text3Ds[id] = new ci::Text3D(L" ", { 1000000000,1000000000,1000000000 });
		text3Ds[id]->SetPos(pos);
		text3Ds[id]->SetText(text);
	}

	virtual void DestroyText3D(uint32_t id)
	{
		auto it = text3Ds.find(id);
		if (it != text3Ds.end())
		{
			delete it->second;
			text3Ds.erase(it);
		}
	}

	//

	ci::LocalPlayer *localPlayer = nullptr;

	void ParseCommand(std::string cmd)
	{
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
			return;
		}
	}

	uint16_t GetID(const ci::IActor *player) const
	{
		for (auto it = this->players.begin(); it != this->players.end(); ++it)
		{
			if (it->second.get() == player)
				return it->first;
		}
		return -1;
	}

	uint32_t GetID(const ci::Object *object) const
	{
		for (auto it = this->objects.begin(); it != this->objects.end(); ++it)
		{
			if (it->second == object)
				return it->first;
		}
		return -1;
	}

	uint32_t GetID(const ci::ItemType *itemType) const
	{
		for (auto it = this->itemTypes.begin(); it != this->itemTypes.end(); ++it)
		{
			if (it->second == itemType)
				return it->first;
		}
		return -1;
	}

	uint32_t GetID(const ci::Spell *spell) const
	{
		for (auto it = this->spells.begin(); it != this->spells.end(); ++it)
		{
			if (it->second == spell)
				return it->first;
		}
		return -1;
	}

	enum class Type : uint8_t
	{
		Static = 0x00,
		Door = 0x01,
		TeleportDoor = 0x02,
		Activator = 0x03,
		Furniture = 0x04,
		Item = 0x05,
		Container = 0x06,
	};

	struct ObjectInfo
	{
		Type type = Type::Static;
		uint16_t hostID = ~0;
		clock_t createMoment = 0;

		float lastSpeed = 0;
	};
	std::map<uint32_t, ObjectInfo> objectsInfo;

	std::map<uint32_t, ci::Spell *> spells;
	std::map<uint32_t, ci::Object *> objects;
	std::map<uint32_t, ci::ItemType *> itemTypes;
	std::map<uint16_t, std::unique_ptr<ci::IActor>> players;
	bool haveName = false;
	bool silentInventoryChanges = false;
	bool dataSearchEnabled = false;
	std::function<void(ci::DataSearch::TeleportDoorsData)> tpdCallback;
	std::function<void()> testUpd;
	std::set<ci::Object *> hostedJustNow;
	std::map<uint16_t, std::shared_ptr<ci::Text3D>> playerBubbles;
	std::map<uint32_t, ci::MagicEffect *> effects;
	std::map<uint32_t, ci::Text3D *> text3Ds;
};

auto clientLogic = new ClientLogic;