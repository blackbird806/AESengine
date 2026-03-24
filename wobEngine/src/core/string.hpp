#ifndef WOB_STRING_HPP
#define WOB_STRING_HPP

#include <cstring>

#include "stringView.hpp"
#include "array.hpp"
#include "hash.hpp"
#include "coreMacros.hpp"

namespace wob
{
	constexpr size_t strlen(const char* start)
	{
		WOB_ASSERT_NOLOG(start);

		const char* end = start;
		while (*end != '\0')
			++end;
		return end - start;
	}

	constexpr void strcpy(char* WOB_RESTRICT(dst), const char* WOB_RESTRICT(src)) noexcept
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
			WOB_ASSERT_NOLOG(cstr);

			buffer.resize(wob::strlen(cstr) + 1);
			wob::strcpy(buffer.data(), cstr);
		}

		/*constexpr*/ String(const Char_t* cstr, uint32_t count) noexcept
		{
			WOB_ASSERT_NOLOG(cstr);

			buffer.resize(count);
			// @review
			// do we want strncpy ?
			strncpy(buffer.data(), cstr, count);
		}

		/*constexpr*/ String(StringView str) noexcept
		{
			buffer.resize(str.size() + 1);
			// @review same
			strncpy(buffer.data(), str.data(), str.size());
			buffer.back() = '\0';
		}

		constexpr String(String const& rhs) noexcept : buffer(rhs.buffer)
		{

		}

		constexpr String(String&& rhs) noexcept : buffer(wob::move(rhs.buffer))
		{

		}

		constexpr String& operator=(String const& rhs) noexcept
		{
			buffer = rhs.buffer;
			return *this;
		}

		constexpr String& operator=(String&& rhs) noexcept
		{
			buffer = wob::move(rhs.buffer);
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
			buffer.insert(pos, wob::forward<Range>(range));
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
			WOB_ASSERT_NOLOG(rhs);
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
			WOB_ASSERT_NOLOG(rhs);
			
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

		operator StringView() const
		{
			return StringView(buffer.data(), size());
		}

	private:
		Array<Char_t> buffer;
	};

	inline/*constexpr*/ wob::Ordering operator<=>(String const& lhs, String const& rhs) noexcept
	{
		return wob::Ordering{ strcmp(lhs.data(), rhs.data()) };
	}

	inline/*constexpr*/ wob::Ordering operator<=>(String const& lhs, String::Char_t const* rhs) noexcept
	{
		return wob::Ordering{ strcmp(lhs.data(), rhs) };
	}

	inline bool operator==(String const& lhs, String const& rhs) noexcept
	{
		return (lhs <=> rhs) == wob::Ordering::equal;
	}

	inline bool operator==(String const& lhs, String::Char_t const* rhs) noexcept
	{
		return (lhs <=> rhs) == wob::Ordering::equal;
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
	struct Hash<StringView>
	{
		uint64_t operator()(StringView str)
		{
			return Hash<const char*>{}(str.data(), str.size());
		}
	};
}

#endif