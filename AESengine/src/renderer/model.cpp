#include "model.hpp"
#include "core/debug.hpp"
#include "core/utility.hpp"
#include "RHI/RHIRenderContext.hpp"
#include <array>

using namespace aes;

constexpr uint32_t cubeIndices[] = {
	//Top
	2, 7, 6,
	3, 7, 2,

	////Bottom
	0, 4, 5,
	0, 5, 1,

	////Left
	0, 2, 6,
	0, 6, 4,

	////Right
	1, 7, 3,
	1, 5, 7,

	//Front
	2, 0, 1,
	2, 1, 3,

	////Back
	4, 6, 7,
	4, 7, 5
};

std::array<Vertex, 8> getCubeVertices()
{
	std::array<Vertex, 8> vertices;

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

std::array<Vertex,8> getCubeVertices(glm::vec4 const& color)
{
	std::array<Vertex,8> vertices;

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
	auto const vertices = getCubeVertices();
	auto const result = cube.init(ArrayView<Vertex const>(vertices.data(), vertices.size()), 
		ArrayView<uint32_t const>(cubeIndices, sizeof(cubeIndices)));
	if (!result)
		return  AESError(result.error());
	return { std::move(cube) };
}

Result<Model> aes::createCube(glm::vec4 const& col)
{
	AES_PROFILE_FUNCTION();

	Model cube;
	auto const vertices = getCubeVertices(col);
	auto const result = cube.init(ArrayView<Vertex const>(vertices.data(), vertices.size()),
		ArrayView<uint32_t const>(cubeIndices, sizeof(cubeIndices)));
	if (!result)
		return  AESError(result.error());
	return { std::move(cube) };
}

Result<void> Model::init(ArrayView<Vertex const> vertices, ArrayView<uint32_t const> indices)
{
	AES_PROFILE_FUNCTION();

	vertexCount = vertices.size();
	indexCount = indices.size();

	BufferDescription vertexBufferInfo{};
	vertexBufferInfo.bindFlags = BindFlagBits::VertexBuffer;
	vertexBufferInfo.usage = MemoryUsage::Immutable;
	vertexBufferInfo.cpuAccessFlags = CPUAccessFlagBits::None;
	vertexBufferInfo.sizeInBytes = vertices.size() * sizeof(Vertex);
	vertexBufferInfo.initialData = (void*)vertices.data();

	auto err = vertexBuffer.init(vertexBufferInfo);
	if (!err)
		return err;
	
	BufferDescription indexBufferInfo{};
	indexBufferInfo.bindFlags = BindFlagBits::IndexBuffer;
	indexBufferInfo.usage = MemoryUsage::Immutable;
	indexBufferInfo.cpuAccessFlags = CPUAccessFlagBits::None;
	indexBufferInfo.sizeInBytes = indices.size() * sizeof(uint32_t);
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

void Model::draw()
{
	AES_PROFILE_FUNCTION();

	modelBuffer.setDataFromPOD(glm::transpose(toWorld));

	RHIRenderContext& renderContext = RHIRenderContext::instance();
	renderContext.setDrawPrimitiveMode(DrawPrimitiveType::TrianglesFill);
	renderContext.bindVertexBuffer(vertexBuffer, sizeof(Vertex));
	renderContext.bindIndexBuffer(indexBuffer, IndexTypeFormat::Uint32);
	renderContext.drawIndexed(indexCount);
}
