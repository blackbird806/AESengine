#include "debug.hpp"
#include "aes.hpp"
#include "context.hpp"
#include "format.hpp"
#include <iostream>

#ifdef __vita__
#include "psvDebugScreen/debugScreen.h"
#endif

using namespace aes;

Logger::Logger() noexcept : sinkCount(0)
{
	std::fill(std::begin(sinks), std::end(sinks), nullptr);
}

Logger& Logger::instance() noexcept
{
	return context.logger;
}

void Logger::addSink(Sink* sink) noexcept
{
	AES_ASSERT(sink);
	AES_ASSERT(sinkCount < std::size(sinks));
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

StreamSink::StreamSink(std::ostream& stream_) noexcept : stream(stream_)
{
	
}

void StreamSink::dispatch_log(const char* message) noexcept
{
	stream << message;
}

void StreamSink::flush() noexcept
{
	stream.flush();
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