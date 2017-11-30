#pragma once
#include "ciMagicEffect.h"

class MagicItem;

namespace ci
{
	class Magic
	{
	public:
		Magic();
		~Magic();

		// Fails if new magic effect is incompatible with already added effects
		bool AddEffect(const MagicEffect *newMagicEffect, float magnitude, float durationSec);

		using CastingType = ci::MagicEffect::CastingType;
		using Delivery = ci::MagicEffect::Delivery;

		// Settings of effects will be changed (!)
		void SetDelivery(Delivery delivery);
		void SetCastingType(CastingType type);

		bool IsEmpty() const;
		size_t GetNumEffects() const;
		const MagicEffect *GetNthEffect(size_t n) const;
		float GetNthEffectMagnitude(size_t n) const;
		float GetNthEffectDuration(size_t n) const;
		CastingType GetCastingType() const;
		Delivery GetDelivery() const;

		bool IsNthEffectKnown(size_t n) const;
		void SetNthEffectKnown(size_t n, bool known);

	private:
		struct Impl;
		Impl *pimpl;

	protected:
		virtual MagicItem *GetMagicItem() const = 0;
	};
}