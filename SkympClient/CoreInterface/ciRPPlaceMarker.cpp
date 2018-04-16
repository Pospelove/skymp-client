#include "../stdafx.h"
#include "CoreInterface.h"
#include "ciRPPlaceMarker.h"

#include "../Sync/SyncEssential.h"

namespace ci
{
	uint32_t markerFormID = 0;
}

inline TESObjectSTAT *GetPlaceAtMeMarkerBase()
{
	return (TESObjectSTAT *)LookupFormByID(0x00000015);
}

void ci::PlaceMarker::Update()
{
	auto GetRelXy = [](float rzrot, float dist, float *outX, float *outY) {
		rzrot = (float)(rzrot * 3.14 / 180);
		*outX += dist * sin(rzrot);
		*outY += dist * cos(rzrot);
	};

	static TESObjectCELL *cellWas = nullptr;
	static void *wpWas = nullptr;
	auto cellNow = sd::GetParentCell(g_thePlayer);
	auto wpNow = sd::GetWorldSpace(g_thePlayer);
	bool isInteriorNow = false;
	if (cellWas != cellNow)
	{
		const bool isInterior[2] = {
			cellWas && cellWas->IsInterior(),
			cellNow && cellNow->IsInterior()
		};
		isInteriorNow = isInterior[1];
		if (isInterior[0] || isInterior[1] || wpWas != wpNow)
		{
			markerFormID = 0;
		}

		cellWas = cellNow;
		wpWas = wpNow;
	}

	auto ref = (TESObjectREFR *)LookupFormByID(markerFormID);
	if (!ref || ref->formType != FormType::Reference || sd::GetParentCell(ref) == nullptr)
	{
		static bool isTaskRunning = false;
		if (!isTaskRunning)
		{
			isTaskRunning = true;

			auto onPlace = [](cd::Value<TESObjectREFR> result) {
				markerFormID = result.GetFormID();
				isTaskRunning = false;
				AnimData_::Register(); // 1.0.1 shitfix
			};

			if (SyncOptions::GetSingleton()->GetInt("UNSAFE_MARKER_PLACEATME") != 0)
			{
				auto refr = sd::PlaceAtMe(g_thePlayer, GetPlaceAtMeMarkerBase(), 1, true, false);
				cd::Value<TESObjectREFR> cdRefr = refr;
				SET_TIMER_LIGHT(0, [cdRefr, onPlace] {
					onPlace(cdRefr);
				});
			}
			else
			{
				cd::PlaceAtMe(g_thePlayer, GetPlaceAtMeMarkerBase(), 1, true, false, onPlace);
			}
		}
		return;
	}

	auto pos = cd::GetPosition(g_thePlayer);
	GetRelXy(sd::GetAngleZ(g_thePlayer) + 128 + 90, SyncOptions::GetRespawnRadius(isInteriorNow) * 0.3, &pos.x, &pos.y);
	static clock_t lastTranslateTo = 0;
	if (clock() - lastTranslateTo > 1000)
	{
		lastTranslateTo = clock();
		if ((pos - cd::GetPosition(ref)).Length() > 2000)
			cd::SetPosition(ref, { pos.x,pos.y, pos.z });
	}
}

uint32_t ci::PlaceMarker::GetFormID()
{
	return markerFormID;
}

void ci::PlaceMarker::SetFormID(uint32_t newFormID)
{
	markerFormID = newFormID;
}