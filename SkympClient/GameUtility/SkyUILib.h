#pragma once

namespace SkyUILib
{
	struct DialogResult
	{
		std::string inputText;
		int32_t listItem;
	};

	using Callback = std::function<void(DialogResult)>;

	enum
	{
		DIALOG_STYLE_MSGBOX = 0,
		DIALOG_STYLE_INPUT = 1,
		DIALOG_STYLE_LIST = 2,
	};

	void ShowPlayerDialog(const std::wstring &title, int32_t dialogStyle, const std::wstring &text, int32_t defaultIndex, Callback callback);
	bool HideDialog();
}