#ifndef AES_D3D11DEVICE_HPP
#define AES_D3D11DEVICE_HPP

#include "core/error.hpp"
#include "renderer/RHI/RHIElements.hpp"
#include "renderer/RHI/RHIBuffer.hpp"
#include "renderer/RHI/RHITexture.hpp"
#include "renderer/RHI/RHIRenderTarget.hpp"
#include "renderer/RHI/RHIShader.hpp"
#include "renderer/RHI/RHISampler.hpp"
#include <d3d11.h>

struct IDXGIFactory;

namespace aes
{

	void initializeGraphicsAPI();
	void terminateGraphicsAPI();

	class D3D11Device
	{
	public:
		D3D11Device() = default;
		D3D11Device(D3D11Device const&) = delete;
		D3D11Device(D3D11Device&&) noexcept;
		~D3D11Device();

		D3D11Device& operator=(D3D11Device&&) noexcept;

		Result<void> init();
		void destroy();

		// resource creation
		Result<RHIRenderTarget> createRenderTarget(RenderTargetDescription const& desc);
		Result<RHIBuffer> createBuffer(BufferDescription const& desc);
		Result<void> copyBuffer(RHIBuffer const& from, RHIBuffer& to);
		Result<RHITexture> createTexture(TextureDescription const& desc);
		Result<RHIVertexShader> createVertexShader(VertexShaderDescription const& desc);
		Result<RHIFragmentShader> createFragmentShader(FragmentShaderDescription const& desc);
		Result<RHISampler> createSampler(SamplerDescription const& desc);

		// d3d11 specifics
		Result<D3D11BlendState> createBlendState(BlendInfo const& desc);

		// not sure about this name
		void swapBuffers(RHIRenderTarget const& oldBuffer, RHIRenderTarget const& newBuffer);

		// draw calls

		void drawIndexed(uint indexCount, uint indexOffset = 0);

		void beginRenderPass(RHIRenderTarget& rt);
		void endRenderPass();

		// state modification

		void setCullMode(CullMode mode);
		void setDrawPrimitiveMode(DrawPrimitiveType mode);

		void setRasterizerState();

		void setFragmentShader(RHIFragmentShader& fs);
		void setVertexShader(RHIVertexShader& vs);

		Result<void> setVertexBuffer(RHIBuffer& buffer, uint stride, uint offset = 0);
		Result<void> setIndexBuffer(RHIBuffer& buffer, IndexTypeFormat typeFormat, uint offset = 0);

		// resources binding

		Result<void> bindFragmentUniformBuffer(RHIBuffer& buffer, uint slot);
		Result<void> bindVertexUniformBuffer(RHIBuffer& buffer, uint slot);
		Result<void> bindFragmentTexture(RHITexture& buffer, uint slot);
		Result<void> bindVertexTexture(RHITexture& buffer, uint slot);

	private:
		void setBlendState(D3D11BlendState& blendState);

		IDXGIFactory* factory = nullptr;
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* deviceContext = nullptr;
		D3D11_RASTERIZER_DESC rasterStateDesc;
		ID3D11RasterizerState* rasterState = nullptr;

#ifdef AES_DEBUG
		ID3D11Debug* debugInterface = nullptr;
#endif
	};

	using RHIDevice = D3D11Device;
}

#endif