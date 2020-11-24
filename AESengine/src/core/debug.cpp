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
	exit(-1);
}

Exception::Exception(const char* msg_) noexcept : exception(msg_)
{

}

