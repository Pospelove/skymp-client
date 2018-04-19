#include "ClientLogic.h"

void ClientLogic::InitRecipesHandlers()
{
	this->SetPacketHandler(ID_RECIPE, [this](RakNet::BitStream &bsIn) {
		uint32_t id;
		uint32_t prodID;
		uint32_t numProd;
		uint32_t kwrdID;

		bsIn.Read(id);
		bsIn.Read(prodID);
		bsIn.Read(numProd);
		bsIn.Read(kwrdID);

		try {
			auto kwrdTxt = keywords.at(kwrdID);
			try {
				auto prod = itemTypes.at(prodID);

				if (recipes[id] == nullptr)
				{
					recipes[id] = new ci::Recipe(kwrdTxt, prod, numProd);
				}

				//recipes[id]->SetPlayerKnows(true);
				recipes[id]->RemoveAllRecipeComponents();

				try {
					uint32_t contsize;
					bsIn.Read(contsize);
					for (uint32_t i = 0; i != contsize; ++i)
					{
						uint32_t itemTypeID;
						uint32_t count;
						bsIn.Read(itemTypeID);
						const bool read = bsIn.Read(count);
						if (!read)
							break;
						auto type = itemTypes.at(itemTypeID);
						recipes[id]->AddRecipeComponent(itemTypes[itemTypeID], count);
					}
				}
				catch (...) {
					ci::Log("ERROR:ClientLogic Bad recipe container");
				}
			}
			catch (...) {
				ci::Log("ERROR:ClientLogic Bad recipe produce " + std::to_string((int32_t &)prodID));
			}
		}
		catch (...) {
			ci::Log("ERROR:ClientLogic Bad recipe keyword " + std::to_string((int32_t &)kwrdID));
		}
	});
}