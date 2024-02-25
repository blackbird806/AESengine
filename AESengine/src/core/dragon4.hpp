#ifndef AES_DRAGON4_HPP
#define AES_DRAGON4_HPP
#include "aes.hpp"

namespace aes
{
	// implementation from:
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

			int32_t operator<=>(BigInt const& rhs) const;

			// Basic type accessors
			void SetU64(uint64_t val);

			void SetU32(uint32_t val);

			void mul2();

			void mul10();

			void pow10(uint32_t exponent);

			BigInt mulPow10(uint32_t exponent);

			void pow2(uint32_t exponent);

			void setZero();
			bool isZero() const;

			uint32_t size;
			uint32_t blocks[bigIntMaxBlocks];
		};

		BigInt operator+(BigInt const& lhs, BigInt const& rhs);

		BigInt operator*(BigInt const& lhs, BigInt const& rhs);

		BigInt operator*(BigInt const& lhs, uint32_t rhs);

		BigInt mulBy2(BigInt const& in);

		//******************************************************************************
		// Different modes for terminating digit output
		//******************************************************************************
		enum CutoffMode
		{
			CutoffMode_Unique,			// as many digits as necessary to print a uniquely identifiable number
			CutoffMode_TotalLength,		// up to cutoffNumber significant digits
			CutoffMode_FractionLength,	// up to cutoffNumber significant digits past the decimal point
		};

	}

}

#endif