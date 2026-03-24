#ifndef WOB_UTILITY_HPP
#define WOB_UTILITY_HPP

#include "macro_helpers.hpp"
#include <cstdarg>
#include <cstdint>

#define WOB_SCOPE(code) ::wob::Scope WOB_CONCAT(WOB_scope_internal_, __COUNTER__) ([&](){code;});
#define WOB_ONCE(code) static short const WOB_CONCAT(WOB_once_internal_, __COUNTER__) = [&]() {code; return 0;}();

namespace wob {

	using nullptr_t = decltype(nullptr);


	template <class _Ty>
	constexpr const _Ty& //
		min(const _Ty& _Left _MSVC_LIFETIMEBOUND, const _Ty& _Right _MSVC_LIFETIMEBOUND)
		noexcept(noexcept(_Right < _Left)) /* strengthened */ {
		// return smaller of _Left and _Right
		return _Right < _Left ? _Right : _Left;
	}

	template <class _Ty>
	constexpr const _Ty& //
		max(const _Ty& _Left _MSVC_LIFETIMEBOUND, const _Ty& _Right _MSVC_LIFETIMEBOUND)
		noexcept(noexcept(_Right > _Left)) /* strengthened */ {
		// return smaller of _Left and _Right
		return _Right > _Left ? _Right : _Left;
	}

	struct Ordering {
		int value; // -1, 0, 1

		constexpr bool operator==(const Ordering&) const = default;

		static const Ordering less;
		static const Ordering equal;
		static const Ordering greater;
	};

	inline constexpr Ordering Ordering::less{ -1 };
	inline constexpr Ordering Ordering::equal{ 0 };
	inline constexpr Ordering Ordering::greater{ 1 };

	template <class _Ty>
		struct remove_reference {
		using type = _Ty;
		using _Const_thru_ref_type = const _Ty;
	};

	template <class _Ty>
	struct remove_reference<_Ty&> {
		using type = _Ty;
		using _Const_thru_ref_type = const _Ty&;
	};

	template <class _Ty>
	struct remove_reference<_Ty&&> {
		using type = _Ty;
		using _Const_thru_ref_type = const _Ty&&;
	};

	template <class _Container>
		constexpr auto size(const _Container& _Cont) noexcept(noexcept(_Cont.size())) /* strengthened */
		-> decltype(_Cont.size()) {
		return _Cont.size();
	}

	template <class _Ty, size_t _Size>
		constexpr size_t size(const _Ty(&)[_Size]) noexcept {
		return _Size;
	}

	//String readFile(std::string_view file);
	//std::vector<uint8_t> readFileBin(std::string_view file);

	//std::vector<std::string> split(std::string_view a, char sep);

	// see Game engine architecture 6.2.1.3 (p431)
	constexpr uintptr_t align(uintptr_t x, uint32_t a)
	{
		uint32_t const mask = a - 1;
//		WOB_ASSERT((a & mask) == 0);
		return (x + mask) & ~mask;
	}

	// from msvc stdlib
	template <class T>
	constexpr remove_reference_t<T>&& move(T&& _Arg) noexcept 
	{
		return static_cast<remove_reference_t<T>&&>(_Arg);
	}

	template <class _Ty>
	constexpr _Ty&& forward(remove_reference_t<_Ty>& _Arg) noexcept 
	{
		return static_cast<_Ty&&>(_Arg);
	}

	template <class _Ty>
	constexpr _Ty&& forward(remove_reference_t<_Ty>&& _Arg) noexcept \
	{
		static_assert(!is_lvalue_reference_v<_Ty>, "bad forward call");
		return static_cast<_Ty&&>(_Arg);
	}

	template <class _Container>
	constexpr auto size(const _Container& _Cont) noexcept(noexcept(_Cont.size())) /* strengthened */
		-> decltype(_Cont.size()) 
	{
		return _Cont.size();
	}

	template <class _Ty, size_t _Size>
	constexpr size_t size(const _Ty(&)[_Size]) noexcept 
	{
		return _Size;
	}

	template <class _Container>
		constexpr auto begin(_Container& _Cont) noexcept(noexcept(_Cont.begin())) /* strengthened */
		-> decltype(_Cont.begin()) {
		return _Cont.begin();
	}

	 template <class _Container>
		 constexpr auto begin(const _Container& _Cont) noexcept(noexcept(_Cont.begin())) /* strengthened */
		-> decltype(_Cont.begin()) {
		return _Cont.begin();
	}

	 template <class _Container>
		 constexpr auto end(_Container& _Cont) noexcept(noexcept(_Cont.end())) /* strengthened */
		-> decltype(_Cont.end()) {
		return _Cont.end();
	}

	 template <class _Container>
		 constexpr auto end(const _Container& _Cont) noexcept(noexcept(_Cont.end())) /* strengthened */
		-> decltype(_Cont.end()) {
		return _Cont.end();
	}

	 template <class _Ty, size_t _Size>
		 constexpr _Ty* begin(_Ty(&_Array)[_Size]) noexcept {
		return _Array;
	}

	 template <class _Ty, size_t _Size>
		 constexpr _Ty* end(_Ty(&_Array)[_Size]) noexcept {
		return _Array + _Size;
	}

	template <class _Ty>
	constexpr void swap(_Ty& _Left, _Ty& _Right)
		noexcept {
		_Ty _Tmp = move(_Left);
		_Left = move(_Right);
		_Right = move(_Tmp);
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
		constexpr Scope(F&& fn) : func(wob::forward<F>(fn))
		{
			
		}

		constexpr ~Scope()
		{
			func();
		}
		
	private:
		F func;
	};

	template<typename T>
	struct underlying_type {
		// For enums, the underlying type is the same as casting to int
		// We use compiler support: __underlying_type (MSVC/Clang/GCC) if available
#if defined(__clang__) || defined(__GNUC__)
		using type = __underlying_type(T);
#elif defined(_MSC_VER)
		using type = __underlying_type(T);
#else
	// Fallback: cast to int (unsafe for large enums)
		using type = int;
#endif
	};

	// Helper alias (like std::underlying_type_t)
	template<typename T>
	using underlying_type_t = typename underlying_type<T>::type;


	template <typename BitType>
	class Flags
	{
	public:
		using MaskType = wob::underlying_type_t<BitType>;

		// constructors
		constexpr Flags() : mask(0) {}

		constexpr Flags(BitType bit) : mask(static_cast<MaskType>(bit)) {}

		constexpr Flags(Flags<BitType> const& rhs) = default;

		constexpr explicit Flags(MaskType flags) : mask(flags) {}

		// relational operators
		constexpr auto operator<=>(Flags<BitType> const&) const = default;

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
		constexpr Flags<BitType>& operator=(MaskType const& rhs)
		{
			mask = rhs;
			return *this;
		}

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
