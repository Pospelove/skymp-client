#pragma once

namespace CellUtil
{
	TESObjectCELL *GetParentNonExteriorCell(TESObjectREFR *ref);
	TESObjectCELL *LookupNonExteriorCellByID(FormID cellID);
}