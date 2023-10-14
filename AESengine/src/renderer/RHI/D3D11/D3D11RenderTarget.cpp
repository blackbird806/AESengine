#include "D3D11RenderTarget.hpp"
#include "D3D11globals.hpp"
#include "renderer/RHI/D3D11/D3D11Elements.hpp"
#include <d3d11.h>
#include <dxgi.h>

using namespace aes;

void D3D11RenderTarget::destroy()
{
	if (renderTargetTexture != nullptr)
	{
		AES_ASSERT(renderTargetView != nullptr);
		renderTargetView->Release();
		renderTargetTexture->Release();
	}
}

aes::D3D11RenderTarget::D3D11RenderTarget(D3D11RenderTarget&& rhs) noexcept
{
	*this = std::move(rhs);
}

D3D11RenderTarget::~D3D11RenderTarget()
{
	destroy();
}

D3D11RenderTarget& aes::D3D11RenderTarget::operator=(D3D11RenderTarget&& rhs) noexcept
{
	destroy();
	renderTargetTexture = rhs.renderTargetTexture;
	renderTargetView = rhs.renderTargetView;
	rhs.renderTargetTexture = nullptr;
	rhs.renderTargetView = nullptr;

	return *this;
}
