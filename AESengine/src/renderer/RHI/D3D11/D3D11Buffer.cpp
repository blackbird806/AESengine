#include "core/aes.hpp"
#include "renderer/RHI/D3D11/D3D11Buffer.hpp"

#include "D3D11Elements.hpp"
#include "D3D11renderer.hpp"
#include "renderer/vertex.hpp"

using namespace aes;

D3D11Buffer::D3D11Buffer(D3D11Buffer&& rhs) noexcept
	: apiBuffer(rhs.apiBuffer), size(size = rhs.size)
{
	rhs.apiBuffer = nullptr;
}

D3D11Buffer& D3D11Buffer::operator=(D3D11Buffer&& rhs) noexcept
{
	apiBuffer = rhs.apiBuffer;
	size = rhs.size;
	rhs.apiBuffer = nullptr;
	return *this;
}

D3D11Buffer::~D3D11Buffer()
{
	if (apiBuffer)
		apiBuffer->Release();
}

Result<void> D3D11Buffer::create(BufferDescription const& desc)
{
	size = desc.sizeInBytes;
	bindFlags = desc.bindFlags;
	
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = rhiBufferUsageToApi(desc.bufferUsage);
	bufferDesc.ByteWidth = size;
	bufferDesc.BindFlags = rhiBufferBindFlagsToApi(bindFlags);
	bufferDesc.CPUAccessFlags = rhiCPUAccessFlagToApi(desc.cpuAccessFlags);
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	ID3D11Device* device = D3D11Renderer::Instance().getDevice();
	
	HRESULT result;
	if (desc.initialData)
	{
		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = desc.initialData;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;
		result = device->CreateBuffer(&bufferDesc, &vertexData, &apiBuffer);
	}
	else
	{
		result = device->CreateBuffer(&bufferDesc, nullptr, &apiBuffer);
	}
	
	if (FAILED(result))
	{
		return { AESError::GPUBufferCreationFailed };
	}
	
	return {};
}

ID3D11Buffer* D3D11Buffer::getHandle() noexcept
{
	return apiBuffer;
}

Result<void*> D3D11Buffer::map()
{
	ID3D11DeviceContext* deviceContext = D3D11Renderer::Instance().getDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	auto const result = deviceContext->Map(apiBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return { AESError::GPUBufferMappingFailed };
	}
	return { mappedResource.pData };
}

Result<void> D3D11Buffer::unmap()
{
	ID3D11DeviceContext* deviceContext = D3D11Renderer::Instance().getDeviceContext();
	deviceContext->Unmap(apiBuffer, 0);
	return {};
}
