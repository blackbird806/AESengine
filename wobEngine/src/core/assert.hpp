#ifndef WOB_ASSERT_HPP
#define WOB_ASSERT_HPP

#include <cstdlib>

#define WOB_ASSERT_NOLOG(x) WOB_ASSERT_CORE(x) // legacy

#ifdef WOB_DEBUG
#define WOB_DEBUG
#ifdef _WIN32
#define WOB_DEBUG_BREAK() __debugbreak()
#elif defined(__vita__)
#define WOB_DEBUG_BREAK() abort()
#else
#define WOB_DEBUG_BREAK() __builtin_trap()
#endif
#define WOB_ASSERT(x) if (x) {} else { WOB_LOG_ERROR("Assertion Failed : " #x); WOB_DEBUG_BREAK(); }
#define WOB_ASSERTF(x, msg, ...) if (x) {} else { WOB_LOG_ERROR("Assertion Failed : " #x " " msg, __VA_ARGS__); WOB_DEBUG_BREAK(); }
#define WOB_ASSERT_CORE(x) if (x) {} else { WOB_DEBUG_BREAK(); }
#else
#define WOB_ASSERT(x) WOB_ASSUME(x)
#define WOB_ASSERTF(x, msg, ...) WOB_ASSUME(x)
#define WOB_DEBUG_BREAK()
#define WOB_ASSERT_CORE(x) WOB_ASSUME(x)
#endif

#define WOB_BOUNDS_CHECK(x) WOB_ASSERT_NOLOG(x)

#define WOB_NOT_IMPLEMENTED() WOB_DEBUG_BREAK()

#define WOB_FATAL_ERROR(msg) {WOB_LOG_ERROR(msg); WOB_DEBUG_BREAK(); abort();}

#endif