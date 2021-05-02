#include "draw2d.hpp"

using namespace aes;

void Draw2d::init()
{
	VertexInputLayout vertexInputLayout[3];
	vertexInputLayout[0].semantic = SemanticType::Position;
	vertexInputLayout[0].offset = 0;
	vertexInputLayout[0].format = RHIFormat::R32G32B32_Float;

	vertexInputLayout[1].semantic = SemanticType::TexCoord;
	vertexInputLayout[1].offset = sizeof(glm::vec3);
	vertexInputLayout[1].format = RHIFormat::R32G32_Float;

	vertexInputLayout[2].semantic = SemanticType::Color;
	vertexInputLayout[2].offset = sizeof(glm::vec3) + sizeof(glm::vec2);
	vertexInputLayout[2].format = RHIFormat::R32G32B32A32_Float;
	
	VertexShaderDescription vertexShaderDescription;
	vertexShaderDescription.source = ""; // TODO
	vertexShaderDescription.verticesLayout = vertexInputLayout;
	vertexShaderDescription.verticesStride = sizeof(Vertex);
	
	vertexShader.init(vertexShaderDescription);

	FragmentShaderDescription fragmentShaderDescription;
	fragmentShaderDescription.source = ""; // TODO

	fragmentShader.init(fragmentShaderDescription);
	
	ensureVertexBuffersCapacity(200 * sizeof(Vertex));
	ensureIndexBuffersCapacity(200 * sizeof(Index_t));
	ensureModelBuffersCapacity(50 * sizeof(Model));
}

void Draw2d::setColor(Color color)
{
	AES_PROFILE_FUNCTION();
	currentState.color = color;
}

void Draw2d::setMatrix(glm::mat2 const& mat)
{
	AES_PROFILE_FUNCTION();
	currentState.transformationMatrix = mat;
}

void Draw2d::drawLine(Line2D const& line)
{
	AES_PROFILE_FUNCTION();
	commands.push_back(Command{ DrawCommandType::Lines, currentState.color });
	// update ram vertices / indices
	// beware of alignement !
}

void Draw2d::drawRect(Rect const& rect)
{
	AES_PROFILE_FUNCTION();
	commands.push_back(Command{ DrawCommandType::FillRects, currentState.color });
}

void Draw2d::executeDrawCommands()
{
	// update buffers
	for (auto const& cmd : commands)
	{
		// execute commands
	}
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
	newBuffer.create(vertexBufferDesc); // TODO handle reallocation error

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


