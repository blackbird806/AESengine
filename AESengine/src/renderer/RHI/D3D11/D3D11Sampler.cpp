#include "D3D11Sampler.hpp"
#include "renderer/RHI/D3D11/D3D11renderer.hpp"
#include "D3D11Elements.hpp"

using namespace aes;

D3D11Sampler::D3D11Sampler(D3D11Sampler&& rhs) noexcept
{
	samplerState = rhs.samplerState;
	rhs.samplerState = nullptr;
}

Result<void> D3D11Sampler::init(SamplerDescription const& desc)
{
	ID3D11Device* device = D3D11Renderer::instance().getDevice();

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = rhiTextureFilterToApi(desc.filter);
	samplerDesc.AddressU = rhiTextureAddressModeToApi(desc.addressU);
	samplerDesc.AddressV = rhiTextureAddressModeToApi(desc.addressV);
	samplerDesc.AddressW = rhiTextureAddressModeToApi(desc.addressU); // @Review
	samplerDesc.MipLODBias = desc.lodBias; // @Review 
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = desc.lodMin; // @Review 
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	auto result = device->CreateSamplerState(&samplerDesc, &samplerState);
	if (FAILED(result))
		return { AESError::SamplerCreationFailed };
	return {};
}

D3D11Sampler& D3D11Sampler::operator=(D3D11Sampler&& rhs) noexcept
{
	destroy();
	samplerState = rhs.samplerState;
	rhs.samplerState = nullptr;
	return *this;
}

void D3D11Sampler::destroy() noexcept
{
	if (samplerState)
	{
		samplerState->Release();
		samplerState = nullptr;
	}
}

D3D11Sampler::~D3D11Sampler()
{
	destroy();
}

ID3D11SamplerState* D3D11Sampler::getSamplerState()
{
	return samplerState;
}
