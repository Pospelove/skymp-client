#include "ClientLogic.h"

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

void ClientLogic::InitItemTypesHandlers()
{
	this->SetPacketHandler(ID_LEARN_EFFECT, [this](RakNet::BitStream &bsIn) {
		uint32_t itemTypeID;
		uint32_t effectIdx;
		bool learn;
		bsIn.Read(itemTypeID);
		bsIn.Read((uint32_t &)effectIdx);
		bsIn.Read(learn);
		try {
			auto itemType = itemTypes.at(itemTypeID);
			itemType->SetNthEffectKnown(effectIdx, learn);
		}
		catch (...) {
			ci::Log("ERROR:ClientLogic Unable to learn effect");
		}
	});

	this->SetPacketHandler(ID_ITEMTYPES, [this](RakNet::BitStream &bsIn) {
		struct
		{
			uint32_t id;
			uint8_t classID;
			uint8_t subclassID;
			uint32_t existingFormID;
			float weight;
			uint32_t goldVal;
			float armorRating;
			float damage;
			uint32_t equipSlot;
			int32_t soulSize, capacity;
			float health;
		} itemType;
		bsIn.Read(itemType.id);
		bsIn.Read(itemType.classID);
		bsIn.Read(itemType.subclassID);
		bsIn.Read(itemType.existingFormID);
		bsIn.Read(itemType.weight);
		bsIn.Read(itemType.goldVal);
		bsIn.Read(itemType.armorRating);
		bsIn.Read(itemType.damage);
		bsIn.Read(itemType.equipSlot);

		if (itemTypes[itemType.id] == nullptr)
			itemTypes[itemType.id] = new ci::ItemType(
				static_cast<ci::ItemType::Class>(itemType.classID),
				static_cast<ci::ItemType::Subclass>(itemType.subclassID),
				itemType.existingFormID);

		itemTypes[itemType.id]->SetGoldValue(itemType.goldVal);
		itemTypes[itemType.id]->SetWeight(itemType.weight);
		itemTypes[itemType.id]->SetArmorRating(itemType.armorRating);
		itemTypes[itemType.id]->SetDamage(itemType.damage);

		uint32_t numEffects = 0;
		if (!bsIn.Read(numEffects))
			numEffects = 0;
		for (uint32_t i = 0; i != numEffects; ++i)
		{
			enum {
				MAX_DECREMENTS = 1,
			};
			uint64_t minusMinus = 0;
			uint32_t effectID;
			float mag, dur, area;
			bsIn.Read(effectID);
			bsIn.Read(mag);
			bsIn.Read(dur);
			bsIn.Read(area);
		pizdaLabel:
			ci::MagicEffect *effect;
			try {
				effect = effects.at(effectID);
			}
			catch (...) {
				effect = nullptr;
				std::string data;
				for (auto &pair : effects)
					data += std::to_string(pair.first) + ' ';
				auto data2 = std::to_string(effectID);
				if (minusMinus < MAX_DECREMENTS)
				{
					minusMinus++;
					effectID--;
					goto pizdaLabel;
				}
				else
					ci::Log("ERROR:ClientLogic effect not found (ItemType) (" + data + ") " + data2);
			}
			if (effect)
				itemTypes[itemType.id]->AddEffect(effect, abs(mag), dur);
		}

		uint32_t enchID = ~0;
		bsIn.Read(enchID);
		if (enchID != ~0)
		{
			try {
				auto ench = dynamic_cast<ci::Enchantment *>(magic.at(enchID));
				if (ench == nullptr)
					ci::Log("ERROR:ClientLogic Magic - Bad dynamic cast");
				else
					itemTypes[itemType.id]->SetEnchantment(ench);
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic Magic not found");
			}
		}

		bsIn.Read(itemType.soulSize);
		bsIn.Read(itemType.capacity);
		bsIn.Read(itemType.health);
		itemTypes[itemType.id]->SetSoulSize(itemType.soulSize);
		itemTypes[itemType.id]->SetCapacity(itemType.capacity);
		itemTypes[itemType.id]->SetItemHealth(itemType.health);
	});
}