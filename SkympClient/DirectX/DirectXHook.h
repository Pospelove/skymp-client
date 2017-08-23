// Exported function
//IDirect3D9* WINAPI Direct3DCreate9(UINT SDKVersion);

#include "MyDirect3D9.h"
#include "MyDirect3DDevice9.h"

extern MyDirect3DDevice9* g_pIDirect3DDevice9;
extern MyDirect3D9* g_pIDirect3D9;

// regular functions
void HookDirect3D9();