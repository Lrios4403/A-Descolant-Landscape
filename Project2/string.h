#ifndef STRING_H
#define STRING_H

#include <string>
#include <codecvt>
#include <locale>
#include <vector>

//Source code can be found at https://riptutorial.com/cplusplus/example/4190/conversion-to-std--wstring

using convert_t = std::codecvt_utf8<wchar_t>;
std::wstring_convert<convert_t, wchar_t> strconverter;

std::string to_string(std::wstring wstr)
{
    return strconverter.to_bytes(wstr);
}

std::wstring to_wstring(std::string str)
{
    return strconverter.from_bytes(str);
}

const std::vector<std::wstring> splitwstring(const std::wstring& s, const wchar_t& c)
{
	std::wstring buff{ L"" };
	std::vector<std::wstring> v;

	for (auto n : s)
	{
		if (n != c) buff += n; else
			if (n == c && buff != L"") { v.push_back(buff); buff = L""; }
	}
	if (buff != L"") v.push_back(buff);

	return v;
}

#endif