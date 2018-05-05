#pragma once

namespace ci
{
	// Not threadsafe
	class Script
	{
	public:
		struct Impls
		{
			const std::function<void(const char *)> sendServerEvent;
		};

		Script(const std::string &scriptNameOrSrc, const Impls &impls);
		~Script();

		bool IsValid() const;
		std::string GetLastError() const;
		void TriggerEvent(const std::string &luaEventName, const std::string &argumentsTable);

		struct Impl;

	private:
		void Register();

		std::unique_ptr<Impl> pImpl;
	};
}