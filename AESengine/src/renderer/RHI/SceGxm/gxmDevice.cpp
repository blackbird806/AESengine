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

Result<void> aes::initializeGraphicsAPI()
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
	if (sceGxmInitialize(&initializeParams) != SCE_OK)
	{
		AES_LOG_ERROR("sceGxmInitialize failed !");
		return { AESError::Undefined };
	}
	return {};
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
	return malloc(size); // TODO use allocator here
}

static void patcherHostFree(void* userData, void* mem)
{
	AES_UNUSED(userData);
	free(mem);
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
	sceGxmEndScene(context, NULL, NULL);
}

void GxmDevice::setCullMode(CullMode mode)
{
	sceGxmSetCullMode(context, rhiCullModeToApi(mode));
}

void GxmDevice::setDrawPrimitiveMode(DrawPrimitiveType mode)
{
	AES_PROFILE_FUNCTION();
	currentState.primitiveType = mode;
	SceGxmPolygonMode polygonMode;
	switch(mode)
	{
		case DrawPrimitiveType::Lines:
		case DrawPrimitiveType::LineStrip:
			polygonMode = SCE_GXM_POLYGON_MODE_LINE;
			break;
		case DrawPrimitiveType::Triangles:
		case DrawPrimitiveType::TriangleStrip:
			polygonMode = SCE_GXM_POLYGON_MODE_TRIANGLE_FILL;
			break;
		case DrawPrimitiveType::Points:
			polygonMode = SCE_GXM_POLYGON_MODE_POINT;
			break;
	}
	// @Review I'm still not sure of what this function is doing since sceGxmDraw has a PrimitiveType parameter
	sceGxmSetFrontPolygonMode(context, polygonMode);
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