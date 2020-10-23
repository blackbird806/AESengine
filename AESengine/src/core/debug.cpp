#include "debug.hpp"

#include "aes.hpp"
#include "os.hpp"
#include "string_converter.hpp"

#include <cstdio>
#include <string>

using namespace aes;

void aes::fatalError(const char* msg)
{
	AES_LOG_ERROR(msg);
	MessageBox(NULL, aes::to_wstring(std::string(msg)).c_str(), L"Fatal error", MB_ICONERROR);
	exit(AES_FATAL_ERROR);
}

Exception::Exception(const char* msg_) noexcept : msg(msg_)
{

}

const char* Exception::what() const noexcept
{
	return msg;
}
