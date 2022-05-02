#ifndef AES_FONTRENDERER_HPP
#define AES_FONTRENDERER_HPP

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include "renderer/RHI/RHITexture.hpp"
#include "renderer/RHI/RHIBuffer.hpp"
#include "renderer/RHI/RHISampler.hpp"
#include "core/array.hpp"

namespace aes
{
	struct BackedChars
	{
		glm::vec2 bmin;
		glm::vec2 bmax;

		float xoff, yoff, xadvance;
		float xoff2, yoff2;

		glm::vec2 getSize() const;
	};

	struct FontRessource
	{
		FontRessource(IAllocator&) noexcept;

		RHITexture texture;
		RHISampler sampler;

		uint width, height;
		Array<BackedChars> backedChars;
	};

	Result<FontRessource> createFontRessource(IAllocator& allocator, std::span<uint8_t> fontData);

	class FontRenderer
	{
	public:
		struct Vertex
		{
			glm::vec2 pos;
			glm::vec2 uv;
			glm::vec4 color;
		};

		Result<void> init();

		void setFont(FontRessource& font);
		void debugDraw();

	private:
		FontRessource* currentFont = nullptr;

		RHIBuffer vertexBuffer;
		RHIBuffer indexBuffer;
		//Array<Vertex> vertices;
		//Array<uint16_t> indices;
	};
}

#endif