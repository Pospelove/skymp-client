#pragma once

#include "../stdafx.h"

#include <MyGUI\MyGUI.h>
#include <MyGUI\MyGUI_DirectXPlatform.h>

#include "..\DirectX\DirectXHook.h"

class GUI
{
	friend class Chat;

public:
	GUI();
	~GUI();
	void OnRender(IDirect3DDevice9* pDevice);
	void OnLostDevice(IDirect3DDevice9* pDevice);
	void SetCursor(bool Visible);
	void InjectKey(unsigned char key, bool isPressed);
	void InjectMouse(unsigned char Key, bool isPressed);
	void MouseMove(unsigned int PositionX, unsigned int PositionY, unsigned int PositionZ);
	void SetRussianLanguage(bool russian);
	bool IsRussianLanguage() const;

private:

	MyGUI::DirectXPlatform* m_pPlatform;
	MyGUI::Gui* myGUI;
	int PositionX, PositionY;
	bool isRussianLanguage = false;
	mutable std::recursive_mutex mutex;
};

extern GUI* TheGUI;