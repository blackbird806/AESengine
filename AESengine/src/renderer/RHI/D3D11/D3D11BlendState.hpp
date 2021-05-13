#ifndef D3D11BLENDSTATE_HPP
#define D3D11BLENDSTATE_HPP

#include <d3d11.h>

#include "core/error.hpp"
#include "renderer/RHI/RHIElements.hpp"

namespace aes
{
	struct BlendInfo
	{
		BlendOp colorOp;
		BlendOp alphaOp;
		BlendFactor colorSrc;
		BlendFactor colorDst;
		BlendFactor alphaSrc;
		BlendFactor alphaDst;
	};
	
	class D3D11BlendState
	{
	public:
		D3D11BlendState() = default;
		D3D11BlendState(D3D11BlendState&&) noexcept;
		Result<void> init(BlendInfo& info);
		
		D3D11BlendState& operator=(D3D11BlendState&&) noexcept;
		~D3D11BlendState();

		ID3D11BlendState* getHandle();
		
		private:
		ID3D11BlendState* blendState = nullptr;
	};
	
	using RHIBlendState = D3D11BlendState;
}

#endif