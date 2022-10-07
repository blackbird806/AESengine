#ifndef AES_FONTRENDERER_HPP
#define AES_FONTRENDERER_HPP

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "renderer/RHI/RHITexture.hpp"
#include "renderer/RHI/RHIBuffer.hpp"
#include "renderer/RHI/RHISampler.hpp"
#include "core/array.hpp"

// Font system RHI complient
// We want simple and efficient bitmap based font rendering

namespace aes
{
	struct Glyph
	{
		int32_t c;
		glm::vec2 x, y;
		glm::vec2 u;
		glm::vec2 v;

		float xoff, yoff, xadvance;
	};

	struct FontRessource
	{
		FontRessource(IAllocator&) noexcept;

		std::optional<Glyph> getGlyph(char c) const;

		RHITexture texture;
		RHISampler sampler;
		float yAdvance = 0.1f;
		Array<Glyph> glyphs;
	};

	struct FontParams
	{
		std::span<uint8_t const> fontData;
		float fontSize = 20;
		int startUnicode = 32; // 32 => space
		int numCharInRange = 127 - 32;
		uint textureWidth = 1024, textureHeight = 1024;
		uint oversampling = 2;
	};

	Result<FontRessource> createFontRessource(IAllocator& allocator, FontParams const& params);
}

#endif