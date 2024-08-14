#include "fontRenderer.hpp"
#include <ranges>
#include <stb/stb_rect_pack.h>
#include <stb/stb_truetype.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "core/color.hpp"

using namespace aes;

std::optional<Glyph> FontRessource::getGlyph(char c) const
{
	auto const it = std::ranges::find_if(glyphs, [c](auto const& e)
		{
			return c == e.c;
		});
	if (it != glyphs.end())
		return *it;
	return {};
}

Result<FontRessource> aes::createFontRessource(FontParams const& params)
{
	AES_PROFILE_FUNCTION();
	AES_ASSERT(params.device);

	stbtt_fontinfo info;
	if (stbtt_InitFont(&info, params.fontData.data(), 0) == 0)
	{
		AES_LOG_ERROR("failed to init default font");
		return { AESError::FontInitFailed };
	}
	FontRessource fontRessource;

	int const width = static_cast<int>(params.textureWidth);
	int const height = static_cast<int>(params.textureHeight);

	int lineGap, ascent, descent;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
	fontRessource.yAdvance = (float)(ascent - descent + lineGap) / (float)width;

	stbtt_pack_context packContext;
	Array<unsigned char> bitmap;
	bitmap.resize(width * height);
	Array<stbtt_packedchar> packChars;
	packChars.resize(params.numCharInRange);

	float const SF = stbtt_ScaleForPixelHeight(&info, params.fontSize);

	stbtt_PackBegin(&packContext, bitmap.data(), width, height, 0, 1, nullptr);
	stbtt_PackSetOversampling(&packContext, params.oversampling, params.oversampling);
	stbtt_PackFontRange(&packContext, params.fontData.data(), 0, params.fontSize, params.startUnicode, params.numCharInRange, packChars.data());
	stbtt_PackEnd(&packContext);

	int fontBoundBoxX0, fontBoundBoxX1, fontBoundBoxY0, fontBoundBoxY1;
	stbtt_GetFontBoundingBox(&info, &fontBoundBoxX0, &fontBoundBoxX1, &fontBoundBoxY0, &fontBoundBoxY1);
	float const baseline = SF * (float) -fontBoundBoxY0;

	fontRessource.glyphs.resize(packChars.size());
	for (uint32_t i = 0; i < packChars.size(); i++)
	{
		auto const& pc = packChars[i];
		fontRessource.glyphs[i] = Glyph{
			.c = params.startUnicode + (int32_t)i,
			.x = {(float)pc.x0, (float)pc.x1},
			.y = {(float)pc.y0, (float)pc.y1},
			.u = {(float)pc.x0 / width, (float)(pc.x1) / width},
			.v = {(float)pc.y0 / height, (float)(pc.y1) / height},
			.xoff = pc.xoff/width, .yoff = pc.yoff/height,
			.xadvance = SF * pc.xadvance/width,
		};
	}

	// @Review
	Array<Color> pixels;
	pixels.resize(width * height);

	for (uint32_t i = 0; i < pixels.size(); i++)
	{
		uint8_t const alpha = bitmap[i];
		pixels[i] = Color(alpha, alpha, alpha, alpha);
	}

	{
		TextureDescription desc;
		desc.width = width;
		desc.height = height;
		desc.format = RHIFormat::R8G8B8A8_Uint; // @TODO
		desc.cpuAccess = CPUAccessFlagBits::None;
		desc.initialData = pixels.data();
		desc.usage = MemoryUsage::Default;
		desc.mipsLevel = 4;
		fontRessource.texture = params.device->createTexture(desc).value();
	}
	return { std::move(fontRessource) };
}
