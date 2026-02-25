#ifndef AES_ARRAY_HPP
#define AES_ARRAY_HPP

#include "coreMacros.hpp"
#include "assert.hpp"
#include "allocator.hpp"
#include <utility>
#include <concepts>
#include <ranges>

namespace aes
{
	/*
	 * Generic Array class similar to std vector
	 * always store the allocator pointer
	 */
	template<typename T>
	class Array
	{
		public:

		using Iterator_t = T*;
		using ConstIterator_t = T const*;

		constexpr Array() noexcept
			: alloc(getContextAllocator()), buffer(nullptr), size_(0), capacity_(0)
		{

		}

		constexpr Array(IAllocator& allocator) noexcept
			: alloc(&allocator), buffer(nullptr), size_(0), capacity_(0)
		{
			
		}

		template<std::ranges::input_range Range>
		constexpr Array(IAllocator& allocator, Range const& range) noexcept
			: alloc(&allocator), buffer(nullptr), size_(0), capacity_(0)
		{
			insert(begin(), range);
		}

		constexpr Array(Array const& rhs) noexcept
		{
			*this = rhs;
		}

		constexpr Array(Array&& rhs) noexcept
		{
			*this = std::move(rhs);
		}

		constexpr IAllocator* getAllocator() const noexcept
		{
			return alloc;
		}

		constexpr void copyFrom(Array const& rhs) noexcept
		{
			clear();
			alloc = rhs.alloc;
			buffer = static_cast<T*>(alloc->allocate<T>(rhs.size_));
			size_ = rhs.size_;
			capacity_ = size_;

			// @performance conditionaly use memcpy here ?
			for (uint32_t i = 0; i < size_; i++)
				new (&buffer[i]) T(rhs.buffer[i]);
		}

		constexpr Array& operator=(Array const& rhs) noexcept
		{
			copyFrom(rhs);
			return *this;
		}

		constexpr Array& operator=(Array&& rhs) noexcept
		{
			clear();
			alloc = rhs.alloc;
			buffer = rhs.buffer;
			size_ = rhs.size_;
			capacity_ = rhs.capacity_;

			rhs.buffer = nullptr;
		#ifdef AES_DEBUG
			rhs.size_ = 0;
			rhs.capacity_ = 0;
		#endif
			return *this;
		}

		constexpr void reserve(uint32_t n) noexcept
		{
			if (n <= capacity_)
				return;

			T* const newBuffer = static_cast<T*>(alloc->allocate<T>(n));

			// @Review @Performance allocator reallocate may improve performances here
			moveBuffer(newBuffer);
			alloc->deallocate(buffer);
			buffer = newBuffer;
			capacity_ = n;
		}

		constexpr void resize(uint32_t n) noexcept
		{
			if (n == size_)
				return;

			if (n > size_)
			{
				if (n > capacity_)
				{
					reserve(n);
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

		constexpr void resizeNoInit(uint32_t n) noexcept
		{
			reserve(n);
			size_ = n;
		}

		constexpr void push(T const& e) noexcept
		{
			if (size_ == capacity_)
			{
				grow();
			}
			new (&buffer[size_++]) T(e);
		}

		constexpr void push(T&& e) noexcept
		{
			if (size_ == capacity_)
			{
				grow();
			}
			new (&buffer[size_++]) T(std::move(e));
		}

		template<std::ranges::input_range Range>
		constexpr void insert(Iterator_t pos, Range&& range) noexcept
		{
			AES_BOUNDS_CHECK(pos <= end());
			AES_BOUNDS_CHECK(pos >= begin());
			uint32_t const rangeSize = std::ranges::size(range);
			uint32_t const newSize = size_ + rangeSize;
			uint32_t const ipos = pos - begin();

			T* workBuffer = buffer;
			if (newSize > capacity_)
			{
				uint32_t newCapacity = increasedCapacity();

				// @Review is this ok ?
				while (newCapacity < newSize)
					newCapacity *= 2;

				T* const newBuffer = static_cast<T*>(alloc->allocate<T>(newCapacity));
				capacity_ = newCapacity;

				// Move elements after insertion point (shifted by rangeSize)
				for (uint32_t i = ipos; i < size_; i++)
				{
					new (&newBuffer[i + rangeSize]) T(std::move(buffer[i]));
				}
				workBuffer = newBuffer;
				alloc->deallocate(buffer);
			}
			else if (ipos < size_)
			{
				for (uint32_t i = size_; i > ipos; i--)
				{
					new (&workBuffer[i + rangeSize - 1]) T(std::move(workBuffer[i - 1]));
				}
			}

			// insert range after pos
			uint32_t insertIdx = ipos;
			for (auto&& e : range)
			{
				new (&workBuffer[insertIdx++]) T(std::move(e));
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

		constexpr Iterator_t begin() noexcept { return buffer; }
		constexpr Iterator_t end() noexcept { return buffer + size_; }

		constexpr ConstIterator_t begin() const noexcept { return buffer; }
		constexpr ConstIterator_t end() const noexcept { return buffer + size_; }

		constexpr T const& front() const noexcept { AES_BOUNDS_CHECK(size_ > 0); return buffer[0]; }
		constexpr T const& back() const noexcept { AES_BOUNDS_CHECK(size_ > 0); return buffer[size_ - 1]; }

		// return first element of the array
		constexpr T& front() noexcept { AES_BOUNDS_CHECK(size_ > 0); return buffer[0]; }

		// returnlast element of the array
		constexpr T& back() noexcept { AES_BOUNDS_CHECK(size_ > 0); return buffer[size_ - 1]; }

		constexpr T const* data() const noexcept { return buffer; }
		constexpr T* data() noexcept { return buffer; }

		/**
		 * \brief free unused memory of capacity
		 * after a successful call of shrink size() == capacity()
		 */
		constexpr void shrink() noexcept
		{
			if (size_ == capacity_)
				return;

			if (size_ == 0)
			{
				alloc->deallocate(buffer);
				buffer = nullptr;
				capacity_ = 0;
				return;
			}

			T* const newBuffer = (T*)alloc->allocate<T>(size_);
			moveBuffer(newBuffer);
			alloc->deallocate(buffer);
			buffer = newBuffer;
			capacity_ = size_;
		}
			 
		constexpr void clear() noexcept
		{
			for (uint32_t i = 0; i < size_; i++)
				buffer[i].~T();
			size_ = 0;
		}

		constexpr ~Array() noexcept
		{
			if (buffer)
			{
				clear();
				alloc->deallocate(buffer);
				buffer = nullptr;
			}
		}

		private:

		constexpr void moveBuffer(T* const newBuffer)
		{
			// @performance conditionaly use use memcpy (or memmove if reallocate is used) here ?
			for (uint32_t i = 0; i < size_; i++)
				new (&newBuffer[i]) T(std::move(buffer[i]));
		}

		constexpr void grow() noexcept
		{
			uint32_t const newCapacity = increasedCapacity();
			reserve(newCapacity);
		}

		constexpr uint32_t increasedCapacity() const noexcept
		{
			// @Review @performance
			// geometric growth with 8 base
			// todo try with other bases 4/16 ?
			// maybe abstract capacity_ grow strategy ?
			return capacity_ >= 8 ? capacity_ * 2 : 8;
		}

		IAllocator* alloc = nullptr;
		T* buffer = nullptr;
		uint32_t size_ = 0;
		uint32_t capacity_ = 0;
	};
}

#endif