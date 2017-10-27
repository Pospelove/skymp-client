#include "../stdafx.h"
#include "3DText.h"

#include <windows.h>
#include <d3d9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>
#include <d3dx9.h>
#include "Skyrim\Camera\PlayerCamera.h"
#include "../CoreInterface/ciOther.h" //ci::IsInPause()
#include "../DirectX/DirectXHook.h"

#include "../CoreInterface/CoreInterface.h" // ci::LocalPlayer::GetPos()

#pragma comment(lib, "Dwmapi.lib")

class Font
{
public:
	Font(const std::string &name, int32_t h, int32_t fontWeight, const std::string &path = "")
	{
		static dlf_mutex m;
		std::lock_guard<dlf_mutex> l(m);
		if (path.empty() == false)
		{
			static std::set<std::string> loaded;
			const bool inserted = loaded.insert(path).second;
			if (inserted)
			{
				const auto n = AddFontResourceExA(path.data(), 0, NULL);
				if (n != 1)
					ErrorHandling::SendError("ERROR:3DText Failed to load font from '%s'", path.data());
			}
		}
		static std::map<Data, LPD3DXFONT> fonts;
		const Data data{ name, h, fontWeight };
		if (fonts[data] == nullptr)
			D3DXCreateFontA(g_pIDirect3DDevice9, h, 0, fontWeight, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, name.data(),
				&fonts[data]
			);
		this->dxFont = fonts[data];
	}

	operator LPD3DXFONT() const {
		return dxFont;
	}

private:
	LPD3DXFONT dxFont = nullptr;

	struct Data
	{
		std::string name;
		int32_t h;
		int32_t fw;

		friend bool operator==(Data l, Data r) {
			return l.name == r.name && l.h == r.h && l.fw == r.fw;
		}

		friend bool operator<(Data l, Data r) {
			return std::tie(l.name, l.h, l.fw) < std::tie(r.name, r.h, r.fw);
		}
	};
};

std::shared_ptr<Font> GetDefaultFont() {
	const std::shared_ptr<Font> p{ new Font("futuralightc", 20, FW_MEDIUM, "MyGUI\\futuralightc1.otf") };
	return p;
}

std::shared_ptr<Font> Text3DCreateFont(const std::string &name, int32_t h, int32_t fontWeight, const std::string &path) {
	const std::shared_ptr<Font> p{ new Font(name, h, fontWeight, path) };
	return p;
}

void Draw(const Text2DData &text)
{
	RECT FontPos = { text.screenPos.x, text.screenPos.y, text.screenPos.x + 0, text.screenPos.y + 0 };
	if (text.font != nullptr)
	{
		auto textRes = text.str;
		if (text.visible)
			LPD3DXFONT(*text.font)->DrawTextW(NULL, textRes.data(), -1, &FontPos, text.format, text.color);
	}
	else
		ErrorHandling::SendError("ERROR:3DText Null font");
}

std::set<Text3D *> all3DT;
dlf_mutex all3DTMutex;

Text3D::Text3D()
{
	std::lock_guard<dlf_mutex> l(all3DTMutex);
	std::lock_guard<dlf_mutex> l1(this->m);

	static bool inited = false;
	if (!inited)
	{
		Text3D::Init();
		inited = true;
	}
	this->pos = { 0,0,0 };
	this->text2d.str = L"";
	this->text2d.color = -1;
	this->text2d.screenPos = { 0,0 };

	all3DT.insert(this);
}

Text3D::~Text3D()
{
	std::lock_guard<dlf_mutex> l(all3DTMutex);
	all3DT.erase(this);
}

void Text3D::Init()
{
	g_pIDirect3DDevice9->OnPresent.Add([](IDirect3DDevice9 *device) {
		Text3D::Render();
	});
}

void Text3D::Render()
{
	std::lock_guard<dlf_mutex> l(all3DTMutex);
	for (auto text3D : all3DT)
	{
		std::lock_guard<dlf_mutex> l(text3D->m);
		if (!text3D->is2D)
		{
			if (MenuManager::GetSingleton()->IsMenuOpen("Main Menu") || ci::IsInPause() == false)
			{
				NiPoint3 screenPoint;
				PlayerCamera::GetSingleton()->WorldPtToScreenPt3(text3D->pos, screenPoint.x, screenPoint.y, screenPoint.z, 1);

				RECT rect;
				if (GetWindowRect(GetForegroundWindow(), &rect))
				{
					const int32_t width = rect.right - rect.left,
						height = rect.bottom - rect.top;

					if (screenPoint.z >= 0
						&& (text3D->pos - (ci::LocalPlayer::GetSingleton()->GetPos() + NiPoint3{ 0, 0, 128 + 16 })).Length() <= text3D->drawDistance)
					{
						text3D->text2d.screenPos.x = screenPoint.x * width;
						text3D->text2d.screenPos.y = (1 - screenPoint.y) * height;
					}
					else
						text3D->text2d.screenPos = { -1000, -1000 };
					Draw(text3D->text2d);
				}
			}
		}
		else
		{
			if (text3D->pos.x > 0 && text3D->pos.x < 1)
			{
				RECT rect;
				if (GetWindowRect(GetForegroundWindow(), &rect))
				{
					const int32_t width = rect.right - rect.left,
						height = rect.bottom - rect.top;
					text3D->text2d.screenPos.x = text3D->pos.x * width;
					text3D->text2d.screenPos.y = text3D->pos.y *height;
				}
				else
					text3D->text2d.screenPos = { -1000, -1000 };
			}
			else
			{
				text3D->text2d.screenPos.x = text3D->pos.x;
				text3D->text2d.screenPos.y = text3D->pos.y;
			}
			Draw(text3D->text2d);
		}
	}
}