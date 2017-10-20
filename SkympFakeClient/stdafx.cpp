#include "stdafx.h"
#include <locale>
#include <codecvt>
#include <string>


std::string WstringToString(const std::wstring &wstring)
{
	char buffer[8192] = "";
	const auto size = min(wstring.size(), (sizeof buffer) - 1);

	for (size_t i = 0; i != size; ++i)
	{
		wctomb(&buffer[i], wstring.data()[i]);

		for (int32_t k = 0; k != 33; ++k)
			if (wstring.data()[i] == L'À' + k)
				buffer[i] = 'À' + k;
		for (int32_t k = 0; k != 33; ++k)
			if (wstring.data()[i] == L'à' + k)
				buffer[i] = 'à' + k;
	}
	buffer[size] = NULL;

	return buffer;
}

std::wstring StringToWstring(const std::string &string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(string);
	return wide;
}