#include "core/aes.hpp"
#include "renderer/RHI/D3D11/D3D11Buffer.hpp"

#include "D3D11Elements.hpp"
#include "D3D11globals.hpp"

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
	validateBufferDescription(desc);
	//AES_ASSERT(desc.sizeInBytes % 16 == 0); // D3D11 buffers size must be a multiple of 16

	size = desc.sizeInBytes;
	
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = rhiMemoryUsageToApi(desc.usage);
	bufferDesc.ByteWidth = size;
	bufferDesc.BindFlags = rhiBufferBindFlagsToApi(desc.bindFlags);
	bufferDesc.CPUAccessFlags = rhiCPUAccessFlagsToApi(desc.cpuAccessFlags);
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	ID3D11Device* device = gD3D11Device;
	
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
		AES_LOG_ERROR("Failed to create GPU buffer");
		return { AESError::GPUBufferCreationFailed };
	}
	
	return {};
}

Result<void> D3D11Buffer::copyTo(D3D11Buffer& dest)
{
	gD3D11DeviceContext->CopyResource(dest.getHandle(), apiBuffer);
	return {};
}

ID3D11Buffer* D3D11Buffer::getHandle() noexcept
{
	return apiBuffer;
}

Result<void*> D3D11Buffer::map()
{

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	auto const result = gD3D11DeviceContext->Map(apiBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return { AESError::GPUBufferMappingFailed };
	}
	return { mappedResource.pData };
}

Result<void> D3D11Buffer::unmap()
{
	gD3D11DeviceContext->Unmap(apiBuffer, 0);
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
