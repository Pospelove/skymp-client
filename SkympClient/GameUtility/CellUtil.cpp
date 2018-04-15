#include "../stdafx.h"
#include "CellUtil.h"

namespace CellUtil
{
	TESObjectCELL *GetParentNonExteriorCell(TESObjectREFR *ref)
	{
		auto cell = sd::GetParentCell(ref);
		if (cell && sd::IsInterior(cell) == false)
			cell = nullptr;
		return cell;
	}

	TESObjectCELL *LookupNonExteriorCellByID(FormID cellID)
	{
		auto cell = (TESObjectCELL *)LookupFormByID(cellID);
		if (!cell)
			return nullptr;
		if (cell->formType != FormType::Cell)
			return nullptr;
		//if (!sd::IsInterior(cell)) // Та самая ошибка с вылетом в интерьере. несколько часов. боль
		//	return nullptr;
		if (!cell->IsInterior())
			return nullptr;
		return cell;
	}
}