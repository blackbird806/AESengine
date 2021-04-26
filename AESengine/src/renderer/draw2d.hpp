#ifndef DRAW2D_HPP
#define DRAW2D_HPP

#include <glm/glm.hpp>
#include <variant>
#include <vector>

#include "RHI/RHIBuffer.hpp"

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
		
	private:

		using Index_t = uint16_t;
		
		void ensureVertexBuffersCapacity(size_t size);
		void ensureIndexBuffersCapacity(size_t size);
		void ensureModelBuffersCapacity(size_t size);
		
		struct Vertex
		{
			glm::vec2 pos;
			Color color;
		};

		struct Model
		{
			glm::mat2 model;
		};
		
		struct Command
		{
			using Shape_t = std::variant<Line2D, Rect>;
			Shape_t shape;
			Color color;
		};

		struct State
		{
			Color color;
			glm::mat2 transformationMatrix;
		};
		
		State currentState;
		std::vector<Command> commands;
		
		RHIBuffer vertexBuffer;
		RHIBuffer indexBuffer;
		RHIBuffer modelBuffer;
	};
}

#endif