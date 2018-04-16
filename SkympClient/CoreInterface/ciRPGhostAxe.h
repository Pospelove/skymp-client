// Requires include CoreInterface.h first
#pragma once

namespace ci
{
	// Isn't thread safe
	class GhostAxeManager
	{
	public:
		static GhostAxeManager &GetSingleton();
		GhostAxeManager();
		~GhostAxeManager();

		uint32_t GetGhostAxeFormID() const;
		void Update();

	private:
		static RemotePlayer *CreateGhostAxe();
		struct Impl;
		Impl *const pimpl;
	};
}