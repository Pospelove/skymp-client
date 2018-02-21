
#pragma once

#pragma comment(lib, "Ws2_32.lib")

#include <RakPeer.h>
#include "MessageID.h"

#include "../CoreInterface/CoreInterface.h"
#include "Serialization.h"

#include <functional>
#include <set>
#include <map>

std::string WstringToString(const std::wstring &wstring);
std::wstring StringToWstring(const std::string &string);

class SkympAgent // <= 0.9
{
public:

	struct ItemTypeData
	{
		uint8_t classID;
		uint8_t subclassID;
		uint32_t existingFormID;
		float weight;
		uint32_t goldVal;
		float armorRating;
		float damage;
		uint32_t equipSlot;
	};

	struct EffectItemData
	{
		uint32_t effectID;
		float mag, dur, area;
	};

public:

	virtual void Output(std::wstring message, bool isNotification = false) {}
	virtual void SendError(std::string message) {}
	virtual void MoveTo(NiPoint3 pos, float angle, uint32_t locationID) {}
	virtual void SetPlayerMovement(uint16_t playerID, ci::MovementData movement) {}
	virtual void SetPlayerName(uint16_t playerID, std::wstring name) {}
	virtual void CreatePlayer(uint16_t playerID, std::wstring name, ci::LookData look, NiPoint3 pos) {}
	virtual void DestroyPlayer(uint16_t playerID) {}
	virtual void SetPlayerPause(uint16_t playerID, bool pause) {}
	virtual void SetPlayerLook(uint16_t playerID, ci::LookData look) {}
	virtual void SetPlayerFurniture(uint16_t playerID, uint32_t objectID) {}
	virtual void ShowRaceMenu() {}
	virtual void HideDialog() {}
	virtual void ShowDialog(std::wstring title, ci::Dialog::Style style, std::wstring text, int32_t defaultIndex, std::function<void(ci::Dialog::Result)> sendResult) {}
	virtual void CreateObject(uint32_t objectID, bool isNative, uint32_t baseFormID, uint32_t locationID, NiPoint3 pos, NiPoint3 rot) {}
	virtual void DestroyObject(uint32_t objectID) {}
	virtual void SetObjectState(uint32_t objectID, NiPoint3 pos, NiPoint3 rot, uint32_t locationID) {}
	virtual void SetObjectName(uint32_t objectID, std::wstring name) {}
	virtual void SetObjectBehavior(uint32_t objectID, int32_t type, bool isOpen, uint32_t teleportTargetID, bool isDisabled, uint32_t itemsCount, uint32_t itemTypeID, uint16_t hostPlayerID, std::vector<std::pair<uint32_t, uint32_t>> toAdd, std::vector<std::pair<uint32_t, uint32_t>> toRemove) {}
	virtual void SetObjectMotionType(uint32_t objectID, int32_t motionTypeID) {}
	virtual void AddToPlayerInventory(uint16_t playerID, std::vector<std::pair<uint32_t, uint32_t>> pairsOfTypeAndCount, bool add) {}
	virtual void SetPlayerSpellList(uint16_t playerID, std::vector<uint32_t> spellList) {}
	virtual void PlayAnimation(uint16_t playerID, uint8_t animID) {}
	virtual void SetPlayerAVs(uint16_t playerID, std::vector<std::pair<uint8_t, ci::AVData>> avs) {}
	virtual void SetPlayerEquipment(uint16_t playerID, uint32_t rightHand, uint32_t leftHand, std::vector<uint32_t> armor, uint32_t ammo) {}
	virtual void SetPlayerMagicEquipment(uint16_t playerID, uint32_t rightHandSpell, uint32_t leftHandSpell) {}
	virtual void FireBow(uint16_t playerID, uint32_t power) {};
	virtual void FireMagic(uint16_t playerID, int32_t handID) {}
	virtual void SetItemTypeData(uint32_t itemTypeID, ItemTypeData itemTypeData) {}
	virtual void SetWeather(uint32_t weatherTypeOrID) {}
	virtual void SetGlobal(uint32_t globalID, float value) {}
	virtual void SetSilentInventoryChanges(bool silent) {}
	virtual void ExecuteCommand(std::string cmdText, int32_t cmdEngine) {}
	virtual void SetPlayerChatBubble(uint16_t playerID, std::wstring text, uint32_t ms) {}
	virtual void SetSpellData(uint32_t spellID, uint32_t formID, std::vector<EffectItemData> effects) {}
	virtual void SetEffectData(uint32_t effectID, uint32_t formID, uint8_t archetype, uint8_t castingType, uint8_t delivery) {}
	virtual void InitDataSearch() {}
	virtual void CreateText3D(uint32_t text3dID, NiPoint3 pos, std::wstring text) {}
	virtual void DestroyText3D(uint32_t text3dID) {}

	virtual ci::MovementData GetMyMovement() const = 0;
	virtual bool AmIOnPause() const = 0;
	virtual std::shared_ptr<uint32_t> GetMyNextAnimation() const = 0;
	virtual ci::AVData GetMyAVData(std::string avName) const = 0;
	virtual std::vector<uint32_t> GetMyEquippedArmor() const = 0;
	virtual uint32_t GetMyEquippedWeapon(int32_t handID) const = 0;
	virtual uint32_t GetMyEquippedAmmo() const = 0;
	virtual uint32_t GetMyEquippedSpell(int32_t handID) const = 0;
	virtual bool GetObjectMustBeHostedByMe(uint32_t objectID) const = 0;
	virtual uint16_t GetObjectHost(uint32_t objectID) const = 0;
	virtual NiPoint3 GetObjectLocalPos(uint32_t objectID) const = 0;
	virtual NiPoint3 GetObjectLocalRot(uint32_t objectID) const = 0;
	virtual bool IsObjectGrabbedByMe(uint32_t objectID) const = 0;
	virtual float GetObjectLocalSpeed(uint32_t objectID) const = 0;


public:
	void ConnectToServer(const std::string &host,
		uint16_t port,
		const std::string &hardcodedPassword,
		const std::string &password,
		const std::wstring &nickname)
	{
		for (int32_t i = 0; i != 1024; ++i)
			this->lastUpdateByServer[i] = NULL;

		net = {};
		net.peer = RakNet::RakPeerInterface::GetInstance();
		net.peer->Startup(1, &net.socket, 1);
		net.password = password;
		net.nickname = nickname;
		net.host = host;
		net.hardcodedPassword = hardcodedPassword;
		net.port = port;
		net.peer->Connect(net.host.data(), net.port, net.hardcodedPassword.data(), net.hardcodedPassword.size());

		net.connectingMsg = L"Connecting to " + StringToWstring(host) + L":" + std::to_wstring(port) + L"...";
	}

	void Tick()
	{
		return this->Update();
	}

	uint16_t GetPlayerID() const
	{
		return net.myID;
	}

	void SendChatInput(const std::wstring &text)
	{
		if (net.peer == nullptr || net.remote == RakNet::UNASSIGNED_SYSTEM_ADDRESS)
			return;

		RakNet::BitStream bsOut;
		bsOut.Write(ID_MESSAGE);
		bsOut.Write((uint16_t)text.size());
		for (auto it = text.begin(); it != text.end(); ++it)
			bsOut.Write(*it);

		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	void CloseConnection()
	{
		if (net.peer != nullptr)
		{
			net.fullyConnected = false;
			net.peer->CloseConnection(net.remoteGUID, true);
		}
		net = {};
	}

	void SendLook(ci::LookData newLook)
	{
		if (net.peer != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_UPDATE_LOOK);
			Serialize(bsOut, newLook);
			net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
		}
	}

	void DropItem(uint32_t itemTypeID, uint32_t count)
	{
		if (net.peer != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_DROPITEM);
			bsOut.Write(itemTypeID);
			bsOut.Write(count);
			net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
		}
	}

	void HitPlayer(uint16_t hitTarget, uint32_t weaponID, uint32_t spellID, bool powerAttack)
	{
		if (net.peer != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_HIT_PLAYER);
			bsOut.Write(hitTarget);
			bsOut.Write(weaponID);
			bsOut.Write(powerAttack);
			bsOut.Write(spellID);
			net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
		}
	}

	void HitObject(uint32_t hitTarget, uint32_t weaponID, uint32_t spellID, bool powerAttack)
	{
		if (net.peer != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_HIT_OBJECT);
			bsOut.Write(hitTarget);
			bsOut.Write(weaponID);
			bsOut.Write(powerAttack);
			bsOut.Write(spellID);
			net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
		}
	}

	void ActivateObject(uint32_t objectID, bool isOpen)
	{
		if (net.peer != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_ACTIVATE);
			bsOut.Write(objectID);
			bsOut.Write(isOpen);
			net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
		}
	}

	void ChangeContainer(uint32_t objectID, uint32_t itemTypeID, uint32_t count, bool isAdd)
	{
		if (net.peer != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_CONTAINER_CHANGED);
			bsOut.Write(itemTypeID);
			bsOut.Write(count);
			bsOut.Write(isAdd);
			net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
		}
	}

	enum class Opcode : uint8_t {
		NavMesh = 0,
		TeleportDoors = 1,
		Container = 2,
		Door = 3,
		Item = 4,
	};

	void SendDataSearchResult(ci::DataSearch::TeleportDoorsData res)
	{
		if (net.peer != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_DATASEARCH_RESULT);
			bsOut.Write(Opcode::TeleportDoors);
			for (int32_t i = 0; i != 2; ++i)
			{
				bsOut.Write(res.doors[i].refID);
				bsOut.Write(res.doors[i].baseID);
				bsOut.Write(res.doors[i].locationID);
				bsOut.Write(res.doors[i].pos.x);
				bsOut.Write(res.doors[i].pos.y);
				bsOut.Write(res.doors[i].pos.z);
				bsOut.Write(res.doors[i].rot.x);
				bsOut.Write(res.doors[i].rot.y);
				bsOut.Write(res.doors[i].rot.z);
			}
			net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
			this->Output(L"BEBEBE" L"TPDData sent");
		}
	}

	void SendDataSearchResult(ci::DataSearch::NavMeshData res)
	{
		if (net.peer != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_DATASEARCH_RESULT);
			bsOut.Write(Opcode::NavMesh);
			bsOut.Write(res.formID);
			bsOut.Write((uint32_t)res.vertices.size());
			for (size_t i = 0; i != res.vertices.size(); ++i)
			{
				bsOut.Write(res.vertices[i].x);
				bsOut.Write(res.vertices[i].y);
				bsOut.Write(res.vertices[i].z);
			}
			bsOut.Write((uint32_t)res.triangles.size());
			for (size_t i = 0; i != res.triangles.size(); ++i)
			{
				bsOut.Write(res.triangles[i].verticeIDs[0]);
				bsOut.Write(res.triangles[i].verticeIDs[1]);
				bsOut.Write(res.triangles[i].verticeIDs[2]);
			}
			bsOut.Write((uint32_t)res.externalConnections.size());
			for (size_t i = 0; i != res.externalConnections.size(); ++i)
			{
				bsOut.Write(res.externalConnections[i]);
			}
			net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
		}
	}

	void SendDataSearchResult(ci::DataSearch::ContainerData res)
	{
		if (net.peer != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_DATASEARCH_RESULT);
			bsOut.Write(Opcode::Container);
			bsOut.Write(res.refID);
			bsOut.Write(res.baseID);
			bsOut.Write(res.locationID);
			bsOut.Write(res.pos.x);
			bsOut.Write(res.pos.y);
			bsOut.Write(res.pos.z);
			bsOut.Write(res.rot.x);
			bsOut.Write(res.rot.y);
			bsOut.Write(res.rot.z);
			net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
		}
	}

	void SendDataSearchResult(ci::DataSearch::DoorData res)
	{
		if (net.peer != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_DATASEARCH_RESULT);
			bsOut.Write(Opcode::Door);
			bsOut.Write(res.refID);
			bsOut.Write(res.baseID);
			bsOut.Write(res.locationID);
			bsOut.Write(res.pos.x);
			bsOut.Write(res.pos.y);
			bsOut.Write(res.pos.z);
			bsOut.Write(res.rot.x);
			bsOut.Write(res.rot.y);
			bsOut.Write(res.rot.z);
			net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE, NULL, net.remote, false);
		}
	}

private:

	std::set<uint32_t> objectIDs;
	uint32_t myLocationID = -1;
	std::vector<std::pair<clock_t, std::function<void()>>> tasks;

	void Update()
	{
		for (auto &pair : tasks)
		{
			if (pair.first < clock())
			{
				pair.first = ~0;
				pair.second();
			}
		}

		{
			const auto it = std::remove_if(tasks.begin(), tasks.end(), [](std::pair<clock_t, std::function<void()>> pair) {
				return pair.first == ~0;
			});
			tasks.erase(it, tasks.end());
		}

		try {
			static bool was = false;
			bool nowOpen = this->AmIOnPause();
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
					this->SendError("UpdateNetworking()");
				}
				try {
					UpdateObjects();
				}
				catch (...) {
					this->SendError("UpdateObjects()");
				}
				try {
					UpdateCombat();
				}
				catch (...) {
					this->SendError("UpdateCombat()");
				}
				try {
					UpdateActorValues();
				}
				catch (...) {
					this->SendError("UpdateActorValues()");
				}
				try {
					UpdateEquippedArmor();
				}
				catch (...) {
					this->SendError("UpdateEquippedArmor()");
				}
				try {
					UpdateEquippedWeapon<0>();
					UpdateEquippedWeapon<1>();
				}
				catch (...) {
					this->SendError("UpdateEquippedWeapon<>()");
				}
				try {
					UpdateEquippedSpell<0>();
					UpdateEquippedSpell<1>();
				}
				catch (...) {
					this->SendError("UpdateEquippedSpell<>()");
				}
				try {
					UpdateEquippedAmmo();
				}
				catch (...) {
					this->SendError("UpdateEquippedAmmo()");
				}
			}
			catch (const std::exception &e) {
				this->SendError("OnUpdate() " + (std::string)e.what());
			}
			catch (...) {
				this->SendError("OnUpdate()");
			}
		}
	}

	void UpdateObjects()
	{
		const uint32_t sendingInterval = 50;
		bool sendMovement = false;
		static clock_t lastSend = 0;
		if (clock() - lastSend > sendingInterval)
		{
			lastSend = clock();
			sendMovement = true;
		}

		for (auto &id : objectIDs)
		{
			enum : uint16_t {
				INVALID_HOST_ID = (uint16_t)~0,
			};

			int32_t motionType = ci::Object::Motion_Keyframed;

			{
				if (this->GetObjectMustBeHostedByMe(id))
				{
					motionType = ci::Object::Motion_Dynamic;
					RakNet::BitStream bsOut;
					bsOut.Write(ID_HOST_START);
					bsOut.Write(id);
					net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
				}
			}

			const uint16_t host = this->GetObjectHost(id);

			const bool hostedByMe = host == net.myID;
			const bool hostedByOther = !hostedByMe && host != INVALID_HOST_ID;
			if (hostedByMe && sendMovement)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_HOSTED_OBJECT_MOVEMENT);
				bsOut.Write(id);
				const auto pos = this->GetObjectLocalPos(id);
				bsOut.Write(pos.x);
				bsOut.Write(pos.y);
				bsOut.Write(pos.z);
				const auto rot = this->GetObjectLocalRot(id);
				bsOut.Write(rot.x);
				bsOut.Write(rot.y);
				bsOut.Write(rot.z);
				const bool isGrabbed = this->IsObjectGrabbedByMe(id);
				bsOut.Write(isGrabbed);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, UNRELIABLE, NULL, net.remote, false);
			}

			static std::map<uint32_t, float> lastSpeed;
			lastSpeed[id] = (this->GetObjectLocalSpeed(id) + lastSpeed[id]) / 2;
			if (lastSpeed[id] > 2.0)
				motionType = ci::Object::Motion_Dynamic;

			if (hostedByOther)
				motionType = ci::Object::Motion_Keyframed;

			this->SetObjectMotionType(id, motionType);

			if (this->IsObjectGrabbedByMe(id))
			{
				static bool timerSet = false;
				if (!timerSet)
				{
					timerSet = true;
					auto task = [=] {
						try {
							if (objectIDs.find(id) != objectIDs.end())
								if (this->GetObjectHost(id) != net.myID)
								{
									auto pos = this->GetObjectLocalPos(id);
									//object->Respawn();
									//object->SetPosition(pos);
									this->SetObjectState(id, pos, { 0,0,0 }, myLocationID);
								}
						}
						catch (...) {
						}
						timerSet = false;
					};
					const int32_t ms = 1333;
					tasks.push_back({ clock() + ms, task });
				}
			}
		}
	}

	void UpdateCombat()
	{
		const auto hitAnimIDPtr = this->GetMyNextAnimation();
		if (hitAnimIDPtr != nullptr)
		{
			RakNet::BitStream bsOut;
			bsOut.Write(ID_ANIMATION_EVENT_HIT);
			bsOut.Write(*hitAnimIDPtr);
			net.peer->Send(&bsOut, LOW_PRIORITY, UNRELIABLE, NULL, net.remote, false);
		}
	}

	bool allowUpdateAVs = false;
	clock_t lastUpdateByServer[1024]; // index is AV ID
	std::map<uint8_t, float> currentAVsOnServer; // percentage

	struct AVLegend {
		enum {
			InvalidAV,
			Health,
			Magicka,
			Stamina,
			HealRate,
			MagickaRate,
			StaminaRate,
			HealRateMult,
			MagickaRateMult,
			StaminaRateMult,
			_fSprintStaminaDrainMult,
			_fSprintStaminaWeightBase,
			_fSprintStaminaWeightMult,
			CarryWeight,
			UnarmedDamage,
			OneHanded,
			TwoHanded,
			Marksman,
			Block,
			Smithing,
			HeavyArmor,
			LightArmor,
			Pickpocket,
			Lockpicking,
			Sneak,
			Alchemy,
			Speechcraft,
			Alteration,
			Conjuration,
			Destruction,
			Illusion,
			Restoration,
			Enchanting,
			NUM_AVS,
		};
		const std::string &GetAVName(uint8_t id) const
		{
			static const std::vector<std::string> names = {
				"",
				"Health",
				"Magicka",
				"Stamina",
				"HealRate",
				"MagickaRate",
				"StaminaRate",
				"HealRateMult",
				"MagickaRateMult",
				"StaminaRateMult",
				"_fSprintStaminaDrainMult",
				"_fSprintStaminaWeightBase",
				"_fSprintStaminaWeightMult",
				"CarryWeight",
				"UnarmedDamage",
				"OneHanded",
				"TwoHanded",
				"Marksman",
				"Block",
				"Smithing",
				"HeavyArmor",
				"LightArmor",
				"Pickpocket",
				"Lockpicking",
				"Sneak",
				"Alchemy",
				"Speechcraft",
				"Alteration",
				"Conjuration",
				"Destruction",
				"Illusion",
				"Restoration",
				"Enchanting"
			};
			if (id < NUM_AVS)
				return names[id];
			else
				return names[InvalidAV];
		}
	} av;

	void UpdateActorValues()
	{
		if (allowUpdateAVs == false)
			return;
		static clock_t lastUpd = 0;
		if (lastUpd + 333 < clock())
		{
			lastUpd = clock();
			for (uint8_t avID = av.Health; avID <= av.Stamina; ++avID)
			{
				if (clock() - this->lastUpdateByServer[avID] < 333)
					continue;
				RakNet::BitStream bsOut;
				bsOut.Write(ID_AV_CHANGED);
				const auto name = av.GetAVName(avID);
				const auto avData = this->GetMyAVData(name);
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
				}
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, UNRELIABLE, NULL, net.remote, false);
			}
		}
	}

	void UpdateEquippedArmor()
	{
		enum {
			INVALID_HAND = -1,
		};
		const int32_t handID = INVALID_HAND;

		const std::vector<uint32_t> armor = this->GetMyEquippedArmor();
		static std::vector<uint32_t> armorLast;

		if (armor != armorLast)
		{
			for (auto itemType : armor)
			{
				const bool sendEquip = std::find(armorLast.begin(), armorLast.end(), itemType) == armorLast.end();
				if (sendEquip)
				{
					RakNet::BitStream bsOut;
					bsOut.Write(ID_EQUIP_ITEM);
					bsOut.Write(itemType);
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
					bsOut.Write(itemType);
					bsOut.Write(handID);
					net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
				}
			}


			armorLast = armor;
		}
	}

	template<int32_t handID>
	void UpdateEquippedWeapon()
	{
		enum : bool {
			isLeftHand = handID != 0
		};
		const uint32_t weap = this->GetMyEquippedWeapon(isLeftHand);
		static uint32_t weapWas = -1;
		if (weap != weapWas)
		{
			if (weapWas != ~0)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_UNEQUIP_ITEM);
				bsOut.Write(weapWas);
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			if (weap != ~0)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_EQUIP_ITEM);
				bsOut.Write(weap);
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			weapWas = weap;
		}
	}

	template<int32_t handID>
	void UpdateEquippedSpell()
	{
		enum : bool {
			isLeftHand = handID != 0
		};
		const uint32_t sp = this->GetMyEquippedSpell(isLeftHand);
		static uint32_t spWas = -1;
		if (sp != spWas)
		{
			if (spWas != ~0)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_UNEQUIP_SPELL);
				bsOut.Write(spWas);
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			if (sp != ~0)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_EQUIP_SPELL);
				bsOut.Write(sp);
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			spWas = sp;
		}
	}

	void UpdateEquippedAmmo()
	{
		uint32_t const ammo = this->GetMyEquippedAmmo();
		static uint32_t ammoWas = -1;
		if (ammo != ammoWas)
		{
			enum {
				INVALID_HAND = -1,
			};
			const int32_t handID = INVALID_HAND;

			if (ammoWas != ~0)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_UNEQUIP_ITEM);
				bsOut.Write(ammoWas);
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			if (ammo != ~0)
			{
				RakNet::BitStream bsOut;
				bsOut.Write(ID_EQUIP_ITEM);
				bsOut.Write(ammo);
				bsOut.Write(handID);
				net.peer->Send(&bsOut, MEDIUM_PRIORITY, RELIABLE, NULL, net.remote, false);
			}

			ammoWas = ammo;
		}
	}

	void UpdateNetworking()
	{
		if (net.peer == nullptr)
			return;

		// SEND:
		try {
			auto movementData = this->GetMyMovement();
			static clock_t cl = 0;
			auto clNow = clock();
			const int32_t delay =
				movementData.runMode == ci::MovementData::RunMode::Standing ? 50 : 40;
			if (cl + delay < clNow)
			{
				cl = clNow;
				if (net.remote != RakNet::UNASSIGNED_SYSTEM_ADDRESS && net.fullyConnected
					&& !movementData.isEmpty()
					&& !ci::IsInPause())
				{
					RakNet::BitStream bsOut;
					bsOut.Write(ID_UPDATE_MOVEMENT);
					Serialize(bsOut, movementData);
					net.peer->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE, NULL, net.remote, false);
				}
			}
		}
		catch (...) {
			this->SendError("UpdateNetworking() Send");
		}

		// RECEIVE:
		for (auto packet = net.peer->Receive(); packet; net.peer->DeallocatePacket(packet), packet = net.peer->Receive())
		{
			if (net.remote == RakNet::UNASSIGNED_SYSTEM_ADDRESS)
				net.remote = packet->systemAddress;
			if (net.remoteGUID == RakNet::UNASSIGNED_RAKNET_GUID)
				net.remoteGUID = packet->guid;
			try {
				this->ProcessPacket(packet);
			}
			catch (...) {
				this->SendError("UpdateNetworking() Receive " + std::to_string((int64_t)packet->data[0]));
			}
		}
	}

	void ProcessPacket(RakNet::Packet *packet)
	{
		RakNet::BitStream bsOut;
		RakNet::BitStream bsIn(&packet->data[1], packet->length, false);

		switch (packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			bsOut.Write(ID_HANDSHAKE);

			char nicknameCStr[MAX_NICKNAME];
			if (net.nickname.size() > MAX_NICKNAME - 1)
				net.nickname.resize(MAX_NICKNAME - 1);
			{
				auto nickname = WstringToString(net.nickname);
				std::memcpy(nicknameCStr, nickname.data(), sizeof(char) * net.nickname.size());
			}
			nicknameCStr[net.nickname.size()] = NULL;
			nicknameCStr[MAX_NICKNAME - 1] = NULL;

			char passwordCStr[MAX_PASSWORD];
			if (net.password.size() > MAX_PASSWORD - 1)
				net.password.resize(MAX_PASSWORD - 1);
			std::memcpy(passwordCStr, net.password.data(), net.password.size());
			passwordCStr[net.password.size()] = NULL;
			passwordCStr[MAX_PASSWORD - 1] = NULL;

			for (size_t i = 0; i != MAX_NICKNAME + 1; ++i)
				bsOut.Write(nicknameCStr[i]);
			bsOut.Write(passwordCStr, MAX_PASSWORD + 1);

			this->Output(L"Connection request sent. Waiting for verification...");

			net.peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, NULL, packet->systemAddress, false);
			break;
		case ID_ALREADY_CONNECTED:
			this->Output(L"Already connected");
			Sleep(250);
			this->ConnectToServer(net.host, net.port, net.hardcodedPassword, net.password, net.nickname);
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			if (net.fullyConnected)
				this->Output(L"Server closed the connection");
			break;
		case ID_CONNECTION_LOST:
			this->Output(L"Connection lost");
			break;
		case ID_CONNECTION_BANNED:
			this->Output(L"You are banned from the server");
			break;
		case ID_SERVER_CLOSED_THE_CONNECTION:
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			this->Output(L"The server is full. Retrying ...");
			this->Output(net.connectingMsg);
			net.peer->Connect(net.host.data(), net.port, net.hardcodedPassword.data(), net.hardcodedPassword.size());
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			this->Output(L"Connection failed. Retrying ...");
			this->Output(net.connectingMsg);
			net.peer->Connect(net.host.data(), net.port, net.hardcodedPassword.data(), net.hardcodedPassword.size());
			break;

		case ID_WRONG_PASS:
			this->Output(L"Wrong password (" + StringToWstring(net.password) + L")");
			break;
		case ID_NAME_INVALID:
			this->Output(L"Name can only contain A-Z, a-z, 0-9 and _");
			break;
		case ID_NAME_ALREADY_USED:
			this->Output(L"Player " + net.nickname + L" is already connected to the server");
			break;
		case ID_WELCOME:
			this->Output(L"Connected.");
			bsIn.Read((uint16_t &)net.myID);
			net.fullyConnected = true;
			break;
		case ID_MESSAGE:
		{
			using T = uint16_t;
			if (packet->length > (2 * sizeof(RakNet::MessageID)) + sizeof(T))
			{
				std::wstring message;
				T characters;
				bsIn.Read(characters);

				if (packet->length == 2 + sizeof(T) + sizeof(wchar_t) * characters)
				{
					for (size_t i = 0; i != characters; ++i)
					{
						wchar_t ch;
						bsIn.Read(ch);
						message += ch;
					}
					bool isNotification;
					bsIn.Read((uint8_t &)isNotification);
					this->Output(message, isNotification);
				}
			}
			break;
		}
		case ID_MOVE_TO:
		{
			struct {
				float x, y, z;
				uint16_t angleZ;
				uint32_t cellOrWorldspace;
			} in;
			bsIn.Read(in.x);
			bsIn.Read(in.y);
			bsIn.Read(in.z);
			bsIn.Read(in.angleZ);
			bsIn.Read(in.cellOrWorldspace);
			this->MoveTo({ in.x, in.y, in.z }, (float)in.angleZ, in.cellOrWorldspace);
			myLocationID = in.cellOrWorldspace;
			break;
		}
		case ID_PLAYER_MOVEMENT:
		{
			uint16_t playerid = ~0;
			bsIn.Read(playerid);
			ci::MovementData movData;
			Deserialize(bsIn, movData);
			{
				uint32_t locationID; // deprecated
				bsIn.Read(locationID);
			}
			this->SetPlayerMovement(playerid, movData);
			break;
		}
		case ID_PLAYER_NAME:
		{
			using T = uint16_t;
			if (packet->length > (3 + sizeof(T)))
			{
				std::wstring name;
				T characters;
				uint16_t id = ~0;
				bsIn.Read(id);
				bsIn.Read(characters);

				if (packet->length == (3 + sizeof(T) + sizeof(wchar_t) * characters))
				{
					for (size_t i = 0; i != characters; ++i)
					{
						wchar_t ch;
						bsIn.Read(ch);
						name += ch;
					}
					this->SetPlayerName(id, name);
				}
			}

			break;
		}
		case ID_PLAYER_CREATE:
		{
			uint16_t id = ~0;
			ci::MovementData movement;
			ci::LookData look;
			uint32_t locationID = 0;

			bsIn.Read(id);
			Deserialize(bsIn, movement);
			Deserialize(bsIn, look);
			bsIn.Read(locationID);

			uint16_t characters;
			bsIn.Read(characters);

			std::wstring name;
			for (size_t i = 0; i != characters; ++i)
			{
				wchar_t ch;
				bsIn.Read(ch);
				name += ch;
			}
			this->CreatePlayer(id, name, look, movement.pos);
			break;
		}
		case ID_PLAYER_DESTROY:
		{
			uint16_t id = ~0;
			bsIn.Read(id);
			this->DestroyPlayer(id);
			break;
		}
		case ID_PLAYER_PAUSE:
		{
			uint16_t id = ~0;
			uint8_t isPaused;

			bsIn.Read(id);
			bsIn.Read(isPaused);

			this->SetPlayerPause(id, isPaused);
			break;
		}
		case ID_PLAYER_LOOK:
		{
			uint16_t id = ~0;
			ci::LookData look;

			bsIn.Read(id);
			Deserialize(bsIn, look);
			this->SetPlayerLook(id, look);
			break;
		}
		case ID_PLAYER_FURNITURE:
		{
			uint16_t playerID = ~0;
			uint32_t furnitureID = 0;

			bsIn.Read(playerID);
			bsIn.Read(furnitureID);
			this->SetPlayerFurniture(playerID, furnitureID);
			break;
		}
		case ID_SHOW_RACE_MENU:
			this->ShowRaceMenu();
			break;
		case ID_SHOW_DIALOG:
			using T = uint16_t;
			using Index = int32_t;
			using DialogID = uint32_t;
			if (packet->length > (1 * sizeof(RakNet::MessageID)) + sizeof(T))
			{
				std::wstring title;
				std::wstring text;
				T characters;
				T characters2;
				bsIn.Read(characters);
				bsIn.Read(characters2);


				if (packet->length == sizeof(DialogID) + sizeof(Index) + sizeof(ci::Dialog::Style) + (1 * sizeof(RakNet::MessageID)) + 2 * sizeof(T) + sizeof(wchar_t) * characters + sizeof(wchar_t) * characters2)
				{
					for (size_t i = 0; i != characters; ++i)
					{
						wchar_t ch;
						bsIn.Read(ch);
						title += ch;
					}
					for (size_t i = 0; i != characters2; ++i)
					{
						wchar_t ch;
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
						this->HideDialog();
						break;
					}

					this->ShowDialog(title, style, text, defaultIndex, [=](ci::Dialog::Result result) {
						this->OnDialogResponse(dialogID, result);
					});
				}
			}
			break;
		case ID_OBJECT_CREATE:
		{
			uint32_t id = 0;
			bool isNative = true;
			uint32_t baseFormID;
			uint32_t locationID;
			NiPoint3 pos, rot;

			bsIn.Read(id);
			bsIn.Read(isNative);
			bsIn.Read(baseFormID);
			bsIn.Read(locationID);
			bsIn.Read(pos.x);
			bsIn.Read(pos.y);
			bsIn.Read(pos.z);
			bsIn.Read(rot.x);
			bsIn.Read(rot.y);
			bsIn.Read(rot.z);

			this->CreateObject(id, isNative, baseFormID, locationID, pos, rot);
			objectIDs.insert(id);
			break;
		}
		case ID_OBJECT_DESTROY:
		{
			uint32_t id = 0;
			bsIn.Read(id);
			this->DestroyObject(id);
			objectIDs.erase(id);
			break;
		}
		case ID_OBJECT_POS_ROT_LOCATION:
		{
			uint32_t id = 0;
			NiPoint3 pos, rot;
			uint32_t locationID;
			bsIn.Read(id);
			bsIn.Read(pos.x);
			bsIn.Read(pos.y);
			bsIn.Read(pos.z);
			bsIn.Read(rot.x);
			bsIn.Read(rot.y);
			bsIn.Read(rot.z);
			bsIn.Read(locationID);
			this->SetObjectState(id, pos, rot, locationID);
			break;
		}
		case ID_OBJECT_NAME:
		{
			uint32_t id = 0;
			bsIn.Read(id);

			uint16_t characters;
			bsIn.Read(characters);

			std::wstring name;
			for (size_t i = 0; i != characters; ++i)
			{
				wchar_t ch;
				bsIn.Read(ch);
				name += ch;
			}
			this->SetObjectName(id, name);
			break;
		}
		case ID_OBJECT_BEHAVIOR:
		{
			uint32_t id = 0;
			uint8_t type;
			bool isOpen;
			uint32_t teleportTargetID = 0;
			bool isDisabled;
			uint32_t itemsCount;
			uint32_t itemTypeID;
			uint16_t hostPlayerID;

			bsIn.Read(id);
			bsIn.Read(type);
			bsIn.Read(isOpen);
			bsIn.Read(teleportTargetID);
			bsIn.Read(isDisabled);
			bsIn.Read(itemsCount);
			bsIn.Read(itemTypeID);
			bsIn.Read(hostPlayerID);

			std::vector<std::pair<uint32_t, uint32_t>> toAdd, toRemove;

			if (type == 0x06) // container
			{
				uint32_t size;
				bsIn.Read(size);
				for (uint32_t i = 0; i != size; ++i)
				{
					uint32_t itemTypeID;
					uint32_t count;
					bsIn.Read(itemTypeID);
					const bool read = bsIn.Read(count);
					if (!read)
						break;
					toAdd.push_back({ itemTypeID, count });
				}

				bsIn.Read(size);
				for (uint32_t i = 0; i != size; ++i)
				{
					uint32_t itemTypeID;
					uint32_t count;
					bsIn.Read(itemTypeID);
					const bool read = bsIn.Read(count);
					if (!read)
						break;
					toRemove.push_back({ itemTypeID, count });
				}
			}

			this->SetObjectBehavior(id, type, isOpen, teleportTargetID, isDisabled, itemsCount, itemTypeID, hostPlayerID, toAdd, toRemove);

			break;
		}
		case ID_PLAYER_INVENTORY:
		{
			uint16_t playerID;
			bool add;
			uint32_t count1;

			bsIn.Read(playerID);
			bsIn.Read(add);
			bsIn.Read(count1);

			std::vector<std::pair<uint32_t, uint32_t>> inventory;

			for (uint32_t i = 0; i != count1; ++i)
			{
				uint32_t itemTypeID;
				uint32_t count;
				bsIn.Read(itemTypeID);
				bsIn.Read(count);

				inventory.push_back({ itemTypeID, count });
			}

			this->AddToPlayerInventory(playerID, inventory, add);
			break;
		}
		case ID_PLAYER_SPELLLIST:
		{
			uint16_t playerID;
			bsIn.Read(playerID);

			std::vector<uint32_t> spells;

			while (true)
			{
				uint32_t spellID;
				const bool read = bsIn.Read(spellID);
				if (!read)
					break;
				spells.push_back(spellID);
			}

			this->SetPlayerSpellList(playerID, spells);

			break;
		}
		case ID_PLAYER_ANIM:
		{
			uint16_t playerID;
			uint32_t hitAnimID;
			bsIn.Read(playerID);
			bsIn.Read(hitAnimID);
			this->PlayAnimation(playerID, hitAnimID);
			break;
		}
		case ID_PLAYER_AV:
		{
			uint16_t playerID;
			bsIn.Read(playerID);

			std::vector<std::pair<uint8_t, ci::AVData>> avs;

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

				currentAVsOnServer[avID] = avData.percentage * (avData.base + avData.modifier);
				avs.push_back({ avID, avData });
				allowUpdateAVs = true;
				lastUpdateByServer[avID] = clock();
			}

			this->SetPlayerAVs(playerID, avs);
			break;
		}
		case ID_PLAYER_EQUIPMENT:
		{
			uint16_t playerID;
			uint32_t ammo, hands[2];
			bsIn.Read(playerID);
			bsIn.Read(ammo);
			bsIn.Read(hands[0]);
			bsIn.Read(hands[1]);


			std::vector<uint32_t> armors;
			uint32_t armor;
			while (bsIn.Read(armor))
			{
				armors.push_back(armor);
			}
			this->SetPlayerEquipment(playerID, hands[0], hands[1], armors, ammo);
		}
		case ID_PLAYER_MAGIC_EQUIPMENT:
		{
			uint16_t playerID;
			uint32_t spellIDs[2];
			bsIn.Read(playerID);
			for (int32_t i = 0; i <= 1; ++i)
				bsIn.Read(spellIDs[i]);
			this->SetPlayerMagicEquipment(playerID, spellIDs[0], spellIDs[1]);
			break;
		}
		case ID_PLAYER_BOW_SHOT:
		{
			uint16_t playerID;
			uint32_t power;
			bsIn.Read(playerID);
			bsIn.Read(power);
			this->FireBow(playerID, power);
			break;
		}
		case ID_PLAYER_SPELL_RELEASE:
		{
			uint16_t playerID;
			int32_t handID;
			bsIn.Read(playerID);
			bsIn.Read(handID);
			this->FireMagic(playerID, handID);
			break;
		}
		case ID_PLAYER_ACTIVE_EFFECTS:
		{
			break;
		}
		case ID_ITEMTYPES:
		{
			ItemTypeData itemType;
			uint32_t id;
			bsIn.Read(id);
			bsIn.Read(itemType.classID);
			bsIn.Read(itemType.subclassID);
			bsIn.Read(itemType.existingFormID);
			bsIn.Read(itemType.weight);
			bsIn.Read(itemType.goldVal);
			bsIn.Read(itemType.armorRating);
			bsIn.Read(itemType.damage);
			bsIn.Read(itemType.equipSlot);

			this->SetItemTypeData(id, itemType);
			break;
		}
		case ID_WEATHER:
		{
			uint32_t wTypeOrID;
			bsIn.Read(wTypeOrID);
			this->SetWeather(wTypeOrID);
			break;
		}
		case ID_GLOBAL_VARIABLE:
		{
			uint32_t globalID;
			float val;
			bsIn.Read(globalID);
			bsIn.Read(val);
			this->SetGlobal(globalID, val);
			break;
		}
		case ID_SILENT:
		{
			bool silent;
			bsIn.Read(silent);
			this->SetSilentInventoryChanges(silent);
			break;
		}
		case ID_COMMAND:
		{
			std::string str;
			ci::CommandType t = ci::CommandType::CDScript;
			bsIn.Read(t);
			char ch;
			while (bsIn.Read(ch))
				str += ch;
			if (str.size() > 0)
				str.erase(str.end() - 1);

			this->ExecuteCommand(str, (uint8_t)t);

			break;
		}
		case ID_CHAT_BUBBLE:
		{
			using T = uint16_t;
			if (packet->length > 2 + sizeof(T))
			{
				std::wstring message;
				T characters;
				bsIn.Read(characters);

				if (packet->length == 7 + sizeof(T) + sizeof(wchar_t) * characters)
				{
					for (size_t i = 0; i != characters; ++i)
					{
						wchar_t ch;
						bsIn.Read(ch);
						message += ch;
					}
					uint16_t playerID;
					uint32_t ms;
					bsIn.Read(playerID);
					bsIn.Read(ms);
					this->SetPlayerChatBubble(playerID, message, ms);
				}
			}
			break;
		}
		case ID_SPELL:
		{
			uint32_t id;
			uint32_t formID;
			uint32_t numEffects;
			bsIn.Read(id);
			bsIn.Read(formID);
			bsIn.Read(numEffects);

			std::vector<EffectItemData> vec;
			for (uint32_t i = 0; i != numEffects; ++i)
			{
				uint32_t effectID;
				float mag, dur, area;
				bsIn.Read(effectID);
				bsIn.Read(mag);
				bsIn.Read(dur);
				bsIn.Read(area);
				vec.push_back({ effectID, mag, dur, area });
			}

			this->SetSpellData(id, formID, vec);
			break;
		}
		case ID_EFFECT:
		{
			uint32_t id = 0;
			uint32_t formID = 0;
			uint8_t archetype = 0;
			uint8_t castingType = 0;
			uint8_t delivery = 0;
			bsIn.Read(id);
			bsIn.Read(formID);
			bsIn.Read(archetype);
			bsIn.Read(castingType);
			bsIn.Read(delivery);
			this->SetEffectData(id, formID, archetype, castingType, delivery);
			break;
		}
		case ID_DATASEARCH_INIT:
		{
			this->InitDataSearch();
			break;
		}
		case ID_TEXT_CREATE:
		{
			uint32_t id;
			NiPoint3 pos;
			uint32_t txtSize;
			std::wstring txt;
			bsIn.Read(id);
			bsIn.Read(pos.x);
			bsIn.Read(pos.y);
			bsIn.Read(pos.z);
			bsIn.Read(txtSize);
			for (uint32_t i = 0; i != txtSize; ++i)
			{
				wchar_t wch;
				bsIn.Read(wch);
				txt += wch;
			}
			this->CreateText3D(id, pos, txt);
			break;
		}
		case ID_TEXT_DESTROY:
		{
			uint32_t id;
			bsIn.Read(id);
			this->DestroyText3D(id);
			break;
		}
		default:
			this->Output(L"Unknown packet type " + std::to_wstring(packet->data[0]));
			break;

		}
	}

	void OnDialogResponse(uint32_t dialogID, ci::Dialog::Result result)
	{
		RakNet::BitStream bsOut;
		bsOut.Write(ID_DIALOG_RESPONSE);
		bsOut.Write((uint16_t)result.inputText.size());
		for (auto it = result.inputText.begin(); it != result.inputText.end(); ++it)
			bsOut.Write(*it);
		bsOut.Write(result.listItem);

		net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE_ORDERED, NULL, net.remote, false);
	}

	struct
	{
		RakNet::RakPeerInterface *peer = nullptr;
		RakNet::SocketDescriptor socket;
		std::string password, host, hardcodedPassword;
		std::wstring nickname;
		uint16_t port = 0;
		std::wstring connectingMsg;

		RakNet::SystemAddress remote = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
		RakNet::RakNetGUID remoteGUID = RakNet::UNASSIGNED_RAKNET_GUID;

		bool fullyConnected = false;
		uint16_t myID = ~0;

	} net;

protected:

	const AVLegend &GetAVLegend() {
		return av;
	}
};