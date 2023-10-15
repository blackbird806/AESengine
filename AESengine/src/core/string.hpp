#ifndef AES_STRING_HPP
#define AES_STRING_HPP

#include "context.hpp"
#include "array.hpp"
#include <cstring>

namespace aes
{
	class String
	{
	public:
		using Char_t = char;
		using Iterator_t = Char_t*;

		constexpr String() noexcept = default;

		/*constexpr*/ String(const char* cstr) noexcept
		{
			buffer.resize(strlen(cstr) + 1);
			strcpy(buffer.data(), cstr);
		}

		/*constexpr*/ String(const char* cstr, size_t count) noexcept
		{
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

		constexpr auto operator<=>(String const& rhs) const noexcept
		{

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
			return buffer.capacity();
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
			buffer.reserve(cap);
		}

		constexpr void resize(size_t sz) noexcept
		{
			buffer.resize(sz);
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

		//constexpr void append(String const&) noexcept;
		//constexpr void append(const Char_t* s) noexcept;
		//constexpr void append(const Char_t* s, size_t count) noexcept;

		template<typename R>
		constexpr void append_range(R&& rg) noexcept;

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
}

#endif