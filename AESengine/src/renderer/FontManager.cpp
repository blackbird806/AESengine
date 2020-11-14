#include "FontManager.hpp"
#include <stb/stb_truetype.h>
#include <vector>

#include "core/debug.hpp"
#include "core/utility.hpp"

using namespace aes;

static const char* getAsciiStr()
{
	static char asciiTable[256];
	for (int i = 0; i < std::size(asciiTable); i++)
		asciiTable[i] = (char)i;
	return asciiTable;
}

#include <stdio.h>
#include <stdlib.h>

// https://github.com/justinmeiners/stb-truetype-example/blob/master/main.c
Result<void> FontManager::init()
{
	stbtt_fontinfo info;
	std::vector<unsigned char> courierFontBuffer = readFileBin("assets/fonts/courier.ttf");
	if (!stbtt_InitFont(&info, courierFontBuffer.data(), 0))
	{
		AES_LOG_ERROR("failed to init default font");
		return { AESError::FontInitFailed };
	}

	uint b_w = 512; /* bitmap width */
	uint b_h = 128; /* bitmap height */
	uint l_h = 64; /* line height */

	std::vector<uchar> bitmap(b_w * b_h);

	float scale = stbtt_ScaleForPixelHeight(&info, l_h);

	const char* alphabet = getAsciiStr();

	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
	ascent = roundf(ascent * scale);
	descent = roundf(descent * scale);

	int x = 0;
	for (int i = 0; i < strlen(alphabet); ++i)
	{
		/* how wide is this character */
		int ax;
		int lsb;
		stbtt_GetCodepointHMetrics(&info, alphabet[i], &ax, &lsb);

		/* get bounding box for character (may be offset to account for chars that dip above or below the line */
		int c_x1, c_y1, c_x2, c_y2;
		stbtt_GetCodepointBitmapBox(&info, alphabet[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

		/* compute y (different characters have different heights */
		int y = ascent + c_y1;

		/* render character (stride and offset is important here) */
		int byteOffset = x + roundf(lsb * scale) + (y * b_w);
		stbtt_MakeCodepointBitmap(&info, bitmap.data() + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, alphabet[i]);

		/* advance x */
		x += roundf(ax * scale);

		/* add kerning */
		int kern;
		kern = stbtt_GetCodepointKernAdvance(&info, alphabet[i], alphabet[i + 1]);
		x += roundf(kern * scale);
	}

	return {};
}
