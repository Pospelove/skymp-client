#include "../stdafx.h"
#include "Input.h"
#include "GUI.h"
#include "Chat.h"

InputListener* TheInputManager = nullptr;

void InputManager::OnPress(BYTE code)
{
	if (!TheChat || !TheGUI)
		return;

	const uint8_t key = code;

	enum { 
		dikOem2 = 53 // /,?
	};
	auto keyboardManager = KeyboardManager::GetSingletone();

	if (code == DIK_1)
	{
		keybd_event(VK_END, DIK_END, NULL, NULL);
		keybd_event(VK_END, DIK_END, KEYEVENTF_KEYUP, NULL);
	}

	if (MyGUI::InputManager::getInstance().isFocusKey() == false && (key == dikOem2 || key == DIK_T || key == DIK_F6) // Right CTRL is pressend, give focus to text box.
		&& MenuManager::GetSingleton()->IsMenuOpen("Journal Menu") == false && MenuManager::GetSingleton()->IsMenuOpen("Console") == false)
	{
		TheChat->SetTyping(false);
		TheIInputHook->SetInputEnabled(false);
	}
	else if (key == DIK_ESCAPE || key == DIK_F6) // Escape is pressed, reset focus of text box.
	{
		TheChat->SetTyping(true);
		TheIInputHook->SetInputEnabled(true);
	}
	else if ((key == DIK_RETURN || key == DIK_NUMPADENTER) && TheChat->IsTyping()) // Enter is pressed, send the message and reset focus of text box.
	{
		TheChat->SendChatMessage();
		TheChat->SetTyping(true);
		TheIInputHook->SetInputEnabled(true);
	}
	else if (key == DIK_F8)
	{
		TheChat->SetVisible(!TheChat->IsVisible());
	}
	else if (key != DIK_RCONTROL && key != DIK_LCONTROL)
	{
		TheGUI->InjectKey(key, true);
	}
}

void InputManager::OnRelease(BYTE code)
{
	if (!TheGUI)
		return;

	uint8_t key = code;
	if (key != DIK_RCONTROL && key != DIK_LCONTROL)
	{
		TheGUI->InjectKey(key, false);
	}
}

void InputManager::OnMousePress(BYTE code)
{
	if (!TheGUI)
		return;

	TheGUI->InjectMouse((uint8_t)code, true);
}

void InputManager::OnMouseRelease(BYTE code)
{
	if (!TheGUI)
		return;

	TheGUI->InjectMouse((uint8_t)code, false);
}

void InputManager::OnMouseMove(unsigned int x, unsigned int y, unsigned int z)
{
	if (!TheGUI)
		return;

	TheGUI->MouseMove(x, y, z);
}