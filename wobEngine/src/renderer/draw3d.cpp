#include "draw3d.hpp"
#include "RHI/RHIDevice.hpp"

using namespace wob;

void Draw3D::init(RHIDevice& device_)
{
	WOB_PROFILE_FUNCTION();

	device = &device_;
	pipeline.init(device);
	states.emplace();

	BufferDescription vertexBufferDesc{};
	vertexBufferDesc.bindFlags = BindFlagBits::VertexBuffer;
	vertexBufferDesc.usage = MemoryUsage::Dynamic;
	vertexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
	vertexBufferDesc.sizeInBytes = vertexBufferSize;
	vertexBuffer = device->createBuffer(vertexBufferDesc).value();

	BufferDescription indexBufferDesc{};
	indexBufferDesc.bindFlags = BindFlagBits::IndexBuffer;
	indexBufferDesc.usage = MemoryUsage::Dynamic;
	indexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
	indexBufferDesc.sizeInBytes = vertexBufferSize;
	indexBuffer = device->createBuffer(indexBufferDesc).value();
}

void Draw3D::setColor(Color col)
{
	currentState.color = col;
}

void Draw3D::drawWireCube(Cube cube)
{
	Command cmd;
}

void Draw3D::drawFillCube(Cube cube)
{

}

void Draw3D::drawLine(Line3D line)
{

}

void Draw3D::executeDrawCommands()
{
	device->bindVertexBuffer(vertexBuffer, 0, pipeline.getVertexShaderDesc().verticesStride);
	device->bindIndexBuffer(indexBuffer, IndexTypeFormat::Uint16);
	pipeline.bind();
}



