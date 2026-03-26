#ifndef WOB_FONTRENDERER_HPP
#define WOB_FONTRENDERER_HPP

#include "renderer/RHI/RHITexture.hpp"
#include "renderer/RHI/RHIBuffer.hpp"
#include "renderer/RHI/RHISampler.hpp"
#include "renderer/RHI/RHIDevice.hpp"
#include "core/array.hpp"
#include "core/arrayView.hpp"

#include "core/vec2.hpp"

// Font system RHI complient
// We want simple and efficient bitmap based font rendering

namespace wob
{
	struct Glyph
	{
		int32_t c;
		vec2 x, y;
		vec2 u;
		vec2 v;

		float xoff, yoff, xadvance;
	};

	struct FontRessource
	{
		Result<Glyph> getGlyph(char c) const;

		RHITexture texture;
		RHISampler sampler;
		float yAdvance = 0.1f;
		Array<Glyph> glyphs;
	};

	struct FontParams
	{
		RHIDevice* device;
		ArrayView<uint8_t const> fontData;
		float fontSize = 20;
		int startUnicode = 32; // 32 => space
		int numCharInRange = 127 - 32;
		uint textureWidth = 1024, textureHeight = 1024;
		uint oversampling = 2;
	};

	Result<FontRessource> createFontRessource(FontParams const& params);
}

#endif