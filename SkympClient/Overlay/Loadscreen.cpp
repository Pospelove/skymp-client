#include "../stdafx.h"
#include "Loadscreen.h"
#include "../DirectX/DirectXHook.h"
#include <D3dx9core.h>

class CSprite 
{
private:
	LPD3DXSPRITE pSprite;
	LPDIRECT3DTEXTURE9 pTexture;
	D3DXIMAGE_INFO tImageInfo;
	UINT WW, WH;
public:
	CSprite(LPDIRECT3DDEVICE9 pD3DDevice, LPCSTR Path, UINT WindowWidth, UINT WindowHeight);
	~CSprite(void);
	void Render(float X, float Y, float Width, float Height, D3DCOLOR Color);
};

CSprite::CSprite(LPDIRECT3DDEVICE9 pD3DDevice, LPCSTR Path, UINT WindowWidth, UINT WindowHeight) 
{
	WW = WindowWidth;
	WH = WindowHeight;
	D3DXGetImageInfoFromFile(Path, &tImageInfo);
	D3DXCreateSprite(pD3DDevice, &pSprite);
	D3DXCreateTextureFromFileEx(pD3DDevice, Path, tImageInfo.Width, tImageInfo.Height,
		tImageInfo.MipLevels, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, D3DCOLOR_XRGB(255, 0, 255),
		NULL, NULL, &pTexture);
}

CSprite::~CSprite(void) 
{
	pSprite->Release();
	pTexture->Release();
	memset(&tImageInfo, 0, sizeof(D3DXIMAGE_INFO));
}

void CSprite::Render(float X, float Y, float Width, float Height, D3DCOLOR Color) 
{
	D3DXMATRIX matXY, matWH, matAll;
	D3DXMatrixTranslation(&matXY, ((float)WW*X) / 100, ((float)WH*Y) / 100, 0.0f);
	D3DXMatrixScaling(&matWH, (((WW*Width) / 100) / tImageInfo.Width), (((WH*Height) / 100) / tImageInfo.Height), 0.0f);
	D3DXMatrixMultiply(&matAll, &matWH, &matXY);
	pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	pSprite->SetTransform(&matAll);
	pSprite->Draw(pTexture, NULL, NULL, NULL, Color);
	pSprite->End();
}

struct Loadscreen::Impl
{
	std::unique_ptr<CSprite> sprite;
};

dlf_mutex gLoadscreenM;
std::set<Loadscreen *> loadscreens;

void Loadscreen::Render()
{
	std::lock_guard<dlf_mutex> l(gLoadscreenM);
	for (auto loadscreen : loadscreens)
	{
		loadscreen->pimpl->sprite->Render(0, 0, 110.0f, 110.0f, -1);
	}
}

Loadscreen::Loadscreen(std::string filePath) : pimpl(new Impl)
{
	const static auto w = GameSettings::GetSkyrimPrefsValue("iSize W");
	const static auto h = GameSettings::GetSkyrimPrefsValue("iSize H");
	pimpl->sprite.reset(
		new CSprite(g_pIDirect3DDevice9, filePath.data(), atoi(w.data()), atoi(h.data()))
	);

	std::lock_guard<dlf_mutex> l(gLoadscreenM);
	loadscreens.insert(this);

	static bool inited = false;
	if (!inited)
	{
		inited = true;
		g_pIDirect3DDevice9->OnPresent.Add([](IDirect3DDevice9 *device) {
			Loadscreen::Render();
		});
	}
}

Loadscreen::~Loadscreen()
{
	std::lock_guard<dlf_mutex> l(gLoadscreenM);
	loadscreens.erase(this);
}