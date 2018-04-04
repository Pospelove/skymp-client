#include "Chat.h"
#include "GUI.h"
#include <MyGUI/MyGUI_Precompiled.h>
#include <atlstr.h>
#include "../Common/clip.h"
#include "SKSE/GameSettings.h"
#include "SKSE/GameInput.h"

// Chat will be invisible in menus
std::vector<BSFixedString> menusWithChatDisabled =
{
	//"CustomMenu",
	"BarterMenu",
	"Book Menu",
	"Console",
	"Console Native UI Menu",
	"ContainerMenu",
	"Crafting Menu",
	"FavoritesMenu",
	"GiftMenu",
	"InventoryMenu",
	"Journal Menu",
	"LevelUp Menu",
	//"Loading Menu",
	"Lockpicking Menu",
	"MagicMenu",
	"MapMenu",
	"MessageBoxMenu",
	"RaceSex Menu",
	"Sleep / Wait Menu",
	"StatsMenu",
	"Training Menu",
	"TweenMenu",
};

Chat *TheChat = nullptr;

const auto cBlack = MyGUI::Colour(0.0, 0.0, 0.0, 1.0);

struct
{
	int32_t normal;
	int32_t over9000;
} chatSizes;

Chat::Chat()
{
	if (!TheGUI)
		return;

	mutex = decltype(mutex)(&TheGUI->mutex);
	std::lock_guard<dlf_mutex> lck(*mutex);

	m_pGUI = TheGUI->myGUI;

	MyGUI::LayoutManager::getInstance().loadLayout("Chat.xml");

	m_pEdit = m_pGUI->findWidget<MyGUI::EditBox>("Chat_Edit");
	m_pList = m_pGUI->findWidget<MyGUI::EditBox>("Chat_List");

	m_pList->setTextAlign(MyGUI::Align::Default);
	m_pList->setEditStatic(true);
	m_pList->setVisibleHScroll(true);
	m_pList->setVisibleVScroll(true);
	m_pList->setOverflowToTheLeft(true);
	m_pList->setEditWordWrap(true);
	m_pList->setTextShadow(true);

	auto pos = m_pList->getPosition();
	pos.top -= 100;
	m_pList->setPosition(pos);

	auto size = m_pList->getSize();

	chatSizes = { -1, -1 };
	chatSizes.normal = size.width + 100;
	chatSizes.over9000 = size.width + 10000;
	size.width = chatSizes.over9000;

	size.height = 300;
	m_pList->setSize(size);
	m_pList->setTextShadowColour(cBlack);

	m_pEdit->eventEditTextChange += MyGUI::newDelegate(this, &Chat::EditKeyPressEvent);

	const auto cGrey = MyGUI::Colour(0xBE / 256.0, 0xBE / 256.0, 0xBE / 256.0, 1.0);
	m_pEdit->setTextSelectionColour(cGrey);
}

Chat::~Chat()
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);
	m_pEdit->eventEditTextChange -= MyGUI::newDelegate(this, &Chat::EditKeyPressEvent);
}

void Chat::SetVisible(bool v)
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);
	if (m_pEdit == nullptr || m_pList == nullptr)
		return;

	bool isOpen = false;
	for (auto it = menusWithChatDisabled.begin(); it != menusWithChatDisabled.end(); ++it)
		isOpen = isOpen || MenuManager::GetSingleton()->IsMenuOpen(*it);
	if (isOpen)
		v = false;

	m_pList->setVisible(v);

	if (!v)
	{
		bool hide = true;
		if (this->IsTyping())
			this->SetTyping(hide);
	}
}


void Chat::SetTyping(bool aForceHide)
{
	if (!TheGUI)
		return;

	bool isOpen = false;
	for (auto it = menusWithChatDisabled.begin(); it != menusWithChatDisabled.end(); ++it)
		isOpen = isOpen || MenuManager::GetSingleton()->IsMenuOpen(*it);
	if (isOpen)
		aForceHide = true;

	std::lock_guard<dlf_mutex> lck(*mutex);

	auto menuManager = MenuManager::GetSingleton();

	if (aForceHide == false /*&& MyGUI::InputManager::getInstance().isFocusKey() == false*/)
	{
		TheGUI->SetCursor(true);
		MyGUI::InputManager::getInstance().setKeyFocusWidget(m_pEdit);
		m_pEdit->setAlpha(0.81);
		m_pEdit->setColour(cBlack);
	}
	else
	{
		TheGUI->SetCursor(false);
		MyGUI::InputManager::getInstance().resetKeyFocusWidget();
		m_pEdit->setOnlyText("");
		m_pEdit->setAlpha(0.00);
		needCloseEscMenu++;
	}

	static SInt8 lastForceHide = -1;
	const SInt8 nowForceHide = aForceHide ? 1 : 0;
	if (nowForceHide != lastForceHide)
	{
		lastForceHide = nowForceHide;
		PlayerControls_::SetEnabled(Control::Movement, aForceHide);
		PlayerControls_::SetEnabled(Control::CamSwitch, aForceHide);
		PlayerControls_::SetEnabled(Control::Looking, aForceHide); 
		PlayerControls_::SetEnabled(Control::Activate, aForceHide);
		PlayerControls_::SetEnabled(Control::Console, aForceHide);

		auto size = m_pList->getSize();
		size.width = aForceHide ? chatSizes.over9000 : chatSizes.normal;
		m_pList->setSize(size);
	}
}

bool Chat::IsTyping() const
{
	if (!TheGUI)
		return false;
	std::lock_guard<dlf_mutex> lck(*mutex);
	return MyGUI::InputManager::getInstance().isFocusKey();
}

bool Chat::IsVisible() const
{
	if (!TheGUI)
		return false;
	std::lock_guard<dlf_mutex> lck(*mutex);
	return /*m_pEdit->isVisible() &&*/ m_pList->isVisible();
}

void Chat::AddChatMessage(const MyGUI::UString& acString)
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);

	m_scrollBarPosition[0] = m_pList->getVScrollRange();
	m_scrollBarPosition[1] = m_pList->getVScrollPosition();

	static size_t numChars = 0;

	if (acString != " ")
	{
		auto msg = "#FFFFFF" + acString + '\n';
		numChars += msg.size();
		m_pList->insertText(msg);
	}
	else
	{
		m_pList->eraseText(0, numChars);
		numChars = 0;
	}

	m_chatList.push_back(acString);

	if (m_pList->getVScrollPosition() - m_scrollBarPosition[1] > m_pList->getVScrollRange() - m_scrollBarPosition[0])
		m_pList->setVScrollPosition(m_scrollBarPosition[1]);
}

void Chat::SendChatMessage()
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);

	const auto len = m_pEdit->getTextLength();
	if (len > 0)
	{
		const auto message = m_pEdit->getOnlyText();
		m_myMessages.push_back(message);
		offset = 0;

		//
		// SHOULD SEND CHAT MESSAGE HERE
		//

		m_pEdit->eraseText(0, len);
	}
}

void Chat::EditKeyPressEvent(MyGUI::EditBox* aSender)
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);
	if (aSender->getTextLength() > 256)
		aSender->eraseText(aSender->getTextLength() - 1, 1);
}

void Chat::InjectArrowPress(bool isUp)
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);

	const auto msgCount = m_myMessages.size();

	if (IsTyping())
	{
		const auto lastOffsetVal = offset;
		if (isUp)
		{
			++offset;
			if (offset > msgCount)
				offset = msgCount;
		}
		else
		{
			--offset;
			if (offset < 0)
				offset = 0;
		}

		if (offset > 0)
		{
			const auto realIdx = msgCount - offset;
			m_pEdit->setOnlyText("");
			m_pEdit->insertText(m_myMessages[realIdx]);
		}
		else
		{
			if (offset != lastOffsetVal)
				m_pEdit->setOnlyText("");
		}
	}
}

void Chat::Cut()
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);

	Copy();
	m_pEdit->eraseText(m_pEdit->getTextSelectionStart(), m_pEdit->getTextSelectionLength());
}

void Chat::Copy()
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);

	MyGUI::UString src = m_pEdit->getSelectedText();

	auto utf8 = std::string(src);
	size_t pos = ~0;
	while ((pos = utf8.find('#')) != utf8.npos)
		utf8.erase(pos, 7);

	clip::set_text(utf8);
}

void Chat::Paste()
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);

	std::string txt;
	clip::get_text(txt);

	const auto len = m_pEdit->getTextSelectionLength();
	if (len)
	{
		const auto pos = m_pEdit->getTextSelectionStart();
		m_pEdit->deleteTextSelection();
		m_pEdit->insertText(txt, pos);
	}
	else
		m_pEdit->insertText(txt, m_pEdit->getTextCursor());
}

void Chat::ToggleScroll(bool show)
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);
	m_pList->showVScroll(show);
}

void Chat::AddToEditBox(const MyGUI::UString &text)
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);
	if (IsTyping())
	{
		m_pEdit->addText(text);
	}
}

void Chat::Update()
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);
}

void Chat::SetRussianUser(bool ru)
{
	if (isRussianUserPtr)
		delete isRussianUserPtr;
	isRussianUserPtr = new bool(ru);
}

void Chat::Update_OT()
{
	if (!TheGUI)
		return;
	std::lock_guard<dlf_mutex> lck(*mutex);

	bool isOpen = false;
	for (auto it = menusWithChatDisabled.begin(); it != menusWithChatDisabled.end(); ++it)
		isOpen = isOpen || MenuManager::GetSingleton()->IsMenuOpen(*it);
	static bool wasOpen = false;
	if (isOpen != wasOpen)
	{
		wasOpen = isOpen;
		SetVisible(!isOpen);
	}

	// Detect if system language is Russian
	if (isRussianUserPtr == nullptr)
	{
#ifdef DETECT_RUSSIAN_USER
		WCHAR localeName[8] = { 0 };
		GetUserDefaultLocaleName(localeName, 8);
		if (std::wstring(localeName, 5) == L"ru-RU")
			isRussianUserPtr = new bool(true);
		else
			isRussianUserPtr = new bool(false);
#else
		isRussianUserPtr = new bool(false);
#endif
	}

	if (isRussianUserPtr != nullptr &&
		*isRussianUserPtr == true)
		this->isRussianUser = true;
	else
		this->isRussianUser = false;

	// Language switch (Shift + Ctrl || Shift + Alt)
	// Only for Russian-speaking users
	if (isRussianUserPtr != nullptr &&
		*isRussianUserPtr == true)
	{
		const bool isRussianLang = TheGUI->IsRussianLanguage();
		const bool shiftState = (GetKeyState(VK_LSHIFT) & 0x0001) != 0,
			altState = (GetKeyState(VK_MENU) & 0x0001) != 0,
			ctrlState = (GetKeyState(VK_CONTROL) & 0x0001) != 0;
		static bool shiftStateOld = false, altStateOld = false, ctrlStateOld = false;
		static UInt64 shiftStateChangeTime = NULL, altStateChangeTime = NULL, ctrlStateChangeTime = NULL;
		bool shiftStateChanged = false, altStateChanged = false, ctrlStateChanged = false;
		if (shiftStateOld != shiftState)
		{
			shiftStateChanged = true;
			shiftStateOld = shiftState;
			shiftStateChangeTime = clock();
		}
		if (altStateOld != altState)
		{
			altStateChanged = true;
			altStateOld = altState;
			altStateChangeTime = clock();
		}
		if (ctrlStateOld != ctrlState)
		{
			ctrlStateChanged = true;
			ctrlStateOld = ctrlState;
			ctrlStateChangeTime = clock();
		}
		if (shiftStateChangeTime && (ctrlStateChangeTime || altStateChangeTime))
		{
			const clock_t possibleDelay = 333;
			if ((shiftStateChangeTime > ctrlStateChangeTime - possibleDelay / 2 && shiftStateChangeTime < ctrlStateChangeTime + possibleDelay / 2)
				|| (shiftStateChangeTime > altStateChangeTime - possibleDelay / 2 && shiftStateChangeTime < altStateChangeTime + possibleDelay / 2))
				TheGUI->SetRussianLanguage(!isRussianLang);
			shiftStateChangeTime = ctrlStateChangeTime = altStateChangeTime = NULL;
		}
		const bool combIsDown = sd::GetKeyPressed(VK_LSHIFT) && (sd::GetKeyPressed(VK_MENU) || sd::GetKeyPressed(VK_CONTROL));
		static bool combWasDown = false;
		if (combIsDown != combWasDown)
		{
			if ((combWasDown = combIsDown) == true)
				TheGUI->SetRussianLanguage(!isRussianLang);
		}
	}
	else TheGUI->SetRussianLanguage(false);

	// Ctrl+C, Ctrl+V, Ctrl+X
	const bool ctrl = sd::GetKeyPressed(VK_CONTROL);
	const bool c = sd::GetKeyPressed('C'),
		x = sd::GetKeyPressed('X'),
		v = sd::GetKeyPressed('V');
	static bool cWas = false, xWas = false, vWas = false;
	if (c != cWas)
	{
		cWas = c;
		if (ctrl && c && TheChat && TheChat->IsTyping())
			TheChat->Copy();
	}
	else if (v != vWas)
	{
		vWas = v;
		if (ctrl && v && TheChat && TheChat->IsTyping())
			TheChat->Paste();
	}
	else if (x != xWas)
	{
		xWas = x;
		if (ctrl && x && TheChat && TheChat->IsTyping())
			TheChat->Cut();
	}

}

std::vector<std::wstring> Chat::GetMyMessages() const
{
	if (!TheGUI)
		return std::vector<std::wstring>();
	std::lock_guard<dlf_mutex> lck(*mutex);
	return m_myMessages;
}