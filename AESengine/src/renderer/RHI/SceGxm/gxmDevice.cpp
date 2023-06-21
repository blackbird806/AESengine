#include "gxmDevice.hpp"
#include "core/utility.hpp"
#include "core/debug.hpp"
#include "core/allocator.hpp"
#include "gxmElements.hpp"
#include "gxmDebug.hpp"
#include "gxmMemory.hpp"

using namespace aes;

struct DisplayData
{
	void* address;
};

// displayCallback may be called on another thread
static void displayCallback(void const* callbackData)
{
	// @TODO clean this
	auto constexpr vita_display_width = 960;
	auto constexpr vita_display_height = 544;
	auto constexpr vita_display_stride_in_pixels = 1024;
	auto constexpr vita_display_pixel_format = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;
	auto constexpr vita_display_color_format = SCE_GXM_COLOR_FORMAT_U8U8U8U8_RGBA;

	DisplayData const* displayData = (DisplayData const*)callbackData;
	SceDisplayFrameBuf frameBuf {
		.size = sizeof(SceDisplayFrameBuf),
		.base = displayData->address,
		.pitch = vita_display_stride_in_pixels,
		.pixelformat = vita_display_pixel_format,
		.width = vita_display_width,
		.height = vita_display_height
	};

	// TODO Thread safe assert / Logs
	sceDisplaySetFrameBuf(&frameBuf, (SceDisplaySetBufSync) SCE_DISPLAY_UPDATETIMING_NEXTVSYNC);
	sceDisplayWaitVblankStart();
}


static void* fragmentUsseAlloc(uint32_t size, SceUID* uid, uint32_t* usseOffset)
{
	// align to memblock alignment for LPDDR
	size = aes::align(size, 4096);

	// allocate some memory
	*uid = sceKernelAllocMemBlock("fragmentUsse", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE, size, nullptr);
	AES_ASSERT(*uid >= SCE_OK);

	// grab the base address
	void* mem = nullptr;
	uint32_t err = sceKernelGetMemBlockBase(*uid, &mem);
	AES_ASSERT(err == SCE_OK);

	// map as fragment USSE code for the GPU
	err = sceGxmMapFragmentUsseMemory(mem, size, usseOffset);
	AES_ASSERT(err == SCE_OK);

	// done
	return mem;
}

static void* vertexUsseAlloc(uint32_t size, SceUID* uid, uint32_t* usseOffset)
{
	// align to memblock alignment for LPDDR
	size = aes::align(size, 4096);

	// allocate some memory
	*uid = sceKernelAllocMemBlock("vertexUsse", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE, size, nullptr);
	AES_ASSERT(*uid >= SCE_OK);

	// grab the base address
	void* mem = nullptr;
	auto err = sceKernelGetMemBlockBase(*uid, &mem);
	AES_ASSERT(err == SCE_OK);

	// map as vertex USSE code for the GPU
	err = sceGxmMapVertexUsseMemory(mem, size, usseOffset);
	AES_ASSERT(err == SCE_OK);

	return mem;
}

static void vertexUsseFree(SceUID uid)
{
	// grab the base address
	void* mem = nullptr;
	uint32_t err = sceKernelGetMemBlockBase(uid, &mem);
	AES_ASSERT(err == SCE_OK);

	// unmap memory
	err = sceGxmUnmapVertexUsseMemory(mem);
	AES_ASSERT(err == SCE_OK);

	// free the memory block
	err = sceKernelFreeMemBlock(uid);
	AES_ASSERT(err == SCE_OK);
}

static void fragmentUsseFree(SceUID uid)
{
	// grab the base address
	void* mem = nullptr;
	uint32_t err = sceKernelGetMemBlockBase(uid, &mem);
	AES_ASSERT(err == SCE_OK);

	// unmap memory
	err = sceGxmUnmapFragmentUsseMemory(mem);
	AES_ASSERT(err == SCE_OK);

	// free the memory block
	err = sceKernelFreeMemBlock(uid);
	AES_ASSERT(err == SCE_OK);
}

static void* patcherHostAlloc(void* userData, uint32_t size)
{
	AES_UNUSED(userData);
	return globalAllocator.allocate(size);
}

static void patcherHostFree(void* userData, void* mem)
{
	AES_UNUSED(userData);
	globalAllocator.deallocate(mem);
}

SceUID patcherBufferUid;
SceUID patcherVertexUsseUid;
SceUID patcherFragmentUsseUid;

void aes::initializeGraphicsAPI()
{
	// @TODO clean this
	auto constexpr vita_display_max_pending_swaps = 2;

	// Set up parameters
	SceGxmInitializeParams initializeParams;
	memset(&initializeParams, 0, sizeof(SceGxmInitializeParams));
	initializeParams.flags = 0;
	initializeParams.displayQueueMaxPendingCount = vita_display_max_pending_swaps;
	initializeParams.displayQueueCallback = displayCallback;
	initializeParams.displayQueueCallbackDataSize = sizeof(DisplayData);
	initializeParams.parameterBufferSize = SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;

	// Start libgxm
	auto err = sceGxmInitialize(&initializeParams);
	AES_ASSERT(err == SCE_OK);

	// @Review buffer sizes
	uint32_t const patcherBufferSize 		= 64 * 1024;
	uint32_t const patcherVertexUsseSize 	= 64 * 1024;
	uint32_t const patcherFragmentUsseSize 	= 64 * 1024;

	// allocate memory for buffers and USSE code
	void* patcherBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		patcherBufferSize,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
		&patcherBufferUid, "patcher buffer");

	uint32_t patcherVertexUsseOffset;
	void* patcherVertexUsse = vertexUsseAlloc(
		patcherVertexUsseSize,
		&patcherVertexUsseUid,
		&patcherVertexUsseOffset);

	uint32_t patcherFragmentUsseOffset;
	void* patcherFragmentUsse = fragmentUsseAlloc(
		patcherFragmentUsseSize,
		&patcherFragmentUsseUid,
		&patcherFragmentUsseOffset);

	// create a shader patcher
	SceGxmShaderPatcherParams patcherParams {};
	patcherParams.userData = nullptr;
	patcherParams.hostAllocCallback = &patcherHostAlloc;
	patcherParams.hostFreeCallback = &patcherHostFree;
	patcherParams.bufferAllocCallback = nullptr;
	patcherParams.bufferFreeCallback = nullptr;
	patcherParams.bufferMem = patcherBuffer;
	patcherParams.bufferMemSize = patcherBufferSize;
	patcherParams.vertexUsseAllocCallback = nullptr;
	patcherParams.vertexUsseFreeCallback = nullptr;
	patcherParams.vertexUsseMem = patcherVertexUsse;
	patcherParams.vertexUsseMemSize = patcherVertexUsseSize;
	patcherParams.vertexUsseOffset = patcherVertexUsseOffset;
	patcherParams.fragmentUsseAllocCallback = nullptr;
	patcherParams.fragmentUsseFreeCallback = nullptr;
	patcherParams.fragmentUsseMem = patcherFragmentUsse;
	patcherParams.fragmentUsseMemSize = patcherFragmentUsseSize;
	patcherParams.fragmentUsseOffset = patcherFragmentUsseOffset;

	err = sceGxmShaderPatcherCreate(&patcherParams, &gxmShaderPatcher);
	AES_ASSERT(err == SCE_OK);
	
#define ENABLE_RAZOR_HUD
#define ENABLE_RAZOR_GPU_CAPTURE
#ifdef ENABLE_RAZOR_HUD
	// Initialize the Razor HUD system.
	// This should be done before the call to sceGxmInitialize().
	// err = sceSysmoduleLoadModule( SCE_SYSMODULE_RAZOR_HUD );
	AES_ASSERT(err == SCE_OK);

#endif

#ifdef ENABLE_RAZOR_GPU_CAPTURE
	// Initialize the Razor capture system.
	// This should be done before the call to sceGxmInitialize().
	// err = sceSysmoduleLoadModule( SCE_SYSMODULE_RAZOR_CAPTURE );
	AES_ASSERT(err == SCE_OK);
	// Trigger a capture after 100 frames.
	// sceRazorGpuCaptureSetTrigger( 100, "app0:basic.sgx" );
#endif
}

void aes::terminateGraphicsAPI()
{
	sceGxmShaderPatcherDestroy(gxmShaderPatcher);
	fragmentUsseFree(patcherFragmentUsseUid);
	vertexUsseFree(patcherVertexUsseUid);
	graphicsFree(patcherBufferUid);

	sceGxmTerminate();
	AES_LOG("GXM terminated successfully");
}


GxmDevice::GxmDevice(GxmDevice&& rhs) noexcept
{
	*this = std::move(rhs);
}

GxmDevice& GxmDevice::operator=(GxmDevice&& rhs) noexcept
{
	context = rhs.context;
	vdmRingBufferUid = rhs.vdmRingBufferUid;
	vertexRingBufferUid = rhs.vertexRingBufferUid;
	fragmentRingBufferUid = rhs.fragmentRingBufferUid;
	fragmentUsseRingBufferUid = rhs.fragmentUsseRingBufferUid;
	hostMem = rhs.hostMem;
	currentState = rhs.currentState;
	rhs.context = nullptr;
	return *this;
}

GxmDevice::~GxmDevice()
{
	destroy();
}

Result<void> GxmDevice::init()
{
	AES_PROFILE_FUNCTION();

	// allocate ring buffers with default sizes
	void* vdmRingBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&vdmRingBufferUid, "vdmRingBuffer");
	
	void* vertexRingBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&vertexRingBufferUid, "vertexRingBuffer");
	
	void* fragmentRingBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&fragmentRingBufferUid, "fragmentRingBuffer");
	
	uint32_t fragmentUsseRingBufferOffset;
	void* fragmentUsseRingBuffer = fragmentUsseAlloc(
		SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE,
		&fragmentUsseRingBufferUid,
		&fragmentUsseRingBufferOffset);

	// create the gxm context
	hostMem = globalAllocator.allocate(SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE);
	SceGxmContextParams contextParams {};
	contextParams.hostMem = hostMem;
	contextParams.hostMemSize = SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE;
	contextParams.vdmRingBufferMem = vdmRingBuffer;
	contextParams.vdmRingBufferMemSize = SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE;
	contextParams.vertexRingBufferMem = vertexRingBuffer;
	contextParams.vertexRingBufferMemSize = SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE;
	contextParams.fragmentRingBufferMem = fragmentRingBuffer;
	contextParams.fragmentRingBufferMemSize = SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE;
	contextParams.fragmentUsseRingBufferMem = fragmentUsseRingBuffer;
	contextParams.fragmentUsseRingBufferMemSize = SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE;
	contextParams.fragmentUsseRingBufferOffset = fragmentUsseRingBufferOffset;

	auto err = sceGxmCreateContext(&contextParams, &context);
	if (err != SCE_OK)
	{
		AES_LOG_ERROR("GXM failed to create render context, code : {}", err);
		return { AESError::RHIDeviceCreationFailed };
	}
	return {};
}

void GxmDevice::swapBuffers(RHIRenderTarget const& oldBuffer, RHIRenderTarget const& newBuffer)
{
	AES_PROFILE_FUNCTION();

	// PA heartbeat to notify end of frame
	auto err = sceGxmPadHeartbeat(&newBuffer.colorSurface, newBuffer.syncObject);
	AES_GXM_CHECK(err);

	// queue the display swap for this frame
	DisplayData displayData;
	displayData.address = newBuffer.colorSurfaceData;
	err = sceGxmDisplayQueueAddEntry(oldBuffer.syncObject, newBuffer.syncObject, &displayData);
	AES_GXM_CHECK(err);
}

void GxmDevice::destroy()
{
	AES_PROFILE_FUNCTION();

	if (context == nullptr)
		return;

	// wait until rendering is done
	sceGxmFinish(context);

	sceGxmDestroyContext(context);
	fragmentUsseFree(fragmentUsseRingBufferUid);
	graphicsFree(fragmentRingBufferUid);
	graphicsFree(vertexRingBufferUid);
	graphicsFree(vdmRingBufferUid);
	globalAllocator.deallocate(hostMem);
	context = nullptr;
}

void GxmDevice::drawIndexed(uint indexCount, uint indexOffset)
{
	AES_PROFILE_FUNCTION();

	if (currentState.indexBufferInfo.typeFormat == IndexTypeFormat::Uint16)
	{
		auto err = sceGxmDraw(context, rhiPrimitiveTypeToApi(currentState.primitiveType),
				rhiIndexFormatToApi(currentState.indexBufferInfo.typeFormat),
				((uint16_t*)currentState.indexBufferInfo.buffer) + indexOffset,
				(uint32_t)indexCount);
		AES_ASSERT(err == SCE_OK);
	}
	else {
		auto err = sceGxmDraw(context, rhiPrimitiveTypeToApi(currentState.primitiveType),
					rhiIndexFormatToApi(currentState.indexBufferInfo.typeFormat),
					((uint32_t*)currentState.indexBufferInfo.buffer) + indexOffset,
					(uint32_t)indexCount);
		AES_ASSERT(err == SCE_OK);
	}
}

void GxmDevice::beginRenderPass(RHIRenderTarget& rt)
{
	AES_PROFILE_FUNCTION();
	auto err = sceGxmBeginScene(
		context,
		0,
		rt.gxmRenderTarget,
		NULL,
		NULL,
		rt.syncObject,
		&rt.colorSurface,
		&rt.depthStencilSurface);
}	

void GxmDevice::endRenderPass()
{
	AES_PROFILE_FUNCTION();
	sceGxmEndScene(context, NULL, NULL);
}

void GxmDevice::setCullMode(CullMode mode)
{
	AES_PROFILE_FUNCTION();
	sceGxmSetCullMode(context, rhiCullModeToApi(mode));
}

void GxmDevice::setDrawPrimitiveMode(DrawPrimitiveType mode)
{
	AES_PROFILE_FUNCTION();
	currentState.primitiveType = mode;
	// @Review I'm still not sure of what this function is doing since sceGxmDraw has a PrimitiveType parameter
	sceGxmSetFrontPolygonMode(context, rhiPolygonModeToApi(mode));
}

void GxmDevice::setFragmentShader(RHIFragmentShader& fs)
{
	AES_PROFILE_FUNCTION();
	sceGxmSetFragmentProgram(context, fs.getHandle());
}

void GxmDevice::setVertexShader(RHIVertexShader& vs)
{
	AES_PROFILE_FUNCTION();
	sceGxmSetVertexProgram(context, vs.getHandle());
}

Result<void> GxmDevice::setVertexBuffer(RHIBuffer& buffer, uint stride, uint offset)
{
	AES_PROFILE_FUNCTION();
	auto const err = sceGxmSetVertexStream(context, 0, buffer.getHandle());
	if (err != SCE_OK)
	{
		AES_LOG_ERROR("failed to set gxm vertex stream err : {}", err);
		// @Review error code
		return { AESError::Undefined };
	}
	return {};
}

Result<void> GxmDevice::setIndexBuffer(RHIBuffer& buffer, IndexTypeFormat typeFormat, uint offset)
{
	AES_PROFILE_FUNCTION();
	currentState.indexBufferInfo.typeFormat = typeFormat;
	currentState.indexBufferInfo.buffer = buffer.getHandle();
	return {};
}

Result<void> GxmDevice::bindFragmentUniformBuffer(RHIBuffer& buffer, uint slot)
{
	AES_PROFILE_FUNCTION();
	AES_ASSERT(slot < SCE_GXM_MAX_UNIFORM_BUFFERS);
	auto const err = sceGxmSetFragmentUniformBuffer(context, slot, buffer.getHandle());
	if (err != SCE_OK)
	{
		AES_LOG_ERROR("failed to set gxm fragment uniform buffer error : {}", err);
		// @Review error code
		return { AESError::Undefined };
	}
	return {};
}

Result<void> GxmDevice::bindVertexUniformBuffer(RHIBuffer& buffer, uint slot)
{
	AES_PROFILE_FUNCTION();
	AES_ASSERT(slot < SCE_GXM_MAX_UNIFORM_BUFFERS);
	auto const err = sceGxmSetVertexUniformBuffer(context, slot, buffer.getHandle());
	if (err != SCE_OK)
	{
		AES_LOG_ERROR("failed to set gxm vertex uniform buffer error : {}", err);
		// @Review error code
		return { AESError::Undefined };
	}
	return {};
}

Result<void> GxmDevice::bindFragmentTexture(RHITexture& tex, uint slot)
{
	AES_PROFILE_FUNCTION();
	auto const err = sceGxmSetFragmentTexture(context, slot, tex.getHandle());
	if (err != SCE_OK)
	{
		AES_LOG_ERROR("failed to set gxm fragment texture error : {}", err);
		// @Review error code
		return { AESError::Undefined };
	}
	return {};
}

Result<void> GxmDevice::bindVertexTexture(RHITexture& tex, uint slot)
{
	AES_PROFILE_FUNCTION();
	auto const err = sceGxmSetVertexTexture(context, slot, tex.getHandle());
	if (err != SCE_OK)
	{
		AES_LOG_ERROR("failed to set gxm vertex texture error : {}", err);
		// @Review error code
		return { AESError::Undefined };
	}
	return {};
}