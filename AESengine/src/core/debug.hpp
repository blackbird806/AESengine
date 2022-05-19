#ifndef AES_DEBUG_HPP
#define AES_DEBUG_HPP

#include <vector>
#include <iosfwd>
#include <memory>
#include <fmt/format.h>
#include "macro_helpers.hpp"

#ifdef __vita__
#define AES_LOG(msg, ...) ::aes::Logger::instance().log(fmt::format("info : " msg "\n" __VA_OPT__(,) __VA_ARGS__))
#define AES_LOG_RAW(msg, ...) ::aes::Logger::instance().log(fmt::format(msg "\n" __VA_OPT__(,) __VA_ARGS__))
#define AES_WARN(msg, ...) ::aes::Logger::instance().log(fmt::format("warn : " msg "\n" __VA_OPT__(,) __VA_ARGS__))
#define AES_LOG_ERROR(msg, ...) ::aes::Logger::instance().log(fmt::format("error {} {} line {} : {}\n", __FUNCTION__, __FILE__, __LINE__, fmt::format(msg __VA_OPT__(,) __VA_ARGS__)))
#else
#define AES_LOG(msg, ...) ::aes::Logger::instance().log(fmt::format("info : " msg "\n", __VA_ARGS__))
#define AES_LOG_RAW(msg, ...) ::aes::Logger::instance().log(fmt::format(msg "\n", __VA_ARGS__))
#define AES_WARN(msg, ...) ::aes::Logger::instance().log(fmt::format("warn : " msg "\n", __VA_ARGS__))
#define AES_LOG_ERROR(msg, ...) ::aes::Logger::instance().log(fmt::format(" error : " __FUNCTION__ " : " AES_STRINGIFY(__LINE__) ": " msg "\n", __VA_ARGS__))
#endif

#define AES_CHECKR(r) if (!r) { AES_LOG_ERROR("{}", r.error()); return r; };
#define AES_CHECK(r) if (!r) { AES_LOG_ERROR("{}", r.error()); };

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