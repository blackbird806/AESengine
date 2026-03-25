#ifndef WOB_ARRAY_VIEW_HPP
#define WOB_ARRAY_VIEW_HPP

#include "wob.hpp"

namespace wob
{
	template<typename T>
	class ArrayView
	{
	public:
		using Iterator_t = T*;

		constexpr ArrayView() noexcept : data_(nullptr), size_(0) { }
		constexpr ArrayView(ArrayView const& rhs) noexcept : data_(rhs.data_), size_(rhs.size_) { }
		constexpr ArrayView(T* str, size_t len) noexcept : data_(str), size_(len) { }
		constexpr ArrayView(T* begin, T* end) noexcept : data_(begin), size_(end - begin) {}
		constexpr ArrayView(std::nullptr_t) = delete;

		constexpr ArrayView& operator=(ArrayView const& view) noexcept = default;

		[[nodiscard]] constexpr T& operator[](size_t i) const noexcept
		{
			WOB_BOUNDS_CHECK(i < size_);
			return data_[i];
		}

		[[nodiscard]] constexpr T* data() const noexcept
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

		[[nodiscard]] constexpr ArrayView subView(size_t start) const noexcept
		{
			WOB_BOUNDS_CHECK(start < size_);
			size_t const len = size_ - start;
			return ArrayView(data_ + start, len);
		}

		[[nodiscard]] constexpr ArrayView subview(size_t start, size_t count) const noexcept
		{
			WOB_BOUNDS_CHECK(start + count < size_);
			return ArrayView(data_ + start, count);
		}

		[[nodiscard]] constexpr Iterator_t begin() const noexcept
		{
			return data_;
		}

		[[nodiscard]] constexpr Iterator_t end() const noexcept
		{
			return data_ + size_;
		}

		[[nodiscard]] constexpr T& front() const noexcept
		{
			WOB_BOUNDS_CHECK(size_ > 0);
			return *data_;
		}

		[[nodiscard]] constexpr T& back() const noexcept
		{
			WOB_BOUNDS_CHECK(size_ > 0);
			return data_[size_ - 1];
		}

	private:
		T* data_;
		size_t size_;
	};
}

#endif