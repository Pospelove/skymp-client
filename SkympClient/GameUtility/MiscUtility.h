#pragma once
#include <vector>

namespace Utility
{
	uint32_t NewFormID();
	std::vector<FormID> GetLoadedRefs(FormID worldSpaceID);
	bool IsForegroundProcess();
}