#ifndef AES_MATH_HPP
#define AES_MATH_HPP

#include <cstdint>

#define _USE_MATH_DEFINES
#include <math.h>

namespace aes {
	
	template <typename T> constexpr int sign(T val) noexcept {
		return (T(0) < val) - (val < T(0));
	}

	template <typename T> constexpr T abs(T val) noexcept {
		return val > 0 ? val : -val;
	}

	constexpr double degToRad = M_PI / 180.0;

	// https://stackoverflow.com/a/1489873
	// '-' count as a digit
	template<typename T>
	constexpr int numDigits(T number)
	{
		int digits = 0;
		if (number < 0) digits = 1; // remove this line if '-' counts as a digit
		while (number) {
			number /= 10;
			digits++;
		}
		return digits;
	}

	template<>
	constexpr int numDigits(uint32_t x) {
		if (x >= 10000u) {
			if (x >= 10000000u) {
				if (x >= 100000000u) {
					if (x >= 1000000000u) {
						if (x >= 10000000000u)
							return 11;
						return 10;
					}
					return 9;
				}
				return 8;
			}
			if (x >= 100000u) {
				if (x >= 1000000u)
					return 7;
				return 6;
			}
			return 5;
		}
		if (x >= 100) {
			if (x >= 1000)
				return 4;
			return 3;
		}
		if (x >= 10)
			return 2;
		return 1;
	}

	template<>
	constexpr int numDigits(uint64_t x) {
			if (x >= 10000000000ull) {
				if (x >= 100000000000000ull) {
					if (x >= 10000000000000000ull) {
						if (x >= 100000000000000000ull) {
							if (x >= 1000000000000000000ull) {
								if (x >= 10000000000000000000ull)
									return 20;
								return 19;
							}
						return 18;
						}
						return 17;
					}
					if (x >= 1000000000000000ull)
						return 16;
					return 15;
				}
				if (x >= 1000000000000ull) {
					if (x >= 10000000000000ull)
						return 14;
					return 13;
				}
				if (x >= 100000000000ull)
					return 12;
				return 11;
			}
			if (x >= 100000ull) {
				if (x >= 10000000ull) {
					if (x >= 100000000ull) {
						if (x >= 1000000000ull)
							return 10;
						return 9;
					}
					return 8;
				}
				if (x >= 1000000ull)
					return 7;
				return 6;
			}
			if (x >= 100ull) {
				if (x >= 1000ull) {
					if (x >= 10000ull)
						return 5;
					return 4;
				}
				return 3;
			}
			if (x >= 10)
				return 2;
			return 1;
	}

	template <>
	constexpr int numDigits(int64_t x) {
		if (x == INT64_MIN) return 19 + 1;
		if (x < 0) return numDigits(-x) + 1;

		if (x >= 10000000000) {
			if (x >= 100000000000000) {
				if (x >= 10000000000000000) {
					if (x >= 100000000000000000) {
						if (x >= 1000000000000000000)
							return 19;
						return 18;
					}
					return 17;
				}
				if (x >= 1000000000000000)
					return 16;
				return 15;
			}
			if (x >= 1000000000000) {
				if (x >= 10000000000000)
					return 14;
				return 13;
			}
			if (x >= 100000000000)
				return 12;
			return 11;
		}
		if (x >= 100000) {
			if (x >= 10000000) {
				if (x >= 100000000) {
					if (x >= 1000000000)
						return 10;
					return 9;
				}
				return 8;
			}
			if (x >= 1000000)
				return 7;
			return 6;
		}
		if (x >= 100) {
			if (x >= 1000) {
				if (x >= 10000)
					return 5;
				return 4;
			}
			return 3;
		}
		if (x >= 10)
			return 2;
		return 1;
	}

	template<>
	constexpr int numDigits(int32_t x)
	{
		if (x == INT32_MIN) return 10 + 1;
		if (x < 0) return numDigits(-x) + 1;

		if (x >= 10000) {
			if (x >= 10000000) {
				if (x >= 100000000) {
					if (x >= 1000000000)
						return 10;
					return 9;
				}
				return 8;
			}
			if (x >= 100000) {
				if (x >= 1000000)
					return 7;
				return 6;
			}
			return 5;
		}
		if (x >= 100) {
			if (x >= 1000)
				return 4;
			return 3;
		}
		if (x >= 10)
			return 2;
		return 1;
	}

}

#endif // !MATH_HPP
