#pragma once
#include <memory>
#include <string>

namespace ci
{
	class IActor
	{
	public:
		virtual void SetName(const std::wstring &name) = 0;
		virtual void SetPos(NiPoint3 pos) = 0;
		virtual void SetAngleZ(float angle) = 0;
		virtual void SetCell(uint32_t cellID) = 0;
		virtual void SetWorldSpace(uint32_t worldSpaceID) = 0;
		virtual void ApplyLookData(const LookData &lookData) = 0;
		virtual void ApplyMovementData(const MovementData &movementData) = 0;
		virtual std::wstring GetName() const = 0;
		virtual NiPoint3 GetPos() const = 0;
		virtual float GetAngleZ() const = 0;
		virtual uint32_t GetCell() const = 0;
		virtual uint32_t GetWorldSpace() const = 0;
		virtual LookData GetLookData() const = 0;
		virtual MovementData GetMovementData() const = 0;

		virtual ~IActor() {}
	};
}