#include "vglRenderer.hpp"

using namespace aes;

void VglRenderer::init(Window& windowHandle)
{
	AES_UNUSED(windowHandle);
	
	// Initializing graphics device
	vglInit(0x800000);
}

void VglRenderer::destroy()
{
	
}

void VglRenderer::startFrame(Camera const& cam)
{
	
}

void VglRenderer::endFrame()
{
	
}
