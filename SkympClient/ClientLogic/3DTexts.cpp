#include "ClientLogic.h"

void ClientLogic::Init3DTextsHandlers()
{
	this->SetPacketHandler(ID_TEXT_CREATE, [this](RakNet::BitStream &bsIn) {
		uint32_t id;
		NiPoint3 pos;
		uint32_t txtSize;
		std::string txt;
		bsIn.Read(id);
		bsIn.Read(pos.x);
		bsIn.Read(pos.y);
		bsIn.Read(pos.z);
		bsIn.Read(txtSize);
		for (uint32_t i = 0; i != txtSize; ++i)
		{
			char wch;
			bsIn.Read(wch);
			txt += wch;
		}
		if (text3Ds.find(id) != text3Ds.end())
			delete text3Ds[id];
		text3Ds[id] = new ci::Text3D(L" ", { 1000000000,1000000000,1000000000 });
		text3Ds[id]->SetPos(pos);
		text3Ds[id]->SetText(decodeRu(StringToWstring(txt)));
	});

	this->SetPacketHandler(ID_TEXT_DESTROY, [this](RakNet::BitStream &bsIn) {
		uint32_t id;
		bsIn.Read(id);
		auto it = text3Ds.find(id);
		if (it != text3Ds.end())
		{
			delete it->second;
			text3Ds.erase(it);
		}
	});
}