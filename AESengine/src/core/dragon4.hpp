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

		enum PrintFloatFormat
		{
			PrintFloatFormat_Positional,	// [-]ddddd.dddd
			PrintFloatFormat_Scientific,	// [-]d.dddde[sign]ddd
		};


		//******************************************************************************
		// Print a 32-bit floating-point number as a decimal string.
		// The output string is always NUL terminated and the string length (not
		// including the NUL) is returned.
		//******************************************************************************
		uint32_t printFloat32
		(
			char* pOutBuffer,		// buffer to output into
			uint32_t			bufferSize,		// size of pOutBuffer
			float				value,			// value to print
			PrintFloatFormat	format,			// format to print with
			int32_t				precision		// If negative, the minimum number of digits to represent a
			// unique 32-bit floating point value is output. Otherwise,
			// this is the number of digits to print past the decimal point.
		);

		//******************************************************************************
		// Print a 64-bit floating-point number as a decimal string.
		// The output string is always NUL terminated and the string length (not
		// including the NUL) is returned.
		//******************************************************************************
		uint32_t printFloat64
		(
			char* pOutBuffer,		// buffer to output into
			uint32_t			bufferSize,		// size of pOutBuffer
			double				value,			// value to print
			PrintFloatFormat	format,			// format to print with
			int32_t				precision		// If negative, the minimum number of digits to represent a
			// unique 64-bit floating point value is output. Otherwise,
			// this is the number of digits to print past the decimal point.
		);
	}

}

#endif