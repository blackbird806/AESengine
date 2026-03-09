#ifndef WOB_DRAW3D_HPP
#define WOB_DRAW3D_HPP

#include "graphicsPipeline.hpp"
#include "core/array.hpp"
#include "core/geometry.hpp"
#include "core/color.hpp"
#include "core/matrix.hpp"

namespace aes
{
	class Draw3D
	{
	public:

		void init(RHIDevice& device);

		void setColor(Color col);
		void drawWireCube(Cube cube);
		void drawFillCube(Cube cube);
		void drawLine(Line3D line);
		void drawTriangle(Triangle3D tri);
		void drawPlane(Plane plane);

	private:
		Result<void> ensureVertexBufferCapacity(size_t sizeInBytes);
		Result<void> ensureIndexBufferCapacity(size_t sizeInBytes);
		struct State
		{
			Color color = Color::Blue;
		};

		enum class DrawType
		{
			Line,
			Triangle
		};

		struct Command
		{
			DrawType type;
			State state;
			mat4 model;
		};

		RHIDevice* device;
		GraphicsPipeline pipeline;
		Array<Command> commands;
		State currentState;
		Array<State> states;

		RHIBuffer vertexBuffer;
		RHIBuffer indexBuffer;
	};
}

#endif