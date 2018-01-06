#pragma once

class Loadscreen
{
public:
	Loadscreen(std::string filePath);
	~Loadscreen();

private:
	struct Impl;
	std::unique_ptr<Impl> pimpl;

	static void Render();
};