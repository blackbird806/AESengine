#ifndef AES_FORMAT_HPP
#define AES_FORMAT_HPP

#include <string_view>
#include <concepts>
#include "string.hpp"
#include "core/maths.hpp"
#include "core/coreMacros.hpp"

namespace aes
{
	constexpr void reverse(char* buffer, int i, int j) noexcept
	{
		while (i < j) {
			std::swap(buffer[i++], buffer[j--]);
		}
	}

	// max number of characters necessary to display numbers
	constexpr int max_int32_chars = 11;
	constexpr int max_int64_chars = 21;

	// based on https://www.techiedelight.com/fr/implement-itoa-function-in-c/
	template<typename T>
	constexpr void itoa(auto value, char* buffer, int base = 10) noexcept;

	template<std::signed_integral T>
	constexpr void itoa(T value, char* buffer, int base = 10) noexcept
	{
		// invalid entry
		if (base < 2 || base > 32)
			return;
		
		auto n = aes::abs(value);

		int i = 0;
		while (n)
		{
			auto r = n % base;

			if (r >= 10) {
				buffer[i++] = 65 + (r - 10);
			}
			else {
				buffer[i++] = 48 + r;
			}

			n = n / base;
		}

		if (i == 0) {
			buffer[i++] = '0';
		}

		if (value < 0 && base == 10) {
			buffer[i++] = '-';
		}

		buffer[i] = '\0';

		reverse(buffer, 0, i - 1);
	}

	template<std::unsigned_integral T>
	constexpr void itoa(T value, char* buffer, int base = 10) noexcept
	{
		// invalid entry
		if (base < 2 || base > 32)
			return;

		auto n = value;

		int i = 0;
		while (n)
		{
			auto r = n % base;

			if (r >= 10) {
				buffer[i++] = 65 + (r - 10);
			}
			else {
				buffer[i++] = 48 + r;
			}

			n = n / base;
		}

		if (i == 0) {
			buffer[i++] = '0';
		}

		buffer[i] = '\0';

		reverse(buffer, 0, i - 1);
	}

	// assume that dst have enough space for src
	constexpr void strcpy_fmtinternal(char* AES_RESTRICT(dst), const char* AES_RESTRICT(src)) noexcept
	{
		while (*dst++ = *src++) {}
	}

	template <typename T>
	constexpr void stringifyToBuff(T v, char* buff)
	{
		// TODO we shouldn't define this one to trigger link time error when using non supported format
		// rn format is still wip so we keep this to allow compilation
	}

	template <>
	constexpr void stringifyToBuff(int32_t v, char* buff)
	{
		itoa(v, buff);
	}

	template <>
	constexpr void stringifyToBuff(uint32_t v, char* buff)
	{
		itoa(v, buff);
	}

	template <>
	constexpr void stringifyToBuff(bool v, char* buff)
	{
		strcpy_fmtinternal(buff, v ? "true" : "false");
	}

	template <>
	constexpr void stringifyToBuff(const char* v, char* buff)
	{
		strcpy_fmtinternal(buff, v);
	}

	template <typename T>
	constexpr uint32_t charsNeededForT(T v)
	{
		return 0;
	}

	template <>
	constexpr uint32_t charsNeededForT(int32_t v)
	{
		return numDigits(v);
	}

	template <>
	constexpr uint32_t charsNeededForT(uint32_t v)
	{
		return numDigits(v);
	}

	template <>
	constexpr uint32_t charsNeededForT(int64_t v)
	{
		return numDigits(v);
	}

	template <>
	constexpr uint32_t charsNeededForT(uint64_t v)
	{
		return numDigits(v);
	}

	template <>
	constexpr uint32_t charsNeededForT(bool v)
	{
		return v ? 4 /*true*/ : 5 /*false*/;
	}

	constexpr size_t strlen(const char* str) noexcept
	{
		size_t len = 0;
		while (str[len])
			len++;
		return len;
	}

	template <>
	constexpr uint32_t charsNeededForT(const char* v)
	{
		return strlen(v);
	}

	struct FormatedArgs
	{
		Array<String> args;

		constexpr uint32_t computeSize() const noexcept
		{
			uint32_t size = 0;
			for (auto const& arg : args)
				size += arg.size();
			return size;
		}
	};

	template<typename ...Args>
	constexpr String format(std::string_view fmt, Args&&... args)
	{
		// maybe use static array here to avoid alloc ?
		FormatedArgs formatedArgs;
		formatedArgs.args.reserve(sizeof...(args));

		// format each arg in parameter pack
		([&] ()
			{
				String str;
				str.resizeNoInit(charsNeededForT(args)); // reserve enough size for the type
				stringifyToBuff(args, str.data());
				formatedArgs.args.push(std::move(str));
			} (), ...);

		String str;
		bool isInfmt = false;
		int nformatedArg = 0;
		for (int i = 0; i < fmt.size(); i++)
		{
			if (fmt[i] == '{')
			{
				isInfmt = true;
				continue;
			}

			if (isInfmt)
			{
				if (fmt[i] == '}')
					isInfmt = false;
				// TODO parse fmt

				// TODO true error handling
				AES_ASSERT_NOLOG(nformatedArg < formatedArgs.args.size());
				str.append(formatedArgs.args[nformatedArg++]);
			}
			else
			{
				str.append(fmt[i]);
			}
		}

		return str;
	}
}

#endif