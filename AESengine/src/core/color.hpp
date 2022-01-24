#ifndef COLOR_HPP
#define COLOR_HPP

#include <cstdint>

#include <glm/vec4.hpp>

namespace aes
{
	// @Review
	struct Color
	{
		Color() {}
		
		Color(uint32_t abgr) :
			r(uint8_t(abgr & 0xff)),
			g(uint8_t((abgr >> 8) & 0xff)),
			b(uint8_t((abgr >> 16) & 0xff)),
			a(uint8_t((abgr >> 24) & 0xff))
		{
			
		}

		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}

		uint32_t argb() const { return ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r; }
		uint32_t rgba() const { return ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | (uint32_t)a; }

		glm::vec4 toVec4() const
		{
			return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
		}
		
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		enum {
			Red = 0xff0000ff,
			Green = 0xff00ff00,
			Blue = 0xffff0000,
			Black = 0xff000000,
			White = 0xffffffff
		};
	};
}

#endif