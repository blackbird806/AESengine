#include "model.hpp"
#include "core/debug.hpp"
#include "core/utility.hpp"
#include "RHI/RHIRenderContext.hpp"

using namespace aes;

std::vector<Vertex> aes::getCubeVertices()
{
	std::vector<Vertex> vertices(8);

	vertices[0].pos = { -1, -1,  1.0f };
	vertices[0].color = { 1.0f, 0.0f, 0.0f, 1.0f };

	vertices[1].pos = { 1, -1,  1.0f };
	vertices[1].color = { 0.0f, 1.0f, 0.0f, 1.0f };

	vertices[2].pos = { -1,  1,  1.0f };
	vertices[2].color = { 0.0f, 0.0f, 1.0f, 1.0f };

	vertices[3].pos = { 1,  1,  1.0f };
	vertices[3].color = { 0.0f, 0.0f, 1.0f, 1.0f };

	vertices[4].pos = { -1, -1, -1.0f };
	vertices[4].color = { 0.0f, 1.0f, 1.0f, 1.0f };

	vertices[5].pos = { 1, -1, -1.0f };
	vertices[5].color = { 0.0f, 1.0f, 1.0f, 1.0f };

	vertices[6].pos = { -1,  1, -1.0f };
	vertices[6].color = { 0.0f, 1.0f, 0.0f, 1.0f };

	vertices[7].pos = { 1,  1, -1.0f };
	vertices[7].color = { 1.0f, 0.0f, 1.0f, 1.0f };

	return vertices;
}

std::vector<Vertex> aes::getCubeVertices(glm::vec4 const& color)
{
	std::vector<Vertex> vertices(8);

	vertices[0].pos = { -1, -1,  1.0f };
	vertices[0].color = color;

	vertices[1].pos = { 1, -1,  1.0f };
	vertices[1].color = color;

	vertices[2].pos = { -1,  1,  1.0f };
	vertices[2].color = color;

	vertices[3].pos = { 1,  1,  1.0f };
	vertices[3].color = color;

	vertices[4].pos = { -1, -1, -1.0f };
	vertices[4].color = color;

	vertices[5].pos = { 1, -1, -1.0f };
	vertices[5].color = color;

	vertices[6].pos = { -1,  1, -1.0f };
	vertices[6].color = color;

	vertices[7].pos = { 1,  1, -1.0f };
	vertices[7].color = color;

	return vertices;
}


Result<Model> aes::createCube()
{
	AES_PROFILE_FUNCTION();

	Model cube;
	auto const result = cube.init(getCubeVertices(), cubeIndices);
	if (!result)
		return { result.error() };
	return { std::move(cube) };
}

Result<Model> aes::createCube(glm::vec4 const& col)
{
	AES_PROFILE_FUNCTION();

	Model cube;
	auto const result = cube.init(getCubeVertices(col), cubeIndices);
	if (!result)
		return { result.error() };
	return { std::move(cube) };
}

Result<void> Model::init(std::span<Vertex const> vertices, std::span<uint32_t const> indices)
{
	AES_PROFILE_FUNCTION();

	vertexCount = vertices.size();
	indexCount = indices.size();

	BufferDescription vertexBufferInfo{};
	vertexBufferInfo.bindFlags = BindFlagBits::VertexBuffer;
	vertexBufferInfo.usage = MemoryUsage::Immutable;
	vertexBufferInfo.cpuAccessFlags = CPUAccessFlagBits::None;
	vertexBufferInfo.sizeInBytes = vertices.size_bytes();
	vertexBufferInfo.initialData = (void*)vertices.data();

	auto err = vertexBuffer.init(vertexBufferInfo);
	if (!err)
		return err;
	
	BufferDescription indexBufferInfo{};
	indexBufferInfo.bindFlags = BindFlagBits::IndexBuffer;
	indexBufferInfo.usage = MemoryUsage::Immutable;
	indexBufferInfo.cpuAccessFlags = CPUAccessFlagBits::None;
	indexBufferInfo.sizeInBytes = indices.size_bytes();
	indexBufferInfo.initialData = (void*)indices.data();

	err = indexBuffer.init(indexBufferInfo);
	if (!err)
		return err;

	BufferDescription modelBufferInfo {};
	modelBufferInfo.bindFlags = BindFlagBits::UniformBuffer;
	modelBufferInfo.usage = MemoryUsage::Dynamic;
	modelBufferInfo.sizeInBytes = sizeof(ModelBuffer);
	modelBufferInfo.cpuAccessFlags = CPUAccessFlagBits::Write;

	err = modelBuffer.init(modelBufferInfo);
	if (!err)
		return err;

	return {};
}

void Model::destroy()
{
	AES_PROFILE_FUNCTION();
}

void Model::draw()
{
	AES_PROFILE_FUNCTION();

	modelBuffer.setDataFromPOD(glm::transpose(toWorld));

	RHIRenderContext& renderContext = RHIRenderContext::instance();
	renderContext.bindVertexBuffer(vertexBuffer, sizeof(Vertex));
	renderContext.bindIndexBuffer(indexBuffer, IndexTypeFormat::Uint32);
	renderContext.setDrawPrimitiveMode(DrawPrimitiveType::Triangles);
	renderContext.drawIndexed(indexCount);
}
