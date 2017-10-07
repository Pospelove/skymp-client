#include "../stdafx.h"
#include "../Overlay/GUI.h"
#include "../Overlay/Chat.h"
#include "CoreInterface.h"

void ci::Chat::Init()
{
	if (!TheGUI)
	{
		TheGUI = new GUI;
		auto &onPresent = g_pIDirect3DDevice9->OnPresent;
		onPresent.Add(std::bind(&GUI::OnRender, TheGUI, std::placeholders::_1));
		auto &onReset = g_pIDirect3DDevice9->OnReset;
		onReset.Add(std::bind(&GUI::OnLostDevice, TheGUI, std::placeholders::_1));
	}
	if (!TheChat)
		TheChat = new ::Chat;
}

void ci::Chat::AddMessage(const std::wstring &message, bool isNotification)
{
	if (message.empty())
		return;

	if (isNotification)
	{
		SET_TIMER(0, [=] {
			sd::PrintNote((char *)WstringToString(message).data());
		});
		return;
	}

	try
	{
		if (TheChat)
			TheChat->AddChatMessage(message);
	}
	catch (...)
	{
	}
}

void ci::Dialog::Show(const std::wstring &title, Style dialogStyle, const std::wstring &text, int32_t defaultIndex, Callback callback)
{
	SkyUILib::ShowPlayerDialog(title, (int32_t)dialogStyle, text, defaultIndex, [=](SkyUILib::DialogResult result) {
		ci::Dialog::Result ciResult;
		ciResult.inputText = StringToWstring(result.inputText);
		ciResult.listItem = result.listItem;
		return callback(ciResult);
	});
}

bool ci::Dialog::Hide()
{
	return SkyUILib::HideDialog();
}