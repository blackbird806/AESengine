#ifndef AES_D3D11RENDERTARGET_HPP
#define AES_D3D11RENDERTARGET_HPP

#include "core/aes.hpp"
#include "core/error.hpp"
#include "renderer/RHI/RHIElements.hpp"

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;

namespace aes
{
	class D3D11RenderTarget
	{
		friend class D3D11Device;
	public:

		D3D11RenderTarget() = default;
		D3D11RenderTarget(D3D11RenderTarget&&) noexcept;
		~D3D11RenderTarget();
		D3D11RenderTarget& operator=(D3D11RenderTarget&&) noexcept;

		void destroy();

	private:

		ID3D11Texture2D* renderTargetTexture = nullptr;
		ID3D11RenderTargetView* renderTargetView = nullptr;
	};

	using RHIRenderTarget = D3D11RenderTarget;
}

#endif