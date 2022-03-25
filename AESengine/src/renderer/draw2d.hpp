#ifndef AES_DRAW2D_HPP
#define AES_DRAW2D_HPP

#include <glm/glm.hpp>
#include <vector>

#include "core/error.hpp"
#include "RHI/RHIBuffer.hpp"
#include "RHI/RHIShader.hpp"

#include "core/color.hpp"
#include "core/geometry.hpp"

namespace aes
{
	class Draw2d
	{
	public:

		Result<void> init();
		
		void setColor(Color color);
		void setMatrix(glm::mat2 const&);
		void drawLine(Line2D const& line);
		void drawPoint(glm::vec2 p, float size = 0.05f);
		void drawFillRect(Rect const& rect);

		void executeDrawCommands();
		
	private:

		using Index_t = uint16_t;
		
		Result<void> ensureVertexBuffersCapacity(size_t sizeInBytes);
		Result<void> ensureIndexBuffersCapacity(size_t sizeInBytes);
		
		struct Vertex
		{
			glm::vec2 pos;
			//glm::vec2 uv;
			glm::vec4 color; // @Review use Color instead of vec4 ?
		};

		enum class DrawCommandType
		{
			Line,
			FillRect,
		};
		
		struct Command
		{
			DrawCommandType type;
			Color color;
		};

		struct State
		{
			Color color;
			glm::mat2 transformationMatrix;
		};
		
		State currentState;
		std::vector<Command> commands;
		std::vector<Vertex> vertices;
		std::vector<Index_t> indices;
		uint iOff = 0;
		
		RHIVertexShader vertexShader;
		RHIFragmentShader fragmentShader;
		//RHIBlendState blendState;
		
		RHIBuffer vertexBuffer;
		RHIBuffer indexBuffer;
		RHIBuffer projectionBuffer;
	};
}

#endif