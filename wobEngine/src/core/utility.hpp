#ifndef WOB_UTILITY_HPP
#define WOB_UTILITY_HPP

#include "macro_helpers.hpp"
#include <cstdarg>
#include <cstdint>

#define WOB_SCOPE(code) ::wob::Scope WOB_CONCAT(WOB_scope_internal_, __COUNTER__) ([&](){code;});
#define WOB_ONCE(code) static short const WOB_CONCAT(WOB_once_internal_, __COUNTER__) = [&]() {code; return 0;}();

namespace wob {

	using nullptr_t = decltype(nullptr);

	template <class... _Types>
	using void_t = void;

	template <class _Ty>
	struct Add_reference_<_Ty, void_t<_Ty&>> { // (referenceable type)
		using Lvalue_ = _Ty&;
		using Rvalue_ = _Ty&&;
	};

	template <class _Ty>
		struct add_lvalue_reference {
		using type = typename Add_reference_<_Ty>::Lvalue_;
	};

	template <class _Ty>
	using add_lvalue_reference_t = typename Add_reference_<_Ty>::Lvalue_;

	template <class _Ty>
		struct add_rvalue_reference {
		using type = typename Add_reference_<_Ty>::Rvalue_;
	};

	template <class _Ty>
	using add_rvalue_reference_t = typename Add_reference_<_Ty>::Rvalue_;

	template <class _Ty>
		add_rvalue_reference_t<_Ty> declval() noexcept {
		static_assert(false, "Calling declval is ill-formed, see N4950 [declval]/2.");
	}

	template <class _From, class _To>
	concept convertible_to = __is_convertible_to(_From, _To) && requires { static_cast<_To>(_STD declval<_From>()); };

	template <class _Ty>
		struct remove_cv { // remove top-level const and volatile qualifiers
		using type = _Ty;

		template <template <class> class _Fn>
		using _Apply = _Fn<_Ty>; // apply cv-qualifiers from the class template argument to _Fn<_Ty>
	};

	template <class _Ty>
	struct remove_cv<const _Ty> {
		using type = _Ty;

		template <template <class> class _Fn>
		using _Apply = const _Fn<_Ty>;
	};

	template <class _Ty>
	struct remove_cv<volatile _Ty> {
		using type = _Ty;

		template <template <class> class _Fn>
		using _Apply = volatile _Fn<_Ty>;
	};

	template <class _Ty>
	struct remove_cv<const volatile _Ty> {
		using type = _Ty;

		template <template <class> class _Fn>
		using _Apply = const volatile _Fn<_Ty>;
	};

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

	 template <class _Ty>
	using remove_reference_t = typename remove_reference<_Ty>::type;

	template <class _Ty>
	using _Const_thru_ref = typename remove_reference<_Ty>::_Const_thru_ref_type;


	template <class _Ty>
	using remove_cv_t = typename remove_cv<_Ty>::type;

	template <class _Ty>
	using _Remove_cvref_t _MSVC_KNOWN_SEMANTICS = remove_cv_t<remove_reference_t<_Ty>>;

	 template <class _Ty>
		using remove_cvref_t = _Remove_cvref_t<_Ty>;

	 template <class _Ty>
		struct remove_cvref {
		using type = remove_cvref_t<_Ty>;
	};
#ifdef __clang__
		template <class _Ty1, class _Ty2>
			constexpr bool is_same_v = __is_same(_Ty1, _Ty2);

		template <class _Ty1, class _Ty2>
			struct is_same : bool_constant<__is_same(_Ty1, _Ty2)> {};
#else // ^^^ Clang / not Clang vvv
		template <class, class>
			constexpr bool is_same_v = false; // determine whether arguments are the same type
		template <class _Ty>
		constexpr bool is_same_v<_Ty, _Ty> = true;
#endif
		template <class _Ty1, class _Ty2>
		concept _Same_impl = // Must be a distinct concept to provide symmetric subsumption for same_as
#ifdef __clang__
			__is_same(_Ty1, _Ty2);
#else // ^^^ use intrinsic / no intrinsic vvv
			is_same_v<_Ty1, _Ty2>;
#endif // ^^^ no intrinsic ^^^

		template <class _Ty, class... _Types>
		constexpr bool _Is_any_of_v = // true if and only if _Ty is in _Types
#if _HAS_CXX17
		(is_same_v<_Ty, _Types> || ...);
#else // ^^^ _HAS_CXX17 / !_HAS_CXX17 vvv
			disjunction_v<is_same<_Ty, _Types>...>;
#endif // ^^^ !_HAS_CXX17 ^^^

		constexpr bool _Is_constant_evaluated() noexcept { // Internal function for any standard mode
			return __builtin_is_constant_evaluated();
		}

#if _HAS_CXX20
		constexpr bool is_constant_evaluated() noexcept {
			return __builtin_is_constant_evaluated();
		}
#endif // _HAS_CXX20

		template <class _Ty>
			constexpr bool is_integral_v = _Is_any_of_v<remove_cv_t<_Ty>, bool, char, signed char, unsigned char, wchar_t,
#ifdef __cpp_char8_t
			char8_t,
#endif // defined(__cpp_char8_t)
			char16_t, char32_t, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

		template <class _Ty1, class _Ty2>
		concept same_as = _Same_impl<_Ty1, _Ty2>&& _Same_impl<_Ty2, _Ty1>;

		 template <class _Ty>
			concept integral = is_integral_v<_Ty>;

		template <class _Ty>
			concept signed_integral = integral<_Ty> && static_cast<_Ty>(-1) < static_cast<_Ty>(0);

		template <class _Ty>
			concept unsigned_integral = integral<_Ty> && !signed_integral<_Ty>;

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

	template <class _Container>
		constexpr auto size(const _Container& _Cont) noexcept(noexcept(_Cont.size())) /* strengthened */
		-> decltype(_Cont.size()) {
		return _Cont.size();
	}

	template <class _Ty, size_t _Size>
		constexpr size_t size(const _Ty(&)[_Size]) noexcept {
		return _Size;
	}
	
	template <class>
	constexpr bool is_lvalue_reference_v = false; // determine whether type argument is an lvalue reference

	template <class _Ty>
	constexpr bool is_lvalue_reference_v<_Ty&> = true;

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
	constexpr _Ty&& forward(remove_reference_t<_Ty>&& _Arg) noexcept
	{
		static_assert(!is_lvalue_reference_v<_Ty>, "bad forward call");
		return static_cast<_Ty&&>(_Arg);
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

		constexpr auto operator==(Flags<BitType> const& rhs) const
		{
			return mask == rhs.mask;
		}

		constexpr auto operator!=(Flags<BitType> const& rhs) const
		{
			return mask != rhs.mask;
		}

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
