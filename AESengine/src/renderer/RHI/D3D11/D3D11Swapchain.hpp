#ifndef AES_D3D11SWAPCHAIN_HPP
#define AES_D3D11SWAPCHAIN_HPP

#include "D3D11RenderTarget.hpp"
#include "core/array.hpp"

class IDXGISwapChain;
class ID3D11Texture2D;
class ID3D11RenderTargetView;
class ID3D11DepthStencilView;

namespace aes
{
	class D3D11Swapchain
	{
		friend class D3D11Device;
	public:

		D3D11Swapchain() = default;
		D3D11Swapchain(D3D11Swapchain&&) noexcept;
		D3D11Swapchain& operator=(D3D11Swapchain&&) noexcept;

		uint getTexturesCount() const noexcept;

		void destroy() noexcept;
		~D3D11Swapchain();

	private:
		IDXGISwapChain* swapchain = nullptr;
		Array<ID3D11Texture2D*> rts;
		Array<ID3D11RenderTargetView*> rtviews;
		ID3D11Texture2D* depthStencilBuffer = nullptr;
		ID3D11DepthStencilView* depthStencilView = nullptr;
	};

	using RHISwapchain = D3D11Swapchain;
}

#endif