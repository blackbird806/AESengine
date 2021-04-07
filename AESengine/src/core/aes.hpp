#ifndef AES_HPP
#define AES_HPP

#define AES_UNUSED(x) ((void)(x))

#ifndef AES_DEBUG
	#define AES_RELEASE
#endif

#ifdef _MSC_VER	
	#define AES_ASSUME(x) __assume(x)
#elif defined(__clang__)
	#define AES_ASSUME(x) __builtin_assume(x)
#else
	#define AES_ASSUME(x)
#endif

#ifdef AES_DEBUG
	#ifdef _WIN32
		#define AES_DEBUG_BREAK() __debugbreak()
	#else
		#define AES_DEBUG_BREAK() __builtin_trap()
	#endif
	#define AES_ASSERT(x) if (x) {} else { AES_LOG_ERROR("Assertion Failed : " #x); AES_DEBUG_BREAK(); }
#else
		#define AES_ASSERT(x) AES_ASSUME(x)
#endif

#define AES_NOT_IMPLEMENTED() AES_DEBUG_BREAK()

#ifdef AES_RELEASE
	#ifdef _MSC_VER
		#define AES_UNREACHABLE() __assume(0)
	#elif defined(__GNUC__) || defined(__clang__)
		#define AES_UNREACHABLE() __builtin_unreachable()
	#endif
#else
	#define AES_UNREACHABLE() AES_ASSERT(false);
#endif

using uint = unsigned int;
using uchar = unsigned char;

#include "profiler.hpp"
#include "debug.hpp"

#endif

