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

		State& currentState();
		State& modifyState();

		RHIDevice* device;
		GraphicsPipeline pipeline;
		uint32_t currentStateIdx;
		Array<Command> commands;
		Array<State> states;
	};
}

#endif