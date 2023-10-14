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
