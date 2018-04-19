#include "ClientLogic.h"

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