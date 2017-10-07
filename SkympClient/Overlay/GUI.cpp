#include "GUI.h"
#include "Chat.h"
#include "InputConverter.h"

GUI* TheGUI = nullptr;

GUI::GUI() : m_pPlatform(nullptr), myGUI(nullptr)
{
	std::lock_guard<dlf_mutex> lck(mutex);

	m_pPlatform = new MyGUI::DirectXPlatform();
	m_pPlatform->initialise(g_pIDirect3DDevice9);
	m_pPlatform->getDataManagerPtr()->addResourceLocation(".\\MyGUI\\", false); // Add resource path for MyGUI.
	D3DDISPLAYMODE disp;
	g_pIDirect3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &disp);
	m_pPlatform->getRenderManagerPtr()->setViewSize(disp.Width, disp.Height);
	myGUI = new MyGUI::Gui();
	myGUI->initialise("MyGUI_Core.xml");

	this->SetCursor(false);
}

GUI::~GUI()
{
	std::lock_guard<dlf_mutex> lck(mutex);

	if (this->myGUI != nullptr)
	{
		this->myGUI->shutdown();
		delete this->myGUI;
		this->myGUI = nullptr;
	}

	if (this->m_pPlatform != nullptr)
	{
		this->m_pPlatform->shutdown();
		delete this->m_pPlatform;
		this->m_pPlatform = nullptr;
	}
}

void GUI::OnRender(IDirect3DDevice9* pDevice)
{
	std::lock_guard<dlf_mutex> lck(mutex);

	IDirect3DStateBlock9* pStateBlock = NULL;

	pDevice->CreateStateBlock(D3DSBT_ALL, &pStateBlock);
	pStateBlock->Capture();

	try
	{
		if (this->myGUI && this->m_pPlatform)
			this->m_pPlatform->getRenderManagerPtr()->drawOneFrame();
	}
	catch (...)
	{
	}

	pStateBlock->Apply();
	pStateBlock->Release();
}

void GUI::OnLostDevice(IDirect3DDevice9* pDevice)
{
	std::lock_guard<dlf_mutex> lck(mutex);
	try
	{
		this->m_pPlatform->getRenderManagerPtr()->deviceLost();
	}
	catch (...)
	{
	}

}

void GUI::SetCursor(bool Visible)
{
	std::lock_guard<dlf_mutex> lck(mutex);
	if (this->m_pPlatform && this->myGUI)
		this->myGUI->setVisiblePointer(Visible);
}

class CharsMap : public ICharsMap
{
public: 
	virtual int ToRussian(int c) override
	{
		for (UInt32 i = 0; i != en.size(); ++i)
		{
			if (c == en[i])
				return ru[i];
		}
		return c;
	}

	virtual int ToEnglish(int c) override
	{
		for (UInt32 i = 0; i != ru.size(); ++i)
		{
			if (c == ru[i])
				return en[i];
		}
		return c;
	}

private:
	std::basic_string<WCHAR> ru = L"éöóêåíãøùçõúôûâàïğîëäæıÿ÷ñìèòüáş.ÉÖÓÊÅÍÃØÙÇÕÚÔÛÂÀÏĞÎËÄÆİß×ÑÌÈÒÜÁŞ",
		ruCaps = L"ÉÖÓÊÅÍÃØÙÇÕÚÔÛÂÀÏĞÎËÄÆİß×ÑÌÈÒÜÁŞ,",
		en = L"qwertyuiop[]asdfghjkl;'zxcvbnm,./QWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>";
};

ICharsMap *ICharsMap::GetSingleton()
{
	static CharsMap map;
	return &map;
}

WCHAR toupperUnicode(WCHAR key)
{
	std::locale::global(std::locale(""));  // (*)
	std::wcout.imbue(std::locale());
	auto& f = std::use_facet<std::ctype<wchar_t>>(std::locale());
	std::wstring str;
	str += key;
	f.toupper(&str[0], &str[0] + str.size());
	return str[0];
}

void GUI::InjectKey(unsigned char Key, bool isPressed)
{
	std::lock_guard<dlf_mutex> lck(mutex);

	if (this->m_pPlatform == nullptr || this->myGUI == nullptr)
		return;

	int key = input::ScanCodeToText(Key);

	if (sd::GetKeyPressed(VK_CONTROL))
		return;

	if (TheChat && isPressed && MenuManager::GetSingleton()->IsMenuOpen("Main Menu") == false)
	{
		const auto upArrow = 0xC8, downArrow = 0xD0;
		if (Key == upArrow)
			TheChat->InjectArrowPress(true);
		else if (Key == downArrow)
			TheChat->InjectArrowPress(false);
	}

	switch (Key)
	{
	case MyGUI::KeyCode::Decimal:
		key = '.';
		break;
	case MyGUI::KeyCode::Numpad0:
		key = '0';
		break;
	case MyGUI::KeyCode::Numpad1:
		key = '1';
		break;
	case MyGUI::KeyCode::Numpad2:
		key = '2';
		break;
	case MyGUI::KeyCode::Numpad3:
		key = '3';
		break;
	case MyGUI::KeyCode::Numpad4:
		key = '4';
		break;
	case MyGUI::KeyCode::Numpad5:
		key = '5';
		break;
	case MyGUI::KeyCode::Numpad6:
		key = '6';
		break;
	case MyGUI::KeyCode::Numpad7:
		key = '7';
		break;
	case MyGUI::KeyCode::Numpad8:
		key = '8';
		break;
	case MyGUI::KeyCode::Numpad9:
		key = '9';
		break;
	case MyGUI::KeyCode::Add:
		key = '+';
		break;
	case MyGUI::KeyCode::Subtract:
		key = '-';
		break;
	case MyGUI::KeyCode::Divide:
		key = '/';
		break;
	}

	auto keyboardManager = KeyboardManager::GetSingletone();
	const bool mainMenuOpen = MenuManager::GetSingleton()->IsMenuOpen("Main Menu");
	//const bool shift = sd::GetKeyPressed(VK_LSHIFT) /*|| (mainMenuOpen && keyboardManager->GetKeyPressed(DIK_LSHIFT))*/;
	//const bool shift = ((sd::GetKeyPressed(VK_LSHIFT, TRUE) || mainMenuOpen) && keyboardManager->GetKeyPressed(DIK_LSHIFT));
	const bool shift = (GetAsyncKeyState(VK_LSHIFT) & (1 << 15)) != 0;
	const bool caps = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
	CharsMap map;
	if (isRussianLanguage)
	{
		/*key = map.ToRussian(key);
		if (shift || caps)
			key = toupperUnicode(key);*/

			/*if (key == L'Q')
				key = L'É';
			if (key == L'q')
				key = L'é';*/
	}
	else
	{
		key = map.ToEnglish(key);
		/*if (caps || shift)
			key = toupper(key);
		else
			key = tolower(key);*/
	}

	std::basic_string<WCHAR> nums = L"1234567890", chars = isRussianLanguage ? L"!\"¹;%:?*()" : L"!@#$%^&*()";
	for (UInt32 i = 0; i != nums.size(); ++i)
	{
		if (shift && nums[i] == key)
			key = chars[i];
		if (!shift && chars[i] == key)
			key = nums[i];
	}

	if (isRussianLanguage)
	{
		if (key == L'@')
			key = L'"';
		if (key == L'#')
			key = L'¹';
		if (key == L'$')
			key = L';';
		if (key == L'^')
			key = L':';
		if (key == L'&')
			key = L'?';

		const bool _7pressed = (mainMenuOpen && keyboardManager->GetKeyPressed(DIK_7)) || sd::GetKeyPressed('7');
		const bool _2pressed = (mainMenuOpen && keyboardManager->GetKeyPressed(DIK_2)) || sd::GetKeyPressed('2');
		if (_7pressed == false && key == L'?')
			key = L',';
		if (shift)
		{
			if (key == L'<')
				key = L'Á';
			if (key == L'>')
				key = L'Ş';
			if (key == L'"' && _2pressed == false)
				key = L'İ';
			if (key == L'{')
				key = L'Õ';
			if (key == L'}')
				key = L'Ú';
			if (key == L':')
				key = L'Æ';
			if (key == L'~')
				key = L'¨';
		}
		else
		{
			if (key == L'[')
				key = L'õ';
			if (key == L']')
				key = L'ú';
			if (key == L'`')
				key = L'¸';
			if (key == L'<')
				key = L'á';
			if (key == L'>')
				key = L'ş';
		}
		enum {
			vkOem2 = 53
		};
		if ((mainMenuOpen && keyboardManager->GetKeyPressed(vkOem2)) || sd::GetKeyPressed(VK_OEM_2))
		{
			if (key == L'Ş' || key == L'Á' || key == L'ş' || key == L'á')
				key = shift ? L',' : L'.';
		}
	}

	//if (1 | !IsRussianLanguage()) // PVS
	{
		key = tolower(key);
		key = map.ToEnglish(key);
		if (caps || shift)
			key = toupper(key);
	}

	if (!IsRussianLanguage())
	{
		if (key == '/' && (sd::GetKeyPressed(VK_OEM_PERIOD) || keyboardManager->GetKeyPressed(DIK_PERIOD)))
			key = L'.';
	}

	if (isRussianLanguage)
	{
		key = map.ToRussian(key);
	}

	if ((key == L'4' || key == L'6') && !keyboardManager->GetKeyPressed(DIK_4) && !keyboardManager->GetKeyPressed(DIK_6) && isRussianLanguage)
	{
		key = L'æ';
		if (shift || caps)
			key = toupperUnicode(key);
	}

	if (key == L'İ' && !keyboardManager->GetKeyPressed(0x28) && isRussianLanguage)
		key = shift ? L'"' : L'2';
	if (key == L'"' && keyboardManager->GetKeyPressed(0x28) && isRussianLanguage)
		key = shift || caps ? L'İ' : L'ı';
	enum {
		vkOem2 = 53
	};
	if (key == L'.' && !keyboardManager->GetKeyPressed(0x34) && isRussianLanguage)
		key = shift || caps ? L'Ş' : L'ş';
	if ((key == L',' || key == L'.') && keyboardManager->GetKeyPressed(0x33) && isRussianLanguage)
		key = shift || caps ? L'Á' : L'á';
	if (key == L'á' && !keyboardManager->GetKeyPressed(0x35) && isRussianLanguage)
		key = shift ? L',' : L'.';

	if (key == L',' || key == L'.')
	{
		if (!keyboardManager->GetKeyPressed(0x35) && isRussianLanguage)
		{
			key = L'á';
			if (shift || caps)
				key = toupperUnicode(key);
		}
	}

	if (isRussianLanguage)
	{
		enum f_ck
		{
			u = 52,
			point = 53,
			h = 26,
			tverdiznak = 27,
			zh = 39,
			e = 40,
			yo = 41
		};
		if (Key == f_ck::u)
		{
			key = L'ş';
			if (shift || caps)
				key = L'Ş';
		}
		else if (Key == point)
		{
			key = L'.';
			if (shift)
				key = L',';
		}
		else if (Key == h)
		{
			key = L'õ';
			if (shift || caps)
				key = L'Õ';
		}
		else if (Key == tverdiznak)
		{
			key = L'ú';
			if (shift || caps)
				key = L'Ú';
		}
		else if (Key == zh)
		{
			key = L'æ';
			if (shift || caps)
				key = L'Æ';
		}
		else if (Key == e)
		{
			key = L'ı';
			if (shift || caps)
				key = L'İ';
		}
		else if (Key == yo)
		{
			key = L'¸';
			if (shift || caps)
				key = L'¨';
		}
		//TheChat->AddChatMessage(std::to_string(Key));
	}

	const WCHAR *shiftNum[] = { L"!@#$%^&*()", L"!\"¹;%:?*()" },
		*num[] = { L"1234567890", L"1234567890" };
	if (Key >= 2 && Key <= 11)
	{
		key = (shift ? shiftNum : num)[isRussianLanguage != false][Key - 2];
	}

	/*if (keyboardManager->GetKeyPressed(0x35) && isRussianLanguage)
	{
		if (key == L'Ş')
			key = L',';
		else if (key == L'ş')
			key = L'.';
	}*/

	if (key != '#')
	{
		const MyGUI::KeyCode code = (MyGUI::KeyCode::Enum)Key;
		if (isPressed)
		{
			MyGUI::InputManager::getInstance().injectKeyPress(code, key);
		}
		else
			MyGUI::InputManager::getInstance().injectKeyRelease(code);
	}
}

void GUI::InjectMouse(unsigned char Key, bool isPressed)
{
	std::lock_guard<dlf_mutex> lck(mutex);

	if (this->m_pPlatform == nullptr || this->myGUI == nullptr)
		return;

	MyGUI::MouseButton code((MyGUI::MouseButton::Enum)Key);

	if (isPressed)
		MyGUI::InputManager::getInstance().injectMousePress(this->PositionX, this->PositionY, code);
	else
		MyGUI::InputManager::getInstance().injectMouseRelease(this->PositionX, this->PositionY, code);
}

void GUI::MouseMove(unsigned int PositionX, unsigned int PositionY, unsigned int PositionZ)
{
	std::lock_guard<dlf_mutex> lck(mutex);

	if (this->m_pPlatform == nullptr || this->myGUI == nullptr)
		return;

	this->PositionX = std::min<int>(PositionX, this->myGUI->getViewSize().width);
	this->PositionY = std::min<int>(PositionY, this->myGUI->getViewSize().height);
	MyGUI::InputManager::getInstance().injectMouseMove(PositionX, PositionY, PositionZ);
}

void GUI::SetRussianLanguage(bool russian)
{
	std::lock_guard<dlf_mutex> lck(mutex);
	this->isRussianLanguage = russian;
}

bool GUI::IsRussianLanguage() const
{
	std::lock_guard<dlf_mutex> lck(mutex);
	return this->isRussianLanguage;
}

int32_t GUI::ScanCodeToText(int32_t v)
{
	return input::ScanCodeToText(v);
}