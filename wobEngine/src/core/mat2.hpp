#ifndef AES_MAT2_HPP
#define AES_MAT2_HPP

#include "vec2.hpp"

namespace aes
{
	struct mat2x2
	{
		constexpr mat2x2(float x1, float x2, float x3, float x4) noexcept 
			: data{ x1, x2, x3, x4 }
		{

		}

		static constexpr mat2x2 Identity() noexcept
		{
			return mat2x2(	1, 0, 
							0, 1);
		}

		float operator[](unsigned i) const
		{
			return data[i];
		}

		float& operator[](unsigned i)
		{
			return data[i];
		}

		mat2x2 operator*(mat2x2 const& rhs)
		{
			//return mat2x2(data[0] * rhs[0] + data[1] * rhs[3], );
		}

		union
		{
			vec2 v[2];
			float data[4];
		};
	};

	using Mat2 = mat2x2;
}

#endif