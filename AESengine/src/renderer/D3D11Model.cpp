#include "D3D11Model.hpp"

#include "core/debug.hpp"
#include "core/aes.hpp"
#include "D3D11renderer.hpp"

using namespace aes;

std::vector<Vertex> aes::getCubeVertices()
{
	std::vector<Vertex> vertices(8);

	vertices[0].pos = { -1, -1,  1.0f };
	vertices[0].color = { 1.0f, 0.0f, 0.0f, 1.0f };

	vertices[1].pos = { 1, -1,  1.0f };
	vertices[1].color = {0.0f, 1.0f, 0.0f, 1.0f};

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

void D3D11Model::init(std::span<Vertex const> vertices, std::span<uint32_t const> indices)
{
	AES_PROFILE_FUNCTION();

	vertexCount = vertices.size();

	indexCount = indices.size();

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ID3D11Device* device = D3D11Renderer::Instance().getDevice();
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		throw Exception("failed to create vertex buffer");
	}
	
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(uint32_t) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if (FAILED(result))
	{
		throw Exception("failed to create index buffer");
	}
}

void D3D11Model::destroy()
{
	AES_PROFILE_FUNCTION();

	AES_ASSERT(vertexBuffer != nullptr);
	AES_ASSERT(indexBuffer != nullptr);
	vertexBuffer->Release();
	indexBuffer->Release();
}

void D3D11Model::render(ID3D11DeviceContext* deviceContext)
{
	AES_PROFILE_FUNCTION();

	// Set vertex buffer stride and offset.
	uint stride = sizeof(Vertex);
	uint offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
