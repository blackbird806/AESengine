#include "string_converter.hpp"

#include "aes.hpp"
#include "debug.hpp"

#include <clocale>
#include <cwchar>

std::wstring aes::to_wstring(const std::string_view in)
{
	std::wstring out{};
	out.reserve(in.length());
	const char* ptr = in.data();
	const char* const end = in.data() + in.length();

	mbstate_t state{};
	wchar_t wc;
	while (size_t len = mbrtowc(&wc, ptr, end - ptr, &state)) {
		AES_ENSURE(len != static_cast<size_t>(-1)) // bad encoding
		if (len == static_cast<size_t>(-2)) // valid but incomplete
			break;                         // nothing to do more
		out.push_back(wc);
		ptr += len; // advance [1...n]
	}
	return out;
}

std::string aes::to_string(const std::wstring_view in)
{
	std::string out{};
	out.reserve(MB_CUR_MAX * in.length());
	mbstate_t state{};
	for (wchar_t wc : in) {
		char mb[8]{}; // ensure null-terminated for UTF-8 (maximum 4 byte)
		const auto len = wcrtomb(mb, wc, &state);
		out += std::string_view{ mb, len };
	}
	return out;
}
