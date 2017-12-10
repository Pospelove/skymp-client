#include "../stdafx.h"
#include "CoreInterface.h"

namespace ci
{
	struct MagicEffect::Impl
	{
		Archetype archetype;
		CastingType castingType;
		Delivery delivery;
		Volume volume;

		uint32_t existingEffectID;
		EffectSetting *effect = nullptr;
		dlf_mutex m;
	};

	MagicEffect::MagicEffect(Archetype archetype, uint32_t existingEffectID, CastingType castingType, Delivery delivery, Volume volume)
	{
		this->Init(archetype, existingEffectID, castingType, delivery, volume);
	}

	MagicEffect::MagicEffect(const MagicEffect &r)
	{
		this->Init(r.GetArchetype(), r.GetBase(), r.GetCastingType(), r.GetDelivery(), r.GetVolume());
	}

	void MagicEffect::Init(Archetype archetype, uint32_t existingEffectID, CastingType castingType, Delivery delivery, Volume volume)
	{
		pimpl = new Impl;

		auto srcEffect = (EffectSetting *)LookupFormByID(existingEffectID);
		if (srcEffect == nullptr || srcEffect->formType != FormType::EffectSetting)
		{
			std::stringstream ss;
			ss << "ERROR:MagicEffect Invalid form ID " << std::hex << "0x" << existingEffectID;
			ErrorHandling::SendError(ss.str().data());
			throw ~0;
		}
		pimpl->effect = FormHeap_Allocate<EffectSetting>();
		memcpy(pimpl->effect, srcEffect, sizeof EffectSetting);
		pimpl->effect->formID = 0;
		pimpl->effect->SetFormID(Utility::NewFormID(), 1);

		pimpl->existingEffectID = existingEffectID;

		this->SetCastingType(castingType);
		this->SetDelivery(delivery);
		this->SetVolume(volume);

		pimpl->archetype = archetype;
		if ((uint32_t)archetype != pimpl->effect->properties.archetype)
			ErrorHandling::SendError("ERROR:MagicEffect Wrong archetype");
	}

	MagicEffect::Archetype MagicEffect::GetArchetype() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		return pimpl->archetype;
	}

	MagicEffect::Delivery MagicEffect::GetDelivery() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		return pimpl->delivery;
	}

	MagicEffect::CastingType MagicEffect::GetCastingType() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		return pimpl->castingType;
	}

	uint32_t MagicEffect::GetBase() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		return pimpl->existingEffectID;
	}

	MagicEffect::Volume MagicEffect::GetVolume() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		return pimpl->volume;
	}

	void MagicEffect::SetDelivery(Delivery del)
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		pimpl->delivery = del;
		pimpl->effect->properties.deliveryType = (int32_t)del;
	}

	void MagicEffect::SetCastingType(CastingType type)
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		pimpl->castingType = type;
		pimpl->effect->properties.castType = (int32_t)type;
	}

	void MagicEffect::SetVolume(Volume vol)
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		pimpl->volume = vol;
		pimpl->effect->properties.soundVolume = (int32_t)vol;
	}

	uint32_t MagicEffect::GetFormID() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		return pimpl->effect->GetFormID();
	}
}