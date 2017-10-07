#pragma once
#include "../DInput/DInput.hpp"

class KeyboardManager : public InputListener
{
public:
	static KeyboardManager *GetSingletone();
	bool GetKeyPressed(uint8_t dxScanCode) const;

private:
	virtual void OnPress(uint8_t code);
	virtual void OnRelease(uint8_t code);
	virtual void OnMousePress(uint8_t code);
	virtual void OnMouseRelease(uint8_t code);
	virtual void OnMouseMove(unsigned int x, unsigned int y, unsigned int z);

	std::set<uint8_t> pressed;
	mutable dlf_mutex mutex;
};