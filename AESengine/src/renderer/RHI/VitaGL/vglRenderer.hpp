#ifndef VGLRENDERER_HPP
#define VGLRENDERER_HPP

#include "core/window.hpp"
#include "renderer/camera.hpp"
#include "renderer/RHI/RHIElements.hpp"
#include "renderer/RHI/RHIBuffer.hpp"

namespace aes
{
	class VglRenderer
	{
		public:

			void init(Window& windowHandle);
			void destroy();

			void startFrame(Camera const& cam);
			void endFrame();
			
			void bindBuffer(RHIBuffer& buffer, uint slot);
			void bindVertexBuffer(RHIBuffer& buffer, uint stride, uint offset = 0);
			void bindIndexBuffer(RHIBuffer& buffer, TypeFormat typeFormat, uint offset = 0);

			void setDrawPrimitiveMode(DrawPrimitiveMode mode);
			void drawIndexed(uint indexCount);

		private:

	};
	
	using RHIRenderContext = VglRenderer;
}

#endif
