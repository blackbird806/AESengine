#ifndef DRAW2D_HPP
#define DRAW2D_HPP

#include <glm/glm.hpp>
#include <variant>
#include <vector>

#include "RHI/RHIBuffer.hpp"
#include "RHI/RHIShader.hpp"
#include "RHI/RHIVertexInputLayout.hpp"
#include "material.hpp"

#include "core/color.hpp"
#include "core/geometry.hpp"

namespace aes
{
	class Draw2d
	{
	public:

		void init();
		
		void setColor(Color color);
		void setMatrix(glm::mat2 const&);
		void drawLine(Line2D const& line);
		void drawRect(Rect const& rect);

		void executeDrawCommands();
		
	private:

		using Index_t = uint16_t;
		
		void ensureVertexBuffersCapacity(size_t size);
		void ensureIndexBuffersCapacity(size_t size);
		void ensureModelBuffersCapacity(size_t size);
		
		struct Vertex
		{
			glm::vec2 pos;
			glm::vec2 uv;
			Color color;
		};

		struct Model
		{
			glm::mat2 model;
		};

		enum class DrawCommandType
		{
			Lines,
			FillRects,
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

		RHIVertexShader vertexShader;
		RHIFragmentShader fragmentShader;
		
		RHIBuffer vertexBuffer;
		RHIBuffer indexBuffer;
		RHIBuffer modelBuffer;
	};
}

#endif