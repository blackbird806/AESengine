#ifndef AES_STATIC_ARRAY_HPP
#define AES_STATIC_ARRAY_HPP

#include "aes.hpp"
#include <utility>
#include <concepts>

namespace aes
{
	template<typename T, size_t capacity_>
	class StaticArray
	{
	public:

		using Iterator_t = T*;

		constexpr StaticArray() noexcept
			: size_(0)
		{

		}

		template<std::ranges::input_range Range>
		constexpr StaticArray(Range const& range) noexcept
		{
			insert(begin(), range);
		}

		constexpr StaticArray(StaticArray const& rhs) noexcept
		{
			*this = rhs;
		}

		constexpr StaticArray(StaticArray&& rhs) noexcept
		{
			*this = std::move(rhs);
		}

		constexpr void copyFrom(StaticArray const& rhs) noexcept
		{
			clear();
			alloc = rhs.alloc;
			size_ = rhs.size_;
			capacity_ = size_;

			// @performance conditionaly use memcpy here ?
			for (uint32_t i = 0; i < size_; i++)
				new (&buffer[i]) T(rhs.buffer[i]);
		}

		constexpr StaticArray& operator=(StaticArray const& rhs) noexcept
		{
			copyFrom(rhs);
			return *this;
		}

		constexpr void resize(uint32_t n) noexcept
		{
			if (n == size_)
				return;

			if (n > size_)
			{
				if (n > capacity_)
				{

				}
				for (uint32_t i = size_; i < n; i++)
					new (&buffer[i]) T;
			}
			else // size < n
			{
				for (uint32_t i = n; i < size_; i++)
					buffer[i].~T();
			}
			size_ = n;
		}

		constexpr void push(T const& e) noexcept
		{
			if (size_ == capacity_)
			{
			}
			new (&buffer[size_++]) T(e);
		}

		constexpr void push(T&& e) noexcept
		{
			if (size_ == capacity_)
			{
			}
			new (&buffer[size_++]) T(std::move(e));
		}

		template<std::ranges::input_range Range>
		constexpr void insert(Iterator_t pos, Range&& range) noexcept
		{
			uint32_t const rangeSize = std::ranges::size(range);
			uint32_t const newSize = size_ + rangeSize;
			T* workBuffer = buffer;
			if (newSize >= capacity_)
			{

			}

			uint32_t ipos = end() - pos;

			// insert range after pos
			for (auto&& e : range)
			{
				new (&workBuffer[ipos++]) T(std::forward<T>(e));
			}

			// move next elements
			for (uint32_t li = ipos - rangeSize; li < size_; li++)
			{
				new (&workBuffer[ipos++]) T(std::move(buffer[li]));
			}
			buffer = workBuffer;
			size_ = newSize;
		}

		constexpr void insert(Iterator_t pos, T&& val) noexcept
		{
			insert(pos, { std::forward<T>(val) });
		}

		constexpr void pop() noexcept
		{
			AES_BOUNDS_CHECK(size_ > 0);
			buffer[--size_].~T();
		}

		constexpr T const& operator[](uint32_t i) const noexcept
		{
			AES_BOUNDS_CHECK(i < size_);
			return buffer[i];
		}

		constexpr T& operator[](uint32_t i) noexcept
		{
			AES_BOUNDS_CHECK(i < size_);
			return buffer[i];
		}

		constexpr uint32_t size() const noexcept { return size_; }
		constexpr uint32_t capacity() const noexcept { return capacity_; }
		constexpr bool empty() const noexcept { return size_ == 0; }

		constexpr Iterator_t begin() const noexcept { return buffer; }
		constexpr Iterator_t end() const noexcept { return buffer ? buffer + size_ : nullptr; }

		constexpr T const& front() const noexcept { AES_BOUNDS_CHECK(size_ > 0); return buffer[0]; }
		constexpr T const& back() const noexcept { AES_BOUNDS_CHECK(size_ > 0); return buffer[size_ - 1]; }

		constexpr T& front() noexcept { AES_BOUNDS_CHECK(size_ > 0); return buffer[0]; }
		constexpr T& back() noexcept { AES_BOUNDS_CHECK(size_ > 0); return buffer[size_ - 1]; }

		constexpr T const* data() const noexcept { return buffer; }
		constexpr T* data() noexcept { return buffer; }

		constexpr void clear() noexcept
		{
			for (uint32_t i = 0; i < size_; i++)
				buffer[i].~T();
			size_ = 0;
		}

		constexpr ~Array() noexcept
		{
			if (buffer)
				clear();
		}

	private:

		constexpr void moveBuffer(T* const newBuffer)
		{
			// @performance conditionaly use use memcpy here ?
			for (uint32_t i = 0; i < size_; i++)
				new (&newBuffer[i]) T(std::move(buffer[i]));
		}

	private:
		T buffer[capacity];
		uint32_t size_;
	};
}

#endif