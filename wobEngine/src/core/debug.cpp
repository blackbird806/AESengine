#include "debug.hpp"
#include "wob.hpp"
#include "context.hpp"
#include "format.hpp"
#include "utility.hpp"
#include <cstdio>

#ifdef __vita__
#include "psvDebugScreen/debugScreen.h"
#endif

using namespace wob;

Logger::Logger() noexcept : sinkCount(0)
{
	for (auto& sink : sinks)
		sink = nullptr;
}

Logger& Logger::instance() noexcept
{
	return context.logger;
}

void Logger::addSink(Sink* sink) noexcept
{
	WOB_ASSERT(sink);
	WOB_ASSERT(sinkCount < wob::size(sinks));
	sinks[sinkCount++] = sink;
}

void Logger::log(const char* message) noexcept
{
	for (int i = 0; i < sinkCount; i++)
	{
		sinks[i]->dispatch_log(message);
		sinks[i]->flush();
	}
}

StreamSink::StreamSink(FILE* instream) noexcept : stream(instream)
{
	
}

void StreamSink::dispatch_log(const char* message) noexcept
{
	fputs(message, stream);
}

void StreamSink::flush() noexcept
{
	fflush(stream);
}

#ifdef __vita__
PsvDebugScreenSink::PsvDebugScreenSink() noexcept
{
	psvDebugScreenInit();
}

void PsvDebugScreenSink::dispatch_log(const char* message) noexcept
{
	psvDebugScreenPuts(message);
}

void PsvDebugScreenSink::flush() noexcept
{
	
}
#endif