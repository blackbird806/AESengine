#include "D3D11RenderTarget.hpp"
#include "D3D11globals.hpp"
#include "renderer/RHI/D3D11/D3D11Elements.hpp"
#include <d3d11.h>
#include <dxgi.h>

using namespace aes;

Result<void> D3D11RenderTarget::init(RenderTargetDescription const& desc)
{
	AES_PROFILE_FUNCTION();

	D3D11_TEXTURE2D_DESC textureDesc;

	///////////////////////// Map's Texture
	// Initialize the  texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the texture description.
	textureDesc.Width = desc.width;
	textureDesc.Height = desc.height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = rhiFormatToApi(desc.format);
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	/////////////////////// Map's Render Target
	// Setup the description of the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the texture
	auto result = gD3D11Device->CreateTexture2D(&textureDesc, NULL, &renderTargetTextureMap);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("device->CreateTexture2D failed");
	}

	result = gD3D11Device->CreateRenderTargetView(renderTargetTextureMap, &renderTargetViewDesc, &renderTargetViewMap);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("device->CreateRenderTargetView failed");
	}

	return {};
}

void D3D11RenderTarget::destroy()
{
	if (renderTargetTextureMap != nullptr)
	{
		AES_ASSERT(renderTargetViewMap != nullptr);

	}
}

D3D11RenderTarget::~D3D11RenderTarget()
{
	destroy();
}