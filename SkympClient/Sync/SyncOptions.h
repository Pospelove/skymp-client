#pragma once

class SyncOptions
{
public:
	static SyncOptions *GetSingleton();
	int32_t GetInt(const char *optionName) const;
	float GetFloat(const char *optionName) const;
	std::string GetString(const char *optionName) const;

private:
	struct Impl;
	Impl *const pimpl;
	SyncOptions();
};