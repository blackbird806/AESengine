#ifndef AES_D3D11BLENDSTATE_HPP
#define AES_D3D11BLENDSTATE_HPP

#include <d3d11.h>

#include "core/error.hpp"
#include "renderer/RHI/RHIElements.hpp"

namespace aes
{
	// blend stated is proper to d3d11 so no RHI interface here
	// for a platform independent blend, you must specify it in the fragment shader (this is a constraint from GXM)
	class D3D11BlendState
	{
	public:
		D3D11BlendState() = default;
		D3D11BlendState(D3D11BlendState&&) noexcept;
		Result<void> init(BlendInfo const& info);
		
		D3D11BlendState& operator=(D3D11BlendState&&) noexcept;
		~D3D11BlendState();

		ID3D11BlendState* getHandle();
		
		private:
		ID3D11BlendState* blendState = nullptr;
	};
}

#endif