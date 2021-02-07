#include "D3D11Model.hpp"

#include "core/debug.hpp"
#include "core/aes.hpp"
#include "D3D11renderer.hpp"

using namespace aes;

Result<void> D3D11Model::init(std::span<Vertex const> vertices, std::span<uint32_t const> indices)
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

	ID3D11Device* device = D3D11Renderer::instance().getDevice();
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if (FAILED(result))
	{
		return { AESError::GPUBufferCreationFailed };
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
		return { AESError::GPUBufferCreationFailed };
	}

	D3D11_BUFFER_DESC modelBufferDesc = {};
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	modelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	modelBufferDesc.ByteWidth = sizeof(ModelBuffer);
	modelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	modelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	modelBufferDesc.MiscFlags = 0;
	modelBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&modelBufferDesc, nullptr, &modelBuffer);
	if (FAILED(result))
	{
		return { AESError::GPUBufferCreationFailed };
	}

	return {};
}

void D3D11Model::destroy()
{
	AES_PROFILE_FUNCTION();

	AES_ASSERT(vertexBuffer != nullptr);
	AES_ASSERT(indexBuffer != nullptr);
	AES_ASSERT(modelBuffer != nullptr);

	modelBuffer->Release();
	vertexBuffer->Release();
	indexBuffer->Release();
}

void D3D11Model::render()
{
	AES_PROFILE_FUNCTION();

	// Set vertex buffer stride and offset.
	uint stride = sizeof(Vertex);
	uint offset = 0;

	ID3D11DeviceContext* deviceContext = D3D11Renderer::instance().getDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	// Lock the constant buffer so it can be written to.
	auto result = deviceContext->Map(modelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		AES_ERROR("failed to map UBO");
		return;
	}

	// Get a pointer to the data in the constant buffer.
	ModelBuffer* dataPtr = (ModelBuffer*)mappedResource.pData;

	dataPtr->world = glm::transpose(toWorld);

	deviceContext->Unmap(modelBuffer, 0);

	deviceContext->VSSetConstantBuffers(1, 1, &modelBuffer);
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
