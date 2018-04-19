#include "ClientLogic.h"

void ClientLogic::InitDataSearchHandlers()
{
	this->SetPacketHandler(ID_DATASEARCH_INIT, [this](RakNet::BitStream &bsIn) {
		static bool inited = false;
		if (inited == false)
		{
			inited = true;
			this->dataSearchEnabled = true;
			ci::Chat::AddMessage(L"DataSearch process activated", true);

			enum class Opcode : uint8_t {
				NavMesh = 0,
				TeleportDoors = 1,
				Container = 2,
				Door = 3,
				Item = 4,
				Actor = 5,
				Acti = 6,
			};

			this->tpdCallback = [this](ci::DataSearch::TeleportDoorsData res) {
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
				ci::Chat::AddMessage(L"#BEBEBE" L"TPDData sent");
			};

			ci::DataSearch::RequestNavMesh([this](ci::DataSearch::NavMeshData res) {
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
			});

			ci::DataSearch::RequestContainers([this](ci::DataSearch::ContainerData res) {
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
			});

			ci::DataSearch::RequestDoors([this](ci::DataSearch::DoorData res) {
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
			});

			ci::DataSearch::RequestActivators([this](ci::DataSearch::ActivatorData res) {
				RakNet::BitStream bsOut;
				bsOut.Write(ID_DATASEARCH_RESULT);
				bsOut.Write(Opcode::Acti);
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
			});

			ci::DataSearch::RequestActors([this](ci::DataSearch::ActorData res) {
				RakNet::BitStream bsOut;
				bsOut.Write(ID_DATASEARCH_RESULT);
				bsOut.Write(Opcode::Actor);
				bsOut.Write(res.refID);
				bsOut.Write(res.baseID);
				bsOut.Write(res.locationID);
				bsOut.Write(res.pos.x);
				bsOut.Write(res.pos.y);
				bsOut.Write(res.pos.z);
				bsOut.Write(res.rot.x);
				bsOut.Write(res.rot.y);
				bsOut.Write(res.rot.z);
				bsOut.Write(res.race);
				net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE, NULL, net.remote, false);
			});

			ci::DataSearch::RequestItems([this](ci::DataSearch::ItemData res) {
				RakNet::BitStream bsOut;
				bsOut.Write(ID_DATASEARCH_RESULT);
				bsOut.Write(Opcode::Item);
				bsOut.Write(res.refID);
				bsOut.Write(res.baseID);
				bsOut.Write(res.locationID);
				bsOut.Write(res.pos.x);
				bsOut.Write(res.pos.y);
				bsOut.Write(res.pos.z);
				bsOut.Write(res.rot.x);
				bsOut.Write(res.rot.y);
				bsOut.Write(res.rot.z);
				bsOut.Write(res.value);
				bsOut.Write(res.damage);
				bsOut.Write(res.cl);
				bsOut.Write(res.subCl);
				net.peer->Send(&bsOut, LOW_PRIORITY, RELIABLE, NULL, net.remote, false);
			});

		}
	});
}