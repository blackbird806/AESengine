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
		void setMatrix(glm::mat3 const&);
		void pushState();
		void popState();
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

		struct UniformBuffer
		{
			glm::mat4 transformMtr = glm::mat4(1.0f);
		};

		enum class DrawCommandType
		{
			Line,
			FillRect,
			Image
		};

		struct State
		{
			Color color = Color::Blue;
			glm::mat3 transformationMatrix = glm::mat3(1.0f);
		};

		struct Command
		{
			DrawCommandType type;
			State state;
			RHITexture* texture = nullptr;
		};
		
		State currentState;
		Array<State> statesStack;

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

		RHIBuffer uniformBuffer;
	};
}

#endif