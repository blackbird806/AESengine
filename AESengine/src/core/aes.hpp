#ifndef AES_HPP
#define AES_HPP

#define AES_UNUSED(x) ((void)(x))

#ifdef _WIN32
	#define AES_DEBUG DEBUG_
#endif

#define AES_ENABLE_EXCEPTION

#ifdef AES_ENABLE_EXCEPTION
	#define AES_THROW(x) throw x;
#else
	#define AES_THROW(x)
#endif

#ifdef AES_DEBUG
	#ifdef _WIN32
		#define AES_DEBUG_BREAK() __debugbreak()
	#else
		#define AES_DEBUG_BREAK()  raise(SIGTRAP) 
	#endif
	#define AES_ASSERT(x) if (x) {} else { AES_DEBUG_BREAK(); }
#else
	#ifdef _WIN32	
		#define AES_ASSERT(x) __assume(x)
	#else
		#define AES_ASSERT(x)
	#endif
#endif

#ifdef _MSC_VER
	#define AES_UNREACHABLE() __assume(0)
#elif defined(__GNUC__) || defined(__clang__)
	#define AES_UNREACHABLE() __builtin_unreachable()
#endif

using uint = unsigned int;
using uchar = unsigned char;

#include "profiler.hpp"

#endif

