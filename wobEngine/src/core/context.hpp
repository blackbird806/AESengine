#ifndef WOB_CONTEXT_HPP
#define WOB_CONTEXT_HPP

#include "allocator.hpp"
#include "debug.hpp"

namespace wob
{
	struct Context
	{
		IAllocator* allocator;
		wob::Logger logger;
	};

	extern thread_local Context context;
}

#endif