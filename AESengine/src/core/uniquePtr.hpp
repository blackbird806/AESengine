#ifndef AES_UNIQUE_PTR_HPP
#define AES_UNIQUE_PTR_HPP

#include <concepts>
#include "allocator.hpp"
#include "context.hpp"

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
	struct GlobalAllocDelete
	{
		void operator()(T* ptr) noexcept
		{
			ptr->~T();
			context.allocator->deallocate(ptr);
		}
	};

	template<typename T>
	struct AllocatorDelete
	{
		template <class>
		friend struct AllocatorDelete;

		AllocatorDelete() noexcept = default;
		AllocatorDelete(IAllocator* a) noexcept : alloc(a) {}

		template<typename T2>
		AllocatorDelete(AllocatorDelete<T2> const& rhs) noexcept : alloc(rhs.alloc) {}

		IAllocator* alloc = nullptr;
		void operator()(T* ptr) noexcept
		{
			ptr->~T();
			alloc->deallocate(ptr);
		}
	};

	template<typename T, typename D = AllocatorDelete<T>>
	class UniquePtr
	{
	public:
		template <class, class>
		friend class UniquePtr;

		constexpr UniquePtr() noexcept : ptr(nullptr) {}
		constexpr UniquePtr(T* p) noexcept : ptr(p) {}
		constexpr UniquePtr(T* p, D&& d) noexcept : ptr(p), deleter(std::move(d)) {}
		constexpr UniquePtr(std::nullptr_t) : ptr(nullptr) {}
		UniquePtr(UniquePtr const&) = delete;

		constexpr UniquePtr(UniquePtr&& rhs) noexcept : ptr(rhs.release()), deleter(std::move(rhs.deleter))
		{ }

		template<typename T2, typename D2>
		requires std::convertible_to<T2*, T*> && std::convertible_to<D2, D>
		constexpr UniquePtr(UniquePtr<T2, D2>&& rhs) noexcept : ptr(rhs.release()), deleter(std::move(rhs.deleter))
		{ }
		
		template<typename T2, typename D2>
		requires std::convertible_to<T2*, T*>
		constexpr UniquePtr(UniquePtr<T2, D2>&& rhs) noexcept : ptr(rhs.release())
		{ }
		
		UniquePtr& operator=(UniquePtr const&) = delete;

		template<typename T2, typename D2>
		requires std::convertible_to<T2*, T*> && std::convertible_to<D2, D>
		constexpr UniquePtr& operator=(UniquePtr<T2, D2>&& rhs) noexcept
		{
			ptr = rhs.release();
			deleter = std::move(rhs.deleter);
			return *this;
		}
		
		template<typename T2, typename D2>
		requires std::convertible_to<T2*, T*>
		constexpr UniquePtr& operator=(UniquePtr<T2, D2>&& rhs) noexcept
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

	template<typename T>
	using GUniquePtr = UniquePtr<T, GlobalAllocDelete<T>>;

	template<typename T, typename... Args>
	auto makeUnique(Args&&... args) noexcept
	{
		void* ptr = context.allocator->allocate(sizeof(T), alignof(T));
		return UniquePtr<T, GlobalAllocDelete<T>>(::new(ptr) T(std::forward<Args>(args)...));
	}

	template<typename T, typename... Args>
	auto makeUnique(IAllocator& alloc, Args&&... args) noexcept
	{
		void* ptr = alloc.allocate(sizeof(T), alignof(T));
		AllocatorDelete<T> del(&alloc);
		return UniquePtr<T, AllocatorDelete<T>>(::new(ptr) T(std::forward<Args>(args)...), std::move(del));
	}
}

#endif