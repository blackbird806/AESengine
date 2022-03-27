#ifndef AES_ARRAY_HPP
#define AES_ARRAY_HPP

#include "allocator.hpp"
#include "error.hpp"
#include <concepts>

namespace aes
{
	enum ArrayError : UnderlyingError_t
	{
		MemoryAllocationFailed
	};

	template<typename T>
	class Array
	{
		public:

		using Iterator_t = T*;

		constexpr Array(IAllocator& allocator) noexcept
			: alloc(&allocator), buffer(nullptr), size_(0), capacity_(0)
		{
			
		}

		template<std::ranges::input_range Range>
		constexpr Array(IAllocator& allocator, Range range) noexcept
			: alloc(&allocator)
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

		constexpr Result<void> copyFrom(Array const& rhs) noexcept
		{
			alloc = rhs.alloc;
			buffer = alloc->allocate(rhs.size_, alignof(T));
			if (!buffer)
				return { ArrayError::MemoryAllocationFailed };
			size_ = rhs.size_;
			capacity_ = size_;

			// @performance conditionaly use use memcpy here ?
			for (uint32_t i = 0; i < size_; i++)
				new (&buffer[i]) T(rhs.buffer[i]);

			return {};
		}

		constexpr Array& operator=(Array const& rhs) noexcept
		{
			auto const err = copyFrom(rhs);
			AES_ASSERT(err);
			return *this;
		}

		constexpr Array& operator=(Array&& rhs) noexcept
		{
			AES_ASSERT(alloc == rhs.alloc);
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

		constexpr Result<void> reserve(uint32_t n) noexcept
		{
			AES_ASSERT(n != 0);

			if (n <= capacity_)
				return {};

			T* const newBuffer = static_cast<T*>(alloc->allocate(n * sizeof(T), alignof(T)));
			if (!newBuffer)
				return { ArrayError::MemoryAllocationFailed };

			// @Review @Performance allocator reallocate may improve performances here
			moveBuffer(newBuffer);
			alloc->deallocate(buffer);
			buffer = newBuffer;
			capacity_ = n;
			return {};
		}

		constexpr Result<void> resize(uint32_t n) noexcept
		{
			if (n == size_)
				return {};

			if (n > size_)
			{
				if (n > capacity_)
				{
					auto const err = reserve(n);
					if (!err)
						return err;
				}
				//for (uint32_t i = size_; i < n; i++)
				//	new (&buffer[i]) T;
			}
			else // size < n
			{
				for (uint32_t i = n; i < size_; i++)
					buffer[i].~T();
			}
			size_ = n;
			return {};
		}

		constexpr Result<void> push(T const& e) noexcept
		{
			if (size_ == capacity_)
			{
				auto const err = grow();
				if (!err)
					return err;
			}
			buffer[size_++] = e;
			return {};
		}

		template<std::ranges::input_range Range>
		constexpr Result<void> insert(Iterator_t pos, Range range) noexcept
		{
			uint32_t const rangeSize = std::ranges::size(range);
			uint32_t const newSize = size_ + rangeSize;
			T* workBuffer = buffer;
			if (newSize >= capacity_)
			{
				uint32_t newCapacity = increasedCapacity();

				// @Review is this ok ?
				while (newCapacity < newSize)
					newCapacity *= 2;

				T* const newBuffer = static_cast<T*>(alloc->allocate(newCapacity * sizeof(T), alignof(T)));
				if (!newBuffer)
					return { ArrayError::MemoryAllocationFailed };
				capacity_ = newCapacity;

				// move previous elements in new buffer
				uint32_t i = 0;
				for (Iterator_t it = begin(); it != pos || it != nullptr; ++it)
				{
					new (&newBuffer[i]) T(std::move(buffer[i]));
					i++;
				}
				workBuffer = newBuffer;
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
				//new (&workBuffer[ipos++]) T(std::move(buffer[li]));
			}

			size_ = newSize;
			return {};
		}

		constexpr Result<void> insert(Iterator_t pos, T&& val) noexcept
		{
			return insert(pos, { std::forward<T>(val) });
		}

		constexpr void pop() noexcept
		{
			AES_BOUNDS_CHECK(size_ > 0);
			buffer[size_-1].~T();
			size_--;
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

		/**
		 * \brief free unused memory of capacity
		 * after a successful call of shrink size() == capacity()
		 * \return AESError::MemoryAllocationFailed or success
		 */
		constexpr Result<void> shrink() noexcept
		{
			if (size_ == capacity_)
				return {};

			T* const newBuffer = alloc->allocate(size_ * sizeof(T), alignof(T));
			if (!newBuffer)
				return { AESError::MemoryAllocationFailed };
			moveBuffer(newBuffer);
			alloc->deallocate(buffer);
			buffer = newBuffer;
			capacity_ = size_;
			return {};
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
			// @performance conditionaly use use memcpy here ?
			for (uint32_t i = 0; i < size_; i++)
				new (&newBuffer[i]) T(std::move(buffer[i]));
		}

		constexpr Result<void> grow() noexcept
		{
			uint32_t const newCapacity = increasedCapacity();
			return reserve(newCapacity);
		}

		constexpr uint32_t increasedCapacity() const noexcept
		{
			// @Review @performance
			// geometric growth with 8 base
			// todo try with other bases 4/16 ?
			// maybe abstract capacity_ grow strategy ?
			return capacity_ > 8 ? capacity_ * 2 : 8;
		}

		IAllocator* alloc = nullptr;
		T* buffer = nullptr;
		uint32_t size_ = 0;
		uint32_t capacity_ = 0;
	};
}

#endif