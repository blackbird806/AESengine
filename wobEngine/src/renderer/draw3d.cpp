#include "draw3d.hpp"

using namespace aes;

void Draw3D::init(RHIDevice& device)
{
	pipeline.init(&device);
	states.emplace();

	//ensureVertexBufferCapacity(4096 * sizeof(TextureVertex));
	//ensureIndexBufferCapacity(8192 * sizeof(Index_t));
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

Result<void> Draw3D::ensureVertexBufferCapacity(size_t sizeInBytes)
{
	AES_PROFILE_FUNCTION();

	// reallocate buffers
	BufferDescription vertexBufferDesc{};
	vertexBufferDesc.bindFlags = BindFlagBits::VertexBuffer;
	vertexBufferDesc.usage = MemoryUsage::Dynamic;
	vertexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
	vertexBufferDesc.sizeInBytes = sizeInBytes;

	return device->ensureBufferCapacity(vertexBuffer, vertexBufferDesc);
}

Result<void> Draw3D::ensureIndexBufferCapacity(size_t sizeInBytes)
{
	AES_PROFILE_FUNCTION();

	// reallocate buffers
	BufferDescription indexBufferDesc{};
	indexBufferDesc.bindFlags = BindFlagBits::IndexBuffer;
	indexBufferDesc.usage = MemoryUsage::Dynamic;
	indexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
	indexBufferDesc.sizeInBytes = sizeInBytes;

	return device->ensureBufferCapacity(indexBuffer, indexBufferDesc);
}


