#include "D3D11Sampler.hpp"
#include "D3D11Elements.hpp"

using namespace aes;

D3D11Sampler::D3D11Sampler(D3D11Sampler&& rhs) noexcept
{
	samplerState = rhs.samplerState;
	rhs.samplerState = nullptr;
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
