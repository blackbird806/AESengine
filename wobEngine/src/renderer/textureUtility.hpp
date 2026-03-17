#ifndef WOB_TEXTUREUTILITY_HPP
#define WOB_TEXTUREUTILITY_HPP

#include <span>

#include "core/wob.hpp"
#include "core/color.hpp"

namespace wob
{
	void buildCheckboard(std::span<Color> data, uint width, uint height, Color c1, Color c2, float segmentSize);
}

#endif