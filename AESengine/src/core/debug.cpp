#include "debug.hpp"
#include "aes.hpp"
#include "context.hpp"
#include <fmt/format.h>

#ifdef __vita__
#include "psvDebugScreen/debugScreen.h"
#endif

using namespace aes;

Logger& Logger::instance()
{
	return context.logger;
}

void Logger::addSink(std::unique_ptr<Sink> sink)
{
	AES_ASSERT(sink);
	sinks.push_back(std::move(sink));
}

void Logger::log(std::string&& message)
{
	for (auto& sink : sinks)
	{
		sink->dispatch_log(message.c_str());
		sink->flush();
	}
}

StreamSink::StreamSink(std::ostream& stream_) : stream(stream_)
{
	
}

void StreamSink::dispatch_log(const char* message)
{
	stream << message;
}

void StreamSink::flush()
{
	stream.flush();
}

#ifdef __vita__
PsvDebugScreenSink::PsvDebugScreenSink()
{
	psvDebugScreenInit();
}

void PsvDebugScreenSink::dispatch_log(const char* message)
{
	psvDebugScreenPuts(message);
}

void PsvDebugScreenSink::flush()
{
	
}
#endif