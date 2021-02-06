#ifndef VGLRENDERER_HPP
#define VGLRENDERER_HPP

#include "core/window.hpp"
#include "renderer/camera.hpp"

namespace aes
{
	class VglRenderer
	{
		public:

			void init(Window& windowHandle);
			void destroy();

			void startFrame(Camera const& cam);
			void endFrame();

		private:

	};
	
	using RHIRenderContext = VglRenderer;
}

#endif