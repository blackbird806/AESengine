#ifndef AES_FORMAT_HPP
#define AES_FORMAT_HPP

#include <string_view>
#include "string.hpp"
#include "core/maths.hpp"
#include "core/coreMacros.hpp"
#include "core/dragon4.hpp"

//TODO
#define AES_FMT_CONSTEXPR inline

namespace aes
{
	template<typename T>
	constexpr T&& forward(std::remove_reference_t<T>& t)
	{
		return static_cast<T&&>(t);
	}

	// idk how to name this
	template<typename T>
	concept FmtCharPtrT = std::same_as<std::remove_cvref_t<T>, char*>;

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

	constexpr void zeroMemory_fmtinternal(char* ptr, size_t n)
	{
		for (size_t i = 0; i < n; i++)
		{
			*ptr = 0;
		}
	}

	template <typename T>
	AES_FMT_CONSTEXPR void stringifyToBuff(T v, char* buff)
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
	AES_FMT_CONSTEXPR void stringifyToBuff(float v, char* buff)
	{
		d4::PrintFloat32(buff, 64, v, d4::tPrintFloatFormat::PrintFloatFormat_Positional, 3);
	}

	template <>
	AES_FMT_CONSTEXPR void stringifyToBuff(double v, char* buff)
	{
		d4::PrintFloat64(buff, 64, v, d4::tPrintFloatFormat::PrintFloatFormat_Positional, 3);
	}

	template <>
	constexpr void stringifyToBuff(bool v, char* buff)
	{
		strcpy_fmtinternal(buff, v ? "true" : "false");
	}

	template <>
	constexpr void stringifyToBuff(const char* v, char* buff)
	{
		if (v == nullptr)
			return;
		strcpy_fmtinternal(buff, v);
	}

	template <>
	constexpr void stringifyToBuff(char* v, char* buff)
	{
		if (v == nullptr)
			return;
		strcpy_fmtinternal(buff, v);
	}

	template <typename T>
	AES_FMT_CONSTEXPR uint32_t charsNeededForT(T v)
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
	constexpr uint32_t charsNeededForT(float v)
	{
		return 0;
	}

	template <>
	constexpr uint32_t charsNeededForT(double v)
	{
		return 0;
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

	template <>
	constexpr uint32_t charsNeededForT(const char* v)
	{
		if (v == nullptr)
			return 0;
		return strlen(v);
	}

	template <>
	constexpr uint32_t charsNeededForT(char* v)
	{
		if (v == nullptr)
			return 0;
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

	template<typename T>
	AES_FMT_CONSTEXPR String formatArg(T arg)
	{
		char buff[64];
		zeroMemory_fmtinternal(buff, sizeof(buff));
		stringifyToBuff(arg, buff);
		String str(buff);
		return str;
	}

	template<>
	AES_FMT_CONSTEXPR String formatArg(char* arg)
	{
		String str;
		str.resizeNoInit(charsNeededForT(arg));
		stringifyToBuff(arg, str.data());
		return str;
	}

	template<>
	AES_FMT_CONSTEXPR String formatArg(const char* arg)
	{
		String str;
		str.resizeNoInit(charsNeededForT(arg));
		stringifyToBuff(arg, str.data());
		return str;
	}

	template<typename ...Args>
	AES_FMT_CONSTEXPR String format(std::string_view fmt, Args&&... args)
	{
		// maybe use static array here to avoid alloc ?
		FormatedArgs formatedArgs;
		formatedArgs.args.reserve(sizeof...(args));
	
		// format each arg in parameter pack
		// TODO perfect forwarding
		(formatedArgs.args.push(formatArg(args)), ...);

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