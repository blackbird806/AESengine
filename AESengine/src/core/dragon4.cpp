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

auto BigInt::operator<=>(BigInt const& rhs) const
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
	BigInt pCurTemp;
	BigInt pNextTemp;

	// initialize the result by looking up a 32-bit power of 10 corresponding to the first 3 bits
	uint32_t smallExponent = exponent & 0x7;
	pCurTemp.SetU32(g_PowerOf10_U32[smallExponent]);

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
			pNextTemp = pCurTemp * g_PowerOf10_Big[tableIdx];

			// swap to the next temporary
			BigInt pSwap = pCurTemp;
			pCurTemp = pNextTemp;
			pNextTemp = pSwap;
		}

		// advance to the next bit
		++tableIdx;
		exponent >>= 1;
	}

	*this = pCurTemp;
}

void BigInt::setZero()
{
	size = 0;
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
	//for (uint32_t* pCur = pResult->m_blocks, *pEnd = pCur + maxResultLen; pCur != pEnd; ++pCur)
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