#pragma once
#include <string>
#include <ostream>
#include <functional>

namespace ci
{
	namespace Chat
	{
		void Init();
		void AddMessage(const std::wstring &message, bool isNotification = false);
	}

	namespace Dialog
	{
		struct Result
		{
			std::wstring inputText;
			int32_t listItem;
		};

		enum class Style : uint8_t
		{
			Message = 0,
			Input = 1,
			List = 2,
		};

		using Callback = std::function<void(Result)>;

		void Show(const std::wstring &title, Style dialogStyle, const std::wstring &text, int32_t defaultIndex, Callback callback);
		bool Hide();
	}
}