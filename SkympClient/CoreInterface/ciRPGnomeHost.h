// Requires include CoreInterface.h first
#pragma once

namespace ci
{
	// Not threadsafe. SD thread only!
	// You can call only only ctor and dtor from other threads
	class GnomeHost
	{
	public:
		GnomeHost();
		void SetActive(bool active);
		bool IsGnomeExist(int32_t handID);
		uint32_t GetGnomeFormID(int32_t handID);
		void UpdateAndMoveTo(Actor *actor, const MovementData &movement, std::array<bool, 2> needsGnome);
		void SetName(const std::wstring &name);
		void DoCombatSpellApply(uint32_t spellID, int32_t handID, uint32_t foxID);

	private:
		void Create();
		void Destroy();

		struct Impl;
		std::shared_ptr<Impl> pimpl;
	};
}