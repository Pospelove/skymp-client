#pragma once

namespace ci
{
	class ItemType;
	class Enchantment : public Magic
	{
		friend class ItemType;
	public:
		Enchantment(uint32_t existingEnchID);

		uint32_t GetBase() const;

	private:
		struct Impl;
		Impl *const pimpl;
		~Enchantment() {}
		virtual MagicItem *GetMagicItem() const;

	protected:
		uint32_t GetFormID() const;
	};
}