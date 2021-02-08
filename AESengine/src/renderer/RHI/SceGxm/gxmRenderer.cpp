#include "gxmRenderer.hpp"

using namespace aes;

GxmRenderer* GxmRenderer::pinstance = nullptr;

GxmRenderer& GxmRenderer::instance()
{
	AES_ASSERT(pinstance != nullptr);
	return *pinstance;
}

void GxmRenderer::init(Window& windowHandle)
{
	AES_PROFILE_FUNCTION();
	AES_UNUSED(windowHandle);

	AES_ASSERT(pinstance == nullptr);
	pinstance = this;
	

}

void GxmRenderer::destroy()
{
	AES_PROFILE_FUNCTION();

}

void GxmRenderer::startFrame(Camera const& cam)
{
	AES_PROFILE_FUNCTION();

}

void GxmRenderer::endFrame()
{
	AES_PROFILE_FUNCTION();

}

void GxmRenderer::bindBuffer(RHIBuffer& buffer, uint slot)
{
	AES_PROFILE_FUNCTION();
}

void GxmRenderer::bindVertexBuffer(RHIBuffer& buffer, uint stride, uint offset)
{
	AES_PROFILE_FUNCTION();
}

void GxmRenderer::bindIndexBuffer(RHIBuffer& buffer, TypeFormat format, uint offset)
{
}

void GxmRenderer::setDrawPrimitiveMode(DrawPrimitiveMode mode)
{
}

void GxmRenderer::drawIndexed(uint indexCount)
{

}





