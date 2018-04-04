#pragma once

#include "../stdafx.h"

#include <MyGUI/MyGUI.h>
#include <MyGUI/MyGUI_DirectXPlatform.h>

class Chat
{
public:

	Chat();

	~Chat();

	void SetVisible(bool aHide);
	void SetTyping(bool aForceHide = false);
	bool IsTyping() const;
	bool IsVisible() const;
	void AddChatMessage(const MyGUI::UString& acString);
	void SendChatMessage();
	void InjectArrowPress(bool isUp);
	void Copy();
	void Cut();
	void Paste();
	void ToggleScroll(bool show);
	void AddToEditBox(const MyGUI::UString &text);
	std::vector<std::wstring> GetMyMessages() const;
	bool IsRussianUser() const {
		std::lock_guard<dlf_mutex> l(*this->mutex);
		return this->isRussianUser;
	}
	void SetRussianUser(bool ru);

	void Update();
	void Update_OT();

private:
	MyGUI::Gui* m_pGUI;
	MyGUI::EditBox* m_pList;
	MyGUI::EditBox* m_pEdit;
	size_t m_scrollBarPosition[2];
	std::list<std::wstring> m_chatList; // To keep number of messages and text to get length of the first message.
	std::vector<std::wstring> m_myMessages;
	dlf_mutex *mutex;
	SInt16 offset = 0;
	SInt16 needCloseEscMenu = 0;
	bool isRussianUser = false;
	bool *isRussianUserPtr = nullptr;

	void EditKeyPressEvent(MyGUI::EditBox* aSender);
};

extern Chat* TheChat;