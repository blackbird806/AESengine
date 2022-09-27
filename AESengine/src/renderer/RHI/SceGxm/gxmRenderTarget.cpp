#include "gxmRenderTarget.hpp"
#include "gxmElements.hpp"

using namespace aes;

Result<void> GxmRenderTarget::init(RenderTargetDescription const& info)
{
	auto result = texture.init(info.textureDesc);

	if (!result)
		return result;

	SceGxmRenderTargetParams renderTargetParams {};
	renderTargetParams.flags = 0;
	renderTargetParams.width = info.textureDesc.width;
	renderTargetParams.height = info.textureDesc.height;
	renderTargetParams.scenesPerFrame = 1;
	renderTargetParams.multisampleMode = rhiMultisampleModeToApi(info.multisampleMode);
	renderTargetParams.multisampleLocations = 0;
	renderTargetParams.driverMemBlock = SCE_UID_INVALID_UID;

	if (sceGxmCreateRenderTarget(&renderTargetParams, &gxmRenderTarget) != SCE_OK)
	{
		AES_LOG_ERROR("sceGxmCreateRenderTarget failed");
		return { AESError::RenderTargetCreationFailed };
	}

	// this is 1024 in examples, but I don't really know what influence this parameter
	auto constexpr vita_display_stride_in_pixels = 1024;
	if (sceGxmColorSurfaceInit(&colorSurface, 
			// I'm still unsure if it's a great idea to confound color format and texture format
			// it seems that vitaGL does this as well (https://github.com/Rinnegatamante/vitaGL/blob/master/source/framebuffers.c#L36)
			rhiFormatToApiColorFormat(info.textureDesc.format), 
			SCE_GXM_COLOR_SURFACE_LINEAR,
			(info.multisampleMode == MultisampleMode::None) ? SCE_GXM_COLOR_SURFACE_SCALE_NONE : SCE_GXM_COLOR_SURFACE_SCALE_MSAA_DOWNSCALE,
			SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT, // @Review when should we use 64bits ?
			info.textureDesc.width,
			info.textureDesc.height,
			vita_display_stride_in_pixels,
			sceGxmTextureGetData(texture.getHandle())) != SCE_OK)
	{
		AES_LOG_ERROR("sceGxmColorSurfaceInit failed");
		return { AESError::RenderTargetCreationFailed };
	}

	return {};
}