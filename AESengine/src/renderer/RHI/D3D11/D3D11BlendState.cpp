#include "D3D11BlendState.hpp"
#include "D3D11renderer.hpp"
#include "D3D11Elements.hpp"

using namespace aes;

D3D11BlendState::D3D11BlendState(D3D11BlendState&& rhs) noexcept : blendState(rhs.blendState)
{
	rhs.blendState = nullptr;
}

Result<void> D3D11BlendState::init(BlendInfo& info)
{
	ID3D11Device* device = D3D11Renderer::instance().getDevice();

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = rhiBlendFactorToApi(info.colorSrc);
	blendDesc.RenderTarget[0].DestBlend = rhiBlendFactorToApi(info.colorDst);
	blendDesc.RenderTarget[0].BlendOp = rhiBlendOpToApi(info.colorOp);
	blendDesc.RenderTarget[0].SrcBlendAlpha = rhiBlendFactorToApi(info.alphaSrc);
	blendDesc.RenderTarget[0].DestBlendAlpha = rhiBlendFactorToApi(info.alphaDst);
	blendDesc.RenderTarget[0].BlendOpAlpha = rhiBlendOpToApi(info.alphaOp);
	// TODO
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	auto err = device->CreateBlendState(&blendDesc, &blendState);
	if (FAILED(err))
	{
		AES_LOG_ERROR("failed to create D3D11 Blendstate");
		return { AESError::BlendStateCreationFailed };
	}
	return {};
}

D3D11BlendState& D3D11BlendState::operator=(D3D11BlendState&& rhs) noexcept
{
	blendState = rhs.blendState;
	rhs.blendState = nullptr;
	return *this;
}

D3D11BlendState::~D3D11BlendState()
{
	if (blendState)
		blendState->Release();
}

ID3D11BlendState* D3D11BlendState::getHandle()
{
	return blendState;
}
