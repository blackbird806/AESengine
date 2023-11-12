#ifndef AES_HPP
#define AES_HPP

#include <cstdlib>

#define AES_UNUSED(x) ((void)(x))

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

#include "assert.hpp"
#include "profiler.hpp"
#include "debug.hpp"

#define AES_FATAL_ERROR(msg) {AES_LOG_ERROR(msg); AES_DEBUG_BREAK(); std::abort();}

#endif

