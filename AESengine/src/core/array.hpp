#ifndef AES_ARRAY_HPP
#define AES_ARRAY_HPP

#include "allocator.hpp"
#include "error.hpp"
#include "aes.hpp"
#ifdef AES_CPP20
#include <concepts>
#endif

namespace aes
{
	template <typename T>
	constexpr std::size_t size(const T& c)
	{
		return c.size();
	}

	template <typename T, std::size_t N>
	constexpr std::size_t size(const T(&array)[N]) noexcept
	{
		return N;
	}

	/*
	 * Generic Array class similar to std vector
	 * always store the allocator pointer
	 */
	template<typename T>
	class Array
	{
		public:

		using Iterator_t = T*;

		AES_CPP20CONSTEXPR Array(IAllocator& allocator) noexcept
			: alloc(&allocator), buffer(nullptr), size_(0), capacity_(0)
		{
			
		}

#ifdef AES_CPP20
		template<std::ranges::input_range Range>
#else
		template<typename Range>
#endif
		AES_CPP20CONSTEXPR Array(IAllocator& allocator, Range const& range) noexcept
			: alloc(&allocator)
		{
			insert(begin(), range);
		}

		AES_CPP20CONSTEXPR Array(Array const& rhs) noexcept
		{
			*this = rhs;
		}

		AES_CPP20CONSTEXPR Array(Array&& rhs) noexcept
		{
			*this = std::move(rhs);
		}

		IAllocator* getAllocator() const noexcept
		{
			return alloc;
		}

		AES_CPP20CONSTEXPR Result<void> copyFrom(Array const& rhs) noexcept
		{
			clear();
			alloc = rhs.alloc;
			buffer = static_cast<T*>(alloc->allocate<T>(rhs.size_));
			if (!buffer)
				return { AESError::MemoryAllocationFailed };
			size_ = rhs.size_;
			capacity_ = size_;

			// @performance conditionaly use memcpy here ?
			for (uint32_t i = 0; i < size_; i++)
				new (&buffer[i]) T(rhs.buffer[i]);

			return {};
		}

		AES_CPP20CONSTEXPR Array& operator=(Array const& rhs) noexcept
		{
			auto const err = copyFrom(rhs);
			AES_ASSERT(err);
			return *this;
		}

		AES_CPP20CONSTEXPR Array& operator=(Array&& rhs) noexcept
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

		AES_CPP20CONSTEXPR Result<void> reserve(uint32_t n) noexcept
		{
			AES_ASSERT(n != 0);

			if (n <= capacity_)
				return {};

			T* const newBuffer = static_cast<T*>(alloc->allocate(n * sizeof(T), alignof(T)));
			if (!newBuffer)
				return { AESError::MemoryAllocationFailed };

			// @Review @Performance allocator reallocate may improve performances here
			moveBuffer(newBuffer);
			alloc->deallocate(buffer);
			buffer = newBuffer;
			capacity_ = n;
			return {};
		}

		AES_CPP20CONSTEXPR Result<void> resize(uint32_t n) noexcept
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
				for (uint32_t i = size_; i < n; i++)
					new (&buffer[i]) T;
			}
			else // size < n
			{
				for (uint32_t i = n; i < size_; i++)
					buffer[i].~T();
			}
			size_ = n;
			return {};
		}

		AES_CPP20CONSTEXPR Result<void> push(T const& e) noexcept
		{
			if (size_ == capacity_)
			{
				auto const err = grow();
				if (!err)
					return err;
			}
			new (&buffer[size_++]) T(e);
			return {};
		}

		AES_CPP20CONSTEXPR Result<void> push(T&& e) noexcept
		{
			if (size_ == capacity_)
			{
				auto const err = grow();
				if (!err)
					return err;
			}
			new (&buffer[size_++]) T(std::move(e));
			return {};
		}

#ifdef AES_CPP20
		template<std::ranges::input_range Range>
#else
		template<typename Range>
#endif
		AES_CPP20CONSTEXPR Result<void> insert(Iterator_t pos, Range const& range) noexcept
		{
			uint32_t const rangeSize = aes::size(range);
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
					return { AESError::MemoryAllocationFailed };
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
				new (&workBuffer[ipos++]) T(std::move<T>(buffer[li]));
			}
			buffer = workBuffer;
			size_ = newSize;
			return {};
		}

		AES_CPP20CONSTEXPR Result<void> insert(Iterator_t pos, T&& val) noexcept
		{
			return insert(pos, { std::forward<T>(val) });
		}

		AES_CPP20CONSTEXPR void pop() noexcept
		{
			AES_BOUNDS_CHECK(size_ > 0);
			buffer[--size_].~T();
		}

		AES_CPP20CONSTEXPR T const& operator[](uint32_t i) const noexcept
		{
			AES_BOUNDS_CHECK(i < size_);
			return buffer[i];
		}

		AES_CPP20CONSTEXPR T& operator[](uint32_t i) noexcept
		{
			AES_BOUNDS_CHECK(i < size_);
			return buffer[i];
		}

		AES_CPP20CONSTEXPR uint32_t size() const noexcept { return size_; }
		AES_CPP20CONSTEXPR uint32_t capacity() const noexcept { return capacity_; }
		AES_CPP20CONSTEXPR bool empty() const noexcept { return size_ == 0; }

		AES_CPP20CONSTEXPR Iterator_t begin() const noexcept { return buffer; }
		AES_CPP20CONSTEXPR Iterator_t end() const noexcept { return buffer ? buffer + size_ : nullptr; }

		AES_CPP20CONSTEXPR T const& front() const noexcept { AES_BOUNDS_CHECK(size_ > 0); return buffer[0]; }
		AES_CPP20CONSTEXPR T const& back() const noexcept { AES_BOUNDS_CHECK(size_ > 0); return buffer[size_ - 1]; }

		AES_CPP20CONSTEXPR T& front() noexcept { AES_BOUNDS_CHECK(size_ > 0); return buffer[0]; }
		AES_CPP20CONSTEXPR T& back() noexcept { AES_BOUNDS_CHECK(size_ > 0); return buffer[size_ - 1]; }

		AES_CPP20CONSTEXPR T const* data() const noexcept { return buffer; }
		AES_CPP20CONSTEXPR T* data() noexcept { return buffer; }

		/**
		 * \brief free unused memory of capacity
		 * after a successful call of shrink size() == capacity()
		 * \return AESError::MemoryAllocationFailed or success
		 */
		AES_CPP20CONSTEXPR Result<void> shrink() noexcept
		{
			if (size_ == capacity_)
				return {};

			T* const newBuffer = alloc->allocate(size_ * sizeof(T), alignof(T));
			if (!newBuffer)
				return { AESError::MemoryAllocationFailed };
			//moveBuffer(newBuffer);
			alloc->deallocate(buffer);
			buffer = newBuffer;
			capacity_ = size_;
			return {};
		}
			 
		AES_CPP20CONSTEXPR void clear() noexcept
		{
			for (uint32_t i = 0; i < size_; i++)
				buffer[i].~T();
			size_ = 0;
		}

		AES_CPP20CONSTEXPR ~Array() noexcept
		{
			if (buffer)
			{
				clear();
				alloc->deallocate(buffer);
				buffer = nullptr;
			}
		}

		private:

		AES_CPP20CONSTEXPR void moveBuffer(T* const newBuffer)
		{
			// @performance conditionaly use use memcpy here ?
			for (uint32_t i = 0; i < size_; i++)
				new (&newBuffer[i]) T(std::move(buffer[i]));
		}

		AES_CPP20CONSTEXPR Result<void> grow() noexcept
		{
			uint32_t const newCapacity = increasedCapacity();
			return reserve(newCapacity);
		}

		AES_CPP20CONSTEXPR uint32_t increasedCapacity() const noexcept
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