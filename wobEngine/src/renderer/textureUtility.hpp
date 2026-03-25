#ifndef WOB_TEXTUREUTILITY_HPP
#define WOB_TEXTUREUTILITY_HPP

#include "core/wob.hpp"
#include "core/color.hpp"

namespace wob
{
	void buildCheckboard(Color* data, uint width, uint height, Color c1, Color c2, float segmentSize);
}

#endif