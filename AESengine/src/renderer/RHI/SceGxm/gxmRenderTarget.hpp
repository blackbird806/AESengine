#ifndef AES_GXMRENDERTARGET_HPP
#define AES_GXMRENDERTARGET_HPP

#include <psp2/gxm.h>
#include "gxmCompatibilty.h"
#include "gxmTexture.hpp"

namespace aes 
{
	class GxmRenderTarget
	{
		public:

		GxmRenderTarget(GxmRenderTarget&&) noexcept;
		~GxmRenderTarget();
		GxmRenderTarget& operator=(GxmRenderTarget&&) noexcept;

		Result<void> init(RenderTargetDescription const&);
		void destroy();

		private:

		GxmTexture texture;

		uint32_t flags;
		SceGxmRenderTarget* gxmRenderTarget;
		SceGxmColorSurface colorSurface;
		SceGxmDepthStencilSurface depthStencilSurface;
	};

	using RHIRenderTarget = GxmRenderTarget;
}

#endif