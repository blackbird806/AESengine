#ifndef DEBUG_HPP
#define DEBUG_HPP


// Deprecated
#define AES_ENSURE(x) if (!(x)) { AES_ERROR("Ensure error experssion: " #x " is false"); }

// TODO: true logging system
#ifndef __vita__

#include <fmt/format.h>

#define AES_LOG(msg, ...) fmt::print("info : " msg "\n", __VA_ARGS__)
#define AES_WARN(msg, ...) fmt::print("warn : " msg "\n", __VA_ARGS__)
#define AES_LOG_ERROR(msg, ...) fmt::print(stderr,  fmt::format("{} line {} error : {}\n", __FUNCTION__, __LINE__, msg), __VA_ARGS__)

#else

//#define AES_LOG(msg, ...) fmt::print("info : " msg "\n" __VA_OPT__(,) __VA_ARGS__)
#define AES_LOG(msg, ...) 
#define AES_WARN(msg, ...) 
#define AES_LOG_ERROR(msg, ...)

#endif

// @Review
#ifdef AES_DEBUG
#define AES_ERROR(msg, ...) AES_DEBUG_BREAK();
#else
#define  AES_ERROR(msg, ...) AES_LOG_ERROR(msg, __VA_ARGS__);
#endif

namespace aes {


}

#endif