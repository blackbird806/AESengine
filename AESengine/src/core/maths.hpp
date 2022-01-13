#ifndef MATH_HPP
#define MATH_HPP

namespace aes {
	
	template <typename T> constexpr int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}
	
}


#endif // !MATH_HPP
