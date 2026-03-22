#ifndef WOB_RHIDEVICE_HPP
#define WOB_RHIDEVICE_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef WOB_GRAPHIC_API_D3D11
	#include "renderer/RHI/D3D11/D3D11Device.hpp"
#elif defined(WOB_GRAPHIC_API_GXM)
	#include "renderer/RHI/SceGxm/gxmDevice.hpp"
#endif

namespace wob 
{
	class RHIDevice : public RHIDeviceBase
	{
	public:
		Result<void> setBufferData(RHIBuffer const& buffer, void* data, size_t size);
		Result<void> reallocBuffer(RHIBuffer& buffer, BufferDescription const& reallocDesc);
		Result<void> ensureBufferCapacity(RHIBuffer& buffer, BufferDescription const& reallocDesc);

		template<typename T>
		auto createRessource(T const& desc)
		{
			static_assert(false);
		}

		template<>
		auto createRessource(SwapchainDescription const& desc)
		{
			return createSwapchain(desc);
		}

		template<>
		auto createRessource(RenderTargetDescription const& desc)
		{
			return createRenderTarget(desc);
		}

		template<>
		auto createRessource(BufferDescription const& desc)
		{
			return createBuffer(desc);
		}

		template<>
		auto createRessource(TextureDescription const& desc)
		{
			return createTexture(desc);
		}

		template<>
		auto createRessource(VertexShaderDescription const& desc)
		{
			return createVertexShader(desc);
		}

		template<>
		auto createRessource(FragmentShaderDescription const& desc)
		{
			return createFragmentShader(desc);
		}

		template<>
		auto createRessource(SamplerDescription const& desc)
		{
			return createSampler(desc);
		}

		template<typename T>
		void destroyRessource(T& res)
		{
			static_assert(false);
		}

		template<>
		void destroyRessource(RHISwapchain& res)
		{
			destroySwapchain(res);
		}

		template<>
		void destroyRessource(RHIRenderTarget& res)
		{
			destroyRenderTarget(res);
		}

		template<>
		void destroyRessource(RHIBuffer& res)
		{
			destroyBuffer(res);
		}

		template<>
		void destroyRessource(RHITexture& res)
		{
			destroyTexture(res);
		}

		template<>
		void destroyRessource(RHIVertexShader& res)
		{
			destroyVertexShader(res);
		}

		template<>
		void destroyRessource(RHIFragmentShader& res)
		{
			destroyFragmentShader(res);
		}

		template<>
		void destroyRessource(RHISampler& res)
		{
			destroySampler(res);
		}
	};
}

#endif