#pragma once

class SimpleRef
{
public:
	SimpleRef(TESForm *baseObject, NiPoint3 pos, float drawDistance, cd::Value<TESObjectREFR> spawner = g_thePlayer);
	~SimpleRef();

	void SetPos(const NiPoint3 &pos);
	NiPoint3 GetPos() const;
	void SetRot(const NiPoint3 &rot);
	NiPoint3 GetRot() const;
	uint32_t GetFormID() const;

	void TaskPersist(std::function<void(TESObjectREFR *)> f) {
		return this->Task(f, true);
	}

	void TaskSingle(std::function<void(TESObjectREFR *)> f) {
		return this->Task(f, false);
	}

private:
	void Update();
	void Task(std::function<void(TESObjectREFR *)> f, bool persist);
	static void UpdateAll();

	struct Impl;
	std::unique_ptr<Impl> pimpl;
};