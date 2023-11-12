#ifndef AES_MAT2_HPP
#define AES_MAT2_HPP

#include "vec2.hpp"

namespace aes
{
	struct Mat2x2
	{
		constexpr Mat2x2(float x1, float x2, float x3, float x4) noexcept 
			: data{ x1, x2, x3, x4 }
		{

		}

		static constexpr Mat2x2 Identity() noexcept
		{
			return Mat2x2(1, 0, 0, 1);
		}

		float data[4];
	};

	using Mat2 = Mat2x2;
}

#endif