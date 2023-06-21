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

		constexpr Optional() noexcept
		{
			storage.hasValue = false;
		}

		constexpr Optional(Optional const& rhs)
		{
			if (rhs.storage.hasValue)
			{
				storage.value = rhs.storage.value;
				storage.hasValue = true;
			}
		}

		constexpr Optional(Optional&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value)
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

		constexpr Optional& operator=(Optional const& rhs)
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

		constexpr Optional& operator=(Optional&& rhs) noexcept(std::is_nothrow_move_assignable<T>::value)
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
		constexpr Optional& operator=(U&& value)
		{

		}

		constexpr T& value() & noexcept
		{
			AES_ASSERT(storage.hasValue);
			return storage.value;
		}

		constexpr T const& value() const& noexcept
		{
			AES_ASSERT(storage.hasValue);
			return storage.value;
		}

		constexpr T&& value() && noexcept
		{
			AES_ASSERT(storage.hasValue);
			return std::move(storage.value);
		}

		constexpr T const&& value() const&& noexcept
		{
			AES_ASSERT(storage.hasValue);
			return std::move(storage.value);
		}

		constexpr T&& value() && noexcept
		{
			AES_ASSERT(storage.hasValue);
			return std::move(storage.value);
		}

		constexpr T const&& value() const&& noexcept
		{
			AES_ASSERT(storage.hasValue);
			return storage.value;
		}

		constexpr T* operator->() noexcept
		{
			AES_ASSERT(storage.hasValue);
			return &storage.value;
		}

		constexpr T const* operator->() const noexcept
		{
			AES_ASSERT(storage.hasValue);
			return &storage.value;
		}

	private:

		template<typename T, typename E = void>
		struct Storage
		{
			constexpr ~Storage()
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

		template<typename T, typename E = std::enable_if<std::is_trivially_destructible<T>>::value>
		struct Storage
		{
			constexpr ~Storage() = default;

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