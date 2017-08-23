#include "../stdafx.h"
#include "KeyboardManager.h"
#include "../Overlay/Chat.h"

KeyboardManager *KeyboardManager::GetSingletone()
{
	static KeyboardManager keyboardManager;
	return &keyboardManager;
}

bool KeyboardManager::GetKeyPressed(uint8_t code) const
{
	std::lock_guard<std::recursive_mutex> l(mutex);
	return pressed.find(code) != pressed.end();
}

void KeyboardManager::OnPress(uint8_t code)
{
	std::lock_guard<std::recursive_mutex> l(mutex);
	pressed.insert(code);
}

void KeyboardManager::OnRelease(uint8_t code)
{
	std::lock_guard<std::recursive_mutex> l(mutex);
	if (pressed.find(code) != pressed.end())
		pressed.erase(code);
}

void KeyboardManager::OnMousePress(uint8_t code)
{
}

void KeyboardManager::OnMouseRelease(uint8_t code)
{
}

void KeyboardManager::OnMouseMove(unsigned int x, unsigned int y, unsigned int z)
{
}