#pragma once

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();

	static void SetGlobalRenderHook(std::function<void()> fn);

private:
	void Render();

	struct Impl;
	Impl *const pImpl;
};