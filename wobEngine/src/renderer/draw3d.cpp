#include "draw3d.hpp"

using namespace aes;

void Draw3D::init(RHIDevice& device)
{
	pipeline.init(&device);
	states.emplace();
}

void Draw3D::setColor(Color col)
{
	modifyState().color = col;
}

void Draw3D::drawWireCube(Cube cube)
{
	Command cmd;
}

Draw3D::State& Draw3D::currentState()
{
	return states.back();
}

Draw3D::State& Draw3D::modifyState()
{
	states.push(states.back());
}
