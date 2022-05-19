#include "fontRenderer.hpp"
#include <ranges>
#include <stb/stb_rect_pack.h>
#include <stb/stb_truetype.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "core/color.hpp"

using namespace aes;

FontRessource::FontRessource(IAllocator& alloc) noexcept : glyphs(alloc)
{

}

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

Result<FontRessource> aes::createFontRessource(IAllocator& allocator, std::span<uint8_t> fontData)
{
	AES_PROFILE_FUNCTION();

	stbtt_fontinfo info;
	if (stbtt_InitFont(&info, fontData.data(), 0) == 0)
	{
		AES_LOG_ERROR("failed to init default font");
		return { AESError::FontInitFailed };
	}
	FontRessource fontRessource(allocator);

	// @review
	int const width = 1024, height = 1024;

	stbtt_pack_context packContext;
	Array<unsigned char> bitmap(allocator);
	bitmap.resize(width * height);
	Array<stbtt_packedchar> packChars(allocator);
	packChars.resize(127);

	stbtt_PackBegin(&packContext, bitmap.data(), width, height, 0, 1, nullptr);

	stbtt_PackSetOversampling(&packContext, 2, 2);
	char const startChar = ' ';
	stbtt_PackFontRange(&packContext, fontData.data(), 0, 20, startChar, 127, packChars.data());

	stbtt_PackEnd(&packContext);

	fontRessource.glyphs.resize(packChars.size());
	for (uint32_t i = 0; i < packChars.size(); i++)
	{
		auto const& pc = packChars[i];
		fontRessource.glyphs[i] = Glyph{
			.c = static_cast<char>(startChar + i),
			.x = {pc.x0, pc.x1},
			.y = {pc.y0, pc.y1},
			.u = {(float)pc.x0 / width, (float)(pc.x1) / width},
			.v = {(float)pc.y0 / height, (float)(pc.y1) / height},
			.xoff = pc.xoff, .yoff = pc.yoff,
			.xadvance = pc.xadvance/width,
		};
	}

	// @Review
	Array<Color> pixels(allocator);
	pixels.resize(width * height);

	for (int i = 0; i < pixels.size(); i++)
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
		desc.usage = MemoryUsage::Immutable;
		desc.mipsLevel = 1;
		fontRessource.texture.init(desc);
	}
	return { std::move(fontRessource) };
}

Result<void> FontRenderer::init()
{
	AES_PROFILE_FUNCTION();
	{
		BufferDescription desc;
		desc.bindFlags = BindFlagBits::VertexBuffer;
		desc.cpuAccessFlags = CPUAccessFlagBits::Write;
		desc.usage = MemoryUsage::Dynamic;
		desc.sizeInBytes = 4_mb; // @Review
		auto const err = vertexBuffer.init(desc);
		if (!err)
			return err;
	}
	{
		BufferDescription desc;
		desc.bindFlags = BindFlagBits::IndexBuffer;
		desc.cpuAccessFlags = CPUAccessFlagBits::Write;
		desc.usage = MemoryUsage::Dynamic;
		desc.sizeInBytes = 2_mb; // @Review
		auto const err = indexBuffer.init(desc);
		if (!err)
			return err;
	}

	return {};
}

void FontRenderer::setFont(FontRessource& font)
{
	currentFont = &font;
}

void FontRenderer::debugDraw()
{

}
