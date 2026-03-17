#ifndef WOB_STRING_CONVERTER_HPP
#define WOB_STRING_CONVERTER_HPP

#include <string_view>
#include <string>

namespace wob
{
	std::wstring to_wstring(std::string_view str);
	std::string to_string(std::wstring_view wstr);
}

#endif // !UTILITY_HPP
