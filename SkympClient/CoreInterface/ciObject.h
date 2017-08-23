#pragma once

class SkympClientDll;
class ActivateEventSink;

namespace ci
{
	class Object
	{
		friend class ::SkympClientDll;
		friend class ::ActivateEventSink;

	public:
		Object(uint32_t existingReferenceID, uint32_t baseID, uint32_t locationID, NiPoint3 pos, NiPoint3 rot, std::function<void(Object *, bool)> onActivate);
		~Object();
		void SetDestroyed(bool destroyed);
		void SetOpen(bool open);
		void BlockActivation(bool block);
		void SetPosition(NiPoint3 pos);
		void SetAngle(NiPoint3 rot);
		void SetName(const std::wstring &name);
		bool SetLocation(uint32_t locationID);
		void Lock(bool locked);

	private:
		static void UpdateAll();
		static void UpdateAll_OT();
		void Update();
		void Update_OT();
		void ForceSpawn();
		void ForceDespawn(const std::string &reason);
		void Warning(const char *);

		struct Impl;
		Impl *pimpl;
	};
}