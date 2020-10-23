#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <fmt/format.h>
#include <fmt/compile.h>

#define AES_ENSURE(x) if (!(x)) { throw ::aes::Exception("Ensure error experssion: " #x " is false"); }

#define AES_LOG(msg, ...) fmt::print("info : " msg "\n", __VA_ARGS__);
#define AES_WARN(msg, ...) fmt::print("warn : " msg "\n", __VA_ARGS__);
#define AES_LOG_ERROR(msg, ...) fmt::print(stderr,  fmt::format("{} line {} error : {}", __FUNCTION__, __LINE__, msg), __VA_ARGS__);

namespace aes {

	// @Deprecated
	void fatalError(const char* msg);

	class Exception
	{
	public:

		Exception(const char* msg) noexcept;
		virtual const char* what() const noexcept;

	private:
		const char* msg;
	};

}

#endif