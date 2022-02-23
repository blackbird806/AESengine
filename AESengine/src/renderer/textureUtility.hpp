#ifndef AES_TEXTUREUTILITY_HPP
#define AES_TEXTUREUTILITY_HPP

#include <span>

#include "core/aes.hpp"
#include "core/color.hpp"

namespace aes
{
	void buildCheckboard(std::span<Color> data, uint width, uint height, Color c1, Color c2, float segmentSize);
}

#endif