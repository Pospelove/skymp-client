#include "../stdafx.h"
#include "ImGuiManager.h"

#include "../DirectX/DirectXHook.h"
#include "../DInput/DInput.hpp"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx9.h"
#include "../Overlay/InputConverter.h"

#include "../CoreInterface/CoreInterface.h"

struct ImGuiManager::Impl
{
	LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
	D3DPRESENT_PARAMETERS g_d3dpp;

	class Data
	{
	public:
		Data()
		{
			mousePos.x = 0;
			mousePos.y = 0;
		}

		dlf_mutex m{ "imguimanager_data" };
		ImVec2 mousePos;
		std::vector<uint8_t> input;
	};

	Data data;
};

ImGuiManager::ImGuiManager() : pImpl(new Impl)
{
	auto hwnd = GetForegroundWindow();
	pImpl->g_pd3dDevice = g_pIDirect3DDevice9;

	// Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplDX9_Init(hwnd, pImpl->g_pd3dDevice);

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	ImFont* font = io.Fonts->AddFontFromFileTTF("MyGUI\\FuturaMediumCTT1.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	//IM_ASSERT(font != NULL);

	g_pIDirect3DDevice9->OnPresent.Add([this](void *) {
		try { 
			this->Render(); 
		}
		catch (...) {
		}
	});

	class MyInputListener : public InputListener
	{
	public:

		virtual void OnPress(uint8_t code) { if(onPress) onPress(code); }
		virtual void OnRelease(uint8_t code) { if(onRelease) onRelease(code); }
		virtual void OnMousePress(uint8_t code) { if(onMousePress) onMousePress(code); }
		virtual void OnMouseRelease(uint8_t code) { if(onMouseRelease) onMouseRelease(code); }
		virtual void OnMouseMove(unsigned int x, unsigned int y, unsigned int z) { if(onMouseMove) onMouseMove(x, y, z); }

		std::function<void(uint8_t)> onPress, onRelease, onMousePress, onMouseRelease;
		std::function<void(float, float, float)> onMouseMove;

	};

	auto listener = new MyInputListener;
	listener->onMouseMove = [this](float newX, float newY, float newZ) {
		std::lock_guard<dlf_mutex> l(pImpl->data.m);
		pImpl->data.mousePos.x = newX;
		pImpl->data.mousePos.y = newY;
	};
	listener->onPress = [this](uint8_t code) {
		std::lock_guard<dlf_mutex> l(pImpl->data.m);
		pImpl->data.input.push_back(code);
	};

	TheIInputHook->AddListener(listener);
}

void ImGuiManager::Render()
{
	static bool show_demo_window = true;
	static bool show_another_window = false;
	static const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 0.00f);

	// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
	// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.turn;

	ImGuiIO& io = ImGui::GetIO();
	io.MouseDrawCursor = true;

	{
		io.MouseDown[0] = sd::GetKeyPressed(VK_LBUTTON);
		io.MouseDown[1] = sd::GetKeyPressed(VK_RBUTTON);
		io.MouseDown[2] = sd::GetKeyPressed(VK_MBUTTON);
		io.KeyCtrl = sd::GetKeyPressed(VK_CONTROL);
		io.KeyAlt = sd::GetKeyPressed(VK_MENU);
		io.KeyShift = sd::GetKeyPressed(VK_SHIFT);
		io.KeySuper = sd::GetKeyPressed(VK_LWIN) || sd::GetKeyPressed(VK_RWIN);

		for (int32_t i = 0; i != 256; ++i)
		{
			io.KeysDown[i] = sd::GetKeyPressed(i);
		}

		std::lock_guard<dlf_mutex> l(pImpl->data.m);
		io.MousePos = pImpl->data.mousePos;
		for (auto scan : pImpl->data.input)
		{
			const wchar_t wstr[] = { input::ScanCodeToText(scan), 0 };
			io.AddInputCharacter(wstr[0]);
		}
		pImpl->data.input = {};
	}

	ImGui_ImplDX9_NewFrame();

	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	{
	}

	// Rendering
	ImGui::EndFrame();
	pImpl->g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
	pImpl->g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	pImpl->g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*255.0f), (int)(clear_color.y*255.0f), (int)(clear_color.z*255.0f), (int)(clear_color.w*255.0f));
	//g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
	if (pImpl->g_pd3dDevice->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		pImpl->g_pd3dDevice->EndScene();
	}
	HRESULT result = pImpl->g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && pImpl->g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		pImpl->g_pd3dDevice->Reset(&pImpl->g_d3dpp);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
};