// Requires include CoreInterface.h first
#pragma once

namespace ci
{
	// Not threadsafe. SD thread only!
	// You can call only only ctor and dtor from other threads
	class Dispenser
	{
	public:
		Dispenser();
		void SetActive(bool active);
		void UpdateAndMoveTo(const MovementData &movement, bool isSpellMode);
		void Fire(const ItemType *bow, const ItemType *ammo, float power);

	private:
		void Create();
		void Destroy();

		struct Impl;
		std::shared_ptr<Impl> pimpl;
	};
}