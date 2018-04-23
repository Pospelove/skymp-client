#pragma once

namespace ci
{
	// Not threadsafe
	class Script
	{
	public:
		Script(const std::string &scriptName);
		~Script();

		bool IsValid() const;
		std::string GetLastError() const;

		struct Impl;

	private:
		void Register();

		std::unique_ptr<Impl> pImpl;
	};
}