#include "fontRenderer.hpp"
#include <stb/stb_rect_pack.h>
#include <stb/stb_truetype.h>

#include "core/color.hpp"

using namespace aes;

FontRessource::FontRessource(IAllocator& alloc) noexcept : backedChars(alloc)
{

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

	int width = 1024, height = 1024;

	stbtt_pack_context packContext;
	Array<unsigned char> bitmap(allocator);
	bitmap.resize(width * height);
	Array<stbtt_packedchar> packChars(allocator);
	packChars.resize(127);

	stbtt_PackBegin(&packContext, bitmap.data(), width, height, 0, 1, nullptr);
	stbtt_PackSetOversampling(&packContext, 2, 2);

	stbtt_PackFontRange(&packContext, fontData.data(), 0, 40, 'a', 127, packChars.data());

	stbtt_PackEnd(&packContext);

	// @Review
	Array<Color> bitmap8pp(allocator);
	bitmap8pp.resize(width * height);

	for (int i = 0; i < bitmap8pp.size(); i++)
	{
		uint8_t const alpha = bitmap[i];
		bitmap8pp[i] = Color(alpha, alpha, alpha, alpha);
	}

	FontRessource fontRessource(allocator);
	{
		TextureDescription desc;
		desc.width = width;
		desc.height = height;
		desc.format = RHIFormat::R8G8B8A8_Uint; // @TODO
		desc.cpuAccess = CPUAccessFlagBits::None;
		desc.initialData = bitmap8pp.data();
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
