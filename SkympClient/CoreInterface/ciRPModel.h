// Requires include CoreInterface.h first
#pragma once

// TODO: TintMasks apply

namespace ci
{
	using TintMasks = decltype(LookData::tintmasks);
	using NPCSource = std::function<TESNPC *(void)>;

	// threadsafe
	class Model
	{
	public:
		Model();
		~Model();

		bool IsSpawned() const;
		Actor *GetActor() const;
		clock_t GetSpawnMoment() const;

		void SetSpawnPoint(const NiPoint3 &spawnPoint);
		void SetNPCSource(NPCSource f);
		void SetTints(const TintMasks &tints);
		void SetName(const std::wstring &name);
		void Despawn(const std::wstring &reason);
		void SetSpawnCondition(std::function<bool()> condition);

		void Update();

	private:
		void SpawnActor();
		void ForgetActor();

		struct Impl;
		std::unique_ptr<Impl> pimpl;
	};
}