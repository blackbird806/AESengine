#ifndef AES_SIMD_HPP
#define AES_SIMD_HPP

#if true //def AES_ENABLE_SIMD

	// r64_t ==> 64bits simd register	
	// r128_t ==> 128bits simd register
	// simple neon guide https://github.com/thenifty/neon-guide
	#if defined(__vita__)
		#define AES_NEON
		#include <arm_neon.h>
		namespace aes
		{
			using r64_t = float32x2_t;
			using r128_t = float32x4_t;
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
		namespace aes
		{
			inline r128_t load_r128(float const* v)
			{
#if defined(__vita__)
				return vld1q_f32(v);
#else
				return _mm_load_ps(v);
#endif
			}

			inline void store_r128(float* v, r128_t r)
			{
#if defined(__vita__)
				vst1q_f32(v, r);
#else
				_mm_store_ps(v, r);
#endif
			}

			inline r128_t mul_r128(r128_t a, r128_t b)
			{
#if defined(__vita__)
				return vmulq_f32(a, b);
#else
				return _mm_mul_ps(a, b);
#endif
			}

			inline r128_t sub_r128(r128_t a, r128_t b)
			{
#if defined(__vita__)
				return vsubq_f32(a, b);
#else
				return _mm_sub_ps(a, b);
#endif
			}

			inline r128_t min_r128(r128_t a, r128_t b)
			{
#if defined(__vita__)
				return vminq_f32(a, b);
#else
				return _mm_min_ps(a, b);
#endif
			}

			inline r128_t max_r128(r128_t a, r128_t b)
			{
#if defined(__vita__)
				return vmaxq_f32(a, b);
#else
				return _mm_max_ps(a, b);
#endif
			}
		}
#endif

#endif