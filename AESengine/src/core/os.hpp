#ifndef OS_HPP
#define OS_HPP

#ifdef _WIN32

	#define WINDOWS_LEAN_AND_MEAN
	#include <windows.h>

#elif defined(__vita__)

	#include <vitasdk.c>

#endif

#endif