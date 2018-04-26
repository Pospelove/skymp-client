#include "../stdafx.h"
#include "CoreInterface.h"
#include "ciRPInvisibleFox.h"

struct ci::InvisibleFox::Impl
{
	std::unique_ptr<ci::Object> myPseudoFox = nullptr;
	NiPoint3 offset = { 0,0,0 };
	uint32_t formID = 0x00000000;
};

ci::InvisibleFox::InvisibleFox()
{
	pimpl.reset(new Impl);
}

void ci::InvisibleFox::SetActive(bool active)
{
	active ? this->CreateTheFox() : this->DestroyTheFox();
}

void ci::InvisibleFox::UpdateAndMoveTo(const MovementData &myMd)
{
	auto f = [this, &myMd] {
		static const auto pointAtSphere = [this](float angleZ, float aimingAngle, float r)->NiPoint3 {
			auto toRad = [](float v) {
				return v / 180 * acos(-1);
			};
			return { float(r * cos(-toRad(-90.f + angleZ)) * sin(toRad(aimingAngle + 90))),
				float(r * sin(-toRad(-90.f + angleZ)) * sin(toRad(aimingAngle + 90))),
				float(r * cos(toRad(aimingAngle + 90)))
			};
		};

		pimpl->formID = 0;
		if (pimpl->myPseudoFox != nullptr)
		{
			const float r = 600;
			const auto pos = myMd.pos + pointAtSphere(myMd.angleZ, myMd.aimingAngle, r) + pimpl->offset;

			pimpl->myPseudoFox->SetPosition(pos);
			auto baseForm = LookupFormByID(pimpl->myPseudoFox->GetBase());
			if (baseForm != nullptr)
			{
				const auto ref = sd::Game::FindClosestReferenceOfType(baseForm, pos.x, pos.y, pos.z, 128.0);
				pimpl->formID = ref ? ref->formID : 0;
			}
		}
	};
	SAFE_CALL("InvisibleFox", f);
}

void ci::InvisibleFox::SetOffsetZ(float offsetZ)
{
	pimpl->offset = { 0.0f,0.0f,offsetZ };
}

uint32_t ci::InvisibleFox::GetFormID() const
{
	return pimpl->formID;
}

void ci::InvisibleFox::DestroyTheFox()
{
	SAFE_CALL("InvisibleFox", [&] {
		pimpl->formID = 0x00000000;
		pimpl->myPseudoFox.reset();
	});
}

void ci::InvisibleFox::CreateTheFox()
{
	SAFE_CALL("InvisibleFox", [&] {
		if (pimpl->myPseudoFox == nullptr)
		{
			pimpl->myPseudoFox.reset(
				new ci::Object(0x00000000, ID_TESObjectSTAT::XMarkerHeading, { 0,0,0 }, { 0,0,0 })
			);
			pimpl->myPseudoFox->SetMotionType(Object::Motion_Keyframed);
		}
	});
}