#ifndef AES_D3D11SAMPLER_HPP
#define AES_D3D11SAMPLER_HPP

#include "core/error.hpp"
#include "renderer/RHI/RHIElements.hpp"
#include "D3D11BlendState.hpp"

namespace aes
{
	class D3D11Sampler
	{
	public:
		D3D11Sampler() = default;
		D3D11Sampler(D3D11Sampler&&) noexcept;
		Result<void> init(SamplerDescription const& desc);

		D3D11Sampler& operator=(D3D11Sampler&&) noexcept;
		~D3D11Sampler();

		ID3D11SamplerState* getSamplerState();

	private:
		ID3D11SamplerState* samplerState = nullptr;
	};

	using RHISampler = D3D11Sampler;
}

#endif