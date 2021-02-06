#include "vglRenderer.hpp"

using namespace aes;

void VglRenderer::init(Window& windowHandle)
{
	AES_PROFILE_FUNCTION();
	AES_UNUSED(windowHandle);
	
	// Initializing graphics device
	// @Review
	vglInit(0x800000);
}

void VglRenderer::destroy()
{
	AES_PROFILE_FUNCTION();
	vglEnd();
}

void VglRenderer::startFrame(Camera const& cam)
{
	AES_PROFILE_FUNCTION();
	vglStartRendering();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void VglRenderer::endFrame()
{
	AES_PROFILE_FUNCTION();
	vglStopRendering();
}

// TODO

void VglRenderer::bindBuffer(RHIBuffer& buffer, uint slot)
{
	AES_PROFILE_FUNCTION();
}

void VglRenderer::bindVertexBuffer(RHIBuffer& buffer, uint stride, uint offset)
{
	AES_PROFILE_FUNCTION();
}

void VglRenderer::bindIndexBuffer(RHIBuffer& buffer, TypeFormat format, uint offset)
{
}

void setDrawPrimitiveMode(DrawPrimitiveMode mode)
{
}

void drawIndexed(uint indexCount)
{

}




