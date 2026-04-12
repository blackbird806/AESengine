#ifndef WOB_CONTEXT_HPP
#define WOB_CONTEXT_HPP

#include "debug.hpp"
//#include "errorCodes.hpp"
//#include "stringView.hpp"
//#include "staticArray.hpp"

namespace wob
{
	struct Context
	{
		//void reportError(ErrorCode code, StringView msg, source_location loc = source_location::current());
		//ErrorEntry const& getLastError() const;
		//ErrorEntry popError();

		struct IAllocator* allocator;
		//struct ErrorContext* errorStack;
		Logger logger;
	};

	extern thread_local Context context;
}

#endif