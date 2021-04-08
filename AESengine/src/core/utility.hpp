#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <vector>
#include "macro_helpers.hpp"

#define AES_SCOPE(code) ::aes::Scope AES_CONCAT(aes_scope_internal_, __COUNTER__) ([&](){code;});
#define AES_ONCE(code) static short const AES_CONCAT(aes_once_internal_, __COUNTER__) = [&]() {code; return 0;}();

namespace aes {
	
	std::string readFile(std::string const& file);
	std::vector<unsigned char> readFileBin(std::string const& file);
	
	constexpr uintptr_t align(uintptr_t x, uint32_t a)
	{
		return (x + (a - 1)) & ~(a - 1);
	}

	constexpr bool isPowerOf2(size_t n)
	{
		return (n > 0 && ((n & (n - 1)) == 0));
	}

	constexpr bool isAligned(uintptr_t p, uint32_t a)
	{
		return p % a == 0;
	}
	
	template<typename F>
	struct Scope
	{
		Scope(F&& fn) : func(std::forward<F>(fn))
		{
			
		}

		~Scope()
		{
			func();
		}
		
	private:
		F func;
	};
}

#endif // !UTILITY_HPP
