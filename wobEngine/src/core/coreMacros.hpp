#ifndef AES_CORE_MACROS_HPP
#define AES_CORE_MACROS_HPP

#define AES_UNUSED(x) ((void)(x))

#ifdef _MSC_VER	
#define AES_ASSUME(x) __assume(x)
#define AES_RESTRICT(x) __restrict x
#elif defined(__clang__)
#define AES_ASSUME(x) __builtin_assume(x)
#define AES_RESTRICT(x) __restrict__ x
#elif defined (__GNUC__)
#define AES_RESTRICT(x) __restrict__ x
#define AES_ASSUME(x)
#else
#define AES_ASSUME(x)
#define AES_RESTRICT(x) x
#endif

#ifdef AES_RELEASE
#ifdef _MSC_VER
#define AES_UNREACHABLE() __assume(0)
#elif defined(__GNUC__) || defined(__clang__)
#define AES_UNREACHABLE() __builtin_unreachable()
#endif
#else
#define AES_UNREACHABLE() AES_ASSERT(false);
#endif

#endif