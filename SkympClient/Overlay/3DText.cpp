#include "../stdafx.h"
#include "3DText.h"

#include <windows.h>
#include <d3d9.h>
#include <iostream>
#include <Dwmapi.h> 
#include <TlHelp32.h>
#include <d3dx9.h>

#pragma comment(lib, "Dwmapi.lib")

#include "../DirectX/DirectXHook.h"


void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt)
{
	RECT FontPos = { x, y, x + 120, y + 16 };
	char buf[1024] = { '\0' };
	va_list va_alist;

	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	g_pFont->DrawText(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}

void Text3D::Init()
{
	g_pIDirect3DDevice9->OnPresent.Add([] (IDirect3DDevice9 *device) {
		Text3D::Render();
	});
}

void Text3D::Render()
{
	static LPD3DXFONT pFont = nullptr;
	if (pFont == nullptr)
	{
		int n = AddFontResourceEx("MyGUI\\futuralightc.otf",0, NULL);
		if (n != 1)
		{
			static bool errLogged = false;
			if (!errLogged)
			{
				errLogged = true;
				ErrorHandling::SendError("ERROR:3DText Failed to load font");
			}
		}
		D3DXCreateFont(g_pIDirect3DDevice9, 20, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "futuralightc", &pFont);
	}
	//DrawString(10, 50, -1, pFont, "Shuraaa");

	//DrawString(10, 100, -1, pFont, "Kuraaa");
}