#include "gxmRenderTarget.hpp"

using namespace aes;

Result<void> GxmRenderTarget::init(RenderTargetDescription const& info)
{
	if (!texture.init(info.textureDesc))
		return { AESError::RenderTargetCreationFailed };

	SceGxmRenderTargetParams renderTargetParams {};
	renderTargetParams.flags = 0;
	renderTargetParams.width = info.textureDesc.width;
	renderTargetParams.height = info.textureDesc.height;
	renderTargetParams.scenesPerFrame = 1;
	renderTargetParams.multisampleMode = info.multiSampleMode;
	renderTargetParams.multisampleLocations = 0;
	renderTargetParams.driverMemBlock = SCE_UID_INVALID_UID;

	if (sceGxmCreateRenderTarget(&renderTargetParams, &renderTarget) != SCE_OK)
	{
		AES_LOG_ERROR("sceGxmCreateRenderTarget failed");
		return { AESError::RenderTargetCreationFailed };
	}

	if (sceGxmColorSurfaceInit(&colorSurface,)
		!= SCE_OK)
	{

	}

	return {};
}