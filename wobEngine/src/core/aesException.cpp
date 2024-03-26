#include "aesException.hpp"

using namespace aes;

AESException::AESException(std::string const& str) noexcept :
	what_(str)
{
}

RHIException::RHIException(std::string const& str) noexcept
	: AESException(str)
{
}
