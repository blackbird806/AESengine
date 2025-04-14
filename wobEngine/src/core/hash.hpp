#ifndef AES_HASH_HPP
#define AES_HASH_HPP

#include <cstdint>

namespace aes
{
	template<typename T>
	struct Hash
	{
	};

	// https://stackoverflow.com/a/12996028
	template<>
	struct Hash<int32_t>
	{
		constexpr uint64_t operator()(int32_t x)
		{
			x = ((x >> 16) ^ x) * 0x45d9f3b;
			x = ((x >> 16) ^ x) * 0x45d9f3b;
			x = (x >> 16) ^ x;
			return x;
		}
	};

	template<>
	struct Hash<int64_t>
	{
		constexpr uint64_t operator()(int64_t x)
		{
			x = ((x >> 16) ^ x) * 0x45d9f3b;
			x = ((x >> 16) ^ x) * 0x45d9f3b;
			x = (x >> 16) ^ x;
			return x;
		}
	};

	template<>
	struct Hash<const char*>
	{
		// https://stackoverflow.com/a/8317622
		constexpr uint64_t operator()(const char* str)
		{
			constexpr uint64_t A = 54059; /* a prime */
			constexpr uint64_t B = 76963; /* another prime */
			//constexpr uint64_t C = 86969; /* yet 1another prime */
			constexpr uint64_t FIRSTH = 37; /* also prime */
			unsigned h = FIRSTH;
			auto* s = str;
			while (*s) {
				h = (h * A) ^ (s[0] * B);
				s++;
			}
			return h; // or return h % C;
		}
	};
}

#endif // !AES_HASH_HPP
