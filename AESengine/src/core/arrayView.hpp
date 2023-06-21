#ifndef AES_ARRAY_VIEW_HPP
#define AES_ARRAY_VIEW_HPP

#include "aes.hpp"

namespace aes
{
	template<typename T>
	class ArrayView
	{
	public:
		using Iterator_t = T*;

		AES_CPP20CONSTEXPR ArrayView() noexcept : data_(nullptr), size_(0) { }
		AES_CPP20CONSTEXPR ArrayView(ArrayView const& rhs) noexcept : data_(rhs.data_), size_(rhs.size_) { }
		AES_CPP20CONSTEXPR ArrayView(T* str, size_t len) noexcept : data_(str), size_(len) { }
		AES_CPP20CONSTEXPR ArrayView(std::nullptr_t) = delete;

		AES_CPP20CONSTEXPR ArrayView& operator=(ArrayView const& view) noexcept = default;

		[[nodiscard]] AES_CPP20CONSTEXPR char const& operator[](size_t i) const noexcept
		{
			AES_BOUNDS_CHECK(i < size_);
			return data_[i];
		}

		[[nodiscard]] AES_CPP20CONSTEXPR char const* data() const noexcept
		{
			return data_;
		}

		[[nodiscard]] AES_CPP20CONSTEXPR size_t size() const noexcept
		{
			return size_;
		}

		[[nodiscard]] AES_CPP20CONSTEXPR bool empty() const noexcept
		{
			return size_ > 0;
		}

		[[nodiscard]] AES_CPP20CONSTEXPR ArrayView subView(size_t start) const noexcept
		{
			AES_BOUNDS_CHECK(start < size_);
			size_t const len = size_ - start;
			return StringView(data_ + start, len);
		}

		[[nodiscard]] AES_CPP20CONSTEXPR ArrayView substr(size_t start, size_t count) const noexcept
		{
			AES_BOUNDS_CHECK(start + count < size_);
			return StringView(data_ + start, count);
		}

		[[nodiscard]] AES_CPP20CONSTEXPR Iterator_t begin() const noexcept
		{
			return data_;
		}

		[[nodiscard]] AES_CPP20CONSTEXPR Iterator_t end() const noexcept
		{
			return data_ + size_;
		}

		[[nodiscard]] AES_CPP20CONSTEXPR char front() const noexcept
		{
			AES_BOUNDS_CHECK(size_ > 0);
			return *data_;
		}

		[[nodiscard]] AES_CPP20CONSTEXPR char back() const noexcept
		{
			AES_BOUNDS_CHECK(size_ > 0);
			return data_[size_ - 1];
		}

	private:
		T* data_;
		size_t size_;
	};
}

#endif