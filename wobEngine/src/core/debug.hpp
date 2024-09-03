#ifndef AES_DEBUG_HPP
#define AES_DEBUG_HPP

#include <iosfwd>
#include "macro_helpers.hpp"
#include "format.hpp"

// loging system should be completly revised
// we may want to get rif of macros and use std::source_location
// also we want filters, warning level and more ....
// maybe take inspiration from spd log ?
#ifdef __vita__
#define AES_LOG(msg, ...) ::aes::Logger::instance().log(aes::format("info : " msg "\n" __VA_OPT__(,) __VA_ARGS__).c_str())
#define AES_LOG_RAW(msg, ...) ::aes::Logger::instance().log(aes::format(msg "\n" __VA_OPT__(,) __VA_ARGS__).c_str())
#define AES_WARN(msg, ...) ::aes::Logger::instance().log(aes::format("warn : " msg "\n" __VA_OPT__(,) __VA_ARGS__).c_str())
#define AES_LOG_ERROR(msg, ...) ::aes::Logger::instance().log(aes::format("error : " msg "\n" __VA_OPT__(,) __VA_ARGS__).c_str())
#else
#define AES_LOG(msg, ...) ::aes::Logger::instance().log(aes::format("info : " msg "\n", __VA_ARGS__).c_str())
#define AES_LOG_RAW(msg, ...) ::aes::Logger::instance().log(aes::format(msg "\n", __VA_ARGS__).c_str())
#define AES_WARN(msg, ...) ::aes::Logger::instance().log(aes::format("warn : " msg "\n", __VA_ARGS__).c_str())
#define AES_LOG_ERROR(msg, ...) ::aes::Logger::instance().log(aes::format("error : " __FUNCTION__ " : " AES_STRINGIFY(__LINE__) ": " msg "\n", __VA_ARGS__).c_str())
#endif

#define AES_CHECKR(r) if (!r) { AES_LOG_ERROR("{}", r.error()); return r; };
#define AES_CHECK(r) if (!r) { AES_LOG_ERROR("{}", r.error()); };

namespace aes {
	// The Sink interface is used to dispatch log message
	// a typical sink implementation will show the log message in the console or send it over network for remote debugging
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
		explicit StreamSink(::std::ostream& stream) noexcept;

		void dispatch_log(const char* message) noexcept override;
		void flush() noexcept override;
	private:
		::std::ostream& stream;
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