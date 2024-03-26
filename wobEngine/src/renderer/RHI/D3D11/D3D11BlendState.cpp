#include "D3D11BlendState.hpp"
#include "D3D11Elements.hpp"

using namespace aes;

D3D11BlendState::D3D11BlendState(D3D11BlendState&& rhs) noexcept : blendState(rhs.blendState)
{
	rhs.blendState = nullptr;
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
	{
		blendState->Release();
		blendState = nullptr;
	}
}

ID3D11BlendState* D3D11BlendState::getHandle()
{
	return blendState;
}
