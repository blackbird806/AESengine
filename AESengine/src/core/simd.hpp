#ifndef AES_SIMD_HPP
#define AES_SIMD_HPP

#ifdef AES_ENABLE_SIMD

	#if defined(__vita__)
		#define AES_NEON
		#include <arm_neon.h>
		namespace aes
		{
			using r64_t = __n64;
			using r128_t = __n128;
		}
	#else
		#define AES_SSE	
		#define AES_SSE2
		#define AES_SSE3
		#define AES_SSSE3
		#define AES_SSE4
		#define AES_SSE4_1
		#define AES_SSE4_2
		#define AES_AVX	
		#define AES_AVX2
		//#define AES_AVX512
		#define AES_FMA3
		#define AES_FMA4

		#include <mmintrin.h>
		#include <emmintrin.h>
		#include <xmmintrin.h>
		#ifdef  AES_SSE3
			#include <pmmintrin.h>
		#endif
		#ifdef AES_SSSE3
			#include <tmmintrin.h>
		#endif
		#ifdef AES_SSE4_1
			#include <smmintrin.h>
		#endif
		#ifdef AES_SSE4_2
			#include <nmmintrin.h>
		#endif
		#if defined(AES_AVX) || defined(AES_AVX512)
			#include <immintrin.h>
		#endif
		namespace aes
		{
			using r64_t = __m64;
			using r128_t = __m128;
		}
	#endif

#endif

#endif