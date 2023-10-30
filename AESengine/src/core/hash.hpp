#ifndef AES_HASH_HPP
#define AES_HASH_HPP

#include "aes.hpp"

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

}

#endif // !AES_HASH_HPP
