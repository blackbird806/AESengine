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
	destroy();
	apiBuffer = rhs.apiBuffer;
	size = rhs.size;
	rhs.apiBuffer = nullptr;
	return *this;
}

void D3D11Buffer::destroy() noexcept
{
	if (apiBuffer)
	{
		apiBuffer->Release();
		apiBuffer = nullptr;
	}
}

D3D11Buffer::~D3D11Buffer()
{
	destroy();
}

Result<void> D3D11Buffer::init(BufferDescription const& desc)
{
	AES_ASSERT(desc.sizeInBytes != 0);
	
	size = desc.sizeInBytes;
	
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = rhiMemoryUsageToApi(desc.usage);
	bufferDesc.ByteWidth = size;
	bufferDesc.BindFlags = rhiBufferBindFlagsToApi(desc.bindFlags);
	bufferDesc.CPUAccessFlags = rhiCPUAccessFlagsToApi(desc.cpuAccessFlags);
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	ID3D11Device* device = D3D11Renderer::instance().getDevice();
	
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

Result<void> D3D11Buffer::copyTo(D3D11Buffer& dest)
{
	ID3D11DeviceContext* deviceContext = D3D11Renderer::instance().getDeviceContext();
	deviceContext->CopyResource(dest.getHandle(), apiBuffer);
	return {};
}

ID3D11Buffer* D3D11Buffer::getHandle() noexcept
{
	return apiBuffer;
}

Result<void*> D3D11Buffer::map()
{
	ID3D11DeviceContext* deviceContext = D3D11Renderer::instance().getDeviceContext();

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
	ID3D11DeviceContext* deviceContext = D3D11Renderer::instance().getDeviceContext();
	deviceContext->Unmap(apiBuffer, 0);
	return {};
}

size_t D3D11Buffer::getSize() const
{
	return size;
}

bool D3D11Buffer::isValid() const
{
	return size != 0;
}
