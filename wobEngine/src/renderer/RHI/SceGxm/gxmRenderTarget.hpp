#ifndef AES_GXMRENDERTARGET_HPP
#define AES_GXMRENDERTARGET_HPP

#include "core/aes.hpp"
#include "core/error.hpp"
#include "renderer/RHI/RHIElements.hpp"
#include <psp2/gxm.h>
#include "gxmCompatibilty.h"

namespace aes
{
	class GxmRenderTarget
	{
		friend class GxmDevice;
		public:

		GxmRenderTarget() = default;
		GxmRenderTarget(GxmRenderTarget&&) noexcept;
		~GxmRenderTarget();
		GxmRenderTarget& operator=(GxmRenderTarget&&) noexcept;

		Result<void> init(RenderTargetDescription const&);
		void destroy();

		private:

		void* colorSurfaceData;
		SceUID colorSurfaceID;

		void* depthSurfaceData;
		SceUID depthSurfaceID;

		uint32_t flags;
		SceGxmRenderTarget* gxmRenderTarget;
		SceGxmColorSurface colorSurface;
		SceGxmDepthStencilSurface depthStencilSurface;
		SceGxmSyncObject* syncObject;
	};

	using RHIRenderTarget = GxmRenderTarget;
}

#endif