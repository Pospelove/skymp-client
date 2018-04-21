#include "stdafx.h"

std::string WstringToString(const std::wstring &wstring)
{
	return (MyGUI::UString) wstring;
}

std::wstring StringToWstring(const std::string &string)
{
	return (MyGUI::UString) string;
}