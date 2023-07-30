#ifndef AES_UTILITY_HPP
#define AES_UTILITY_HPP

#include <string>
#include <type_traits>
#include "aes.hpp"
#include "stringView.hpp"
#include "array.hpp"
#include "macro_helpers.hpp"

#define AES_SCOPE(code) ::aes::Scope AES_CONCAT(aes_scope_internal_, __COUNTER__) ([&](){code;});
#define AES_ONCE(code) static short const AES_CONCAT(aes_once_internal_, __COUNTER__) = [&]() {code; return 0;}();

namespace aes {
	
	std::string readFile(StringView file);
	Array<uint8_t> readFileBin(StringView file);

	std::vector<std::string> split(StringView a, char sep);

	// see Game engine architecture 6.2.1.3 (p431)
	AES_CPP20CONSTEXPR uintptr_t align(uintptr_t x, uint32_t a)
	{
		uint32_t const mask = a - 1;
		AES_ASSERT((a & mask) == 0);
		return (x + mask) & ~mask;
	}

	template<typename T>
	constexpr T* alignPointer(T* ptr, uint32_t a)
	{
		uintptr_t constexpr addr = reinterpret_cast<uintptr_t>(ptr);
		uintptr_t constexpr addrAligned = align(addr, a);
		return reinterpret_cast<T*>(addrAligned);
	}

	constexpr bool isPowerOf2(size_t n)
	{
		return (n > 0 && ((n & (n - 1)) == 0));
	}

	constexpr bool isAligned(uintptr_t p, uint32_t a)
	{
		return p % a == 0;
	}
	
	template<typename F>
	struct Scope
	{
		Scope(F&& fn) : func(std::forward<F>(fn))
		{
			
		}

		~Scope()
		{
			func();
		}
		
	private:
		F func;
	};

//#ifdef AES_CPP20
	template <typename BitType>
	class Flags
	{
	public:
		//using MaskType = std::underlying_type_t<BitType>;
		using MaskType = uint32_t;

		// constructors
		AES_CPP20CONSTEXPR Flags() : mask(0) {}

		AES_CPP20CONSTEXPR Flags(BitType bit) : mask(static_cast<MaskType>(bit)) {}

		AES_CPP20CONSTEXPR Flags(Flags<BitType> const& rhs) = default;

		AES_CPP20CONSTEXPR explicit Flags(MaskType flags) : mask(flags) {}

		// relational operators
		//auto operator<=>(Flags<BitType> const&) const = default;

		// logical operator
		AES_CPP20CONSTEXPR bool operator!() const
		{
			return !mask;
		}

		// bitwise operators
		AES_CPP20CONSTEXPR Flags<BitType> operator&(Flags<BitType> const& rhs) const
		{
			return Flags<BitType>(mask & rhs.mask);
		}

		AES_CPP20CONSTEXPR Flags<BitType> operator|(Flags<BitType> const& rhs) const
		{
			return Flags<BitType>(mask | rhs.mask);
		}

		AES_CPP20CONSTEXPR Flags<BitType> operator^(Flags<BitType> const& rhs) const
		{
			return Flags<BitType>(mask ^ rhs.mask);
		}

		AES_CPP20CONSTEXPR Flags<BitType> operator~() const
		{
			return Flags<BitType>(mask ^ 0);
		}

		// assignment operators
		AES_CPP20CONSTEXPR Flags<BitType>& operator=(MaskType const& rhs)
		{
			mask = rhs;
			return *this;
		}

		AES_CPP20CONSTEXPR Flags<BitType>& operator=(Flags<BitType> const& rhs) = default;

		AES_CPP20CONSTEXPR Flags<BitType>& operator|=(Flags<BitType> const& rhs)
		{
			mask |= rhs.mask;
			return *this;
		}

		AES_CPP20CONSTEXPR Flags<BitType>& operator&=(Flags<BitType> const& rhs)
		{
			mask &= rhs.mask;
			return *this;
		}

		AES_CPP20CONSTEXPR Flags<BitType>& operator^=(Flags<BitType> const& rhs)
		{
			mask ^= rhs.mask;
			return *this;
		}

		// cast operators
		AES_CPP20CONSTEXPR explicit operator bool() const
		{
			return !!mask;
		}

		template<typename T>
		AES_CPP20CONSTEXPR explicit operator T() const
		{
			static_assert(std::is_integral<T>::value);
			static_assert(sizeof(T) <= sizeof(MaskType));
			return static_cast<T>(mask);
		}

		//AES_CPP20CONSTEXPR explicit operator MaskType() const
		//{
		//	return mask;
		//}

	private:
		MaskType mask;
	};
	#define AES_FLAG_T(T) Flags<T>
	#define AES_DEFINE_IMPLICIT_FLAG_CAST(T)
//#else

	//#define AES_FLAG_T(T) std::underlying_type<T>::type
	//#define AES_FLAG_CAST(T, F) static_cast<std::underlying_type<T>::type>(F);

//#endif
}

#endif // !UTILITY_HPP
