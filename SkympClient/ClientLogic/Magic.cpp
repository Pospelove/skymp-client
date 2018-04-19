#include "ClientLogic.h"

void ClientLogic::InitMagicHandlers()
{
	this->SetPacketHandler(ID_SPELL, [this](RakNet::BitStream &bsIn) {
		uint8_t isEnch = false;
		uint8_t isCustomEnch = false;
		uint32_t id;
		uint32_t formID;
		uint32_t numEffects;
		bsIn.Read(isEnch);
		bsIn.Read(isCustomEnch);
		bsIn.Read(id);
		bsIn.Read(formID);
		bsIn.Read(numEffects);

		if (magic[id] == nullptr)
		{
			if (isEnch)
			{
				auto newEnch = new ci::Enchantment(formID, isCustomEnch);
				newEnch->SetPlayerKnows(false);
				magic[id] = newEnch;
			}
			else
				magic[id] = new ci::Spell(formID);
			for (uint32_t i = 0; i != numEffects; ++i)
			{
				uint32_t effectID;
				float mag, dur, area;
				bsIn.Read(effectID);
				bsIn.Read(mag);
				bsIn.Read(dur);
				bsIn.Read(area);
				try {
					magic[id]->AddEffect(effects.at(effectID), abs(mag), dur);
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic effect not found");
				}
			}
		}
		else
			bsIn.IgnoreBytes(16 * numEffects);

		const auto asSpell = dynamic_cast<ci::Spell *>(magic[id]);
		if (asSpell != nullptr)
		{
			float cost = 0;
			bsIn.Read(cost);
			asSpell->SetCost(cost);
		}
	});

	this->SetPacketHandler(ID_EFFECT, [this](RakNet::BitStream &bsIn) {
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

		using FormID = uint32_t;
		static std::map<FormID, ci::MagicEffect *> effectPool;

		if (effectPool[formID] == nullptr)
		{
			if (effects[id] == nullptr)
			{
				effects[id] = new ci::MagicEffect(ci::MagicEffect::Archetype(archetype), formID, ci::MagicEffect::CastingType(castingType), ci::MagicEffect::Delivery(delivery));
			}
			effectPool[formID] = effects[id];
		}
		else
			effects[id] = effectPool[formID];

		static size_t count = 0;
		++count;
		if (count != effects.size())
		{
			//ci::Log("ERROR:ClientLogic wat");
			// TODO: Explain how should it work and refactor
		}
	});
}