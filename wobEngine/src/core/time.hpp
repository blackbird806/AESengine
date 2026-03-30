#ifndef WOB_TIME_HPP
#define WOB_TIME_HPP

#include <time.h>
#include "os.hpp"

namespace wob
{
#ifdef _WIN32
	
	//https://stackoverflow.com/a/1825740
	long long getPerfCount()
	{
		LARGE_INTEGER ret;
		QueryPerformanceCounter(&ret);
		return ret.QuadPart;
	}

	long long getCPUFrequency()
	{
		LARGE_INTEGER ret;
		QueryPerformanceFrequency(&ret);
		return ret.QuadPart;
	}

#endif

}

#endif