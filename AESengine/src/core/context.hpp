#ifndef AES_CONTEXT_HPP
#define AES_CONTEXT_HPP

#include "allocator.hpp"
#include "debug.hpp"

namespace aes
{
	struct Context
	{
		IAllocator* allocator;
		Logger logger;
	};

	extern thread_local Context context;
}

#endif