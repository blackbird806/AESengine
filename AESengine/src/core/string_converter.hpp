#ifndef AES_STRING_CONVERTER_HPP
#define AES_STRING_CONVERTER_HPP

#ifdef _WIN32
#include <string_view>

namespace aes
{
	std::wstring to_wstring(std::string_view str);
	std::string to_string(std::wstring_view wstr);
}
#endif

#endif // !UTILITY_HPP
