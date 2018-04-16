#pragma once
#include <memory>
#include "ciMagicEffect.h"

namespace ci
{
	class IActor;
	class RemotePlayer;
	class LocalPlayer;
	class Spell : public Magic
	{
		friend class IActor;
		friend class RemotePlayer;
		friend class LocalPlayer;
		friend class MagicAccess;
	public:

		Spell(uint32_t existingSpellID);

		void SetCost(float cost);
		uint32_t GetBase() const;
		float GetCost() const;

	private:
		struct Impl;
		std::unique_ptr<Impl> pimpl;
		~Spell();
		virtual MagicItem *GetMagicItem() const;

	protected:
		uint32_t GetFormID() const;
	};
}
