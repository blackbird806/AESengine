#ifndef AES_MATRIX_HPP
#define AES_MATRIX_HPP

#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"

namespace aes
{
	struct mat2x2
	{
		constexpr mat2x2(float x1, float x2, float x3, float x4) noexcept
			: data{ x1, x2, x3, x4 }
		{

		}

		static constexpr mat2x2 identity() noexcept
		{
			return mat2x2(	1, 0,
							0, 1);
		}

		float const& operator[](unsigned i) const
		{
			return data[i];
		}

		float& operator[](unsigned i)
		{
			return data[i];
		}

		//mat2x2 operator*(mat2x2 const& rhs)
		//{
		//	return mat2x2(data[0] * rhs[0] + data[1] * rhs[3], data[0] * rhs[0] + data[1] * rhs[1]);
		//}

		union
		{
			vec2 v[2];
			float data[4];
		};
	};


	struct mat3x3
	{
		static mat3x3 identity() noexcept
		{
			AES_NOT_IMPLEMENTED();
			return mat3x3{};
		}

		mat3x3() noexcept
		{
			v[0] = vec3();
			v[1] = vec3();
			v[2] = vec3();
		}

		mat3x3(mat3x3 const& rhs) noexcept
		{
			v[0] = rhs[0];
			v[1] = rhs[1];
			v[2] = rhs[2];
		}

		float const& operator[](unsigned i) const
		{
			return data[i];
		}

		float& operator[](unsigned i)
		{
			return data[i];
		}

		union
		{
			vec3 v[3];
			float data[9];
		};
	};

	using mat3 = mat3x3;

	struct mat3x4
	{

		float const& operator[](unsigned i) const
		{
			return data[i];
		}

		float& operator[](unsigned i)
		{
			return data[i];
		}

		union
		{
			vec4 v[3];
			float data[12];
		};
	};

	struct mat4x4
	{
		static mat4x4 identity() noexcept
		{
			AES_NOT_IMPLEMENTED();
			return mat4x4{};
		}

		float const& operator[](unsigned i) const
		{
			return data[i];
		}

		float& operator[](unsigned i)
		{
			return data[i];
		}

		union
		{
			vec4 v[4];
			float data[16];
		};

	};

	using mat4 = mat4x4;

}

#endif