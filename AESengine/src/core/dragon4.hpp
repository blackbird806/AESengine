#ifndef AES_DRAGON4_HPP
#define AES_DRAGON4_HPP
#include "aes.hpp"

namespace aes
{
	// implemntation from:
	// https://www.ryanjuckett.com/printing-floating-point-numbers-part-2-dragon4/
	namespace d4
	{
		// Maximum number of 32 bit blocks needed in high precision arithmetic
		// to print out 64 bit IEEE floating point values.
		constexpr uint32_t bigIntMaxBlocks = 35;

		struct BigInt
		{
			BigInt& operator=(BigInt const& rhs);
		
			int32_t compare(BigInt const& rhs) const;

			auto operator<=>(BigInt const& rhs) const;

			// Basic type accessors
			void SetU64(uint64_t val);

			void SetU32(uint32_t val);

			void mul2();

			void mul10();

			void pow10(uint32_t exponent);

			void setZero();

			uint32_t size;
			uint32_t blocks[bigIntMaxBlocks];
		};

		BigInt operator+(BigInt const& lhs, BigInt const& rhs);

		BigInt operator*(BigInt const& lhs, BigInt const& rhs);

		BigInt operator*(BigInt const& lhs, uint32_t rhs);

		BigInt mulBy2(BigInt const& in);

		void dragon4(float v, const char* buff);
	}

}

#endif