#ifndef AES_VEC3_HPP
#define AES_VEC3_HPP

#include "aes.hpp"

namespace aes
{
	struct vec3
	{
		constexpr vec3() noexcept : x(0), y(0), z(0)
		{
		}

		constexpr vec3(vec3 const& v) noexcept : x(v.x), y(v.y), z(v.z)
		{

		}

		constexpr vec3(float v) noexcept : x(v), y(v), z(v)
		{
		}

		constexpr vec3(float vx, float vy) noexcept : x(vx), y(vy), z(0)
		{
		}

		constexpr vec3(float vx, float vy, float vz) noexcept : x(vx), y(vy), z(vz)
		{
		}

		constexpr float operator[](uint i) const noexcept
		{
			AES_ASSERT(i < 3);
			return data[i];
		}

		constexpr float& operator[](uint i) noexcept
		{
			AES_ASSERT(i < 3);
			return data[i];
		}

		constexpr vec3 operator-() const noexcept
		{
			return vec3(-x, -y, -z);
		}

		constexpr vec3& operator+=(float v) noexcept
		{
			x += v;
			y += v;
			z += v;
			return *this;
		}

		constexpr vec3& operator-=(float v) noexcept
		{
			x -= v;
			y -= v;
			z -= v;
			return *this;
		}

		constexpr vec3& operator*=(float v) noexcept
		{
			x *= v;
			y *= v;
			z *= v;
			return *this;
		}

		constexpr vec3& operator/=(float v) noexcept
		{
			x /= v;
			y /= v;
			z /= v;
			return *this;
		}

		constexpr float dot(vec3 rhs) const noexcept
		{
			return x * rhs.x + y * rhs.y + z * rhs.z;
		}

		constexpr float sqrLength() const noexcept
		{
			return x * x + y * y + z * z;
		}

		/*constexpr*/ float length() const noexcept
		{
			return sqrt(sqrLength());
		}

		/*constexpr*/ void normalize() noexcept;

		/*constexpr*/ vec3 getNormalized() const noexcept
		{
			vec3 v(*this);
			v.normalize();
			return v;
		}

		union
		{
			float data[3];
			struct { float x, y, z; };
			struct { float r, g, b; };
		};
	};

	constexpr vec3 operator+(vec3 const& lhs, float v) noexcept
	{
		return vec3(lhs.x + v, lhs.y + v, lhs.z + v);
	}

	constexpr vec3 operator-(vec3 const& lhs, float v) noexcept
	{
		return vec3(lhs.x - v, lhs.y - v, lhs.z - v);
	}

	constexpr vec3 operator*(vec3 const& lhs, float v) noexcept
	{
		return vec3(lhs.x * v, lhs.y * v, lhs.z * v);
	}

	constexpr vec3 operator/(vec3 const& lhs, float v) noexcept
	{
		return vec3(lhs.x / v, lhs.y / v, lhs.z / v);
	}

	constexpr vec3 operator+(vec3 const& lhs, vec3 const& rhs) noexcept
	{
		return vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	constexpr vec3 operator-(vec3 const& lhs, vec3 const& rhs) noexcept
	{
		return vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}

	constexpr vec3 operator*(vec3 const& lhs, vec3 const& rhs) noexcept
	{
		return vec3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
	}

	constexpr vec3 operator/(vec3 const& lhs, vec3 const& rhs) noexcept
	{
		return vec3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
	}

	inline void vec3::normalize() noexcept
	{
		*this = *this / length();
	}

}

#endif