#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <exception>
#include <fmt/format.h>
#include <fmt/compile.h>

#include "aes.hpp"

// Deprecated
#define AES_ENSURE(x) if (!(x)) { throw ::aes::Exception("Ensure error experssion: " #x " is false"); }

#define AES_LOG(msg, ...) fmt::print("info : " msg "\n", __VA_ARGS__);
#define AES_WARN(msg, ...) fmt::print("warn : " msg "\n", __VA_ARGS__);
#define AES_LOG_ERROR(msg, ...) fmt::print(stderr,  fmt::format("{} line {} error : {}\n", __FUNCTION__, __LINE__, msg), __VA_ARGS__);

#ifdef AES_DEBUG
#define  AES_ERROR(msg, ...) AES_DEBUG_BREAK();
#else
#define  AES_ERROR(msg, ...)AES_LOG_ERROR(msg, __VA_ARGS__);
#endif
namespace aes {

	// @TODO move these into error
	// @Deprecated
	void fatalError(const char* msg);

	class Exception : std::exception
	{
	public:

		Exception(const char* msg) noexcept;
	};

}

#endif