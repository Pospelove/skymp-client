// Requires include CoreInterface.h first
#pragma once

namespace ci
{
	// Not threadsafe. SD thread only!
	// You can call only ctor and dtor from other threads
	class InvisibleFox
	{
	public:
		InvisibleFox();

		void SetActive(bool active);
		void UpdateAndMoveTo(const MovementData &movement); // only active fox needs update
		void SetOffsetZ(float offsetZ);
		uint32_t GetFormID() const;

	private:
		void CreateTheFox();
		void DestroyTheFox();

		struct Impl;
		std::shared_ptr<Impl> pimpl;
	};
}