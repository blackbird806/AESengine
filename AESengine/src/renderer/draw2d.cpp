#include "draw2d.hpp"

using namespace aes;

void Draw2d::init()
{
	ensureVertexBuffersCapacity(200 * sizeof(Vertex));
	ensureIndexBuffersCapacity(200 * sizeof(Index_t));
	ensureModelBuffersCapacity(50 * sizeof(Model));
}

void Draw2d::setColor(Color color)
{
	AES_PROFILE_FUNCTION();
	currentState.color = color;
}

void Draw2d::drawLine(Line2D const& line)
{
	AES_PROFILE_FUNCTION();
	commands.push_back(Command{ line, currentState.color });
}

void Draw2d::drawRect(Rect const& rect)
{
	AES_PROFILE_FUNCTION();
	commands.push_back(Command{ rect, currentState.color });
}

void Draw2d::ensureVertexBuffersCapacity(size_t size)
{
	AES_PROFILE_FUNCTION();

	if (vertexBuffer.isValid() && vertexBuffer.getSize() > size)
		return;

	size_t const newCapacity = vertexBuffer.getSize() * 2;
	// reallocate buffers
	BufferDescription vertexBufferDesc{};
	vertexBufferDesc.bindFlags = BindFlags::VertexBuffer;
	vertexBufferDesc.bufferUsage = BufferUsage::Dynamic;
	vertexBufferDesc.cpuAccessFlags = (uint8_t)CPUAccessFlags::Write;
	vertexBufferDesc.sizeInBytes = newCapacity;

	RHIBuffer newBuffer;
	newBuffer.create(vertexBufferDesc);

	if (vertexBuffer.isValid())
	{
		vertexBuffer.copyTo(newBuffer);
	}
	vertexBuffer = std::move(newBuffer);
}

void Draw2d::ensureIndexBuffersCapacity(size_t size)
{
	AES_PROFILE_FUNCTION();

	if (indexBuffer.isValid() && indexBuffer.getSize() > size)
		return;

	size_t const newCapacity = indexBuffer.getSize() * 2;
	// reallocate buffers
	BufferDescription indexBufferDesc{};
	indexBufferDesc.bindFlags = BindFlags::IndexBuffer;
	indexBufferDesc.bufferUsage = BufferUsage::Dynamic;
	indexBufferDesc.cpuAccessFlags = (uint8_t)CPUAccessFlags::Write;
	indexBufferDesc.sizeInBytes = newCapacity;

	RHIBuffer newBuffer;
	newBuffer.create(indexBufferDesc);

	if (indexBuffer.isValid())
	{
		indexBuffer.copyTo(newBuffer);
	}
	indexBuffer = std::move(newBuffer);
}

void Draw2d::ensureModelBuffersCapacity(size_t size)
{
	AES_PROFILE_FUNCTION();

	if (modelBuffer.isValid() && modelBuffer.getSize() > size)
		return;

	size_t const newCapacity = modelBuffer.getSize() * 2;
	// reallocate buffers
	BufferDescription modelBufferDesc{};
	modelBufferDesc.bindFlags = BindFlags::UniformBuffer;
	modelBufferDesc.bufferUsage = BufferUsage::Dynamic;
	modelBufferDesc.cpuAccessFlags = (uint8_t)CPUAccessFlags::Write;
	modelBufferDesc.sizeInBytes = newCapacity;

	RHIBuffer newBuffer;
	newBuffer.create(modelBufferDesc);

	if (modelBuffer.isValid())
	{
		modelBuffer.copyTo(newBuffer);
	}
	modelBuffer = std::move(newBuffer);
}


