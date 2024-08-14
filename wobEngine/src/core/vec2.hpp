#ifndef AES_VEC2_HPP
#define AES_VEC2_HPP

#include "aes.hpp"

namespace aes
{
	struct vec2
	{
		constexpr vec2() noexcept : x(0), y(0)
		{
		}

		constexpr vec2(vec2 const& v) noexcept : x(v.x), y(v.y)
		{

		}

		constexpr vec2(float v) noexcept : x(v), y(v)
		{
		}

		constexpr vec2(float vx, float vy) noexcept : x(vx), y(vy)
		{
		}

		constexpr float operator[](uint i) const noexcept
		{
			AES_ASSERT(i < 2);
			return data[i];
		}

		constexpr float& operator[](uint i) noexcept
		{
			AES_ASSERT(i < 2);
			return data[i];
		}


		constexpr vec2& operator+=(float v) noexcept
		{
			x += v;
			y += v;
			return *this;
		}

		constexpr vec2& operator-=(float v) noexcept
		{
			x -= v;
			y -= v;
			return *this;
		}

		constexpr vec2& operator*=(float v) noexcept
		{
			x *= v;
			y *= v;
			return *this;
		}

		constexpr vec2& operator/=(float v) noexcept
		{
			x /= v;
			y /= v;
			return *this;
		}

		constexpr float dot(vec2 rhs) const noexcept
		{
			return x * rhs.x + y * rhs.y;
		}

		constexpr float sqrLength() const noexcept
		{
			return x * x + y * y;
		}

		/*constexpr*/ float length() const noexcept
		{
			return sqrt(sqrLength());
		}

		/*constexpr*/ void normalize() noexcept;

		/*constexpr*/ vec2 getNormalized() const noexcept
		{
			vec2 v(*this);
			v.normalize();
			return v;
		}

		union
		{
			float data[2];
			struct { float x, y; };
			struct { float r, g; };
		};
	};

	constexpr vec2 operator+(vec2 lhs, float v) noexcept
	{
		return vec2(lhs.x + v, lhs.y + v);
	}

	constexpr vec2 operator-(vec2 lhs, float v) noexcept
	{
		return vec2(lhs.x - v, lhs.y - v);
	}

	constexpr vec2 operator*(vec2 lhs, float v) noexcept
	{
		return vec2(lhs.x * v, lhs.y * v);
	}

	constexpr vec2 operator/(vec2 lhs, float v) noexcept
	{
		return vec2(lhs.x / v, lhs.y / v);
	}

	constexpr vec2 operator+(vec2 lhs, vec2 rhs) noexcept
	{
		return vec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	constexpr vec2 operator-(vec2 lhs, vec2 rhs) noexcept
	{
		return vec2(lhs.x - rhs.x, lhs.y - rhs.y);
	}

	constexpr vec2 operator*(vec2 lhs, vec2 rhs) noexcept
	{
		return vec2(lhs.x * rhs.x, lhs.y * rhs.y);
	}

	constexpr vec2 operator/(vec2 lhs, vec2 rhs) noexcept
	{
		return vec2(lhs.x / rhs.x, lhs.y / rhs.y);
	}

	inline void vec2::normalize() noexcept
	{
		*this = *this / length();
	}

}

#endif