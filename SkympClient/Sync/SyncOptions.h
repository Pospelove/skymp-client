#pragma once

class SyncOptions
{
public:
	static SyncOptions *GetSingleton();
	int32_t GetInt(std::string optionName) const;
	float GetFloat(std::string optionName) const;
	std::string GetString(std::string optionName) const;
	void Set(std::string optionName, std::string value);

	static float GetRespawnRadius(bool isInterior) {
		return SyncOptions::GetSingleton()->GetFloat("DRAW_DISTANCE");
	}

private:
	struct Impl;
	Impl *const pimpl;
	SyncOptions();
};