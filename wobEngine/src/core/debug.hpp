#ifndef WOB_DEBUG_HPP
#define WOB_DEBUG_HPP

#include "macro_helpers.hpp"
#include <cstdio>

// loging system should be completly revised
// we may want to get rif of macros and use 
// 
// 
// 
// source_location
// also we want filters, warning level and more ....
// maybe take inspiration from spd log ?
#ifdef __vita__
	#define WOB_LOG(msg, ...) ::wob::Logger::instance().log(::wob::format("info: " msg "\n" __VA_OPT__(,) __VA_ARGS__).c_str())
	#define WOB_LOG_RAW(msg, ...) ::wob::Logger::instance().wob(::wob::format(msg "\n" __VA_OPT__(,) __VA_ARGS__).c_str())
	#define WOB_WARN(msg, ...) ::wob::Logger::instance().log(::wob::format("warn: " msg "\n" __VA_OPT__(,) __VA_ARGS__).c_str())
	#define WOB_LOG_ERROR(msg, ...) ::wob::Logger::instance().log(::wob::format("error: " msg "\n" __VA_OPT__(,) __VA_ARGS__).c_str())
#else
	#define WOB_LOG(msg, ...) ::wob::Logger::instance().log(::wob::format("info: " msg "\n", __VA_ARGS__).c_str())
	#define WOB_LOG_RAW(msg, ...) ::wob::Logger::instance().log(::wob::format(msg "\n", __VA_ARGS__).c_str())
	#define WOB_WARN(msg, ...) ::wob::Logger::instance().log(::wob::format("warn: " msg "\n", __VA_ARGS__).c_str())
	#define WOB_LOG_ERROR(msg, ...) ::wob::Logger::instance().log(::wob::format("error: " __FUNCTION__ " : " WOB_STRINGIFY(__LINE__) ": " msg "\n", __VA_ARGS__).c_str())
#endif

#define WOB_CHECKR(r) if (!r) { WOB_LOG_ERROR("{}", r.error()); return r; };
#define WOB_CHECK(r) if (!r) { WOB_LOG_ERROR("{}", r.error()); };

namespace wob
{
	/// The Sink interface is used to dispatch log message
	/// a typical sink implementation will show the log message in the console or send it over network for remote debugging
	class Sink
	{
	public:
		virtual void dispatch_log(const char* message) noexcept = 0;
		virtual void flush() noexcept = 0;
		virtual ~Sink() {};
	};

	class Logger
	{
	public:
		Logger() noexcept;

		// [[deprecated("Use context logger instead")]] 
		[[nodiscard]] static Logger& instance() noexcept;

		void addSink(Sink* sink) noexcept;
		void log(const char* message) noexcept;

	private:

		Sink* sinks[8];
		int sinkCount;
	};

	class StreamSink final : public Sink
	{
	public:
		explicit StreamSink(FILE* stream) noexcept;

		void dispatch_log(const char* message) noexcept override;
		void flush() noexcept override;
	private:
		FILE* stream;
	};

#ifdef __vita__
	class PsvDebugScreenSink final : public Sink
	{
	public:
		PsvDebugScreenSink() noexcept;
		void dispatch_log(const char* message) noexcept override;
		void flush() noexcept override;
	};
#endif
	
}

#endif