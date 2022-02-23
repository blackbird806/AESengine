#include "textureUtility.hpp"

void aes::buildCheckboard(std::span<Color> data, uint width, uint height, Color c1, Color c2, float segmentSize)
{
	AES_ASSERT(data.size() >= width * height);

	uint const segmentSizeX = std::round(width * segmentSize);
	uint const segmentSizeY = std::round(height * segmentSize);

	for (uint i = 0; i < width; i++)
	{
		uint const segmentNumI = i / segmentSizeX;
		for (uint j = 0; j < height; j++)
		{
			int const segmentNumJ = j / segmentSizeY;

			data[i + j * width] = segmentNumI % 2 != segmentNumJ % 2 ? c1 : c2;
		}
	}
}
