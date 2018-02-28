#pragma once

#define CYRILLIC_PREFIX L"<cy:"
#define CYRILLIC_POSTFIX L">"
#define CYRILLIC_PREFIXA "<cy:"
#define CYRILLIC_POSTFIXA ">"

inline std::string encodeRu(const std::wstring &ws)
{
	std::string res;
	static const std::wstring abc = L"éöóêåíãøùçõúôûâàïğîëäæıÿ÷ñìèòüáşÉÖÓÊÅÍÃØÙÇÕÚÔÛÂÀÏĞÎËÄÆİß×ÑÌÈÒÜÁŞ¸¨";
	for (auto ch : ws)
	{
		auto szWas = res.size();
		for (size_t i = 0; i != abc.size(); ++i)
		{
			if (ch == abc[i])
			{
				res += CYRILLIC_PREFIXA + std::to_string(i + 1) + CYRILLIC_POSTFIXA;
				break;
			}
		}
		if (szWas == res.size())
			res += ch;
	}
	return res;
}

inline std::string encodeRu(const std::string &s)
{
	std::string res;
	static const std::string abc = "éöóêåíãøùçõúôûâàïğîëäæıÿ÷ñìèòüáşÉÖÓÊÅÍÃØÙÇÕÚÔÛÂÀÏĞÎËÄÆİß×ÑÌÈÒÜÁŞ¸¨";
	for (auto ch : s)
	{
		auto szWas = res.size();
		for (size_t i = 0; i != abc.size(); ++i)
		{
			if (ch == abc[i])
			{
				res += CYRILLIC_PREFIXA + std::to_string(i + 1) + CYRILLIC_POSTFIXA;
				break;
			}
		}
		if (szWas == res.size())
			res += ch;
	}
	return res;
}

using str_t = std::wstring;

inline str_t decodeRu(str_t str)
{
	static str_t abc = L"éöóêåíãøùçõúôûâàïğîëäæıÿ÷ñìèòüáşÉÖÓÊÅÍÃØÙÇÕÚÔÛÂÀÏĞÎËÄÆİß×ÑÌÈÒÜÁŞ¸¨";

	auto replace = [](str_t s,
		const str_t &toReplace,
		const str_t &replaceWith)
	{
	rep:
		std::size_t pos = s.find(toReplace);
		if (pos == str_t::npos)
			return s;
		str_t lastS = s;
		s.replace(pos, toReplace.length(), replaceWith);
		if (s != lastS)
		{
			goto rep;
		}
		return s;
	};

	str = replace(str, CYRILLIC_PREFIX + std::to_wstring(65) + CYRILLIC_POSTFIX, L"e");

	for (size_t i = 0; i != abc.size(); ++i)
	{
		str_t  abci;
		abci.push_back(abc[i]);

		str = replace(str, CYRILLIC_PREFIX + std::to_wstring(i + 1) + CYRILLIC_POSTFIX, abci);
	}

	return str;
}

#ifdef CYRILLIC_PREFIX
#undef CYRILLIC_PREFIX
#endif

#ifdef CYRILLIC_POSTFIX
#undef CYRILLIC_POSTFIX
#endif

#ifdef CYRILLIC_PREFIXA
#undef CYRILLIC_PREFIXA
#endif

#ifdef CYRILLIC_POSTFIXA
#undef CYRILLIC_POSTFIXA
#endif