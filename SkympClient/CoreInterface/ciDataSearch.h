#pragma once

namespace ci
{
	namespace DataSearch
	{
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

		void RequestNavMesh(std::function<void(NavMeshData)> callback);
	}
}