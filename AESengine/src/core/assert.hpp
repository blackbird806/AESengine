#ifndef AES_ASSERT_HPP
#define AES_ASSERT_HPP

#include <cstdlib>

#ifdef AES_DEBUG
#ifdef _WIN32
#define AES_DEBUG_BREAK() __debugbreak()
#elif defined(__vita__)
#define AES_DEBUG_BREAK() std::abort()
#else
#define AES_DEBUG_BREAK() __builtin_trap()
#endif
#define AES_ASSERT(x) if (x) {} else { AES_LOG_ERROR("Assertion Failed : " #x); AES_DEBUG_BREAK(); }
#define AES_ASSERTF(x, msg, ...) if (x) {} else { AES_LOG_ERROR("Assertion Failed : " #x " " msg, __VA_ARGS__); AES_DEBUG_BREAK(); }
#define AES_ASSERT_CORE(x) if (x) {} else { AES_DEBUG_BREAK(); }
#define AES_ASSERT_NOLOG(x) AES_ASSERT_CORE(x) // legacy
#else
#define AES_ASSERT(x) AES_ASSUME(x)
#define AES_ASSERTF(x, msg, ...) AES_ASSUME(x)
#define AES_DEBUG_BREAK()
#define AES_ASSERT_CORE(x) AES_ASSUME(x)
#endif

#define AES_BOUNDS_CHECK(x) AES_ASSERT_NOLOG(x)

#define AES_NOT_IMPLEMENTED() AES_DEBUG_BREAK()

#endif