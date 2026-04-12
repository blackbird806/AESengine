#include "context.hpp"
#include "allocator.hpp"
#include "string.hpp"

thread_local wob::Context wob::context{&wob::profilerAlloc};

namespace wob
{
	//struct ErrorEntry
	//{
	//	ErrorCode code;
	//	String message;
	//};

	//struct ErrorContext
	//{
	//	static constexpr int StackSize = 32;

	//	void reportError(ErrorCode code, StringView msg, source_location loc = source_location::current());

	//	ErrorEntry const& getLastError() const;
	//	ErrorEntry popError();

	//	bool printErrors = true;

	//	// TODO circular buffer
	//	int index = 0;
	//	StaticArray<ErrorEntry, StackSize> errorStack;
	//};

	//void ErrorContext::reportError(ErrorCode code, StringView msg, source_location loc)
	//{
	//	if (index == StackSize)
	//	{
	//		index = 0;
	//	}

	//	errorStack[index] = ErrorEntry{ code, String(msg) };
	//	index++;

	//	if (printErrors)
	//	{
	//		// use source location here ?
	//		WOB_UNUSED(loc);
	//		ErrorEntry const& entry = getLastError();
	//		context.logger.log(cFormat("error: %d %s - %s", entry.code, to_string(entry.code), entry.message).c_str());
	//	}
	//}

	//ErrorEntry const& ErrorContext::getLastError() const
	//{
	//	return errorStack.back();
	//}

	//ErrorEntry ErrorContext::popError()
	//{
	//	return errorStack[index--];
	//}

	//void Context::reportError(ErrorCode code, StringView msg, source_location loc)
	//{
	//	errorStack->reportError(code, msg, loc);
	//}

	//ErrorEntry const& Context::getLastError() const
	//{
	//	errorStack->getLastError();
	//}

	//ErrorEntry Context::popError()
	//{
	//	return errorStack->popError();
	//}
}