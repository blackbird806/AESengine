#ifndef AES_VEC4_HPP
#define AES_VEC4_HPP

#include "aes.hpp"

namespace aes
{
	struct vec4
	{
		constexpr vec4() noexcept : x(0), y(0), z(0), w(0)
		{
		}

		constexpr vec4(vec4 const& v) noexcept : x(v.x), y(v.y), z(v.z), w(v.w)
		{

		}

		constexpr vec4(float v) noexcept : x(v), y(v), z(v), w(v)
		{
		}

		constexpr vec4(float vx, float vy) noexcept : x(vx), y(vy), z(0), w(0)
		{
		}

		constexpr vec4(float vx, float vy, float vz) noexcept : x(vx), y(vy), z(vz), w(0)
		{
		}

		constexpr vec4(float vx, float vy, float vz, float vw) noexcept : x(vx), y(vy), z(vz), w(vw)
		{
		}

		constexpr float operator[](uint i) const noexcept
		{
			AES_ASSERT(i < 4);
			return data[i];
		}

		constexpr float& operator[](uint i) noexcept
		{
			AES_ASSERT(i < 4);
			return data[i];
		}

		constexpr vec4 operator+(float v) const noexcept
		{
			return vec4(x + v, y + v, z + v, w + v);
		}

		constexpr vec4 operator-(float v) const noexcept
		{
			return vec4(x - v, y - v, z - v, w - v);
		}

		constexpr vec4 operator*(float v) const noexcept
		{
			return vec4(x * v, y * v, z * v, w * v);
		}

		constexpr vec4 operator/(float v) const noexcept
		{
			return vec4(x / v, y / v, z / v, w / v);
		}

		constexpr vec4& operator+=(float v) noexcept
		{
			x += v;
			y += v;
			z += v;
			w += v;
			return *this;
		}

		constexpr vec4& operator-=(float v) noexcept
		{
			x -= v;
			y -= v;
			z -= v;
			w -= v;
			return *this;
		}

		constexpr vec4& operator*=(float v) noexcept
		{
			x *= v;
			y *= v;
			z *= v;
			w *= v;
			return *this;
		}

		constexpr vec4& operator/=(float v) noexcept
		{
			x /= v;
			y /= v;
			z /= v;
			w /= v;
			return *this;
		}

		constexpr float dot(vec4 rhs) const noexcept
		{
			return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
		}

		constexpr float sqrLength() const noexcept
		{
			return x * x + y * y + z * z + w * w;
		}

		/*constexpr*/ float length() const noexcept
		{
			return sqrt(sqrLength());
		}

		/*constexpr*/ void normalize() noexcept
		{
			*this = *this / length();
		}

		/*constexpr*/ vec4 getNormalized() const noexcept
		{
			vec4 v(*this);
			v.normalize();
			return v;
		}

		union
		{
			float data[4];
			struct { float x, y, z, w; };
			struct { float r, g, b, a; };
		};
	};
}

#endif