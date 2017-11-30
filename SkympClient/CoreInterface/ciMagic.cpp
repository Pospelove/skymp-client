#include "../stdafx.h"
#include "CoreInterface.h"

namespace ci
{
	// Copyable
	class EffectItem
	{
	public:
		EffectItem(const MagicEffect *magicEffect, float magnitude, float durationSec) :
			magicEffect(magicEffect), magnitude(magnitude), durationSec(durationSec)
		{
			if (magicEffect == nullptr)
				ErrorHandling::SendError("ERROR:EffectItem Bad magic effect");
		}

		explicit operator ::EffectItem *() const
		{
			if (this->ei == nullptr)
			{
				this->ei = new ::EffectItem;
				this->ei->conditions.nodes = nullptr;
				this->ei->area = 0;
				this->ei->cost = 0;
				this->ei->magnitude = magnitude;
				this->ei->duration = durationSec;
				this->ei->mgef = (EffectSetting *)LookupFormByID(magicEffect->GetFormID());
			}
			return this->ei;
		}

		const MagicEffect *const magicEffect;
		const float magnitude;
		const float durationSec;

	private:
		mutable ::EffectItem *ei = nullptr; // it's OK to copy
	};

	struct Magic::Impl
	{
		std::vector<ci::EffectItem> effects;
		dlf_mutex m;
	};

	Magic::Magic()
	{
		pimpl = new Impl;
	}

	Magic::~Magic()
	{
		delete pimpl;
	}

	bool Magic::AddEffect(const MagicEffect *newMagicEffect, float magnitude, float durationSec)
	{
		bool ret = false;
		SAFE_CALL("Magic", [&] {
			if (!newMagicEffect)
				return;
			std::lock_guard<dlf_mutex> l(pimpl->m);
			if (pimpl->effects.empty() == false)
			{
				const auto anyMagEf = pimpl->effects.front().magicEffect;
				if (newMagicEffect->GetCastingType() != anyMagEf->GetCastingType()
					|| newMagicEffect->GetDelivery() != anyMagEf->GetDelivery())
				{
					return;
				}
			}
			pimpl->effects.emplace_back(newMagicEffect, magnitude, abs(durationSec));

			auto effectItem = static_cast<::EffectItem *>(pimpl->effects.back());
			auto magicItem = this->GetMagicItem();
			if (magicItem != nullptr)
				magicItem->effectItemList.Add(effectItem);
			ret = true;
			return;
		});
		return ret;
	}

	void Magic::SetDelivery(Delivery del)
	{
		SAFE_CALL("Magic", [&] {
			std::lock_guard<dlf_mutex> l(pimpl->m);
			for (auto effectItem : pimpl->effects)
				const_cast<MagicEffect *>(effectItem.magicEffect)->SetDelivery(del);
		});
	}

	void Magic::SetCastingType(CastingType type)
	{
		SAFE_CALL("Magic", [&] {
			std::lock_guard<dlf_mutex> l(pimpl->m);
			for (auto effectItem : pimpl->effects)
				const_cast<MagicEffect *>(effectItem.magicEffect)->SetCastingType(type);
		});
	}

	bool Magic::IsEmpty() const
	{
		bool res = false;
		SAFE_CALL("Magic", [&] {
			res = this->GetNumEffects() == 0;
		});
		return res;
	}

	size_t Magic::GetNumEffects() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		return pimpl->effects.size();
	}

	const MagicEffect *Magic::GetNthEffect(size_t n) const
	{
		if (n >= pimpl->effects.size())
			return nullptr;
		return pimpl->effects[n].magicEffect;
	}

	float Magic::GetNthEffectMagnitude(size_t n) const
	{
		if (n >= pimpl->effects.size())
			return 0;
		return pimpl->effects[n].magnitude;
	}

	float Magic::GetNthEffectDuration(size_t n) const
	{
		if (n >= pimpl->effects.size())
			return 0;
		return pimpl->effects[n].durationSec;
	}

	Magic::CastingType Magic::GetCastingType() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		if (pimpl->effects.empty() == false)
			return pimpl->effects.front().magicEffect->GetCastingType();
		else
			return CastingType::ConstantEffect;
	}

	Magic::Delivery Magic::GetDelivery() const
	{
		std::lock_guard<dlf_mutex> l(pimpl->m);
		if (pimpl->effects.empty() == false)
			return pimpl->effects.front().magicEffect->GetDelivery();
		else
			return Delivery::Self;
	}
}