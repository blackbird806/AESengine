#ifndef WOB_STRING_VIEW_HPP
#define WOB_STRING_VIEW_HPP

#include "wob.hpp"
#include "utility.hpp"
#include <cstring>

namespace wob
{
	class StringView
	{
	public:
		
		using Iterator_t = const char*;

		constexpr StringView() noexcept : data_(nullptr), size_(0) { }
		StringView(const char* str) noexcept : data_(str), size_(strlen(str)) { }
		constexpr StringView(StringView const& rhs) noexcept : data_(rhs.data_), size_(rhs.size_) { }
		constexpr StringView(const char* str, size_t len) noexcept : data_(str), size_(len) { }
		constexpr StringView(wob::nullptr_t) = delete;

		constexpr StringView& operator=(StringView const& view) noexcept = default;

		[[nodiscard]] constexpr char const& operator[](size_t i) const noexcept
		{ 
			WOB_BOUNDS_CHECK(i < size_);
			return data_[i];
		}

		[[nodiscard]] constexpr char const* data() const noexcept
		{
			return data_;
		}

		[[nodiscard]] constexpr size_t size() const noexcept
		{
			return size_;
		}

		[[nodiscard]] constexpr bool empty() const noexcept
		{
			return size_ > 0;
		}

		[[nodiscard]] constexpr StringView substr(size_t start) const noexcept
		{
			WOB_BOUNDS_CHECK(start < size_);
			size_t const len = size_ - start;
			return StringView(data_ + start, len);
		}

		[[nodiscard]] constexpr StringView substr(size_t start, size_t count) const noexcept
		{
			WOB_BOUNDS_CHECK(start + count < size_);
			return StringView(data_ + start, count);
		}

		[[nodiscard]] /*constexpr*/ bool operator==(StringView const& rhs) const noexcept
		{
			return strncmp(data_, rhs.data_, wob::min(size_, rhs.size_)) == 0;
		}

		[[nodiscard]] /*constexpr*/ bool operator!=(StringView const& rhs) const noexcept
		{
			return !(*this == rhs);
		}

		[[nodiscard]] constexpr Iterator_t begin() const noexcept
		{
			return data_;
		}

		[[nodiscard]] constexpr Iterator_t end() const noexcept
		{
			return data_ + size_;
		}

		[[nodiscard]] constexpr char front() const noexcept
		{
			WOB_BOUNDS_CHECK(size_ > 0);
			return *data_;
		}

		[[nodiscard]] constexpr char back() const noexcept
		{
			WOB_BOUNDS_CHECK(size_ > 0);
			return data_[size_-1];
		}

	private:
		const char* data_;
		size_t size_;
	};
}

#endif