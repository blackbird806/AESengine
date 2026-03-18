#ifndef WOB_DRAW3D_HPP
#define WOB_DRAW3D_HPP

#include "graphicsPipeline.hpp"
#include "core/array.hpp"
#include "core/geometry.hpp"
#include "core/color.hpp"
#include "core/matrix.hpp"

namespace wob
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
		
		void executeDrawCommands();

		static constexpr int32_t vertexBufferSize = 64_kb;
		static constexpr int32_t indexBufferSize = 32_kb;

		GraphicsPipeline pipeline;

	private:

		struct Vertex
		{
			vec3 pos;
			vec4 color;
		};

		struct State
		{
			Color color = Color::Blue;
			mat4 model = mat4::identity();
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
		};

		RHIDevice* device;
		Array<Command> commands;
		State currentState;
		Array<State> states;

		Array<Vertex> vertices;
		Array<uint16_t> indices;
		uint16_t indicesOffset = 0;

		RHIBuffer vertexBuffer;
		RHIBuffer indexBuffer;
	};
}

#endif