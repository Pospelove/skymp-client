#include "ClientLogic.h"

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

void ClientLogic::InitObjectsHandlers()
{
	this->SetPacketHandler(ID_OBJECT_CREATE, [this](RakNet::BitStream &bsIn) {
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

		auto onActivate = [this, id](bool isOpen, ci::IActor *source) {
			try {
				this->OnActivate(objects.at(id), isOpen, source);
			}
			catch (...) {
			}
		};

		auto onContainerChanged = [this, id](const ci::ItemType *itemType, uint32_t count, bool isAdd) {
			try {
				this->OnContainerChanged(objects.at(id), itemType, count, isAdd);
			}
			catch (...) {
			}
		};

		auto onHit = [this, id](const ci::HitEventData &eventData) {
			try {
				this->OnHit(objects.at(id), eventData);
			}
			catch (...) {
			}
		};

		objects[id] = new ci::Object(isNative ? id : 0, baseFormID, /*locationID*/localPlayer->GetLocation(), pos, rot, onActivate, onContainerChanged, onHit);
		objects[id]->SetDestroyed(true);
		objects[id]->SetMotionType(ci::Object::Motion_Keyframed);
		objectsInfo[id] = {};
		objectsInfo[id].createMoment = clock();
	});

	this->SetPacketHandler(ID_OBJECT_DESTROY, [this](RakNet::BitStream &bsIn) {
		uint32_t id = 0;
		bsIn.Read(id);
		try {
			delete objects.at(id);
			objects.erase(id);
		}
		catch (...) {
		}
	});

	this->SetPacketHandler(ID_OBJECT_POS_ROT_LOCATION, [this](RakNet::BitStream &bsIn) {
		uint32_t id = 0;
		NiPoint3 pos, rot;
		uint32_t locationID;
		uint8_t isHostable;
		bsIn.Read(id);
		bsIn.Read(pos.x);
		bsIn.Read(pos.y);
		bsIn.Read(pos.z);
		bsIn.Read(rot.x);
		bsIn.Read(rot.y);
		bsIn.Read(rot.z);
		bsIn.Read(locationID);
		bsIn.Read(isHostable);
		try {
			auto obj = objects.at(id);
			if (objectsInfo[id].hostID != net.myID)
			{
				enum : uint16_t {
					INVALID_HOST_ID = (uint16_t)~0,
				};
				if (objectsInfo[id].hostID != INVALID_HOST_ID)
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
			//obj->SetLocation(locationID);
			obj->SetLocation(localPlayer->GetLocation());

			objectsInfo[id].isHostable = isHostable;
			if (isHostable == false)
			{
				obj->SetMotionType(ci::Object::Motion_Keyframed);
				for (auto ms : { 100, 200, 300, 500 })
				{
					ci::SetTimer(ms, [=] {
						try {
							objects.at(id)->SetMotionType(ci::Object::Motion_Keyframed);
						}
						catch (...) {
						}
					});
				}
			}
		}
		catch (...) {
		}
	});

	this->SetPacketHandler(ID_OBJECT_NAME, [this](RakNet::BitStream &bsIn) {
		uint32_t id = 0;
		bsIn.Read(id);

		uint16_t characters;
		bsIn.Read(characters);

		std::string name;
		for (size_t i = 0; i != characters; ++i)
		{
			char ch;
			bsIn.Read(ch);
			name += ch;
		}

		try {
			auto name1 = decodeRu(StringToWstring(name));
			if (name1.find(L'<') != name1.npos)
			{
				ci::Log("ERROR:ClientLogic bad object name");
				name1 = L" ";
			}
			objects.at(id)->SetName(name1);
		}
		catch (...) {
		}
	});

	this->SetPacketHandler(ID_OBJECT_BEHAVIOR, [this](RakNet::BitStream &bsIn) {
		uint32_t id = 0;
		Type type;
		bool isOpen;
		uint32_t teleportTargetID = 0;
		bool isDisabled;
		uint32_t itemsCount;
		uint32_t itemTypeID;
		uint16_t hostPlayerID;
		uint8_t lockLevel;
		bool destroyed = false;

		bsIn.Read(id);
		bsIn.Read(type);
		bsIn.Read(isOpen);
		bsIn.Read(teleportTargetID);
		bsIn.Read(isDisabled);
		bsIn.Read(itemsCount);
		bsIn.Read(itemTypeID);
		bsIn.Read(hostPlayerID);
		bsIn.Read(lockLevel);
		bsIn.Read(destroyed);

		try {
			auto object = objects.at(id);

			if (type == Type::Container)
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
					auto type = itemTypes.at(itemTypeID);
					object->AddItem(itemTypes[itemTypeID], count);
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
					auto type = itemTypes.at(itemTypeID);
					object->RemoveItem(itemTypes[itemTypeID], count);
				}
			}


			objectsInfo[id].type = type;
			objectsInfo[id].hostID = hostPlayerID;

			if (hostPlayerID == net.myID)
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
			object->SetLockLevel(lockLevel);

			switch (type)
			{
			case Type::Static:
				break;
			case Type::Door:
				object->SetOpen(isOpen);
				object->SetDestroyed(destroyed);
				object->BlockActivation(FALSE);
				break;
			case Type::TeleportDoor:
				object->SetOpen(isOpen);
				switch (object->GetBase())
				{
				case 0x31897:
				case 0x351EB:
				case 0x180D8:
					object->SetDestroyed(FALSE);
					object->BlockActivation(FALSE);
					break;
				default:
					object->SetDestroyed(destroyed);
					object->BlockActivation(false);
					break;
				}
				break;
			case Type::Activator:
			case Type::Furniture:
				object->SetDestroyed(destroyed);
				object->BlockActivation(true);
				object->SetMotionType(ci::Object::Motion_Keyframed);
				break;
			case Type::Container:
				object->SetDestroyed(destroyed);
				object->BlockActivation(true);
				object->SetOpen(isOpen);
				break;
			case Type::Item:
				object->SetDestroyed(destroyed);
				object->BlockActivation(true);
				object->SetBase(itemTypes[itemTypeID]);
				object->SetCount(itemsCount);
			}
		}
		catch (...) {
		}
	});

	this->SetPacketHandler(ID_OBJECT_KEYWORDS, [this](RakNet::BitStream &bsIn) {
		uint32_t id, numKs;
		std::list<uint32_t> keywords;
		bsIn.Read(id);
		bsIn.Read(numKs);

		uint32_t temp;
		for (size_t i = 0; i != numKs; ++i)
		{
			bsIn.Read(temp);
			keywords.push_back(temp);
		}

		try {
			for (auto kID : keywords)
			{
				ci::RemoveAllKeywords(objects.at(id)->GetBase());
				const auto keywordStr = this->keywords[kID];
				ci::AddKeyword(objects.at(id)->GetBase(), keywordStr);

				if (this->keywords[kID].empty())
					ci::Log("ERROR:ClientLogic id_object_keywords keyword not found");
			}
		}
		catch (...) {
			ci::Log("ERROR:ClientLogic id_object_keywords object not found");
		}
	});
}