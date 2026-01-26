#ifndef AES_STRING_HPP
#define AES_STRING_HPP

#include <cstring>
#include <compare>
#include <math.h>
#include <string_view>

#include "array.hpp"
#include "hash.hpp"
#include "coreMacros.hpp"

namespace aes
{
	constexpr size_t strlen(const char* start)
	{
		AES_ASSERT_NOLOG(start);

		const char* end = start;
		while (*end != '\0')
			++end;
		return end - start;
	}

	constexpr void strcpy(char* AES_RESTRICT(dst), const char* AES_RESTRICT(src)) noexcept
	{
		while (*dst++ = *src++) {}
	}
	/// General usage string class, for now just a wrapper over Array<Char>, we may want to implemet SSO in the future
	// TODO try some optimisations details
	// TODO constexpr functions that relies on cstring funcs
	class String
	{

	public:
		using Char_t = char;
		using Iterator_t = Char_t*;

		constexpr String() noexcept = default;

		constexpr String(const Char_t* cstr) noexcept
		{
			AES_ASSERT_NOLOG(cstr);

			buffer.resize(aes::strlen(cstr) + 1);
			aes::strcpy(buffer.data(), cstr);
		}

		/*constexpr*/ String(const Char_t* cstr, uint32_t count) noexcept
		{
			AES_ASSERT_NOLOG(cstr);

			buffer.resize(count);
			// @review
			// do we want strncpy ?
			strncpy(buffer.data(), cstr, count);
		}

		/*constexpr*/ String(std::string_view str) noexcept
		{
			buffer.resize(str.size());
			// @review same
			strncpy(buffer.data(), str.data(), str.size());
		}

		constexpr String(String const& rhs) noexcept : buffer(rhs.buffer)
		{

		}

		constexpr String(String&& rhs) noexcept : buffer(std::move(rhs.buffer))
		{

		}

		constexpr String& operator=(String const& rhs) noexcept
		{
			buffer = rhs.buffer;
			return *this;
		}

		constexpr String& operator=(String&& rhs) noexcept
		{
			buffer = std::move(rhs.buffer);
			return *this;
		}

		constexpr Char_t const& operator[](uint32_t i) const noexcept
		{
			return buffer[i];
		}

		constexpr Char_t& operator[](uint32_t i) noexcept
		{
			return buffer[i];
		}

		constexpr Char_t& front() noexcept
		{
			return buffer.front();
		}

		constexpr Char_t const& front() const noexcept
		{
			return buffer.front();
		}

		constexpr Char_t& back() noexcept
		{
			return buffer.back();
		}

		constexpr Char_t const& back() const noexcept
		{
			return buffer.back();
		}

		constexpr const char* c_str() const noexcept
		{
			return buffer.data();
		}

		constexpr Char_t* data() noexcept
		{
			return buffer.data();
		}

		constexpr Char_t const* data() const noexcept
		{
			return buffer.data();
		}

		constexpr uint32_t size() const noexcept
		{
			return buffer.empty() ? 0 : buffer.size() - 1;
		}

		constexpr uint32_t capacity() const noexcept
		{
			return buffer.capacity() == 0 ? 0 : buffer.capacity() - 1;
		}

		constexpr bool empty() const noexcept
		{
			return buffer.empty();
		}

		constexpr void shrink() noexcept
		{
			buffer.shrink();
		}

		constexpr void reserve(uint32_t cap) noexcept
		{
			buffer.reserve(cap + 1);
		}

		constexpr void resizeNoInit(uint32_t cap) noexcept
		{
			buffer.resizeNoInit(cap + 1);
		}

		constexpr void resize(uint32_t sz) noexcept
		{
			buffer.resize(sz + 1);
			buffer.back() = '\0';
		}

		constexpr void clear() noexcept
		{
			buffer.clear();
		}

		template<std::ranges::input_range Range>
		constexpr void insert(Iterator_t pos, Range&& range) noexcept
		{
			buffer.insert(pos, std::forward<Range>(range));
			buffer.push('\0');
		}

		/*constexpr*/ void append(String const& rhs) noexcept
		{
			if (rhs.empty())
				return;
			size_t const oldSize = size();
			resize(size() + rhs.size());
			memcpy(&buffer[oldSize], rhs.buffer.data(), rhs.buffer.size());
		}

		/*constexpr*/ void append(const Char_t* rhs) noexcept
		{
			AES_ASSERT_NOLOG(rhs);
			size_t const rhslen = strlen(rhs);
			if (rhslen == 0) // string is empty
				return;

			size_t const oldSize = size();
			resize(size() + rhslen);

			if (oldSize == 0)
			{
				memcpy(buffer.data(), rhs, rhslen);
			}
			else
			{
				memcpy(&buffer[oldSize], rhs, rhslen);
			}
		}

		constexpr void append(Char_t c) noexcept
		{
			if (empty())
				buffer.push(c);
			else
				buffer[size()] = c;
			buffer.push('\0');
		}

		/*constexpr*/ void append(const Char_t* rhs, uint32_t count) noexcept
		{
			AES_ASSERT_NOLOG(rhs);
			
			resize(buffer.capacity() + count);
			memcpy(&buffer[size()], rhs, count);
		}

		constexpr Iterator_t begin() noexcept
		{
			return buffer.begin();
		}

		constexpr Iterator_t end() noexcept
		{
			return buffer.end() - 1;
		}

	private:
		Array<Char_t> buffer;
	};

	inline/*constexpr*/ std::strong_ordering operator<=>(String const& lhs, String const& rhs) noexcept
	{
		return strcmp(lhs.data(), rhs.data()) <=> 0;
	}

	inline/*constexpr*/ std::strong_ordering operator<=>(String const& lhs, String::Char_t const* rhs) noexcept
	{
		return strcmp(lhs.data(), rhs) <=> 0;
	}

	inline bool operator==(String const& lhs, String const& rhs) noexcept
	{
		return (lhs <=> rhs) == std::strong_ordering::equal;
	}

	inline bool operator==(String const& lhs, String::Char_t const* rhs) noexcept
	{
		return (lhs <=> rhs) == std::strong_ordering::equal;
	}

	template<>
	struct Hash<String>
	{
		constexpr uint64_t operator()(String const& str)
		{
			return Hash<const char*>{}(str.c_str());
		}
	};

	template<>
	struct Hash<std::string_view>
	{
		uint64_t operator()(std::string_view str)
		{
			return std::hash<std::string_view>()(str);
		}
	};
}

#endif