#ifndef AES_MATH_HPP
#define AES_MATH_HPP

namespace aes {
	
	template <typename T> constexpr int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}
	
}


#endif // !MATH_HPP
