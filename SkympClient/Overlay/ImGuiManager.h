#pragma once

class ImGuiManager
{
public:
	ImGuiManager();
	~ImGuiManager();

private:
	void Render();

	struct Impl;
	Impl *const pImpl;
};