#include "stdafx.h"

std::string WstringToString(const std::wstring &wstring)
{
	char buffer[8192] = "";
	const auto size = min(wstring.size(), (sizeof buffer) - 1);

	for (size_t i = 0; i != size; ++i)
	{
		wctomb(&buffer[i], wstring.data()[i]);

		for (SInt32 k = 0; k != 33; ++k)
			if (wstring.data()[i] == L'À' + k)
				buffer[i] = 'À' + k;
		for (SInt32 k = 0; k != 33; ++k)
			if (wstring.data()[i] == L'à' + k)
				buffer[i] = 'à' + k;
	}
	buffer[size] = NULL;

	return buffer;
}

std::wstring StringToWstring(const std::string &string)
{
	return (MyGUI::UString) string;
}

std::string WstringToUtf(const std::wstring &string)
{
	return (MyGUI::UString) string;
}