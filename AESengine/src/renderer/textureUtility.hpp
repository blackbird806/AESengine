#ifndef AES_TEXTUREUTILITY_HPP
#define AES_TEXTUREUTILITY_HPP


#include "core/aes.hpp"
#include "core/color.hpp"
#include "core/arrayView.hpp"

namespace aes
{
	void buildCheckboard(ArrayView<Color> data, uint width, uint height, Color c1, Color c2, float segmentSize);
}

#endif