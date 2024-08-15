#ifndef AES_DRAW2D_HPP
#define AES_DRAW2D_HPP

#include <string_view>

#include "core/error.hpp"
#include "core/array.hpp"
#include "RHI/RHIBuffer.hpp"
#include "RHI/RHIShader.hpp"
#include "RHI/RHITexture.hpp"

#include "core/color.hpp"
#include "core/geometry.hpp"
#include "RHI/RHISampler.hpp"

#include "core/vec2.hpp"
#include "core/matrix.hpp"

namespace aes
{
	struct FontRessource;

	/*
	 * Batch 2D renderer inspired by Blah and SDL2
	 */
	class Draw2d
	{
	public:

		Result<void> init();
		
		void setColor(Color color);
		void setMatrix(mat3 const&);
		void pushState();
		void popState();
		void drawLine(Line2D const& line);
		void drawPoint(vec2 p, float size = 0.05f);
		void drawFillRect(Rect const& rect);
		void drawRect(Rect const& rect);
		void drawImage(RHITexture& texture, Rect const& rect);
		void drawText(FontRessource& font, std::string_view str, vec2 pos);

		void executeDrawCommands();
		
	private:

		using Index_t = uint16_t;
		
		Result<void> ensureVertexBufferCapacity(size_t sizeInBytes);
		Result<void> ensureIndexBufferCapacity(size_t sizeInBytes);

		struct TextureVertex
		{
			vec2 pos;
			vec4 color;
			vec2 uv;
		};

		struct UniformBuffer
		{
			mat4 transformMtr = mat4::identity();
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
			mat3 transformationMatrix = mat3::identity();
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

		Array<TextureVertex> vertices;
		Array<Index_t> indices;
		Index_t offset = 0;

		RHIVertexShader vertexShader;
		RHIFragmentShader fragmentShader;
		RHISampler sampler;

		RHIBuffer vertexBuffer;
		RHIBuffer indexBuffer;

		RHIBuffer uniformBuffer;
	};
}

#endif