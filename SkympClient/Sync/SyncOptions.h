#pragma once

class SyncOptions
{
public:
	static SyncOptions *GetSingleton();
	int32_t GetInt(std::string optionName) const;
	float GetFloat(std::string optionName) const;
	std::string GetString(std::string optionName) const;

private:
	struct Impl;
	Impl *const pimpl;
	SyncOptions();
};