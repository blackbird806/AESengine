#include "immediateRenderer.hpp"
#include "core/debug.hpp"
#include "D3D11renderer.hpp"

using namespace aes;

void ImmediateRenderer::init()
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ImVertex) * maxVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = nullptr;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ID3D11Device* device = D3D11Renderer::Instance().getDevice();
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		throw Exception("failed to create immediate vertex buffer");
	}
}

void ImmediateRenderer::drawLine(glm::vec3 const& p1, glm::vec3 const& p2, Color col)
{
	AES_ASSERT(lineList.vertices.size() + 2 < maxVertices);
	lineList.type = PrimitiveType::Line;
	lineList.vertices.push_back({ p1, col });
	lineList.vertices.push_back({ p2, col });
}

void ImmediateRenderer::submitDrawList(DrawList const& list)
{
	ID3D11DeviceContext* ctx = D3D11Renderer::Instance().getDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	// Lock the constant buffer so it can be written to.
	auto result = ctx->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		throw Exception("failed to map immediate vertexBuffer");
	}

	// Get a pointer to the data in the constant buffer.
	ImVertex* dataPtr = (ImVertex*)mappedResource.pData;
	memcpy(dataPtr, lineList.vertices.data(), lineList.vertices.size() * sizeof(ImVertex));
	// Unlock the constant buffer.
	ctx->Unmap(vertexBuffer, 0);

	uint stride = sizeof(ImVertex);
	uint offset = 0;
	ctx->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	switch (list.type)
	{
	case PrimitiveType::Line:
		ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		break;

	default:
		break;
	}
}
