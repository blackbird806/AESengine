#ifndef AES_UNIQUE_PTR_HPP
#define AES_UNIQUE_PTR_HPP

#include <type_traits>
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
		void operator()(T* ptr)
		{
			ptr->~T();
			globalAllocator.deallocate(ptr);
		}
	};

	template<typename T, typename D = DefaultDelete<T>>
	class UniquePtr
	{

	public:

		UniquePtr() noexcept : ptr(nullptr) {}
		UniquePtr(T* p) noexcept : ptr(p) {}
		UniquePtr(T* p, D&& d) noexcept : ptr(p), deleter(std::forward<D>(d)) {}
		UniquePtr(std::nullptr_t) : ptr(nullptr) {}
		UniquePtr(UniquePtr const&) = delete;

		UniquePtr(UniquePtr&& rhs) noexcept : ptr(rhs.release())
			,deleter(std::move(rhs.deleter))
		{
			
		}

		UniquePtr& operator=(UniquePtr const&) = delete;

		UniquePtr& operator=(UniquePtr&& rhs) noexcept
		{
			ptr = rhs.release();
			return *this;
		}

		T* release() noexcept
		{
			T* tmp = ptr;
			ptr = nullptr;
			return tmp;
		}

		void reset(T* val) noexcept
		{
			if (val != ptr)
			{
				if (T* p = exchange(ptr, val))
				{
					deleter(p);
				}
			}
		}

		UniquePtr& operator=(T* rhs) noexcept
		{
			reset(rhs);
			return *this;
		}

		bool operator==(UniquePtr const& rhs) const noexcept
		{
			return ptr == rhs.ptr;
		}

		bool operator!=(UniquePtr const& rhs) const noexcept
		{
			return ptr != rhs.ptr;
		}

		operator bool() const noexcept
		{
			return ptr != nullptr;
		}

		T* get() const noexcept
		{
			return ptr;
		}

		std::add_lvalue_reference_t<T> operator*() const noexcept
		{
			return *ptr;
		}

		T* operator->() const noexcept
		{
			return ptr;
		}

	private:
		T* ptr;
		[[no_unique_address]] D deleter;
	};

	template<typename T, typename... Args>
	UniquePtr<T> makeUnique(Args&&... args)
	{
		void* ptr = globalAllocator.allocate(sizeof(T), alignof(T));
		return UniquePtr<T>(::new(ptr) T(std::move(args)...));
	}

	template<typename T, typename... Args>
	auto makeUnique(IAllocator& alloc, Args&&... args)
	{
		void* ptr = alloc.allocate(sizeof(T), alignof(T));

		auto del = [&alloc](void* p)
		{
			static_cast<T*>(p)->~T();
			alloc.deallocate(p);
		};

		return UniquePtr<T>(::new(ptr) T(std::move(args)...), std::move(del));
	}
}

#endif