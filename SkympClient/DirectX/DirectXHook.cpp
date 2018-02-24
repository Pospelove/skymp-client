#include "../stdafx.h"
#include "DirectXHook.h"
#include "MyDirect3D9.h"
#include "../Mhook/mhook.h"

MyDirect3DDevice9* g_pIDirect3DDevice9;
MyDirect3D9* g_pIDirect3D9;
typedef IDirect3D9* (WINAPI *tDirect3DCreate9)(UINT);
tDirect3DCreate9 Direct3DCreate9_RealFunc;

IDirect3D9* WINAPI FakeDirect3DCreate9(UINT SDKVersion)
{
	IDirect3D9 *pIDirect3D9 = Direct3DCreate9_RealFunc(SDKVersion);
	g_pIDirect3D9 = new MyDirect3D9(pIDirect3D9);

	return (g_pIDirect3D9);
}

void HookDirect3D9()
{
	std::ofstream f("loadlibrary.log");
	f << "d3d9.dll begin" << std::endl;
	HMODULE d3d9 = LoadLibraryA("d3d9.dll");
	if (!d3d9)
	{
		std::ofstream f("ERROR_126_DETECTED.txt");
		f << "d3d9.dll" << std::endl;
		f.close();
		return;
	}
	Direct3DCreate9_RealFunc = (tDirect3DCreate9)GetProcAddress(d3d9, "Direct3DCreate9");
	Mhook_SetHook((PVOID*)&Direct3DCreate9_RealFunc, FakeDirect3DCreate9);
	f << "d3d9.dll end" << std::endl;
}