#ifndef AES_EXCEPTION_HPP
#define AES_EXCEPTION_HPP

#include <string>

namespace aes
{
	class AESException
	{
	public:
		AESException(std::string const& str) noexcept;
		AESException(AESException const& rhs) noexcept = default;
		AESException(AESException&& rhs) noexcept = default;

		std::string const& what() const noexcept
		{
			return what_;
		}

	private:
		std::string what_;
	};

	class RHIException : public AESException
	{
	public:
		RHIException(std::string const& str) noexcept;
	};
}

#endif