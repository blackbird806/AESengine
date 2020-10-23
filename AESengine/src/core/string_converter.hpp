#ifndef STRING_CONVERTER_HPP
#define STRING_CONVERTER_HPP

#include <string_view>

namespace aes
{
	std::wstring to_wstring(const std::string_view str);
	std::string to_string(const std::wstring_view wstr);
}

#endif // !UTILITY_HPP
