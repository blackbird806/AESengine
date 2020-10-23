#include "D3D11Model.hpp"

#include "core/debug.hpp"
#include "core/aes.hpp"

#include <vector>

using namespace aes;

void D3D11Model::init(ID3D11Device* device)
{
	AES_PROFILE_FUNCTION();

	vertexCount = 3;
	indexCount = 3;

	std::vector<Vertex> vertices(vertexCount);
	std::vector<uint32_t> indices(indexCount);

	// Load the vertex array with data.
	vertices[0].pos = { 0.0f, 0.5f, 0.0f };  // Bottom left.
	vertices[0].color = { 0.0f, 0.0f, 1.0f, 1.0f };

	vertices[1].pos = { 0.5f, -0.5f, 0.0f };  // Top middle.
	vertices[1].color = {0.0f, 0.0f, 0.0f, 1.0f};

	vertices[2].pos = { -0.5f, -0.5f, .0f };  // Bottom right.
	vertices[2].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	//vertices[0].y = 0.5;
	//vertices[0].g = 1;

	//vertices[1].y = -0.5;
	//vertices[1].x = 0.5;
	//vertices[1].b = 1.0;

	//vertices[2].y = -0.5;
	//vertices[2].x = -0.5;
	//vertices[2].r = 1.0;


	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

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

	HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create vertex buffer");
		return;
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
		AES_LOG_ERROR("failed to create index buffer");
		return;
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
