#ifndef WOB_CONTEXT_HPP
#define WOB_CONTEXT_HPP

#include "debug.hpp"

namespace wob
{
	struct Context
	{
		struct IAllocator* allocator;
		Logger logger;
	};

	extern thread_local Context context;
}

#endif