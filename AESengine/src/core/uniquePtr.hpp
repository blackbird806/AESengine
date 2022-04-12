#ifndef AES_UNIQUE_PTR_HPP
#define AES_UNIQUE_PTR_HPP

#include <concepts>
#include "allocator.hpp"

namespace aes
{
	template<class T, class U = T>
	constexpr T exchange(T& obj, U&& new_value) noexcept
	{
		T old_value = std::move(obj);
		obj = std::forward<U>(new_value);
		return old_value;
	}

	template<typename T>
	struct DefaultDelete
	{
		void operator()(T* ptr) noexcept
		{
			ptr->~T();
			globalAllocator.deallocate(ptr);
		}
	};

	template<typename T, typename D = DefaultDelete<T>>
	class UniquePtr
	{
	public:
		template <class, class>
		friend class UniquePtr;

		constexpr UniquePtr() noexcept : ptr(nullptr) {}
		constexpr UniquePtr(T* p) noexcept : ptr(p) {}
		constexpr UniquePtr(T* p, D&& d) noexcept : ptr(p), deleter(std::forward<D>(d)) {}
		constexpr UniquePtr(std::nullptr_t) : ptr(nullptr) {}
		UniquePtr(UniquePtr const&) = delete;

		constexpr UniquePtr(UniquePtr&& rhs) noexcept : ptr(rhs.release()), deleter(std::move(rhs.deleter))
		{ }

		template<typename T2, typename D2>
		requires std::convertible_to<T2*, T*> && std::assignable_from<D2, D>
		constexpr UniquePtr(UniquePtr<T2, D2>&& rhs) noexcept : ptr(rhs.release()), deleter(std::move(rhs.deleter))
		{ }

		template<typename T2, typename D2>
		requires std::convertible_to<T2*, T*>
		constexpr UniquePtr(UniquePtr<T2, D2>&& rhs) noexcept : ptr(rhs.release())
		{ }

		UniquePtr& operator=(UniquePtr const&) = delete;

		template<typename T2, typename D2>
		requires std::convertible_to<T2*, T*> && std::assignable_from<D2, D>
		constexpr UniquePtr& operator=(UniquePtr<T2, D2>&& rhs) noexcept : ptr(rhs.ptr), deleter(std::move(rhs.deleter))
		{
			ptr = rhs.release();
			deleter = std::move(rhs.deleter);
			return *this;
		}

		template<typename T2, typename D2>
		requires std::convertible_to<T2*, T*>
		constexpr UniquePtr& operator=(UniquePtr<T2, D2>&& rhs) noexcept : ptr(rhs.ptr), deleter(std::move(rhs.deleter))
		{
			ptr = rhs.release();
			return *this;
		}

		constexpr UniquePtr& operator=(UniquePtr&& rhs) noexcept
		{
			ptr = rhs.release();
			deleter = std::move(rhs.deleter);
			return *this;
		}

		constexpr UniquePtr& operator=(nullptr_t) noexcept
		{
			reset(nullptr);
			return *this;
		}

		constexpr UniquePtr& operator=(T* rhs) noexcept
		{
			reset(rhs);
			return *this;
		}

		constexpr ~UniquePtr() noexcept
		{
			reset();
		}

		[[nodiscard]] constexpr T* release() noexcept
		{
			T* tmp = ptr;
			ptr = nullptr;
			return tmp;
		}

		constexpr void reset(T* val = nullptr) noexcept
		{
			if (val != ptr)
			{
				if (T* p = exchange(ptr, val))
				{
					deleter(p);
				}
			}
		}

		[[nodiscard]] constexpr bool operator==(UniquePtr const& rhs) const noexcept
		{
			return ptr == rhs.ptr;
		}

		[[nodiscard]] constexpr bool operator!=(UniquePtr const& rhs) const noexcept
		{
			return ptr != rhs.ptr;
		}

		[[nodiscard]] constexpr operator bool() const noexcept
		{
			return ptr != nullptr;
		}

		[[nodiscard]] constexpr T* get() const noexcept
		{
			return ptr;
		}

		[[nodiscard]] constexpr std::add_lvalue_reference_t<T> operator*() const noexcept
		{
			return *ptr;
		}

		[[nodiscard]] constexpr T* operator->() const noexcept
		{
			return ptr;
		}

	private:
		T* ptr;
		[[no_unique_address]] D deleter;
	};

	template<typename T, typename... Args>
	UniquePtr<T> makeUnique(Args&&... args) noexcept
	{
		void* ptr = globalAllocator.allocate(sizeof(T), alignof(T));
		return UniquePtr<T>(::new(ptr) T(std::move(args)...));
	}

	template<typename T, typename... Args>
	auto makeUnique(IAllocator& alloc, Args&&... args) noexcept
	{
		void* ptr = alloc.allocate(sizeof(T), alignof(T));

		auto del = [&alloc](void* p) noexcept
		{
			static_cast<T*>(p)->~T();
			alloc.deallocate(p);
		};

		return UniquePtr<T>(::new(ptr) T(std::move(args)...), std::move(del));
	}
}

#endif