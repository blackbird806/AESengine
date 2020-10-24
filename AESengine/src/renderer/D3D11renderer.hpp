#ifndef D3D11RENDERER_HPP
#define D3D11RENDERER_HPP

#include "core/aes.hpp"
#include "core/window.hpp"

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

/// DEBUG
#include "camera.hpp"
#include "D3D11Model.hpp"
#include "D3D11shader.hpp"
/// ///

namespace aes {
	
	class D3D11Renderer
	{

	public:

		void init(Window& windowHandle);
		void destroy();

		void startFrame();
		void endFrame();

	/// debug
		D3D11Model model;
		D3D11Shader shader;
		Camera cam;
	/// 

	private:
		void createDevice();
		void createSwapchain();
		void createRenderTarget();
		void createDepthStencil();
		void setupRasterizerState();
		void setupViewport();

		Window* renderWindow;
		bool destroyed = false;
		bool vsyncEnabled = false;
		int videoCardMemory;
		char videoCardDescription[128];
		
		IDXGIFactory* factory;
		ID3D11Device* device;
		IDXGISwapChain* swapChain;
		ID3D11DeviceContext* deviceContext;
		ID3D11Texture2D* depthStencilBuffer;
		ID3D11DepthStencilState* depthStencilState;
		ID3D11DepthStencilView* depthStencilView;
		ID3D11RasterizerState* rasterState;
		ID3D11RenderTargetView* renderTargetView;
	};

}

#endif