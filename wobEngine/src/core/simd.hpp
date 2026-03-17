#ifndef WOB_SIMD_HPP
#define WOB_SIMD_HPP

#ifdef WOB_ENABLE_SIMD

	#if defined(__vita__)
		#define WOB_NEON
		#include <arm_neon.h>
		namespace aes
		{
			using r128_t = __n128;
		}
	#else
		#define WOB_SSE	
		#define WOB_SSE2
		#define WOB_SSE3
		#define WOB_SSSE3
		#define WOB_SSE4
		#define WOB_SSE4_1
		#define WOB_SSE4_2
		#define WOB_AVX	
		#define WOB_AVX2
		//#define WOB_AVX512
		#define WOB_FMA3
		#define WOB_FMA4

		#include <mmintrin.h>
		#include <emmintrin.h>
		#include <xmmintrin.h>
		#ifdef  WOB_SSE3
			#include <pmmintrin.h>
		#endif
		#ifdef WOB_SSSE3
			#include <tmmintrin.h>
		#endif
		#ifdef WOB_SSE4_1
			#include <smmintrin.h>
		#endif
		#ifdef WOB_SSE4_2
			#include <nmmintrin.h>
		#endif
		#if defined(WOB_AVX) || defined(WOB_AVX512)
			#include <immintrin.h>
		#endif
		namespace aes
		{
			using r128_t = __m128;
		}
	#endif

#endif

#endif