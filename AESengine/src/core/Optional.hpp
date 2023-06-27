#ifndef AES_OPTIONAL_HPP
#define AES_OPTIONAL_HPP

#include "aes.hpp"
#include <type_traits>

namespace aes
{
	// see : http://www.club.cc.cmu.edu/~ajo/disseminate/2017-02-15-Optional-From-Scratch.pdf
	template<typename T>
	class Optional
	{
	public:
		using ValueType = T;

		AES_CPP20CONSTEXPR Optional() noexcept
		{
			storage.hasValue = false;
		}

		AES_CPP20CONSTEXPR Optional(Optional const& rhs)
		{
			if (rhs.storage.hasValue)
			{
				storage.value = rhs.storage.value;
				storage.hasValue = true;
			}
		}

		AES_CPP20CONSTEXPR Optional(Optional&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
		{
			if (rhs.storage.hasValue)
			{
				storage.value = std::move(rhs.storage.value);
				storage.hasValue = true;
			}
		}

		constexpr bool hasValue() const noexcept
		{
			return storage.hasValue;
		}

		constexpr operator bool() const noexcept
		{
			return hasValue();
		}

		AES_CPP20CONSTEXPR Optional& operator=(Optional const& rhs)
		{
			if (rhs.storage.hasValue)
			{
				storage.value = rhs.storage.value;
				storage.hasValue = true;
			}
			else
			{
				if (storage.hasValue)
				{
					storage.hasValue = false;
					storage.value.~T();
				}
			}
		}

		AES_CPP20CONSTEXPR Optional& operator=(Optional&& rhs) noexcept(std::is_nothrow_move_assignable<T>::value)
		{
			if (rhs.storage.hasValue)
			{
				storage.value = std::move(rhs.storage.value);
				storage.hasValue = true;
			}
			else
			{
				if (storage.hasValue)
				{
					storage.hasValue = false;
					storage.value.~T();
				}
			}
		}

		template<typename U = T>
		AES_CPP20CONSTEXPR Optional& operator=(U&& value)
		{

		}

		AES_CPP20CONSTEXPR T& value() & noexcept
		{
			AES_ASSERT(storage.hasValue);
			return storage.value;
		}

		AES_CPP20CONSTEXPR T& operator*() & noexcept
		{
			AES_ASSERT(storage.hasValue);
			return storage.value;
		}

		AES_CPP20CONSTEXPR T const& value() const& noexcept
		{
			AES_ASSERT(storage.hasValue);
			return storage.value;
		}

		//constexpr T&& value() && noexcept
		//{
		//	AES_ASSERT(storage.hasValue);
		//	return std::move(storage.value);
		//}

		//constexpr T const&& value() const&& noexcept
		//{
		//	AES_ASSERT(storage.hasValue);
		//	return std::move(storage.value);
		//}

		//constexpr T&& value() && noexcept
		//{
		//	AES_ASSERT(storage.hasValue);
		//	return std::move(storage.value);
		//}

		//constexpr T const&& value() const&& noexcept
		//{
		//	AES_ASSERT(storage.hasValue);
		//	return storage.value;
		//}

		AES_CPP20CONSTEXPR T const* operator->() noexcept
		{
			AES_ASSERT(storage.hasValue);
			return &storage.value;
		}

		//constexpr T const* operator->() const noexcept
		//{
		//	AES_ASSERT(storage.hasValue);
		//	return &storage.value;
		//}

	private:

		template<typename T, typename E = void>
		struct Storage
		{
			AES_CPP20CONSTEXPR ~Storage()
			{
				if (hasValue)
					value.~T();
			}

			union
			{
				char dummy;
				T value;
			};
			bool hasValue;
		};

		template<typename T>
		struct Storage<T, typename std::enable_if<std::is_trivially_destructible<T>::value>::type>
		{
			AES_CPP20CONSTEXPR ~Storage() = default;

			union
			{
				char dummy;
				T value;
			};
			bool hasValue;
		};

		Storage<T> storage;
	};
}

#endif