#ifndef AES_FORMAT_HPP
#define AES_FORMAT_HPP

#include <format>
#include "string.hpp"

namespace aes
{
	template<typename ...Args>
	String format(String const& fmt, Args&&... args)
	{
		size_t const bufferSize = std::formatted_size(fmt.c_str(), args...);
		String str;
		str.resize(bufferSize);
		std::format_to_n(str.data(), bufferSize, fmt.data(), std::forward<Args>(args)...);
		return str;
	}
}

#endif