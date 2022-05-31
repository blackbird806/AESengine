#ifndef AES_D3D11RENDERER_HPP
#define AES_D3D11RENDERER_HPP

#include "core/aes.hpp"
#include "core/window.hpp"
#include "renderer/RHI/RHIElements.hpp"

#include <dxgi.h>
#include <d3d11.h>
#include "renderer/RHI/RHIBuffer.hpp"
#include "renderer/RHI/RHIShader.hpp"
#include "renderer/RHI/RHISampler.hpp"
#include "renderer/RHI/RHITexture.hpp"
#include "renderer/RHI/RHIElements.hpp"
#include "D3D11BlendState.hpp"

namespace aes {

	class D3D11Renderer
	{
		//@Review https://www.gamedev.net/forums/topic/586063-dx11-and-screen-coordinates/
	public:

		static D3D11Renderer& instance();

		void init(Window& windowHandle);
		void destroy();

		void bindVSUniformBuffer(RHIBuffer& buffer, uint slot);
		void bindFSUniformBuffer(RHIBuffer& buffer, uint slot);


		void setVertexSampler(RHISampler& sampler, uint index);
		void setFragmentSampler(RHISampler& sampler, uint index);
		void bindVertexTexture(RHITexture& tex, uint index);
		void bindFragmentTexture(RHITexture& tex, uint index);

		void bindVertexBuffer(RHIBuffer& buffer, uint stride, uint offset = 0);
		void bindIndexBuffer(RHIBuffer& buffer, IndexTypeFormat typeFormat, uint offset = 0);

		void setFragmentShader(RHIFragmentShader& fs);
		void setVertexShader(RHIVertexShader& vs);

		void setDrawPrimitiveMode(DrawPrimitiveType mode);
		void setBlendState(D3D11BlendState& blendState);
		void drawIndexed(uint indexCount, uint indexOffset = 0);
		
		void startFrame();
		void endFrame();
		
		ID3D11Device* getDevice();
		ID3D11DeviceContext* getDeviceContext();

	private:
		
		static D3D11Renderer* pinstance;

		void createDevice();
		void createSwapchain();
		void createRenderTarget();
		void createDepthStencil();
		void setupRasterizerState();
		void setupViewport();

		void resizeRender();

		void destroySwapchain();
		void destroyDepthStencil();
		void destroyRenderTarget();

		Window* renderWindow;
		bool destroyed = false;
		bool vsyncEnabled = false;
		int videoCardMemory;
		char videoCardDescription[128];
		
		IDXGIFactory* factory;
		ID3D11Device* device;
		ID3D11Debug* debugInterface;
		IDXGISwapChain* swapChain;
		ID3D11DeviceContext* deviceContext;
		ID3D11Texture2D* depthStencilBuffer;
		ID3D11DepthStencilState* depthStencilState;
		ID3D11DepthStencilView* depthStencilView;
		ID3D11RasterizerState* rasterState;
		ID3D11RenderTargetView* renderTargetView;
	};

	using RHIRenderContext = D3D11Renderer;
}

#endif