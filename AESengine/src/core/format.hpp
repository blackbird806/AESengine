#ifndef AES_FORMAT_HPP
#define AES_FORMAT_HPP

#include <format>
#include <string_view>
#include <tuple>
#include "string.hpp"
#include "core/maths.hpp"

namespace aes
{
	constexpr char* reverse(char* buffer, int i, int j) noexcept
	{
		while (i < j) {
			std::swap(buffer[i++], buffer[j--]);
		}

		return buffer;
	}

	// max number of characters necessary to display numbers
	constexpr int max_int32_chars = 11;
	constexpr int max_int64_chars = 21;

	// based on https://www.techiedelight.com/fr/implement-itoa-function-in-c/
	constexpr char* itoa(int value, char* buffer, int base = 10) noexcept
	{
		// invalid entry
		if (base < 2 || base > 32)
			return buffer;
		
		int n = aes::abs(value);

		int i = 0;
		while (n)
		{
			int r = n % base;

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

		return reverse(buffer, 0, i - 1);
	}

	// assume that dst have enough space for src
	constexpr void strccat_internal(char* __restrict dst, const char* __restrict src) noexcept
	{
		while (*dst++ = *src++) {}
	}

	template <typename T>
	constexpr char* toStringBuff(T v, char* buff)
	{
		return nullptr;
	}

	template <>
	constexpr char* toStringBuff(int v, char* buff)
	{
		return itoa(v, buff);
	}

	template <>
	constexpr char* toStringBuff(char* v, char* buff)
	{
		strccat_internal(buff, v);
		return nullptr;
	}

	constexpr uint32_t charsNeededForInt32(int32_t i)
	{
		const uint32_t neg = static_cast<uint32_t>(i < 0);
		return numDigits(i) + neg;
	}

	template <typename T>
	constexpr uint32_t charsNeededForT(T v)
	{
		return 0;
	}

	template <>
	constexpr uint32_t charsNeededForT(int32_t v)
	{
		return charsNeededForInt32(v);
	}

	constexpr size_t strlen(const char* str) noexcept
	{
		size_t len = 0;
		while (str[len])
			len++;
		return len;
	}

	template <>
	constexpr uint32_t charsNeededForT(char* v)
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

		([&] ()
			{
				String str;
				str.resizeNoInit(str.size() + charsNeededForT(args)); // reserve enough size for int
				toStringBuff(args, str.data());
				formatedArgs.args.push(std::move(str));
			} (), ...);

		String str(fmt.data(), fmt.size());
		// {} are still counted here though
		str.reserve(str.size() + formatedArgs.computeSize());
		auto it = str.begin();
		for (int i = 0; i < formatedArgs.args.size() && it != str.end(); i++)
		{
			it = std::find(str.begin(), str.end(), '{');
			str.insert(it, formatedArgs.args[i]);
		}

		return fmt.data();
	}

}

#endif