#ifndef COLOR_HPP
#define COLOR_HPP

#include <cstdint>

namespace aes
{
	struct Color {

		Color() {}
		Color(uint32_t abgr) {
			r = uint8_t(abgr & 0xff);
			g = uint8_t((abgr >> 8) & 0xff);
			b = uint8_t((abgr >> 16) & 0xff);
			a = uint8_t((abgr >> 24) & 0xff);
		}

		Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}

		uint32_t abgr() const { return ((uint32_t)a << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r; }

		uint8_t r{};
		uint8_t g{};
		uint8_t b{};
		uint8_t a{};

		static constexpr uint32_t Red = 0xff0000ff;
		static constexpr uint32_t Green = 0xff00ff00;
		static constexpr uint32_t Blue = 0xffff0000;
		static constexpr uint32_t Black = 0xff000000;
	};
}

#endif