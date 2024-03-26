#ifndef AES_D3D11SWAPCHAIN_HPP
#define AES_D3D11SWAPCHAIN_HPP

#include "D3D11RenderTarget.hpp"

struct IDXGISwapChain;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;

namespace aes
{
	class D3D11Swapchain
	{
		friend class D3D11Device;
	public:

		D3D11Swapchain() = default;
		D3D11Swapchain(D3D11Swapchain&&) noexcept;
		D3D11Swapchain& operator=(D3D11Swapchain&&) noexcept;

		void destroy() noexcept;
		~D3D11Swapchain();

	private:
		IDXGISwapChain* swapchain = nullptr;
		ID3D11Texture2D* rt = nullptr;
		ID3D11RenderTargetView* rtview = nullptr;
		ID3D11Texture2D* depthStencilBuffer = nullptr;
		ID3D11DepthStencilView* depthStencilView = nullptr;
	};

	using RHISwapchain = D3D11Swapchain;
}

#endif