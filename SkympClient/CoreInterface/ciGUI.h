#pragma once
#include <string>
#include <ostream>

namespace ci
{
	namespace Chat
	{
		void Init();
		void AddMessage(const std::wstring &message, bool isNotification = false);
	}
}