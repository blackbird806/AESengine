#include "core/aes.hpp"
#include "renderer/RHI/D3D11/D3D11Buffer.hpp"

#include "D3D11Elements.hpp"

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


size_t D3D11Buffer::getSize() const
{
	return size;
}

bool D3D11Buffer::isValid() const
{
	return size != 0;
}
