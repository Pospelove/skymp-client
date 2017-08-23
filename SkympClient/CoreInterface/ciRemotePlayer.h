#pragma once

class SkympClientDll;
class TESNPC;

namespace ci
{
	class RemotePlayer : public IActor
	{
		friend class ::SkympClientDll;
	public:
		RemotePlayer(
			const std::wstring &name,
			const LookData &lookData,
			NiPoint3 spawnPoint,
			uint32_t cellID,
			uint32_t worldSpaceID);

		~RemotePlayer();

		void SetName(const std::wstring &name) override;
		void SetPos(NiPoint3 pos) override;
		void SetAngleZ(float angle) override;
		void SetCell(uint32_t cellID) override;
		void SetWorldSpace(uint32_t worldSpaceID) override;
		void ApplyLookData(const LookData &lookData) override;
		void ApplyMovementData(const MovementData &movementData) override;
		std::wstring GetName() const override;
		NiPoint3 GetPos() const  override;
		float GetAngleZ() const override;
		uint32_t GetCell() const override;
		uint32_t GetWorldSpace() const override;
		LookData GetLookData() const override;
		MovementData GetMovementData() const override;

		void SetInAFK(bool val);

	private:
		static void UpdateAll();
		static void UpdateAll_OT();
		void Update();
		void Update_OT();
		void ForceSpawn(uint32_t npcID);
		void ForceDespawn(const wchar_t *reason = L"");

	protected:

		virtual TESNPC *GetNPC() const;
		virtual void ApplyMovementDataImpl();

		struct Impl;
		std::unique_ptr<Impl> pimpl;
	};
}