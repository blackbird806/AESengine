#include "dragon4.hpp"

using namespace aes::d4;

static uint32_t g_PowerOf10_U32[] =
{
	1,          // 10 ^ 0
	10,         // 10 ^ 1
	100,        // 10 ^ 2
	1000,       // 10 ^ 3
	10000,      // 10 ^ 4
	100000,     // 10 ^ 5
	1000000,    // 10 ^ 6
	10000000,   // 10 ^ 7
};

static BigInt g_PowerOf10_Big[] =
{
	// 10 ^ 8
	{ 1, { 100000000 } },
	// 10 ^ 16
	{ 2, { 0x6fc10000, 0x002386f2 } },
	// 10 ^ 32
	{ 4, { 0x00000000, 0x85acef81, 0x2d6d415b, 0x000004ee, } },
	// 10 ^ 64
	{ 7, { 0x00000000, 0x00000000, 0xbf6a1f01, 0x6e38ed64, 0xdaa797ed, 0xe93ff9f4, 0x00184f03, } },
	// 10 ^ 128
	{ 14, { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x2e953e01, 0x03df9909, 0x0f1538fd,
			0x2374e42f, 0xd3cff5ec, 0xc404dc08, 0xbccdb0da, 0xa6337f19, 0xe91f2603, 0x0000024e, } },
			// 10 ^ 256
	{ 27, { 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000, 0x982e7c01, 0xbed3875b, 0xd8d99f72, 0x12152f87, 0x6bde50c6, 0xcf4a6e70,
			0xd595d80f, 0x26b2716e, 0xadc666b0, 0x1d153624, 0x3c42d35a, 0x63ff540e, 0xcc5573c0,
			0x65f9ef17, 0x55bc28f2, 0x80dcc7f7, 0xf46eeddc, 0x5fdcefce, 0x000553f7, } }
};

//******************************************************************************
// Get the log base 2 of a 32-bit unsigned integer.
// http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogLookup
//******************************************************************************
static uint32_t LogBase2(uint32_t val)
{
	static const uint8_t logTable[256] =
	{
		0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
	};

	uint32_t temp = val >> 24;
	if (temp)
		return 24 + logTable[temp];

	temp = val >> 16;
	if (temp)
		return 16 + logTable[temp];

	temp = val >> 8;
	if (temp)
		return 8 + logTable[temp];

	return logTable[val];
}

static uint32_t LogBase2(uint64_t val)
{
	uint64_t temp = val >> 32;
	if (temp)
		return 32 + LogBase2((uint32_t)temp);

	return LogBase2((uint32_t)val);
}

BigInt& BigInt::operator=(BigInt const& rhs)
{
	size = rhs.size;
	memcpy(blocks, rhs.blocks, size * sizeof(uint32_t));
	return *this;
}

int32_t BigInt::compare(BigInt const& rhs) const
{
	int32_t const lengthDiff = size - rhs.size;
	if (lengthDiff != 0)
		return lengthDiff;

	for (int32_t i = size - 1; i >= 0; --i)
	{
		if (blocks[i] == rhs.blocks[i])
			continue;
		else if (blocks[i] > blocks[i])
			return 1;
		else
			return -1;
	}

	// no blocks differed
	return 0;
}

int32_t BigInt::operator<=>(BigInt const& rhs) const
{
	return compare(rhs);
}

// Basic type accessors
void BigInt::SetU64(uint64_t val)
{
	if (val > 0xFFFFFFFF)
	{
		blocks[0] = val & 0xFFFFFFFF;
		blocks[1] = (val >> 32) & 0xFFFFFFFF;
		size = 2;
	}
	else if (val != 0)
	{
		blocks[0] = val & 0xFFFFFFFF;
		size = 1;
	}
	else
	{
		size = 0;
	}
}

void BigInt::SetU32(uint32_t val)
{
	if (val != 0)
	{
		blocks[0] = val;
		size = (val != 0);
	}
	else
	{
		size = 0;
	}
}

void BigInt::mul2()
{
	// shift all the blocks by one
	uint32_t carry = 0;

	uint32_t* pCur = blocks;
	uint32_t* pEnd = blocks + size;
	for (; pCur != pEnd; ++pCur)
	{
		uint32_t cur = *pCur;
		*pCur = (cur << 1) | carry;
		carry = cur >> 31;
	}

	if (carry != 0)
	{
		// grow the array
		AES_ASSERT(size + 1 <= bigIntMaxBlocks);
		*pCur = carry;
		++size;
	}
}

void BigInt::mul10()
{
	// multiply all the blocks
	uint64_t carry = 0;

	uint32_t* pCur = blocks;
	uint32_t* pEnd = blocks + size;
	for (; pCur != pEnd; ++pCur)
	{
		uint64_t product = (uint64_t)(*pCur) * 10ull + carry;
		(*pCur) = (uint32_t)(product & 0xFFFFFFFF);
		carry = product >> 32;
	}

	if (carry != 0)
	{
		// grow the array
		AES_ASSERT(size + 1 <= bigIntMaxBlocks);
		*pCur = (uint32_t)carry;
		++size;
	}
}

void BigInt::pow10(uint32_t exponent)
{
	// make sure the exponenet is within the bounds of the lookup table data
	AES_ASSERT(exponent < 512);

	// create two temporary values to reduce large integer copy operations
	BigInt temp1, temp2;
	BigInt* pCurTemp = &temp1;
	BigInt* pNextTemp = &temp2;

	// initialize the result by looking up a 32-bit power of 10 corresponding to the first 3 bits
	uint32_t smallExponent = exponent & 0x7;
	pCurTemp->SetU32(g_PowerOf10_U32[smallExponent]);

	// remove the low bits that we used for the 32-bit lookup table
	exponent >>= 3;
	uint32_t tableIdx = 0;

	// while there are remaining bits in the exponent to be processed
	while (exponent != 0)
	{
		// if the current bit is set, multiply it with the corresponding power of 10
		if (exponent & 1)
		{
			// multiply into the next temporary
			// BigInt_Multiply(pNextTemp, *pCurTemp, g_PowerOf10_Big[tableIdx]);
			*pNextTemp = *pCurTemp * g_PowerOf10_Big[tableIdx];

			// swap to the next temporary
			BigInt* pSwap = pCurTemp;
			pCurTemp = pNextTemp;
			pNextTemp = pSwap;
		}

		// advance to the next bit
		++tableIdx;
		exponent >>= 1;
	}

	*this = *pCurTemp;
}

BigInt BigInt::mulPow10(uint32_t exponent)
{
	// make sure the exponent is within the bounds of the lookup table data
	AES_ASSERT(exponent < 512);

	// create two temporary values to reduce large integer copy operations
	BigInt temp1;
	BigInt temp2;
	BigInt* pCurTemp = &temp1;
	BigInt* pNextTemp = &temp2;

	// initialize the result by looking up a 32-bit power of 10 corresponding to the first 3 bits
	uint32_t smallExponent = exponent & 0x7;
	if (smallExponent != 0)
	{
		*pCurTemp = *this * g_PowerOf10_U32[smallExponent];
	}
	else
	{
		*pCurTemp = *this;
	}

	// remove the low bits that we used for the 32-bit lookup table
	exponent >>= 3;
	uint32_t tableIdx = 0;

	// while there are remaining bits in the exponent to be processed
	while (exponent != 0)
	{
		// if the current bit is set, multiply it with the corresponding power of 10
		if (exponent & 1)
		{
			// multiply into the next temporary
			*pNextTemp = *pCurTemp * g_PowerOf10_Big[tableIdx];

			// swap to the next temporary
			BigInt* pSwap = pCurTemp;
			pCurTemp = pNextTemp;
			pNextTemp = pSwap;
		}

		// advance to the next bit
		++tableIdx;
		exponent >>= 1;
	}

	// output the result
	*this = *pCurTemp;
}

void BigInt::pow2(uint32_t exponent)
{
	uint32_t blockIdx = exponent / 32;
	AES_ASSERT(blockIdx < bigIntMaxBlocks);

	for (uint32_t i = 0; i <= blockIdx; ++i)
		blocks[i] = 0;

	size = blockIdx + 1;

	uint32_t bitIdx = (exponent % 32);
	blocks[blockIdx] |= (1 << bitIdx);
}

void BigInt::setZero()
{
	size = 0;
}

bool aes::d4::BigInt::isZero() const
{
	return size == 0;
}

BigInt aes::d4::operator+(BigInt const& lhs, BigInt const& rhs)
{
	// determine which operand has the smaller length
	const BigInt* pLarge;
	const BigInt* pSmall;

	BigInt result;

	if (lhs.size < rhs.size)
	{
		pSmall = &lhs;
		pLarge = &rhs;
	}
	else
	{
		pSmall = &rhs;
		pLarge = &lhs;
	}

	const uint32_t largeLen = pLarge->size;
	const uint32_t smallLen = pSmall->size;

	// The output will be at least as long as the largest input
	result.size = largeLen;

	// Add each block and add carry the overflow to the next block
	uint64_t carry = 0;
	const uint32_t* pLargeCur = pLarge->blocks;
	const uint32_t* pLargeEnd = pLargeCur + largeLen;
	const uint32_t* pSmallCur = pSmall->blocks;
	const uint32_t* pSmallEnd = pSmallCur + smallLen;
	uint32_t* pResultCur = result.blocks;

	while (pSmallCur != pSmallEnd)
	{
		uint64_t sum = carry + (uint64_t)(*pLargeCur) + (uint64_t)(*pSmallCur);
		carry = sum >> 32;
		(*pResultCur) = sum & 0xFFFFFFFF;
		++pLargeCur;
		++pSmallCur;
		++pResultCur;
	}

	// Add the carry to any blocks that only exist in the large operand
	while (pLargeCur != pLargeEnd)
	{
		uint64_t sum = carry + (uint64_t)(*pLargeCur);
		carry = sum >> 32;
		(*pResultCur) = sum & 0xFFFFFFFF;
		++pLargeCur;
		++pResultCur;
	}

	// If there's still a carry, append a new block
	if (carry != 0)
	{
		AES_ASSERT(carry == 1);
		AES_ASSERT((uint64_t)(pResultCur - result.blocks) == largeLen && (largeLen < bigIntMaxBlocks));
		*pResultCur = 1;
		result.size = largeLen + 1;
	}
	else
	{
		result.size = largeLen;
	}

	return result;
}

BigInt aes::d4::operator*(BigInt const& lhs, BigInt const& rhs)
{
	BigInt result{};

	// determine which operand has the smaller length
	const BigInt* pLarge;
	const BigInt* pSmall;
	if (lhs.size < rhs.size)
	{
		pSmall = &lhs;
		pLarge = &rhs;
	}
	else
	{
		pSmall = &rhs;
		pLarge = &lhs;
	}

	// set the maximum possible result length
	uint32_t maxResultLen = pLarge->size + pSmall->size;
	AES_ASSERT(maxResultLen <= bigIntMaxBlocks);

	// clear the result data
	//for (uint32_t* pCur = pResult->blocks, *pEnd = pCur + maxResultLen; pCur != pEnd; ++pCur)
	//	*pCur = 0;

	memset(result.blocks, 0, maxResultLen * sizeof(uint32_t));

	// perform standard long multiplication
	const uint32_t* pLargeBeg = pLarge->blocks;
	const uint32_t* pLargeEnd = pLargeBeg + pLarge->size;

	// for each small block
	uint32_t* pResultStart = result.blocks;
	for (const uint32_t* pSmallCur = pSmall->blocks, *pSmallEnd = pSmallCur + pSmall->size;
		pSmallCur != pSmallEnd;
		++pSmallCur, ++pResultStart)
	{
		// if non-zero, multiply against all the large blocks and add into the result
		const uint32_t multiplier = *pSmallCur;
		if (multiplier != 0)
		{
			const uint32_t* pLargeCur = pLargeBeg;
			uint32_t* pResultCur = pResultStart;
			uint64_t carry = 0;
			do
			{
				uint64_t product = (*pResultCur) + (*pLargeCur) * (uint64_t)multiplier + carry;
				carry = product >> 32;
				*pResultCur = product & 0xFFFFFFFF;
				++pLargeCur;
				++pResultCur;
			} while (pLargeCur != pLargeEnd);

			AES_ASSERT(pResultCur < result.blocks + maxResultLen);
			*pResultCur = (uint32_t)(carry & 0xFFFFFFFF);
		}
	}

	// check if the terminating block has no set bits
	if (maxResultLen > 0 && result.blocks[maxResultLen - 1] == 0)
		result.size = maxResultLen - 1;
	else
		result.size = maxResultLen;
	return result;
}

BigInt aes::d4::operator*(BigInt const& lhs, uint32_t rhs)
{
	BigInt result{};

	// perform long multiplication
	uint32_t carry = 0;
	uint32_t* pResultCur = result.blocks;
	const uint32_t* pLhsCur = lhs.blocks;
	const uint32_t* pLhsEnd = lhs.blocks + lhs.size;
	for (; pLhsCur != pLhsEnd; ++pLhsCur, ++pResultCur)
	{
		uint64_t product = (uint64_t)(*pLhsCur) * rhs + carry;
		*pResultCur = (uint32_t)(product & 0xFFFFFFFF);
		carry = product >> 32;
	}

	// if there is a remaining carry, grow the array
	if (carry != 0)
	{
		// grow the array
		AES_ASSERT(lhs.size + 1 <= bigIntMaxBlocks);
		*pResultCur = (uint32_t)carry;
		result.size = lhs.size + 1;
	}
	else
	{
		result.size = lhs.size;
	}

	return result;
}

BigInt aes::d4::mulBy2(BigInt const& in)
{
	BigInt result{};
	// shift all the blocks by one
	uint32_t carry = 0;

	uint32_t* pResultCur = result.blocks;
	const uint32_t* pLhsCur = in.blocks;
	const uint32_t* pLhsEnd = in.blocks + in.size;
	for (; pLhsCur != pLhsEnd; ++pLhsCur, ++pResultCur)
	{
		uint32_t cur = *pLhsCur;
		*pResultCur = (cur << 1) | carry;
		carry = cur >> 31;
	}

	if (carry != 0)
	{
		// grow the array
		AES_ASSERT(in.size + 1 <= bigIntMaxBlocks);
		*pResultCur = carry;
		result.size = in.size + 1;
	}
	else
	{
		result.size = in.size;
	}

	return result;
}

//******************************************************************************
// This function will divide two large numbers under the assumption that the
// result is within the range [0,10) and the input numbers have been shifted
// to satisfy:
// - The highest block of the divisor is greater than or equal to 8 such that
//   there is enough precision to make an accurate first guess at the quotient.
// - The highest block of the divisor is less than the maximum value on an
//   unsigned 32-bit integer such that we can safely increment without overflow.
// - The dividend does not contain more blocks than the divisor such that we
//   can estimate the quotient by dividing the equivalently placed high blocks.
//
// quotient  = floor(dividend / divisor)
// remainder = dividend - quotient*divisor
//
// pDividend is updated to be the remainder and the quotient is returned.
//******************************************************************************
static uint32_t BigInt_DivideWithRemainder_MaxQuotient9(BigInt* pDividend, const BigInt& divisor)
{
	// Check that the divisor has been correctly shifted into range and that it is not
	// smaller than the dividend in length.
	AES_ASSERT(!divisor.isZero() &&
		divisor.blocks[divisor.size - 1] >= 8 &&
		divisor.blocks[divisor.size - 1] < 0xFFFFFFFF &&
		pDividend->size <= divisor.size);

	// If the dividend is smaller than the divisor, the quotient is zero and the divisor is already
	// the remainder.
	uint32_t length = divisor.size;
	if (pDividend->size < divisor.size)
		return 0;

	const uint32_t* pFinalDivisorBlock = divisor.blocks + length - 1;
	uint32_t* pFinalDividendBlock = pDividend->blocks + length - 1;

	// Compute an estimated quotient based on the high block value. This will either match the actual quotient or
	// undershoot by one.
	uint32_t  quotient = *pFinalDividendBlock / (*pFinalDivisorBlock + 1);
	AES_ASSERT(quotient <= 9);

	// Divide out the estimated quotient
	if (quotient != 0)
	{
		// dividend = dividend - divisor*quotient
		const uint32_t* pDivisorCur = divisor.blocks;
		uint32_t* pDividendCur = pDividend->blocks;

		uint64_t borrow = 0;
		uint64_t carry = 0;
		do
		{
			uint64_t product = (uint64_t)*pDivisorCur * (uint64_t)quotient + carry;
			carry = product >> 32;

			uint64_t difference = (uint64_t)*pDividendCur - (product & 0xFFFFFFFF) - borrow;
			borrow = (difference >> 32) & 1;

			*pDividendCur = difference & 0xFFFFFFFF;

			++pDivisorCur;
			++pDividendCur;
		} while (pDivisorCur <= pFinalDivisorBlock);

		// remove all leading zero blocks from dividend
		while (length > 0 && pDividend->blocks[length - 1] == 0)
			--length;

		pDividend->size = length;
	}

	// If the dividend is still larger than the divisor, we overshot our estimate quotient. To correct,
	// we increment the quotient and subtract one more divisor from the dividend.
	if (pDividend->compare(divisor) >= 0)
	{
		++quotient;

		// dividend = dividend - divisor
		const uint32_t* pDivisorCur = divisor.blocks;
		uint32_t* pDividendCur = pDividend->blocks;

		uint64_t borrow = 0;
		do
		{
			uint64_t difference = (uint64_t)*pDividendCur - (uint64_t)*pDivisorCur - borrow;
			borrow = (difference >> 32) & 1;

			*pDividendCur = difference & 0xFFFFFFFF;

			++pDivisorCur;
			++pDividendCur;
		} while (pDivisorCur <= pFinalDivisorBlock);

		// remove all leading zero blocks from dividend
		while (length > 0 && pDividend->blocks[length - 1] == 0)
			--length;

		pDividend->size = length;
	}

	return quotient;
}

//******************************************************************************
// result = result << shift
//******************************************************************************
static void BigInt_ShiftLeft(BigInt* pResult, uint32_t shift)
{
	AES_ASSERT(shift != 0);

	uint32_t shifboollocks = shift / 32;
	uint32_t shifboolits = shift % 32;

	// process blocks high to low so that we can safely process in place
	const uint32_t* pInBlocks = pResult->blocks;
	int32_t			inLength = pResult->size;
	AES_ASSERT(inLength + shifboollocks <= bigIntMaxBlocks);

	// check if the shift is block aligned
	if (shifboolits == 0)
	{
		// copy blocks from high to low
		for (uint32_t* pInCur = pResult->blocks + inLength - 1, *pOutCur = pInCur + shifboollocks;
			pInCur >= pInBlocks;
			--pInCur, --pOutCur)
		{
			*pOutCur = *pInCur;
		}

		// zero the remaining low blocks
		for (uint32_t i = 0; i < shifboollocks; ++i)
			pResult->blocks[i] = 0;

		pResult->size += shifboollocks;
	}
	// else we need to shift partial blocks
	else
	{
		int32_t inBlockIdx = inLength - 1;
		uint32_t ouboollockIdx = inLength + shifboollocks;

		// set the length to hold the shifted blocks
		AES_ASSERT(ouboollockIdx < bigIntMaxBlocks);
		pResult->size = ouboollockIdx + 1;

		// output the initial blocks
		const uint32_t lowBitsShift = (32 - shifboolits);
		uint32_t highBits = 0;
		uint32_t block = pResult->blocks[inBlockIdx];
		uint32_t lowBits = block >> lowBitsShift;
		while (inBlockIdx > 0)
		{
			pResult->blocks[ouboollockIdx] = highBits | lowBits;
			highBits = block << shifboolits;

			--inBlockIdx;
			--ouboollockIdx;

			block = pResult->blocks[inBlockIdx];
			lowBits = block >> lowBitsShift;
		}

		// output the final blocks
		AES_ASSERT(ouboollockIdx == shifboollocks + 1);
		pResult->blocks[ouboollockIdx] = highBits | lowBits;
		pResult->blocks[ouboollockIdx - 1] = block << shifboolits;

		// zero the remaining low blocks
		for (uint32_t i = 0; i < shifboollocks; ++i)
			pResult->blocks[i] = 0;

		// check if the terminating block has no set bits
		if (pResult->blocks[pResult->size - 1] == 0)
			--pResult->size;
	}
}

//******************************************************************************
// This is an implementation the Dragon4 algorithm to convert a binary number
// in floating point format to a decimal number in string format. The function
// returns the number of digits written to the output buffer and the output is
// not NUL terminated.
//
// The floating point input value is (mantissa * 2^exponent).
//
// See the following papers for more information on the algorithm:
//  "How to Print Floating-Point Numbers Accurately"
//    Steele and White
//    http://kurtstephens.com/files/p372-steele.pdf
//  "Printing Floating-Point Numbers Quickly and Accurately"
//    Burger and Dybvig
//    http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.72.4656&rep=rep1&type=pdf
//******************************************************************************
static uint32_t dragon4
(
	const uint64_t			mantissa,			// value significand
	const int32_t			exponent,			// value exponent in base 2
	const uint32_t			mantissaHighBitIdx,	// index of the highest set mantissa bit
	const bool			hasUnequalMargins,	// is the high margin twice as large as the low margin
	const CutoffMode	cutoffMode,			// how to determine output length
	uint32_t				cutoffNumber,		// parameter to the selected cutoffMode
	char* pOubooluffer,			// buffer to output into
	uint32_t				bufferSize,			// maximum characters that can be printed to pOubooluffer
	int32_t* pOutExponent		// the base 10 exponent of the first digit
)
{
	char* pCurDigit = pOubooluffer;

	AES_ASSERT(bufferSize > 0);

	// if the mantissa is zero, the value is zero regardless of the exponent
	if (mantissa == 0)
	{
		*pCurDigit = '0';
		*pOutExponent = 0;
		return 1;
	}

	// compute the initial state in integral form such that 
	//  value     = scaledValue / scale
	//  marginLow = scaledMarginLow / scale
	BigInt scale;				// positive scale applied to value and margin such that they can be
	//  represented as whole numbers
	BigInt scaledValue;		// scale * mantissa
	BigInt scaledMarginLow;	// scale * 0.5 * (distance between this floating-point number and its
	//  immediate lower value)

	// For normalized IEEE floating point values, each time the exponent is incremented the margin also
	// doubles. That creates a subset of transition numbers where the high margin is twice the size of
	// the low margin.
	BigInt* pScaledMarginHigh;
	BigInt optionalMarginHigh;

	if (hasUnequalMargins)
	{
		// if we have no fractional component
		if (exponent > 0)
		{
			// 1) Expand the input value by multiplying out the mantissa and exponent. This represents
			//    the input value in its whole number representation.
			// 2) Apply an additional scale of 2 such that later comparisons against the margin values
			//    are simplified.
			// 3) Set the margin value to the lowest mantissa bit's scale.

			// scaledValue      = 2 * 2 * mantissa*2^exponent
			scaledValue.SetU64(4 * mantissa);
			BigInt_ShiftLeft(&scaledValue, exponent);

			// scale            = 2 * 2 * 1
			scale.SetU32(4);

			// scaledMarginLow  = 2 * 2^(exponent-1)
			scaledMarginLow.pow2(exponent);

			// scaledMarginHigh = 2 * 2 * 2^(exponent-1)
			optionalMarginHigh.pow2(exponent + 1);
		}
		// else we have a fractional exponent
		else
		{
			// In order to track the mantissa data as an integer, we store it as is with a large scale

			// scaledValue      = 2 * 2 * mantissa
			scaledValue.SetU64(4 * mantissa);

			// scale            = 2 * 2 * 2^(-exponent)
			scale.pow2(-exponent + 2);

			// scaledMarginLow  = 2 * 2^(-1)
			scaledMarginLow.SetU32(1);

			// scaledMarginHigh = 2 * 2 * 2^(-1)
			optionalMarginHigh.SetU32(2);
		}

		// the high and low margins are different
		pScaledMarginHigh = &optionalMarginHigh;
	}
	else
	{
		// if we have no fractional component
		if (exponent > 0)
		{
			// 1) Expand the input value by multiplying out the mantissa and exponent. This represents
			//    the input value in its whole number representation.
			// 2) Apply an additional scale of 2 such that later comparisons against the margin values
			//    are simplified.
			// 3) Set the margin value to the lowest mantissa bit's scale.

			// scaledValue     = 2 * mantissa*2^exponent
			scaledValue.SetU64(2 * mantissa);
			BigInt_ShiftLeft(&scaledValue, exponent);

			// scale           = 2 * 1
			scale.SetU32(2);

			// scaledMarginLow = 2 * 2^(exponent-1)
			scaledMarginLow.pow2(exponent);
		}
		// else we have a fractional exponent
		else
		{
			// In order to track the mantissa data as an integer, we store it as is with a large scale

			// scaledValue     = 2 * mantissa
			scaledValue.SetU64(2 * mantissa);

			// scale           = 2 * 2^(-exponent)
			scale.pow2(-exponent + 1);
			// scaledMarginLow = 2 * 2^(-1)
			scaledMarginLow.SetU32(1);
		}

		// the high and low margins are equal
		pScaledMarginHigh = &scaledMarginLow;
	}

	// Compute an estimate for digitExponent that will be correct or undershoot by one.
	// This optimization is based on the paper "Printing Floating-Point Numbers Quickly and Accurately"
	// by Burger and Dybvig http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.72.4656&rep=rep1&type=pdf
	// We perform an additional subtraction of 0.69 to increase the frequency of a failed estimate
	// because that lets us take a faster branch in the code. 0.69 is chosen because 0.69 + log10(2) is
	// less than one by a reasonable epsilon that will account for any floating point error.
	//
	// We want to set digitExponent to floor(log10(v)) + 1
	//  v = mantissa*2^exponent
	//  log2(v) = log2(mantissa) + exponent;
	//  log10(v) = log2(v) * log10(2)
	//  floor(log2(v)) = mantissaHighBitIdx + exponent;
	//  log10(v) - log10(2) < (mantissaHighBitIdx + exponent) * log10(2) <= log10(v)
	//  log10(v) < (mantissaHighBitIdx + exponent) * log10(2) + log10(2) <= log10(v) + log10(2)
	//  floor( log10(v) ) < ceil( (mantissaHighBitIdx + exponent) * log10(2) ) <= floor( log10(v) ) + 1
	const double log10_2 = 0.30102999566398119521373889472449;
	int32_t digitExponent = (int32_t)(ceil(double((int32_t)mantissaHighBitIdx + exponent) * log10_2 - 0.69));

	// if the digit exponent is smaller than the smallest desired digit for fractional cutoff,
	// pull the digit back into legal range at which point we will round to the appropriate value.
	// Note that while our value for digitExponent is still an estimate, this is safe because it
	// only increases the number. This will either correct digitExponent to an accurate value or it
	// will clamp it above the accurate value.
	if (cutoffMode == CutoffMode_FractionLength && digitExponent <= -(int32_t)cutoffNumber)
	{
		digitExponent = -(int32_t)cutoffNumber + 1;
	}

	// Divide value by 10^digitExponent. 
	if (digitExponent > 0)
	{
		// The exponent is positive creating a division so we multiply up the scale.
		scale = scale.mulPow10(digitExponent);
	}
	else if (digitExponent < 0)
	{
		// The exponent is negative creating a multiplication so we multiply up the scaledValue,
		// scaledMarginLow and scaledMarginHigh.
		BigInt pow10;
		pow10.pow10(-digitExponent);

		scaledValue = scaledValue * pow10;

		scaledMarginLow = scaledMarginLow * pow10;

		if (pScaledMarginHigh != &scaledMarginLow)
		{
			*pScaledMarginHigh = scaledMarginLow;
			pScaledMarginHigh->mul2();
		}
	}

	// If (value >= 1), our estimate for digitExponent was too low
	if (scaledValue.compare(scale) >= 0)
	{
		// The exponent estimate was incorrect.
		// Increment the exponent and don't perform the premultiply needed
		// for the first loop iteration.
		digitExponent = digitExponent + 1;
	}
	else
	{
		// The exponent estimate was correct.
		// Multiply larger by the output base to prepare for the first loop iteration.
		scaledValue.mul10();
		scaledMarginLow.mul10();
		if (pScaledMarginHigh != &scaledMarginLow)
		{
			*pScaledMarginHigh = scaledMarginLow;
			pScaledMarginHigh->mul2();
		}
	}

	// Compute the cutoff exponent (the exponent of the final digit to print).
	// Default to the maximum size of the output buffer.
	int32_t cutoffExponent = digitExponent - bufferSize;
	switch (cutoffMode)
	{
		// print digits until we pass the accuracy margin limits or buffer size
	case CutoffMode_Unique:
		break;

		// print cutoffNumber of digits or until we reach the buffer size
	case CutoffMode_TotalLength:
	{
		int32_t desiredCutoffExponent = digitExponent - (int32_t)cutoffNumber;
		if (desiredCutoffExponent > cutoffExponent)
			cutoffExponent = desiredCutoffExponent;
	}
	break;

	// print cutoffNumber digits past the decimal point or until we reach the buffer size
	case CutoffMode_FractionLength:
	{
		int32_t desiredCutoffExponent = -(int32_t)cutoffNumber;
		if (desiredCutoffExponent > cutoffExponent)
			cutoffExponent = desiredCutoffExponent;
	}
	break;
	}

	// Output the exponent of the first digit we will print
	*pOutExponent = digitExponent - 1;

	// In preparation for calling BigInt_DivideWithRemainder_MaxQuotient9(), 
	// we need to scale up our values such that the highest block of the denominator
	// is greater than or equal to 8. We also need to guarantee that the numerator
	// can never have a length greater than the denominator after each loop iteration.
	// This requires the highest block of the denominator to be less than or equal to
	// 429496729 which is the highest number that can be multiplied by 10 without
	// overflowing to a new block.
	AES_ASSERT(scale.size > 0);
	uint32_t hiBlock = scale.blocks[scale.size - 1];
	if (hiBlock < 8 || hiBlock > 429496729)
	{
		// Perform a bit shift on all values to get the highest block of the denominator into
		// the range [8,429496729]. We are more likely to make accurate quotient estimations
		// in BigInt_DivideWithRemainder_MaxQuotient9() with higher denominator values so
		// we shift the denominator to place the highest bit at index 27 of the highest block.
		// This is safe because (2^28 - 1) = 268435455 which is less than 429496729. This means
		// that all values with a highest bit at index 27 are within range.			
		uint32_t hiBlockLog2 = LogBase2(hiBlock);
		AES_ASSERT(hiBlockLog2 < 3 || hiBlockLog2 > 27);
		uint32_t shift = (32 + 27 - hiBlockLog2) % 32;

		BigInt_ShiftLeft(&scale, shift);
		BigInt_ShiftLeft(&scaledValue, shift);
		BigInt_ShiftLeft(&scaledMarginLow, shift);
		if (pScaledMarginHigh != &scaledMarginLow)
		{
			*pScaledMarginHigh = scaledMarginLow;
			pScaledMarginHigh->mul2();
		}
	}

	// These values are used to inspect why the print loop terminated so we can properly
	// round the final digit.
	bool		low;			// did the value get within marginLow distance from zero
	bool		high;			// did the value get within marginHigh distance from one
	uint32_t	outputDigit;	// current digit being output

	if (cutoffMode == CutoffMode_Unique)
	{
		// For the unique cutoff mode, we will try to print until we have reached a level of
		// precision that uniquely distinguishes this value from its neighbors. If we run
		// out of space in the output buffer, we terminate early.
		for (;;)
		{
			digitExponent = digitExponent - 1;

			// divide out the scale to extract the digit
			outputDigit = BigInt_DivideWithRemainder_MaxQuotient9(&scaledValue, scale);
			AES_ASSERT(outputDigit < 10);

			// update the high end of the value
			BigInt scaledValueHigh = scaledValue * *pScaledMarginHigh;

			// stop looping if we are far enough away from our neighboring values
			// or if we have reached the cutoff digit
			low = scaledValue.compare(scaledMarginLow) < 0;
			high = scaledValueHigh.compare(scale) > 0;
			if (low | high | (digitExponent == cutoffExponent))
				break;

			// store the output digit
			*pCurDigit = (char)('0' + outputDigit);
			++pCurDigit;

			// multiply larger by the output base
			scaledValue.mul10();
			scaledMarginLow.mul10();
			if (pScaledMarginHigh != &scaledMarginLow)
			{
				*pScaledMarginHigh = scaledMarginLow;
				pScaledMarginHigh->mul2();
			}
		}
	}
	else
	{
		// For length based cutoff modes, we will try to print until we
		// have exhausted all precision (i.e. all remaining digits are zeros) or
		// until we reach the desired cutoff digit.
		low = false;
		high = false;

		for (;;)
		{
			digitExponent = digitExponent - 1;

			// divide out the scale to extract the digit
			outputDigit = BigInt_DivideWithRemainder_MaxQuotient9(&scaledValue, scale);
			AES_ASSERT(outputDigit < 10);

			if (scaledValue.isZero() | (digitExponent == cutoffExponent))
				break;

			// store the output digit
			*pCurDigit = (char)('0' + outputDigit);
			++pCurDigit;

			// multiply larger by the output base
			scaledValue.mul10();
		}
	}

	// round off the final digit
	// default to rounding down if value got too close to 0
	bool roundDown = low;

	// if it is legal to round up and down
	if (low == high)
	{
		// round to the closest digit by comparing value with 0.5. To do this we need to convert
		// the inequality to large integer values.
		//  compare( value, 0.5 )
		//  compare( scale * value, scale * 0.5 )
		//  compare( 2 * scale * value, scale )
		scaledValue.mul2();
		int32_t compare = scaledValue.compare(scale);
		roundDown = compare < 0;

		// if we are directly in the middle, round towards the even digit (i.e. IEEE rouding rules)
		if (compare == 0)
			roundDown = (outputDigit & 1) == 0;
	}

	// print the rounded digit
	if (roundDown)
	{
		*pCurDigit = (char)('0' + outputDigit);
		++pCurDigit;
	}
	else
	{
		// handle rounding up
		if (outputDigit == 9)
		{
			// find the first non-nine prior digit
			for (;;)
			{
				// if we are at the first digit
				if (pCurDigit == pOubooluffer)
				{
					// output 1 at the next highest exponent
					*pCurDigit = '1';
					++pCurDigit;
					*pOutExponent += 1;
					break;
				}

				--pCurDigit;
				if (*pCurDigit != '9')
				{
					// increment the digit
					*pCurDigit += 1;
					++pCurDigit;
					break;
				}
			}
		}
		else
		{
			// values in the range [0,8] can perform a simple round up
			*pCurDigit = (char)('0' + outputDigit + 1);
			++pCurDigit;
		}
	}

	// return the number of digits output
	uint32_t outputLen = (uint32_t)(pCurDigit - pOubooluffer);
	AES_ASSERT(outputLen <= bufferSize);
	return outputLen;
}

uint32_t aes::d4::printFloat32(char* pOutBuffer, uint32_t bufferSize, float value, PrintFloatFormat format, int32_t precision)
{


	return 0;
}