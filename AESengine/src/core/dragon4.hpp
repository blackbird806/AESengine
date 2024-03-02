#ifndef AES_DRAGON4_HPP
#define AES_DRAGON4_HPP

#include <cstdint>

// implementation from https://github.com/ahaldane/Dragon4
// algo and original code from https://www.ryanjuckett.com/printing-floating-point-numbers/

/******************************************************************************
  Copyright (c) 2014 Ryan Juckett
  http://www.ryanjuckett.com/

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

  3. This notice may not be removed or altered from any source
     distribution.
******************************************************************************/

// Assertion macro
#define RJ_ASSERT(condition) AES_ASSERT(condition)

namespace aes::d4
{
    // Boolean types
    typedef bool        tB;

    // Character types
    typedef char        tC8;

    // Unsigned integer types
    typedef uint8_t     tU8;
    typedef uint16_t    tU16;
    typedef uint32_t    tU32;
    typedef uint64_t    tU64;

    // Signed integer types
    typedef int8_t      tS8;
    typedef int16_t     tS16;
    typedef int32_t     tS32;
    typedef int64_t     tS64;

    // Floating point types
    typedef float       tF32;
    typedef double      tF64;

    // Size types
    typedef size_t		tSize;
    typedef ptrdiff_t	tPtrDiff;

    // TODO make this constexpr

    //******************************************************************************
    // Different modes for terminating digit output
    //******************************************************************************
    enum tCutoffMode
    {
        CutoffMode_Unique,          // as many digits as necessary to print a uniquely identifiable number
        CutoffMode_TotalLength,     // up to cutoffNumber significant digits
        CutoffMode_FractionLength,  // up to cutoffNumber significant digits past the decimal point
    };

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
    tU32 Dragon4
    (
        tU64        mantissa,           // value significand
        tS32        exponent,           // value exponent in base 2
        tU32        mantissaHighBitIdx, // index of the highest set mantissa bit
        tB          hasUnequalMargins,  // is the high margin twice as large as the low margin
        tCutoffMode cutoffMode,         // how to determine output length
        tU32        cutoffNumber,       // parameter to the selected cutoffMode
        tC8* pOutBuffer,         // buffer to output into
        tU32        bufferSize,         // maximum characters that can be printed to pOutBuffer
        tS32* pOutExponent        // the base 10 exponent of the first digit
    );

    enum tPrintFloatFormat
    {
        PrintFloatFormat_Positional,    // [-]ddddd.dddd
        PrintFloatFormat_Scientific,    // [-]d.dddde[sign]ddd
    };

    //******************************************************************************
    // Print a 32-bit floating-point number as a decimal string.
    // The output string is always NUL terminated and the string length (not
    // including the NUL) is returned.
    //******************************************************************************
    tU32 PrintFloat32
    (
        tC8* pOutBuffer,     // buffer to output into
        tU32                bufferSize,     // size of pOutBuffer
        tF32                value,          // value to print
        tPrintFloatFormat   format,         // format to print with
        tS32                precision       // If negative, the minimum number of digits to represent a
        // unique 32-bit floating point value is output. Otherwise,
        // this is the number of digits to print past the decimal point.
    );

    //******************************************************************************
    // Print a 64-bit floating-point number as a decimal string.
    // The output string is always NUL terminated and the string length (not
    // including the NUL) is returned.
    //******************************************************************************
    tU32 PrintFloat64
    (
        tC8* pOutBuffer,     // buffer to output into
        tU32                bufferSize,     // size of pOutBuffer
        tF64                value,          // value to print
        tPrintFloatFormat   format,         // format to print with
        tS32                precision       // If negative, the minimum number of digits to represent a
        // unique 64-bit floating point value is output. Otherwise,
        // this is the number of digits to print past the decimal point.
    );
}

#endif