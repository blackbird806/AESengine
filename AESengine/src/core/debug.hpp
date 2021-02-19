#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <vector>
#include <ostream>
#include <memory>
#include <fmt/format.h>
#include "macro_helpers.hpp"

// Deprecated
#define AES_ENSURE(x) if (!(x)) { AES_ERROR("Ensure error experssion: " #x " is false"); }

#ifdef __vita__
#define AES_LOG(msg, ...) ::aes::Logger::instance().log(fmt::format("info : " msg "\n" __VA_OPT__(,) __VA_ARGS__))
#define AES_WARN(msg, ...) ::aes::Logger::instance().log(fmt::format("warn : " msg "\n" __VA_OPT__(,) __VA_ARGS__))
#define AES_LOG_ERROR(msg, ...) ::aes::Logger::instance().log(fmt::format("function {} line {} error {}\n", __FUNCTION__, __LINE__, fmt::format(msg __VA_OPT__(,) __VA_ARGS__)))
#else
#define AES_LOG(msg, ...) ::aes::Logger::instance().log(fmt::format("info : " msg "\n", __VA_ARGS__))
#define AES_WARN(msg, ...) ::aes::Logger::instance().log(fmt::format("warn : " msg "\n", __VA_ARGS__))
#define AES_LOG_ERROR(msg, ...) ::aes::Logger::instance().log(fmt::format(__FUNCTION__ " line " AES_STRINGIFY(__LINE__) " error : " msg "\n", __VA_ARGS__))
#endif

// @Review
#ifdef AES_DEBUG
#define AES_ERROR(msg, ...) AES_DEBUG_BREAK();
#else
#define  AES_ERROR(msg, ...) AES_LOG_ERROR(msg, __VA_ARGS__);
#endif

namespace aes {

	class Sink
	{
	public:
		virtual void dispatch_log(const char* message) = 0;
		virtual void flush() = 0;
		virtual ~Sink() {};
	};

	class Logger
	{
		
	public:

		static Logger& instance();
		
		void addSink(std::unique_ptr<Sink> sink);
		void log(std::string&& message);

	private:

		std::vector<std::unique_ptr<Sink>> sinks;
	};

	class StreamSink final : public Sink
	{
	public:
		explicit StreamSink(std::ostream& stream);

		void dispatch_log(const char* message) override;
		void flush() override;
	private:
		std::ostream& stream;
	};

#ifdef __vita__
	class PsvDebugScreenSink final : public Sink
	{
	public:
		PsvDebugScreenSink();
		void dispatch_log(const char* message) override;
		void flush() override;
	};
#endif
	
}

#endif