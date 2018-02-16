#pragma once

namespace ci
{
	namespace DataSearch
	{
		struct ReferenceData
		{
			uint32_t refID = 0;
			uint32_t baseID = 0;
			NiPoint3 pos = { 0,0,0 };
			NiPoint3 rot = { 0,0,0 };
			uint32_t locationID = 0;
		};

		struct NavMeshData
		{
			struct Triangle
			{
				uint16_t verticeIDs[3];

				Triangle() {
					verticeIDs[0] = verticeIDs[1] = verticeIDs[2] = 0;
				}
			};

			std::vector<NiPoint3> vertices;
			std::vector<Triangle> triangles;

			uint32_t formID = 0;
			std::vector<uint32_t> externalConnections;
		};

		struct TeleportDoorsData
		{
			struct TeleportDoor : ReferenceData
			{
			};

			TeleportDoor doors[2];
		};

		struct ContainerData : ReferenceData
		{
		};

		struct DoorData : ReferenceData
		{
		};

		struct ActivatorData : ReferenceData
		{
		};

		struct ItemData : ReferenceData
		{
			uint32_t value = 0;
			uint32_t damage = 0;
			ci::ItemType::Class cl;
			ci::ItemType::Subclass subCl;
		};

		struct ActorData : ReferenceData
		{
			uint32_t race;
		};

		void RequestNavMesh(std::function<void(NavMeshData)> callback);
		void RequestTeleportDoorsManual(std::function<void(TeleportDoorsData)> callback);
		void RequestContainers(std::function<void(ContainerData)> callback);
		void RequestDoors(std::function<void(DoorData)> callback);
		void RequestActivators(std::function<void(ActivatorData)> callback);
		void RequestItems(std::function<void(ItemData)> callback);
		void RequestActors(std::function<void(ActorData)> callback);
		void LuaCodegenStart(std::function<void()> onFinish = nullptr);
	}
}