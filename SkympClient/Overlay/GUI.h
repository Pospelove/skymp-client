#pragma once

#include "../stdafx.h"

#include <MyGUI\MyGUI.h>
#include <MyGUI\MyGUI_DirectXPlatform.h>

#include "..\DirectX\DirectXHook.h"

class ICharsMap
{
public:
	virtual int ToRussian(int c) = 0;
	virtual int ToEnglish(int c) = 0;
	static ICharsMap *GetSingleton();
};

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
	static int32_t ScanCodeToText(int32_t c);

private:

	MyGUI::DirectXPlatform* m_pPlatform;
	MyGUI::Gui* myGUI;
	int PositionX = 0, PositionY = 0;
	bool isRussianLanguage = false;
	mutable dlf_mutex mutex;
};

extern GUI* TheGUI;