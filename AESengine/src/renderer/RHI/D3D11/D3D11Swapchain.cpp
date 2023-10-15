#include "D3D11Swapchain.hpp"
#include <d3d11.h>
#include <utility>

using namespace aes;

D3D11Swapchain::D3D11Swapchain(D3D11Swapchain&& rhs) noexcept
{
	*this = std::move(rhs);
}

D3D11Swapchain& D3D11Swapchain::operator=(D3D11Swapchain&& rhs) noexcept
{
	destroy();
	swapchain = rhs.swapchain;
	rhs.swapchain = nullptr;
	return *this;
}

uint aes::D3D11Swapchain::getTexturesCount() const noexcept
{
	return rts.size();
}

void D3D11Swapchain::destroy() noexcept
{
	if (swapchain)
	{
		swapchain->Release();
		swapchain = nullptr;
	}
}

D3D11Swapchain::~D3D11Swapchain()
{
	destroy();
}
