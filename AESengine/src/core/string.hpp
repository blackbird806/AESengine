#ifndef AES_STRING_HPP
#define AES_STRING_HPP

#include <cstring>
#include <compare>

#include "context.hpp"
#include "array.hpp"
#include "hash.hpp"

namespace aes
{
	// General usage string class
	// TODO try some optimisations details 
	// TODO constexpr functions that relies on cstring funcs

	class String
	{

	public:
		using Char_t = char;
		using Iterator_t = Char_t*;

		constexpr String() noexcept = default;

		/*constexpr*/ String(const Char_t* cstr) noexcept
		{
			AES_ASSERT(cstr);

			buffer.resize(strlen(cstr) + 1);
			strcpy(buffer.data(), cstr);
		}

		/*constexpr*/ String(const Char_t* cstr, size_t count) noexcept
		{
			AES_ASSERT(cstr);

			buffer.resize(count);
			strncpy(buffer.data(), cstr, count);
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

		constexpr Char_t const& operator[](size_t i) const noexcept
		{
			return buffer[i];
		}

		constexpr Char_t& operator[](size_t i) noexcept
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

		constexpr size_t size() const noexcept
		{
			return buffer.empty() ? 0 : buffer.size() - 1;
		}

		constexpr size_t capacity() const noexcept
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

		constexpr void reserve(size_t cap) noexcept
		{
			buffer.reserve(cap + 1);
		}

		constexpr void resize(size_t sz) noexcept
		{
			buffer.resize(sz + 1);
			buffer[buffer.size() - 1] = 0;
		}

		constexpr void clear() noexcept
		{
			buffer.clear();
		}

		template<std::ranges::input_range Range>
		constexpr void insert(Iterator_t pos, Range&& range) noexcept
		{
			buffer.insert(pos, std::forward(range));
		}

		/*constexpr*/ void append(String const& rhs) noexcept
		{
			resize(buffer.capacity() + rhs.buffer.size());
			memcpy(&buffer[size()], rhs.buffer.data(), rhs.buffer.size());
		}

		/*constexpr*/ void append(const Char_t* rhs) noexcept
		{
			AES_ASSERT(rhs);
			size_t const rhslen = strlen(rhs) + 1;
			if (rhslen == 1) // string is empty
				return;
			size_t const oldSize = size();
			resize(buffer.capacity() + rhslen);
			memcpy(&buffer[oldSize], rhs, rhslen);
		}

		/*constexpr*/ void append(const Char_t* rhs, size_t count) noexcept
		{
			AES_ASSERT(rhs);
			
			resize(buffer.capacity() + count);
			memcpy(&buffer[size()], rhs, count);
		}

		constexpr Iterator_t begin() noexcept
		{
			return buffer.begin();
		}

		constexpr Iterator_t end() noexcept
		{
			return buffer.end();
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
		// https://stackoverflow.com/a/8317622
		constexpr uint64_t operator()(String const& str)
		{
			constexpr uint64_t A = 54059; /* a prime */
			constexpr uint64_t B = 76963; /* another prime */
			constexpr uint64_t C = 86969; /* yet 1another prime */
			constexpr uint64_t FIRSTH = 37; /* also prime */
			unsigned h = FIRSTH;
			auto* s = str.data();
			while (*s) {
				h = (h * A) ^ (s[0] * B);
				s++;
			}
			return h; // or return h % C;
		}
	};
}

#endif