#ifndef AES_STRING_CONVERTER_HPP
#define AES_STRING_CONVERTER_HPP

#include <string_view>
#include <string>

namespace aes
{
	std::wstring to_wstring(std::string_view str);
	std::string to_string(std::wstring_view wstr);
}

#endif // !UTILITY_HPP
