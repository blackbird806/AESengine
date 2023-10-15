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

	depthStencilBuffer = rhs.depthStencilBuffer;
	rhs.depthStencilBuffer = nullptr;

	depthStencilView = rhs.depthStencilView;
	rhs.depthStencilView = nullptr;

	rt = rhs.rt;
	rhs.rt = nullptr;

	rtview = rhs.rtview;
	rhs.rtview = nullptr;

	return *this;
}

void D3D11Swapchain::destroy() noexcept
{
	if (swapchain)
	{
		swapchain->Release();
		swapchain = nullptr;

		depthStencilBuffer->Release();
		depthStencilBuffer = nullptr;

		depthStencilView->Release();
		depthStencilView = nullptr;

		rt->Release();
		rt = nullptr;

		rtview->Release();
		rtview = nullptr;
	}
}

D3D11Swapchain::~D3D11Swapchain()
{
	destroy();
}
