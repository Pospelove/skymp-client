#pragma once
#include <memory>

namespace ci
{
	class ItemType
	{
		friend class LocalPlayer;
		friend class RemotePlayer;
		friend class Object;
	public:
		enum class Class
		{
			Misc =					0x00,
			Weapon =				0x01,
			Armor =					0x02,
			Ammo =					0x03,
		};

		enum class Subclass
		{
			MISC_Misc =				0x01,
			MISC_Gold =				0x02,
			MISC_Lockpick =			0x03,
			MISC_Torch =			0x04,
			WEAP_Sword =			0x05,
			WEAP_WarAxe =			0x06,
			WEAP_Mace =				0x07,
			WEAP_Dagger =			0x08,
			WEAP_Greatsword =		0x09,
			WEAP_Battleaxe =		0x0A,
			WEAP_Warhammer =		0x0B,
			WEAP_Bow =				0x0C,
			WEAP_Crossbow =			0x0D,
			ARMO_Armor =			0x0E,
			ARMO_Boots =			0x0F,
			ARMO_Gauntlets =		0x10,
			ARMO_Helmet =			0x11,
			ARMO_Shield =			0x12,
		};

		ItemType(Class class_, Subclass subclass, uint32_t existingItemID);
		~ItemType();

		Class GetClass() const;
		Subclass GetSubclass() const;
		std::wstring GetNativeName() const;

		void SetWeight(float weight);
		void SetGoldValue(uint32_t value);
		void SetArmorRating(float armorRAting);
		void SetDamage(float damage);

	private:
		uint32_t GetFormID() const;

		struct Impl;
		std::unique_ptr<Impl> pimpl;
	};
}