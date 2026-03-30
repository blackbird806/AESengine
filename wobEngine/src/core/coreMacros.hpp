#ifndef WOB_CORE_MACROS_HPP
#define WOB_CORE_MACROS_HPP

#define WOB_UNUSED(x) ((void)(x))

#if _MSC_VER
#ifdef _M_X86
#define WOB_ARCH_X86
#endif
#endif

#if __GNUC__ || __clang__
#ifdef __i386__
#define WOB_ARCH_X86
#endif
#endif

#ifdef _MSC_VER	
#define WOB_ASSUME(x) __assume(x)
#define WOB_RESTRICT(x) __restrict x
#elif defined(__clang__)
#define WOB_ASSUME(x) __builtin_assume(x)
#define WOB_RESTRICT(x) __restrict__ x
#elif defined (__GNUC__)
#define WOB_RESTRICT(x) __restrict__ x
#define WOB_ASSUME(x)
#else
#define WOB_ASSUME(x)
#define WOB_RESTRICT(x) x
#endif

#ifdef WOB_RELEASE
#ifdef _MSC_VER
#define WOB_UNREACHABLE() __assume(0)
#elif defined(__GNUC__) || defined(__clang__)
#define WOB_UNREACHABLE() __builtin_unreachable()
#endif
#else
#define WOB_UNREACHABLE() WOB_ASSERT(false);
#endif

#endif