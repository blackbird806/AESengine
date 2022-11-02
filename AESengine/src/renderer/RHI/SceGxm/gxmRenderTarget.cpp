#include "gxmRenderTarget.hpp"
#include "gxmElements.hpp"
#include "gxmCompatibilty.h"
#include "gxmMemory.hpp"

using namespace aes;

Result<void> GxmRenderTarget::init(RenderTargetDescription const& info)
{
	AES_PROFILE_FUNCTION();

	SceGxmRenderTargetParams renderTargetParams {};
	renderTargetParams.flags = 0;
	renderTargetParams.width = info.width;
	renderTargetParams.height = info.height;
	renderTargetParams.scenesPerFrame = 1;
	renderTargetParams.multisampleMode = rhiMultisampleModeToApi(info.multisampleMode);
	renderTargetParams.multisampleLocations = 0;
	renderTargetParams.driverMemBlock = SCE_UID_INVALID_UID;

	if (sceGxmCreateRenderTarget(&renderTargetParams, &gxmRenderTarget) != SCE_OK)
	{
		AES_LOG_ERROR("sceGxmCreateRenderTarget failed");
		return { AESError::RenderTargetCreationFailed };
	}

	size_t const colorSurfaceSize = info.width * info.height * getFormatSize(info.format);
	colorSurfaceData = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
		colorSurfaceSize,
		SCE_GXM_COLOR_SURFACE_ALIGNMENT,
		SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
		&colorSurfaceID, "rt color");
	if (!colorSurfaceData)
	{
		AES_LOG_ERROR("failed to allocate colorSurface data, graphicsAlloc failed");
		return { AESError::RenderTargetCreationFailed };
	}

	// this is 1024 in examples, but I don't really know what influence this parameter
	auto constexpr vita_display_stride_in_pixels = 1024;
	if (sceGxmColorSurfaceInit(&colorSurface, 
			// I'm still unsure if it's a great idea to confound color format and texture format
			// it seems that vitaGL does this as well (https://github.com/Rinnegatamante/vitaGL/blob/master/source/framebuffers.c#L36)
			rhiFormatToApiColorFormat(info.format), 
			SCE_GXM_COLOR_SURFACE_LINEAR,
			(info.multisampleMode == MultisampleMode::None) ? SCE_GXM_COLOR_SURFACE_SCALE_NONE : SCE_GXM_COLOR_SURFACE_SCALE_MSAA_DOWNSCALE,
			SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT, // @Review when should we use 64bits ?
			info.width,
			info.height,
			vita_display_stride_in_pixels,
			colorSurfaceData) != SCE_OK)
	{
		AES_LOG_ERROR("sceGxmColorSurfaceInit failed");
		return { AESError::RenderTargetCreationFailed };
	}

	uint32_t const alignedWidth = aes::align(info.width, SCE_GXM_TILE_SIZEX);
	uint32_t const alignedHeight = aes::align(info.height, SCE_GXM_TILE_SIZEY);
	uint32_t sampleCount = alignedWidth * alignedHeight;
	uint32_t depthStrideInSamples = alignedWidth;
	if (info.multisampleMode == MultisampleMode::X4) {
		// samples increase in X and Y
		sampleCount *= 4;
		depthStrideInSamples *= 2;
	}
	else if (info.multisampleMode == MultisampleMode::X2) {
		// samples increase in Y only
		sampleCount *= 2;
	}

	// depth buffer allocation
	depthSurfaceData = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		4 * sampleCount,
		SCE_GXM_DEPTHSTENCIL_SURFACE_ALIGNMENT,
		SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
		&depthSurfaceID, "rt depth");

	if (sceGxmDepthStencilSurfaceInit(
			&depthStencilSurface,
			SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24, // @Review
			SCE_GXM_DEPTH_STENCIL_SURFACE_TILED, 
			depthStrideInSamples,
			depthSurfaceData,
			nullptr) != SCE_OK)
	{
		AES_LOG_ERROR("sceGxmDepthStencilSurfaceInit failed");
		return { AESError::RenderTargetCreationFailed };
	}
	
	if (sceGxmSyncObjectCreate(&syncObject) != SCE_OK)
	{
		AES_LOG_ERROR("sceGxmSyncObjectCreate failed");
		return { AESError::RenderTargetCreationFailed };
	}

	return {};
}

// @Review this is not a robust way to ensure that ressources are dealocated correctly
void GxmRenderTarget::destroy()
{
	AES_PROFILE_FUNCTION();
	if (colorSurfaceData != nullptr)
	{
		sceGxmDestroyRenderTarget(gxmRenderTarget);
		graphicsFree(colorSurfaceID);
		graphicsFree(depthSurfaceID);
		sceGxmSyncObjectDestroy(syncObject);
		colorSurfaceData = nullptr;
		depthSurfaceData = nullptr;
	}
}

GxmRenderTarget::~GxmRenderTarget()
{
	destroy();
}
