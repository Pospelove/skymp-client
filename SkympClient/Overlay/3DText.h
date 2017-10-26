#pragma once

struct Text2DData;
class Font;

void Draw(const Text2DData &text2d);
std::shared_ptr<Font> GetDefaultFont();
std::shared_ptr<Font> Text3DCreateFont(const std::string &name, int32_t h, int32_t fontWeight, const std::string &path = "");

struct Text2DData
{
	std::shared_ptr<Font> font{ GetDefaultFont() };
	std::wstring str;
	NiPoint2 screenPos;
	D3DCOLOR color;
	DWORD format = DT_NOCLIP | DT_CENTER | DT_VCENTER;
};

class Text3D
{
public:
	Text3D();
	~Text3D();

	NiPoint3 pos;
	bool is2D = false;
	Text2DData text2d;
	dlf_mutex m;

private:
	static void Init();
	static void Render();
};