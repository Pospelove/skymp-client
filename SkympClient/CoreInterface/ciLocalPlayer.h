#pragma once

class SkympClientDll;
namespace ci
{
	class LocalPlayer : public IActor
	{
		friend class ::SkympClientDll;
	public:
		static LocalPlayer *GetSingleton();

		void SetName(const std::wstring &name) override;
		void SetPos(NiPoint3 pos) override;
		void SetAngleZ(float angle) override;
		void SetCell(uint32_t cellID) override;
		void SetWorldSpace(uint32_t worldSpaceID) override;
		void ApplyLookData(const LookData &lookData) override;
		void ApplyMovementData(const MovementData &movementData) override;
		std::wstring GetName() const override;
		NiPoint3 GetPos() const override;
		float GetAngleZ() const override;
		uint32_t GetCell() const override;
		uint32_t GetWorldSpace() const override;
		LookData GetLookData() const override;
		MovementData GetMovementData() const override;

		virtual ~LocalPlayer() {}

	private:
		void DoTeleport_OT(bool ot = true);
		void Update();
		LocalPlayer() {}
	};
}