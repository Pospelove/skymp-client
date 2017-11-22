#include "../stdafx.h"
#include "CoreInterface.h"

class CIAccess
{
public:
	static ci::Mutex &GetMutex() {
		return ci::IClientLogic::callbacksMutex;
	}
};

void ci::DataSearch::RequestNavMesh(std::function<void(NavMeshData)> callback)
{
	std::thread([=] {
		for (auto i = 0ui32; i != 0x0010DFF9 + 1; ++i)
		{
			auto form = (NavMesh *)LookupFormByID(i);
			if (form)
			{
				if (form->formType == FormType::NAVM)
				{
					NavMeshData res;

					for (size_t i = 0; i != form->triangles.size(); ++i)
					{
						NavMeshData::Triangle tr;
						for (int32_t ver = 0; ver != 3; ++ver)
							tr.verticeIDs[ver] = form->triangles[i].vertices[ver];
						res.triangles.push_back(tr);
					}

					for (size_t i = 0; i != form->vertices.size(); ++i)
						res.vertices.push_back(form->vertices[i]);

					for (size_t i = 0; i != form->externalConnections.size(); ++i)
						res.externalConnections.push_back(form->externalConnections[i].navmeshID);

					res.formID = form->formID;

					{
						std::lock_guard<ci::Mutex> l(CIAccess::GetMutex());
						callback(res);
					}
					Sleep(50);
				}
			}
		}
	}).detach();
}