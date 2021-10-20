#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <string_view>
#include <vector>
#include "macro_helpers.hpp"

#define AES_SCOPE(code) ::aes::Scope AES_CONCAT(aes_scope_internal_, __COUNTER__) ([&](){code;});
#define AES_ONCE(code) static short const AES_CONCAT(aes_once_internal_, __COUNTER__) = [&]() {code; return 0;}();

namespace aes {
	
	std::string readFile(std::string const& file);
	std::vector<uint8_t> readFileBin(std::string const& file);

	std::vector<std::string> split(std::string_view a, char sep);
	
	constexpr uintptr_t align(uintptr_t x, uint32_t a)
	{
		return (x + (a - 1)) & ~(a - 1);
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

	template <typename BitType>
	class Flags
	{
	public:
		using MaskType = typename std::underlying_type<BitType>::type;

		// constructors
		constexpr Flags() : mask(0) {}

		constexpr Flags(BitType bit) : mask(static_cast<MaskType>(bit)) {}

		constexpr Flags(Flags<BitType> const& rhs) = default;

		constexpr explicit Flags(MaskType flags) : mask(flags) {}

		// relational operators
		auto operator<=>(Flags<BitType> const&) const = default;

		// logical operator
		constexpr bool operator!() const
		{
			return !mask;
		}

		// bitwise operators
		constexpr Flags<BitType> operator&(Flags<BitType> const& rhs) const
		{
			return Flags<BitType>(mask & rhs.mask);
		}

		constexpr Flags<BitType> operator|(Flags<BitType> const& rhs) const
		{
			return Flags<BitType>(mask | rhs.mask);
		}

		constexpr Flags<BitType> operator^(Flags<BitType> const& rhs) const
		{
			return Flags<BitType>(mask ^ rhs.mask);
		}

		constexpr Flags<BitType> operator~() const
		{
			return Flags<BitType>(mask ^ 0);
		}

		// assignment operators
		constexpr Flags<BitType>& operator=(Flags<BitType> const& rhs) = default;

		constexpr Flags<BitType>& operator|=(Flags<BitType> const& rhs) 
		{
			mask |= rhs.mask;
			return *this;
		}

		constexpr Flags<BitType>& operator&=(Flags<BitType> const& rhs)
		{
			mask &= rhs.mask;
			return *this;
		}

		constexpr Flags<BitType>& operator^=(Flags<BitType> const& rhs)
		{
			mask ^= rhs.mask;
			return *this;
		}

		// cast operators
		explicit constexpr operator bool() const
		{
			return !!mask;
		}

		explicit constexpr operator MaskType() const 
		{
			return mask;
		}

	private:
		MaskType mask;
	};
}

#endif // !UTILITY_HPP
