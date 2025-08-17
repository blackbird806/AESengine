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

		decltype(auto) operator[](this auto& self, unsigned i)
		{
			return self.data[i];
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
			mat3x3 m;
			memset(&m, 0, sizeof(m));
			m.data[0] = 1;
			m.data[4] = 1;
			m.data[8] = 1;
			return m;
		}

		mat3x3() noexcept
		{
			v[0] = vec3();
			v[1] = vec3();
			v[2] = vec3();
		}

		mat3x3(mat3x3 const& rhs) noexcept
		{
			v[0] = rhs.v[0];
			v[1] = rhs.v[1];
			v[2] = rhs.v[2];
		}

		decltype(auto) operator[](this auto& self, unsigned i)
		{
			return self.data[i];
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

		decltype(auto) operator[](this auto& self, unsigned i)
		{
			return self.data[i];
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
			mat4x4 m;
			memset(&m, 0, sizeof(m));
			m.data[0] = 1;
			m.data[5] = 1;
			m.data[10] = 1;
			m.data[15] = 1;
			return m;
		}

		mat4x4() noexcept
		{
			v[0] = vec4();
			v[1] = vec4();
			v[2] = vec4();
			v[3] = vec4();
		}

		mat4x4(mat4x4 const& rhs) noexcept
		{
			v[0] = rhs.v[0];
			v[1] = rhs.v[1];
			v[2] = rhs.v[2];
			v[3] = rhs.v[3];
		}

		decltype(auto) operator[](this auto& self, unsigned i)
		{
			return self.data[i];
		}

		decltype(auto) operator[](this auto& self, unsigned i, unsigned j)
		{
			// @Review
			return self.data[i * 4 + j];
		}

		static mat4x4 translationMat(vec3 v)
		{
			mat4x4 result = mat4x4::identity();
			result[0, 3] = v.x;
			result[1, 3] = v.y;
			result[2, 3] = v.z;
			return result;
		}

		static mat4x4 rotateXMat(float a)
		{
			mat4x4 result = mat4x4::identity();
			result[1, 1] = cosf(a);
			result[1, 2] = -sinf(a);
			result[2, 1] = sinf(a);
			result[2, 2] = cosf(a);
			return result;
		}

		static mat4x4 rotateYMat(float a)
		{
			mat4x4 result = mat4x4::identity();
			result[0, 0] = cosf(a);
			result[0, 2] = sinf(a);
			result[2, 0] = -sinf(a);
			result[3, 3] = cosf(a);
			return result;
		}

		static mat4x4 rotateZMat(float a)
		{
			mat4x4 result = mat4x4::identity();
			result[0, 0] = cosf(a);
			result[0, 1] = -sinf(a);
			result[1, 0] = sinf(a);
			result[1, 1] = cosf(a);
			return result;
		}

		static mat4x4 scaleMat(vec3 s)
		{
			mat4x4 result = mat4x4::identity();
			result[0] = s.x;
			result[5] = s.y;
			result[10] = s.z;
			return result;
		}

		mat4x4 operator*(mat4x4 const& r) const
		{
			mat4x4 result;
			mat4x4 const& s = *this;

			result[0, 0] = s[0, 0] * r[0, 0] + s[1, 0] * r[0, 1] + s[2, 0] * r[0, 2] + s[3, 0] * r[0, 3];
			result[1, 0] = s[0, 0] * r[1, 0] + s[1, 0] * r[1, 1] + s[2, 0] * r[1, 2] + s[3, 0] * r[1, 3];
			result[2, 0] = s[0, 0] * r[2, 0] + s[1, 0] * r[2, 1] + s[2, 0] * r[2, 2] + s[3, 0] * r[2, 3];
			result[3, 0] = s[0, 0] * r[3, 0] + s[1, 0] * r[3, 1] + s[2, 0] * r[3, 2] + s[3, 0] * r[3, 3];

			result[0, 1] = s[0, 1] * r[0, 0] + s[1, 1] * r[0, 1] + s[2, 1] * r[0, 2] + s[3, 1] * r[0, 3];
			result[1, 1] = s[0, 1] * r[1, 0] + s[1, 1] * r[1, 1] + s[2, 1] * r[1, 2] + s[3, 1] * r[1, 3];
			result[2, 1] = s[0, 1] * r[2, 0] + s[1, 1] * r[2, 1] + s[2, 1] * r[2, 2] + s[3, 1] * r[2, 3];
			result[3, 1] = s[0, 1] * r[3, 0] + s[1, 1] * r[3, 1] + s[2, 1] * r[3, 2] + s[3, 1] * r[3, 3];

			result[0, 2] = s[0, 2] * r[0, 0] + s[1, 2] * r[0, 1] + s[2, 2] * r[0, 2] + s[3, 2] * r[0, 3];
			result[1, 2] = s[0, 2] * r[1, 0] + s[1, 2] * r[1, 1] + s[2, 2] * r[1, 2] + s[3, 2] * r[1, 3];
			result[2, 2] = s[0, 2] * r[2, 0] + s[1, 2] * r[2, 1] + s[2, 2] * r[2, 2] + s[3, 2] * r[2, 3];
			result[3, 2] = s[0, 2] * r[3, 0] + s[1, 2] * r[3, 1] + s[2, 2] * r[3, 2] + s[3, 2] * r[3, 3];

			result[0, 3] = s[0, 3] * r[0, 0] + s[1, 3] * r[0, 1] + s[2, 3] * r[0, 2] + s[3, 3] * r[0, 3];
			result[1, 3] = s[0, 3] * r[1, 0] + s[1, 3] * r[1, 1] + s[2, 3] * r[1, 2] + s[3, 3] * r[1, 3];
			result[2, 3] = s[0, 3] * r[2, 0] + s[1, 3] * r[2, 1] + s[2, 3] * r[2, 2] + s[3, 3] * r[2, 3];
			result[3, 3] = s[0, 3] * r[3, 0] + s[1, 3] * r[3, 1] + s[2, 3] * r[3, 2] + s[3, 3] * r[3, 3];

			return result;
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