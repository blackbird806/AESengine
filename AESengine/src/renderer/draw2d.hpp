#ifndef AES_DRAW2D_HPP
#define AES_DRAW2D_HPP

#include <glm/glm.hpp>
#include <string_view>

#include "core/error.hpp"
#include "core/array.hpp"
#include "RHI/RHIBuffer.hpp"
#include "RHI/RHIShader.hpp"
#include "RHI/RHITexture.hpp"

#include "core/color.hpp"
#include "core/geometry.hpp"
#include "RHI/RHISampler.hpp"

namespace aes
{
	struct FontRessource;

	class Draw2d
	{
	public:

		Draw2d(IAllocator&);
		Result<void> init();
		
		void setColor(Color color);
		void setMatrix(glm::mat2 const&);
		void drawLine(Line2D const& line);
		void drawPoint(glm::vec2 p, float size = 0.05f);
		void drawFillRect(Rect const& rect);
		void drawRect(Rect const& rect);
		void drawImage(RHITexture& texture, Rect const& rect);
		void drawText(FontRessource& font, std::string_view str, glm::vec2 pos);

		void executeDrawCommands();
		
	private:

		using Index_t = uint16_t;
		
		Result<void> ensureColorVertexBufferCapacity(size_t sizeInBytes);
		Result<void> ensureColorIndexBufferCapacity(size_t sizeInBytes);

		Result<void> ensureTextureVertexBufferCapacity(size_t sizeInBytes);
		Result<void> ensureTextureIndexBufferCapacity(size_t sizeInBytes);

		struct ColorVertex
		{
			glm::vec2 pos;
			Color color;
		};

		struct TextureVertex
		{
			glm::vec2 pos;
			glm::vec2 uv;
		};

		enum class DrawCommandType
		{
			Line,
			FillRect,
			Image
		};
		
		struct Command
		{
			DrawCommandType type;
			Color color;
			RHITexture* texture = nullptr;
		};

		struct State
		{
			Color color = Color::Blue;
			glm::mat2 transformationMatrix = glm::mat2(1.0f);
		};
		
		State currentState;
		Array<Command> commands;

		Array<ColorVertex> colorVertices;
		Array<Index_t> colorIndices;
		Index_t colorOffset = 0;
		
		RHIVertexShader colorVertexShader;
		RHIFragmentShader colorFragmentShader;

		Array<TextureVertex> textureVertices;
		Array<Index_t> textureIndices;
		Index_t textureOffset = 0;

		RHIVertexShader textureVertexShader;
		RHIFragmentShader textureFragmentShader;
		RHISampler sampler;

		RHIBuffer colorVertexBuffer;
		RHIBuffer colorIndexBuffer;

		RHIBuffer textureVertexBuffer;
		RHIBuffer textureIndexBuffer;

		RHIBuffer projectionBuffer;
	};
}

#endif