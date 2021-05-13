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

Result<Model> aes::createCube()
{
	AES_PROFILE_FUNCTION();

	Model cube;
	auto const result = cube.init(getCubeVertices(), cubeIndices);
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
	vertexBufferInfo.bindFlags = BindFlags::VertexBuffer;
	vertexBufferInfo.bufferUsage = BufferUsage::Immutable;
	vertexBufferInfo.sizeInBytes = vertices.size_bytes();
	vertexBufferInfo.initialData = (void*)vertices.data();
	
	auto err = vertexBuffer.init(vertexBufferInfo);
	if (!err)
		return err;
	
	BufferDescription indexBufferInfo{};
	indexBufferInfo.bindFlags = BindFlags::IndexBuffer;
	indexBufferInfo.bufferUsage = BufferUsage::Immutable;
	indexBufferInfo.sizeInBytes = indices.size_bytes();
	indexBufferInfo.initialData = (void*)indices.data();

	err = indexBuffer.init(indexBufferInfo);
	if (!err)
		return err;

	BufferDescription modelBufferInfo {};
	modelBufferInfo.bindFlags = BindFlags::UniformBuffer;
	modelBufferInfo.bufferUsage = BufferUsage::Dynamic;
	modelBufferInfo.sizeInBytes = sizeof(ModelBuffer);
	modelBufferInfo.cpuAccessFlags = (uint8_t)CPUAccessFlags::Write;

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
	renderContext.bindIndexBuffer(indexBuffer, TypeFormat::Uint32);
	renderContext.setDrawPrimitiveMode(DrawPrimitiveType::Triangles);
	renderContext.drawIndexed(indexCount);
}
