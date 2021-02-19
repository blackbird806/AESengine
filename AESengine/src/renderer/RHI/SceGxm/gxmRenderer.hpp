#ifndef GXMRENDERER_HPP
#define GXMRENDERER_HPP

#include "core/window.hpp"
#include "renderer/camera.hpp"
#include "renderer/RHI/RHIElements.hpp"
#include "renderer/RHI/RHIBuffer.hpp"

namespace aes
{
	class GxmRenderer
	{
		public:

			static GxmRenderer& instance();
		
			void init(Window& windowHandle);
			void destroy();

			void startFrame(Camera const& cam);
			void endFrame();
			
			void bindBuffer(RHIBuffer& buffer, uint slot);
			void bindVertexBuffer(RHIBuffer& buffer, uint stride, uint offset = 0);
			void bindIndexBuffer(RHIBuffer& buffer, TypeFormat typeFormat, uint offset = 0);

			void setDrawPrimitiveMode(DrawPrimitiveType mode);
			void drawIndexed(uint indexCount);

		private:
			static GxmRenderer* pinstance;
	};
	
	using RHIRenderContext = GxmRenderer;
}

#endif
