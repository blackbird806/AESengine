#ifndef AES_MATH_HPP
#define AES_MATH_HPP

#include <cstdint>

namespace aes {
	
	template <typename T> constexpr int sign(T val) noexcept {
		return (T(0) < val) - (val < T(0));
	}

	template <typename T> constexpr T abs(T val) noexcept {
		return val > 0 ? val : -val;
	}

	//https://stackoverflow.com/a/1498561
	constexpr int numDigits(uint32_t x) {
		return (x < 10 ? 1 :   
			(x < 100 ? 2 :   
			(x < 1000 ? 3 :   
			(x < 10000 ? 4 :   
			(x < 100000 ? 5 :   
			(x < 1000000 ? 6 :   
			(x < 10000000 ? 7 :  
			(x < 100000000 ? 8 :  
			(x < 1000000000 ? 9 :  
			10)))))))));
	}
}

#endif // !MATH_HPP
