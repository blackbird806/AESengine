#include "draw2d.hpp"

#include "core/utility.hpp"
#include "RHI/RHIRenderContext.hpp"
#include <glm/ext/matrix_clip_space.hpp>

using namespace aes;

Result<void> Draw2d::init()
{
	AES_PROFILE_FUNCTION();
	
	VertexInputLayout vertexInputLayout[2];
	vertexInputLayout[0].semantic = SemanticType::Position;
	vertexInputLayout[0].parameterName = "aPosition";
	vertexInputLayout[0].offset = 0;
	vertexInputLayout[0].format = RHIFormat::R32G32_Float;

	//vertexInputLayout[1].semantic = SemanticType::TexCoord;
	//vertexInputLayout[1].parameterName = "aTexCoord";
	// vertexInputLayout[1].offset = sizeof(glm::vec2);
	// vertexInputLayout[1].format = RHIFormat::R32G32_Float;

	vertexInputLayout[1].semantic = SemanticType::Color;
	vertexInputLayout[1].parameterName = "aColor";
	vertexInputLayout[1].offset = sizeof(glm::vec2);
	vertexInputLayout[1].format = RHIFormat::R32G32B32A32_Float;
	
	VertexShaderDescription vertexShaderDescription;
#ifdef __vita__
	auto const source_vs = readFileBin("app0:assets/shaders/vita/basic2d_vs.gxp");
	vertexShaderDescription.source = source_vs.data();
#else
	vertexShaderDescription.source = readFile("assets/shaders/HLSL/draw2d.vs");
#endif
	vertexShaderDescription.verticesLayout = vertexInputLayout;
	vertexShaderDescription.verticesStride = sizeof(Vertex);

	auto err = vertexShader.init(vertexShaderDescription);
	if (!err)
		return err;

	FragmentShaderDescription fragmentShaderDescription;
#ifdef __vita__
	auto const source_fs = readFileBin("app0:assets/shaders/vita/basic2d_fs.gxp");
	fragmentShaderDescription.source = source_fs.data();
	fragmentShaderDescription.gxpVertexProgram = vertexShader.getGxpShader();
#else
	fragmentShaderDescription.source = readFile("assets/shaders/HLSL/draw2d.fs");
#endif
	err = fragmentShader.init(fragmentShaderDescription);
	if (!err)
		return err;
	
	ensureVertexBuffersCapacity(20 * sizeof(Vertex));
	ensureIndexBuffersCapacity(40 * sizeof(Index_t));

	AES_LOG("draw2d initialized");
	return {};
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
	commands.push_back(Command{ DrawCommandType::Line, currentState.color });
	
	glm::vec2 const from = line.p1 * currentState.transformationMatrix;
	glm::vec2 const to = line.p2 * currentState.transformationMatrix;
	vertices.push_back({ from, currentState.color.toVec4() });
	vertices.push_back({ to, currentState.color.toVec4() });
	indices.push_back(iOff + 0);
	indices.push_back(iOff + 1);
	iOff += 2;
}

void Draw2d::drawPoint(glm::vec2 p, float size)
{
	drawLine({ {p.x - size, p.y}, {p.x + size, p.y} });
	drawLine({ {p.x, p.y - size}, {p.x, p.y + size} });
}

void Draw2d::drawFillRect(Rect const& rect)
{
	AES_PROFILE_FUNCTION();
	commands.push_back(Command{ DrawCommandType::FillRect, currentState.color });
	Rect const transformedRect{
		rect.min * currentState.transformationMatrix, 
		rect.max * currentState.transformationMatrix };
	
	RectBounds const bounds = transformedRect.getBounds();
	vertices.push_back({ bounds.minL, currentState.color.toVec4() });
	vertices.push_back({ bounds.minR, currentState.color.toVec4() });
	vertices.push_back({ bounds.topL, currentState.color.toVec4() });
	vertices.push_back({ bounds.topR, currentState.color.toVec4() });

	indices.push_back(iOff + 0);
	indices.push_back(iOff + 1);
	indices.push_back(iOff + 2);
	indices.push_back(iOff + 3);
	iOff += 4;
}

void Draw2d::executeDrawCommands()
{
	AES_PROFILE_FUNCTION();

	iOff = 0;
	vertexBuffer.setData(vertices.data(), vertices.size() * sizeof(Vertex));
	indexBuffer.setData(indices.data(), indices.size() * sizeof(Index_t));

	auto& context = RHIRenderContext::instance();
	context.setVertexShader(vertexShader);
	context.setFragmentShader(fragmentShader);

	context.bindVertexBuffer(vertexBuffer, sizeof(Vertex));
	context.bindIndexBuffer(indexBuffer, IndexTypeFormat::Uint16);

	uint indicesOffset = 0;
	uint indicesCount;

	for (auto const& cmd : commands)
	{
		// @Review only draw triangles ?
		if (cmd.type == DrawCommandType::Line)
		{
			context.setDrawPrimitiveMode(DrawPrimitiveType::Lines);
			indicesCount = 2;
		}
		else
		{
			context.setDrawPrimitiveMode(DrawPrimitiveType::TriangleStrip);
			indicesCount = 4;
		}

		context.drawIndexed(indicesCount, indicesOffset);
		indicesOffset += indicesCount;
	}

	indices.clear();
	vertices.clear();
	commands.clear();
}

// @Review

Result<void> Draw2d::ensureVertexBuffersCapacity(size_t size)
{
	AES_PROFILE_FUNCTION();

	if (vertexBuffer.isValid() && vertexBuffer.getSize() > size)
		return {};

	size_t const newCapacity = size;
	
	// reallocate buffers
	BufferDescription vertexBufferDesc{};
	vertexBufferDesc.bindFlags = BindFlagBits::VertexBuffer;
	vertexBufferDesc.usage = MemoryUsage::Dynamic;
	vertexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
	vertexBufferDesc.sizeInBytes = newCapacity;

	RHIBuffer newBuffer;
	auto err = newBuffer.init(vertexBufferDesc);
	if (!err)
		return err;

	if (vertexBuffer.isValid())
	{
		err = vertexBuffer.copyTo(newBuffer);
		if (!err) 
			return err;
	}
	vertexBuffer = std::move(newBuffer);

	return {};
}

Result<void> Draw2d::ensureIndexBuffersCapacity(size_t size)
{
	AES_PROFILE_FUNCTION();

	if (indexBuffer.isValid() && indexBuffer.getSize() > size)
		return {};

	size_t const newCapacity = size;
	
	// reallocate buffers
	BufferDescription indexBufferDesc{};
	indexBufferDesc.bindFlags = BindFlagBits::IndexBuffer;
	indexBufferDesc.usage = MemoryUsage::Dynamic;
	indexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
	indexBufferDesc.sizeInBytes = newCapacity;

	RHIBuffer newBuffer;
	auto err = newBuffer.init(indexBufferDesc);
	if (!err)
		return err;
	
	if (indexBuffer.isValid())
	{
		err = indexBuffer.copyTo(newBuffer);
		if (!err)
			return err;
	}
	indexBuffer = std::move(newBuffer);
	
	return {};
}


