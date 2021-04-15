#include "gxmRenderer.hpp"
#include "core/utility.hpp"
#include "gxmElements.hpp"

using namespace aes;

void* aes::graphicsAlloc(SceKernelMemBlockType type, uint32_t size, uint32_t alignement, uint32_t attribs, SceUID* uid, const char* name)
{
	AES_ASSERT(uid != nullptr);
	
	// Page align the size
	if (type == SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW)
	{
		// 	CDRAM memblock must be 256Kib aligned
		AES_ASSERT(alignement <= 256 * 1024);
		size = aes::align(size, 256 * 1024);
	} 
	else
	{
		// LPDDR memblocks must be 4KiB aligned
		AES_ASSERT(alignment <= 4 * 1024);
		size = aes::align(size, 4 * 1024);
	}
	
	uint32_t err = SCE_OK;
	*uid = sceKernelAllocMemBlock(name, type, size, nullptr);
	AES_ASSERT(*uid > SCE_OK);

	void* mem = nullptr;
	err = (SceGxmErrorCode) sceKernelGetMemBlockBase(*uid, &mem);
	AES_ASSERT(err == SCE_OK);

	// map for the GPU
	err = (SceGxmErrorCode) sceGxmMapMemory(mem, size, (SceGxmMemoryAttribFlags)attribs);
	AES_ASSERT(err == SCE_OK);
	
	return mem;
}

void* fragmentUsseAlloc(uint32_t size, SceUID* uid, uint32_t* usseOffset)
{
	// align to memblock alignment for LPDDR
	size = aes::align(size, 4096);

	// allocate some memory
	*uid = sceKernelAllocMemBlock("basic", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE, size, nullptr);
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
	*uid = sceKernelAllocMemBlock("basic", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE, size, nullptr);
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

void aes::graphicsFree(SceUID uid)
{
	// grab the base address
	void* mem = nullptr;
	uint32_t err = sceKernelGetMemBlockBase(uid, &mem);
	AES_ASSERT(err == SCE_OK);

	// unmap memory
	err = sceGxmUnmapMemory(mem);
	AES_ASSERT(err == SCE_OK);

	// free the memory block
	err = sceKernelFreeMemBlock(uid);
	AES_ASSERT(err == SCE_OK);
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
	return malloc(size);
}

static void patcherHostFree(void* userData, void* mem)
{
	AES_UNUSED(userData);
	free(mem);
}

GxmRenderer& GxmRenderer::instance()
{
	static GxmRenderer inst;
	return inst;
}

struct DisplayData
{
	void* address;
};

static void displayCallback(void const* callbackData)
{
	DisplayData const* displayData = (DisplayData const*)callbackData;
	SceDisplayFrameBuf frameBuf {
		.size = sizeof(SceDisplayFrameBuf),
		.base = displayData->address,
		.pitch = vita_display_stride_in_pixels,
		.pixelformat = vita_display_pixel_format,
		.width = vita_display_width,
		.height = vita_display_height
	};

	auto err = sceDisplaySetFrameBuf(&frameBuf, (SceDisplaySetBufSync) SCE_DISPLAY_UPDATETIMING_NEXTVSYNC);
	AES_ASSERT(err == SCE_OK);
	
	err = sceDisplayWaitVblankStart();
	AES_ASSERT(err == SCE_OK);
}

void GxmRenderer::init(Window& windowHandle)
{
	AES_PROFILE_FUNCTION();
	AES_UNUSED(windowHandle);

	// init gxm
	SceGxmInitializeParams initializeParams = {
		.flags = 0,
		.displayQueueMaxPendingCount = vita_display_max_pending_swaps,
		.displayQueueCallback = displayCallback,
		.displayQueueCallbackDataSize = sizeof(DisplayData),
		.parameterBufferSize = SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE
	};

	auto err = sceGxmInitialize(&initializeParams);
	AES_ASSERT(err == SCE_OK);

	// allocate ring buffers with default sizes
	void* vdmRingBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&vdmRingBufferUid);
	
	void* vertexRingBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&vertexRingBufferUid);
	
	void* fragmentRingBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&fragmentRingBufferUid);
	
	uint32_t fragmentUsseRingBufferOffset;
	void* fragmentUsseRingBuffer = fragmentUsseAlloc(
		SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE,
		&fragmentUsseRingBufferUid,
		&fragmentUsseRingBufferOffset);

	// create the gxm context
	hostMem = malloc(SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE);
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

	err = sceGxmCreateContext(&contextParams, &context);
	AES_ASSERT(err == SCE_OK);

	// create render target
	SceGxmRenderTargetParams renderTargetParams {};
	renderTargetParams.flags = 0;
	renderTargetParams.width = vita_display_width;
	renderTargetParams.height = vita_display_height;
	renderTargetParams.scenesPerFrame = 1;
	renderTargetParams.multisampleMode = vita_msaa_mode;
	renderTargetParams.multisampleLocations = 0;
	renderTargetParams.driverMemBlock = SCE_UID_INVALID_UID;

	err = sceGxmCreateRenderTarget(&renderTargetParams, &renderTarget);
	AES_ASSERT(err == SCE_OK);

	// allocate and initialize display buffers
	for (uint32_t i = 0; i < vita_display_buffer_count; ++i) 
	{
		// allocate memory for display
		displayBufferData[i] = graphicsAlloc(
			SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
			4 * vita_display_stride_in_pixels * vita_display_height,
			SCE_GXM_COLOR_SURFACE_ALIGNMENT,
			SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
			&displayBufferUid[i]);

		// memset the buffer to black
		for (uint32_t y = 0; y < vita_display_height; ++y) {
			uint32_t* row = (uint32_t*)displayBufferData[i] + y * vita_display_stride_in_pixels;
			for (uint32_t x = 0; x < vita_display_width; ++x) {
				row[x] = 0xff000000;
			}
		}

		// initialize a color surface for this display buffer
		err = sceGxmColorSurfaceInit(
			&displaySurface[i],
			vita_display_color_format,
			SCE_GXM_COLOR_SURFACE_LINEAR,
			(vita_msaa_mode == SCE_GXM_MULTISAMPLE_NONE) ? SCE_GXM_COLOR_SURFACE_SCALE_NONE : SCE_GXM_COLOR_SURFACE_SCALE_MSAA_DOWNSCALE,
			SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
			vita_display_width,
			vita_display_height,
			vita_display_stride_in_pixels,
			displayBufferData[i]);
		AES_ASSERT(err == SCE_OK);

		// create a sync object that we will associate with this buffer
		err = sceGxmSyncObjectCreate(&displayBufferSync[i]);
		AES_ASSERT(err == SCE_OK);
	}

	// compute the memory footprint of the depth buffer
	uint32_t const alignedWidth = aes::align(vita_display_width, SCE_GXM_TILE_SIZEX);
	uint32_t const alignedHeight = aes::align(vita_display_height, SCE_GXM_TILE_SIZEY);
	uint32_t sampleCount = alignedWidth * alignedHeight;
	uint32_t depthStrideInSamples = alignedWidth;
	if (vita_msaa_mode == SCE_GXM_MULTISAMPLE_4X) {
		// samples increase in X and Y
		sampleCount *= 4;
		depthStrideInSamples *= 2;
	}
	else if (vita_msaa_mode == SCE_GXM_MULTISAMPLE_2X) {
		// samples increase in Y only
		sampleCount *= 2;
	}

	// depth buffer allocation
	void* depthBufferData = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		4 * sampleCount,
		SCE_GXM_DEPTHSTENCIL_SURFACE_ALIGNMENT,
		SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
		&depthBufferUid);

	// create the SceGxmDepthStencilSurface structure
	err = sceGxmDepthStencilSurfaceInit(
		&depthSurface,
		SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24,
		SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
		depthStrideInSamples,
		depthBufferData,
		nullptr);
	AES_ASSERT(err == SCE_OK);


	// @Reviwe buffer sizes
	uint32_t const patcherBufferSize 		= 64 * 1024;
	uint32_t const patcherVertexUsseSize 	= 64 * 1024;
	uint32_t const patcherFragmentUsseSize 	= 64 * 1024;

	// allocate memory for buffers and USSE code
	void* patcherBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		patcherBufferSize,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
		&patcherBufferUid);

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

	err = sceGxmShaderPatcherCreate(&patcherParams, &shaderPatcher);
	AES_ASSERT(err == SCE_OK);

	auto const clearShaderData_vs = aes::readFileBin("app0:assets/shaders/vita/clear_vs.gxp");
	auto const clearShaderData_fs = aes::readFileBin("app0:assets/shaders/vita/clear_fs.gxp");
	auto const basicShaderData_fs = aes::readFileBin("app0:assets/shaders/vita/basic_fs.gxp");
	auto const basicShaderData_vs = aes::readFileBin("app0:assets/shaders/vita/basic_vs.gxp");
	
	SceGxmProgram const* clearShaderGxp_vs = reinterpret_cast<SceGxmProgram const*>(clearShaderData_vs.data());
	SceGxmProgram const* clearShaderGxp_fs = reinterpret_cast<SceGxmProgram const*>(clearShaderData_fs.data());
	SceGxmProgram const* basicShaderGxp_vs = reinterpret_cast<SceGxmProgram const*>(basicShaderData_vs.data());
	SceGxmProgram const* basicShaderGxp_fs = reinterpret_cast<SceGxmProgram const*>(basicShaderData_fs.data());

	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, clearShaderGxp_vs, &clearVertexProgramId);
	AES_ASSERT(err == SCE_OK);
	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, clearShaderGxp_fs, &clearFragmentProgramId);
	AES_ASSERT(err == SCE_OK);
	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, basicShaderGxp_vs, &basicVertexProgramId);
	AES_ASSERT(err == SCE_OK);
	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, basicShaderGxp_fs, &basicFragmentProgramId);
	AES_ASSERT(err == SCE_OK);

	// get attributes by name to create vertex format bindings
	const SceGxmProgramParameter* paramClearPositionAttribute = sceGxmProgramFindParameterByName(clearShaderGxp_vs, "aPosition");
	AES_ASSERT(paramClearPositionAttribute && (sceGxmProgramParameterGetCategory(paramClearPositionAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE));

	// create clear vertex format
	SceGxmVertexAttribute clearVertexAttributes[1];
	SceGxmVertexStream clearVertexStreams[1];
	clearVertexAttributes[0].streamIndex = 0;
	clearVertexAttributes[0].offset = 0;
	clearVertexAttributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	clearVertexAttributes[0].componentCount = 2;
	clearVertexAttributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(paramClearPositionAttribute);
	clearVertexStreams[0].stride = sizeof(glm::vec2);
	clearVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

	// create clear programs
	err = sceGxmShaderPatcherCreateVertexProgram(
		shaderPatcher,
		clearVertexProgramId,
		clearVertexAttributes,
		1,
		clearVertexStreams,
		1,
		&clearVertexProgram);
	AES_ASSERT(err == SCE_OK);
	
	err = sceGxmShaderPatcherCreateFragmentProgram(
		shaderPatcher,
		clearFragmentProgramId,
		SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
		vita_msaa_mode,
		NULL,
		clearShaderGxp_vs,
		&clearFragmentProgram);
	AES_ASSERT(err == SCE_OK);

	// create the clear triangle vertex/index data
	clearVertices = (glm::vec2*)graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		3 * sizeof(glm::vec2),
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&clearVerticesUid);

	clearIndices = (uint16_t*)graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		3 * sizeof(uint16_t),
		2,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&clearIndicesUid);

	clearVertices[0].x = -1.0f;
	clearVertices[0].y = -1.0f;
	clearVertices[1].x = 3.0f;
	clearVertices[1].y = -1.0f;
	clearVertices[2].x = -1.0f;
	clearVertices[2].y = 3.0f;

	clearIndices[0] = 0;
	clearIndices[1] = 1;
	clearIndices[2] = 2;

	// get attributes by name to create vertex format bindings
   	// first retrieve the underlying program to extract binding information
   	const SceGxmProgramParameter* paramBasicPositionAttribute = sceGxmProgramFindParameterByName(basicShaderGxp_vs, "aPosition");
	AES_ASSERT(paramBasicPositionAttribute && (sceGxmProgramParameterGetCategory(paramBasicPositionAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE));
	const SceGxmProgramParameter* paramBasicColorAttribute = sceGxmProgramFindParameterByName(basicShaderGxp_vs, "aColor");
	AES_ASSERT(paramBasicColorAttribute && (sceGxmProgramParameterGetCategory(paramBasicColorAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE));

	
	// create shaded triangle vertex format
	SceGxmVertexAttribute basicVertexAttributes[2];
	SceGxmVertexStream basicVertexStreams[1];
	basicVertexAttributes[0].streamIndex = 0;
	basicVertexAttributes[0].offset = 0;
	basicVertexAttributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	basicVertexAttributes[0].componentCount = 3;
	basicVertexAttributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(paramBasicPositionAttribute);
	basicVertexAttributes[1].streamIndex = 0;
	basicVertexAttributes[1].offset = 12;
	basicVertexAttributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_U8N;
	basicVertexAttributes[1].componentCount = 4;
	basicVertexAttributes[1].regIndex = sceGxmProgramParameterGetResourceIndex(paramBasicColorAttribute);
	basicVertexStreams[0].stride = sizeof(BasicVertex);
	basicVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

	err = sceGxmShaderPatcherCreateVertexProgram(
		shaderPatcher,
		basicVertexProgramId,
		basicVertexAttributes,
		2,
		basicVertexStreams,
		1,
		&basicVertexProgram);
	AES_ASSERT(err == SCE_OK);

	err = sceGxmShaderPatcherCreateFragmentProgram(
		shaderPatcher,
		basicFragmentProgramId,
		SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
		vita_msaa_mode,
		NULL,
		basicShaderGxp_vs,
		&basicFragmentProgram);
	AES_ASSERT(err == SCE_OK);

	// find vertex uniforms by name and cache parameter information
	wvpParam = sceGxmProgramFindParameterByName(basicShaderGxp_vs, "wvp");
	AES_ASSERT(wvpParam && (sceGxmProgramParameterGetCategory(wvpParam) == SCE_GXM_PARAMETER_CATEGORY_UNIFORM));

	BufferDescription bufferInfo = {
		.sizeInBytes = 16 * sizeof(float),
		.bufferUsage = Usage::Dynamic,
		.bindFlags = BindFlags::UniformBuffer,
		.gpuAccessFlags = (uint8_t)CPUAccessFlags::Write | (uint8_t)CPUAccessFlags::Read
	};
	wvpBuffer.create(bufferInfo);

	// create shaded triangle vertex/index data
	basicVertices = (BasicVertex*)graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		3 * sizeof(BasicVertex),
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&basicVerticesUid);

	basicIndices = (uint16_t*)graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		3 * sizeof(uint16_t),
		2,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&basicIndiceUid);

	basicVertices[0].x = -0.5f;
	basicVertices[0].y = -0.5f;
	basicVertices[0].z = 0.0f;
	basicVertices[0].color = 0xff0000ff;
	basicVertices[1].x = 0.5f;
	basicVertices[1].y = -0.5f;
	basicVertices[1].z = 0.0f;
	basicVertices[1].color = 0xff00ff00;
	basicVertices[2].x = -0.5f;
	basicVertices[2].y = 0.5f;
	basicVertices[2].z = 0.0f;
	basicVertices[2].color = 0xffff0000;

	basicIndices[0] = 0;
	basicIndices[1] = 1;
	basicIndices[2] = 2;
	
	AES_LOG("GXM initialized successfully");
}

void GxmRenderer::destroy()
{
	AES_PROFILE_FUNCTION();
	
	// wait until rendering is done
	sceGxmFinish(context);

	// clean up allocations
	sceGxmShaderPatcherReleaseFragmentProgram(shaderPatcher, basicFragmentProgram);
	sceGxmShaderPatcherReleaseVertexProgram(shaderPatcher, basicVertexProgram);
	sceGxmShaderPatcherReleaseFragmentProgram(shaderPatcher, clearFragmentProgram);
	sceGxmShaderPatcherReleaseVertexProgram(shaderPatcher, clearVertexProgram);
	graphicsFree(basicIndiceUid);
	graphicsFree(basicVerticesUid);
	graphicsFree(clearIndicesUid);
	graphicsFree(clearVerticesUid);

	// wait until display queue is finished before deallocating display buffers
	sceGxmDisplayQueueFinish();

	// clean up display queue
	graphicsFree(depthBufferUid);
	for (uint32_t i = 0; i < vita_display_buffer_count; ++i) {
		// clear the buffer then deallocate
		memset(displayBufferData[i], 0, vita_display_height * vita_display_stride_in_pixels * 4);
		graphicsFree(displayBufferUid[i]);

		// destroy the sync object
		sceGxmSyncObjectDestroy(displayBufferSync[i]);
	}

	// unregister programs and destroy shader patcher
	sceGxmShaderPatcherUnregisterProgram(shaderPatcher, basicFragmentProgramId);
	sceGxmShaderPatcherUnregisterProgram(shaderPatcher, basicVertexProgramId);
	sceGxmShaderPatcherUnregisterProgram(shaderPatcher, clearFragmentProgramId);
	sceGxmShaderPatcherUnregisterProgram(shaderPatcher, clearVertexProgramId);
	sceGxmShaderPatcherDestroy(shaderPatcher);
	fragmentUsseFree(patcherFragmentUsseUid);
	vertexUsseFree(patcherVertexUsseUid);
	graphicsFree(patcherBufferUid);

	// destroy the render target
	sceGxmDestroyRenderTarget(renderTarget);

	// destroy the context
	sceGxmDestroyContext(context);
	fragmentUsseFree(fragmentUsseRingBufferUid);
	graphicsFree(fragmentRingBufferUid);
	graphicsFree(vertexRingBufferUid);
	graphicsFree(vdmRingBufferUid);
	free(hostMem);

	// terminate libgxm
	sceGxmTerminate();
}

void GxmRenderer::startFrame(Camera const& cam)
{
	AES_PROFILE_FUNCTION();

	// update triangle angle
	static float rotationAngle = 0;
	rotationAngle += 6.28 / 60.0f;
	if (rotationAngle > 6.28)
		rotationAngle -= 6.28;

	// set up a 4x4 matrix for a rotation
	float constexpr aspectRatio = (float)vita_display_width / (float)vita_display_height;

	float const s = sin(rotationAngle);
	float const c = cos(rotationAngle);

	float wvpData[16];
	wvpData[0] = c / aspectRatio;
	wvpData[1] = s;
	wvpData[2] = 0.0f;
	wvpData[3] = 0.0f;

	wvpData[4] = -s / aspectRatio;
	wvpData[5] = c;
	wvpData[6] = 0.0f;
	wvpData[7] = 0.0f;

	wvpData[8] = 0.0f;
	wvpData[9] = 0.0f;
	wvpData[10] = 1.0f;
	wvpData[11] = 0.0f;

	wvpData[12] = 0.0f;
	wvpData[13] = 0.0f;
	wvpData[14] = 0.0f;
	wvpData[15] = 1.0f;

	sceGxmBeginScene(
		context,
		0,
		renderTarget,
		NULL,
		NULL,
		displayBufferSync[backBufferIndex],
		&displaySurface[backBufferIndex],
		&depthSurface);
	
	// set clear shaders
	sceGxmSetVertexProgram(context, clearVertexProgram);
	sceGxmSetFragmentProgram(context, clearFragmentProgram);

	// draw the clear triangle
	sceGxmSetVertexStream(context, 0, clearVertices);
	sceGxmDraw(context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, clearIndices, 3);
	// render the rotating triangle
	sceGxmSetVertexProgram(context, basicVertexProgram);
	sceGxmSetFragmentProgram(context, basicFragmentProgram);
	
	// set the vertex program constants
//	void* vertexDefaultBuffer;
//	sceGxmReserveVertexDefaultUniformBuffer(context, &vertexDefaultBuffer);
//	sceGxmSetUniformDataF(vertexDefaultBuffer, wvpParam, 0, 16, wvpData);
	AES_ONCE(AES_LOG("writting wvp"));
	wvpBuffer.setData(wvpData, sizeof(float) * 16);
	AES_ONCE(AES_LOG("end writting wvp"));

	AES_ASSERT(isAligned(wvpBuffer.getHandle(), 64));
	sceGxmSetVertexUniformBuffer(context, 0, wvpBuffer.getHandle());

	// draw the spinning triangle
	sceGxmSetVertexStream(context, 0, basicVertices);
	sceGxmDraw(context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, basicIndices, 3);
}

void GxmRenderer::endFrame()
{
	AES_PROFILE_FUNCTION();

	// end the scene on the main render target, submitting rendering work to the GPU
	sceGxmEndScene(context, NULL, NULL);
	// PA heartbeat to notify end of frame
	sceGxmPadHeartbeat(&displaySurface[backBufferIndex], displayBufferSync[backBufferIndex]);

	// queue the display swap for this frame
	DisplayData displayData;
	displayData.address = displayBufferData[backBufferIndex];
	sceGxmDisplayQueueAddEntry(
		displayBufferSync[frontBufferIndex],	// front buffer is OLD buffer
		displayBufferSync[backBufferIndex],		// back buffer is NEW buffer
		&displayData);

	// update buffer indices
	frontBufferIndex = backBufferIndex;
	backBufferIndex = (backBufferIndex + 1) % vita_display_buffer_count;
}

void GxmRenderer::bindBuffer(RHIBuffer& buffer, uint slot)
{
	AES_PROFILE_FUNCTION();
	
}

void GxmRenderer::bindVertexBuffer(RHIBuffer& buffer, uint stride, uint offset)
{
	AES_PROFILE_FUNCTION();
	sceGxmSetVertexStream(context, (uint32_t)offset, buffer.getHandle());
}

void GxmRenderer::bindIndexBuffer(RHIBuffer& buffer, TypeFormat format, uint offset)
{
	currentState.indexBufferInfo.typeFormat = format;
	currentState.indexBufferInfo.buffer = buffer.getHandle();
}

void GxmRenderer::setDrawPrimitiveMode(DrawPrimitiveType mode)
{
	currentState.primitiveType = mode;
}

void GxmRenderer::drawIndexed(uint indexCount)
{
	sceGxmDraw(context, rhiPrimitiveTypeToApi(currentState.primitiveType), 
				rhiIndexFormatToApi(currentState.indexBufferInfo.typeFormat), 
				currentState.indexBufferInfo.buffer,
				(uint32_t)indexCount);
}


