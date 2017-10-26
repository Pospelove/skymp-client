#include "../stdafx.h"
#include "../Overlay/GUI.h"
#include "../Overlay/Chat.h"
#include "../Overlay/3DText.h"
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

struct ci::Text3D::Impl
{
	::Text3D label;
};

ci::Text3D::Text3D(const std::wstring &str, NiPoint3 pos) :
	pimpl(new Impl)
{
	std::lock_guard<dlf_mutex> l(pimpl->label.m);
	pimpl->label.is2D = false;
	pimpl->label.pos = pos;
	pimpl->label.text2d.str = str;
	pimpl->label.text2d.color = -1;
}

ci::Text3D::~Text3D()
{
	delete pimpl;
}

void ci::Text3D::SetText(const std::wstring &str)
{
	std::lock_guard<dlf_mutex> l(pimpl->label.m);
	pimpl->label.text2d.str = str;
}

void ci::Text3D::SetPos(const NiPoint3 &pos)
{
	std::lock_guard<dlf_mutex> l(pimpl->label.m);
	pimpl->label.pos = pos;
}

void ci::Text3D::SetVisible(bool v)
{
	std::lock_guard<dlf_mutex> l(pimpl->label.m);
	pimpl->label.text2d.visible = v;
}

void ci::Text3D::SetDrawDistance(float d)
{
	std::lock_guard<dlf_mutex> l(pimpl->label.m);
	pimpl->label.drawDistance = d;
}

std::wstring ci::Text3D::GetText() const
{
	std::lock_guard<dlf_mutex> l(pimpl->label.m);
	return pimpl->label.text2d.str;
}

NiPoint3 ci::Text3D::GetPos() const
{
	std::lock_guard<dlf_mutex> l(pimpl->label.m);
	return pimpl->label.pos;
}

bool ci::Text3D::IsVisible() const
{
	std::lock_guard<dlf_mutex> l(pimpl->label.m);
	return pimpl->label.text2d.visible;
}

float ci::Text3D::GetDrawDistance() const
{
	std::lock_guard<dlf_mutex> l(pimpl->label.m);
	return pimpl->label.drawDistance;
}