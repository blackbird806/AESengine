#ifndef AES_HPP
#define AES_HPP

enum ErrorCodes
{
	AES_FATAL_ERROR = -1,
};


#ifdef _WIN32
	#define AES_DEBUG DEBUG_
#endif

#ifdef AES_DEBUG
	#ifdef _WIN32
		#define AES_DEBUG_BREAK() __debugbreak()
	#else
		#define AES_DEBUG_BREAK()  raise(SIGTRAP) 
	#endif
		#define AES_ASSERT(x) if (!(x)) { AES_DEBUG_BREAK(); }
#else
	#ifdef _WIN32	
		#define AES_ASSERT(x) __assume(x)
	#else
		#define AES_ASSERT(x) { false ? (void)(x) : (void)0; }
	#endif
#endif

using uint = unsigned int;
using uchar = unsigned char;

#include "profiler.hpp"

#endif

