#include <glm/gtx/transform.hpp>
#include <iostream>
#include <numbers>
#include <fstream>
#include "core/debugMath.hpp"
#include "engine.hpp"
#include "renderer/RHI/model.hpp"
#include "core/utility.hpp"
#include "core/os.hpp"

#include "core/color.hpp"

class Game : public aes::Engine
{

public:
	
	Game(InitInfo const& info) : Engine(info)
	{
		AES_LOG("Game constructed");
	}
	
	aes::Model model, model2;

	void start() override
	{
		AES_PROFILE_FUNCTION();

		AES_LOG("start");
		auto rcube = aes::createCube();
		auto rcube2 = aes::createCube();
		
		if (!rcube || !rcube2)
		{
			AES_LOG_ERROR("Failed to create cubes");
			return;
		}
		
		model = std::move(rcube.value());
		model2 = std::move(rcube2.value());
		
		model.toWorld = glm::scale(model.toWorld, { 1.0f, 2.0f, 1.0f });
		model2.toWorld = glm::scale(model2.toWorld, { 2.0f, 1.0f, 1.0f });
		model2.toWorld *= glm::translate(model2.toWorld, { 0.0f, 1.0f, 3.0f });
		mainCamera.pos = { 0.0, 0.0, -5.0 };
		getMousePos(lastMousePosX, lastMousePosY);
	}

	float speed = 5.0f, sensitivity = 50.f;
	float lastMousePosX, lastMousePosY;
	glm::vec3 direction = {0.0, 0.0, 1.0};
	float yaw = 91, pitch = 2;
	
	void update(double dt) override
	{
		AES_PROFILE_FUNCTION();
		glm::vec4 movePos = { 0.0f, 0.f, 0.f, 0.0f };
		if (getKeyState(aes::Key::W) == aes::InputState::Down)
		{
			movePos.z += speed * dt;
		}

		if (getKeyState(aes::Key::S) == aes::InputState::Down)
		{
			movePos.z -= speed * dt;
		}

		if (getKeyState(aes::Key::D) == aes::InputState::Down)
		{
			movePos.x += speed * dt;
		}

		if (getKeyState(aes::Key::A) == aes::InputState::Down)
		{
			movePos.x -= speed * dt;
		}

		if (getKeyState(aes::Key::E) == aes::InputState::Down)
		{
			movePos.y += speed * dt;
		}

		if (getKeyState(aes::Key::Q) == aes::InputState::Down)
		{
			movePos.y -= speed * dt;
		}

		mainCamera.pos += glm::vec3(movePos * mainCamera.viewMatrix);

		if (getKeyState(aes::Key::RClick) == aes::InputState::Down)
		{
			float mx, my;
			getMousePos(mx, my);

			float xoffset = mx - lastMousePosX;
			float yoffset = my - lastMousePosY; // reversed since y-coordinates range from bottom to top

			lastMousePosX = mx;
			lastMousePosY = my;

			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		}
		else
		{
			getMousePos(lastMousePosX, lastMousePosY);
		}
		mainCamera.lookAt(mainCamera.pos + glm::normalize(direction));

		{
			float const ex = 0.0055f;
			float const csx = 0.025f;

			uint windowWidth, windowHeight;
			mainWindow->getScreenSize(windowWidth, windowHeight);
			float const aspect = (float)windowWidth / (float)windowHeight;
			mainCamera.projMatrix = glm::perspectiveLH_ZO(glm::radians(45.0f), aspect, 0.0001f, 1000.0f);
		}

		//fm.drawString({ "hello wordl : 45.45f", {0.1, 0.1}, {1.0, 0.0, 0.0, 1.0}, 64.0f });
	}

	void draw() override
	{
		AES_PROFILE_FUNCTION();
		model.render();
		//fm.render();
	}
};

#ifdef __vita__

auto constexpr vita_display_width = 960;
auto constexpr vita_display_height = 544;
auto constexpr vita_display_stride_in_pixels = 1024;
auto constexpr vita_display_pixel_format = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;
auto constexpr vita_display_color_format = SCE_GXM_COLOR_FORMAT_A8B8G8R8;
auto constexpr vita_display_buffer_count = 3;
auto constexpr vita_msaa_mode = SCE_GXM_MULTISAMPLE_NONE;
auto constexpr vita_display_max_pending_swaps = 2;
#define SCE_UID_INVALID_UID (static_cast<SceUID>(0xFFFFFFFF))
#define SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE
#define SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW
#define SCE_MATH_TWOPI (2.0 * std::numbers::pi)

#define SCE_DISPLAY_UPDATETIMING_NEXTHSYNC	0
#define SCE_DISPLAY_UPDATETIMING_NEXTVSYNC	1

void* graphicsAlloc(SceKernelMemBlockType type, uint32_t size, uint32_t alignement, uint32_t attribs, SceUID* uid)
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
		// LPDDR memblocks must be 4 Kib aligned
		AES_ASSERT(alignement <= 256 * 1024);
		size = aes::align(size, 256 * 1024);
	}
	
	auto err = SCE_OK;
	*uid = sceKernelAllocMemBlock("basic", type, size, nullptr);
	AES_ASSERT(*uid > SCE_OK);

	void* mem = nullptr;
	err = (decltype(err))sceKernelGetMemBlockBase(*uid, &mem);
	AES_ASSERT(err == SCE_OK);

	// map for the GPU
	err = (decltype(err))sceGxmMapMemory(mem, size, (SceGxmMemoryAttribFlags)attribs);
	AES_ASSERT(err == SCE_OK);
	
	return mem;
}

static void* fragmentUsseAlloc(uint32_t size, SceUID* uid, uint32_t* usseOffset)
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

static void graphicsFree(SceUID uid)
{
	// grab the base address
	void* mem = nullptr;
	auto err = sceKernelGetMemBlockBase(uid, &mem);
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
	void* mem = NULL;
	auto err = sceKernelGetMemBlockBase(uid, &mem);
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
	void* mem = NULL;
	auto err = sceKernelGetMemBlockBase(uid, &mem);
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

struct DisplayData
{
	void* address;
};

void displayCallback(void const* callbackData)
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

void initGxm()
{
	SceGxmInitializeParams initializeParams = {
		.flags = 0,
		.displayQueueMaxPendingCount = vita_display_max_pending_swaps,
		.displayQueueCallback = displayCallback,
		.displayQueueCallbackDataSize = sizeof(DisplayData),
		.parameterBufferSize = SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE
	};

	auto err = sceGxmInitialize(&initializeParams);
	AES_ASSERT(err == SCE_OK);
	
	SceUID vdmRingBufferUid, vertexRingBufferUid, fragmentRingBufferUid, fragmentUsseRingBufferUid;
	
	// Allocate ring buffer memory using default sizes
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
	
	// Set up parameters
	SceGxmContextParams contextParams {};
	contextParams.hostMem = malloc(SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE);
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

	// Create the context
	SceGxmContext* context = nullptr;
	err = sceGxmCreateContext(&contextParams, &context);
	AES_ASSERT(err == SCE_OK);

	// set up parameters
	SceGxmRenderTargetParams renderTargetParams {};
	renderTargetParams.flags = 0;
	renderTargetParams.width = vita_display_width;
	renderTargetParams.height = vita_display_height;
	renderTargetParams.scenesPerFrame = 1;
	renderTargetParams.multisampleMode = vita_msaa_mode;
	renderTargetParams.multisampleLocations = 0;
	renderTargetParams.driverMemBlock = SCE_UID_INVALID_UID;

	SceGxmRenderTarget* renderTarget;
	err = sceGxmCreateRenderTarget(&renderTargetParams, &renderTarget);
	AES_ASSERT(err == SCE_OK);

	/* ---------------------------------------------------------------------
	5. Allocate display buffers and sync objects

	We will allocate our back buffers in CDRAM, and create a color
	surface for each of them.

	To allow display operations done by the CPU to be synchronized with
	rendering done by the GPU, we also create a SceGxmSyncObject for each
	display buffer.  This sync object will be used with each scene that
	renders to that buffer and when queueing display flips that involve
	that buffer (either flipping from or to).
   --------------------------------------------------------------------- */

   	// allocate memory and sync objects for display buffers
	void* displayBufferData[vita_display_buffer_count];
	SceUID displayBufferUid[vita_display_buffer_count];
	SceGxmColorSurface displaySurface[vita_display_buffer_count];
	SceGxmSyncObject* displayBufferSync[vita_display_buffer_count];
	for (uint32_t i = 0; i < vita_display_buffer_count; ++i) {
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

	/* ---------------------------------------------------------------------
		6. Allocate a depth buffer

		Note that since this sample renders in a strictly back-to-front order,
		a depth buffer is not strictly required.  However, since it is usually
		necessary to create one to handle partial renders, we will create one
		now.  Note that we do not enable force load or store, so this depth
		buffer will not actually be read or written by the GPU when this
		sample is executed, so will have zero performance impact.
	   --------------------------------------------------------------------- */

	// compute the memory footprint of the depth buffer
	const uint32_t alignedWidth = aes::align(vita_display_width, SCE_GXM_TILE_SIZEX);
	const uint32_t alignedHeight = aes::align(vita_display_height, SCE_GXM_TILE_SIZEY);
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

	// allocate it
	SceUID depthBufferUid;
	void* depthBufferData = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		4 * sampleCount,
		SCE_GXM_DEPTHSTENCIL_SURFACE_ALIGNMENT,
		SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
		&depthBufferUid);

	// create the SceGxmDepthStencilSurface structure
	SceGxmDepthStencilSurface depthSurface;
	err = sceGxmDepthStencilSurfaceInit(
		&depthSurface,
		SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24,
		SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
		depthStrideInSamples,
		depthBufferData,
		nullptr);
	AES_ASSERT(err == SCE_OK);

	/* ---------------------------------------------------------------------
		7. Create a shader patcher and register programs

		A shader patcher object is required to produce vertex and fragment
		programs from the shader compiler output.  First we create a shader
		patcher instance, using callback functions to allow it to allocate
		and free host memory for internal state.

		We will use the shader patcher's internal heap to handle buffer
		memory and USSE memory for the final programs.  To do this, we
		leave the callback functions as NULL, but provide static memory
		blocks.

		In order to create vertex and fragment programs for a particular
		shader, the compiler output must first be registered to obtain an ID
		for that shader.  Within a single ID, vertex and fragment programs
		are reference counted and could be shared if created with identical
		parameters.  To maximise this sharing, programs should only be
		registered with the shader patcher once if possible, so we will do
		this now.
	   --------------------------------------------------------------------- */

	   // set buffer sizes for this sample
	const uint32_t patcherBufferSize = 64 * 1024;
	const uint32_t patcherVertexUsseSize = 64 * 1024;
	const uint32_t patcherFragmentUsseSize = 64 * 1024;

	// allocate memory for buffers and USSE code
	SceUID patcherBufferUid;
	void* patcherBuffer = graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		patcherBufferSize,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
		&patcherBufferUid);
	SceUID patcherVertexUsseUid;
	uint32_t patcherVertexUsseOffset;
	void* patcherVertexUsse = vertexUsseAlloc(
		patcherVertexUsseSize,
		&patcherVertexUsseUid,
		&patcherVertexUsseOffset);
	SceUID patcherFragmentUsseUid;
	uint32_t patcherFragmentUsseOffset;
	void* patcherFragmentUsse = fragmentUsseAlloc(
		patcherFragmentUsseSize,
		&patcherFragmentUsseUid,
		&patcherFragmentUsseOffset);

	// create a shader patcher
	SceGxmShaderPatcherParams patcherParams {};
	patcherParams.userData = NULL;
	patcherParams.hostAllocCallback = &patcherHostAlloc;
	patcherParams.hostFreeCallback = &patcherHostFree;
	patcherParams.bufferAllocCallback = NULL;
	patcherParams.bufferFreeCallback = NULL;
	patcherParams.bufferMem = patcherBuffer;
	patcherParams.bufferMemSize = patcherBufferSize;
	patcherParams.vertexUsseAllocCallback = NULL;
	patcherParams.vertexUsseFreeCallback = NULL;
	patcherParams.vertexUsseMem = patcherVertexUsse;
	patcherParams.vertexUsseMemSize = patcherVertexUsseSize;
	patcherParams.vertexUsseOffset = patcherVertexUsseOffset;
	patcherParams.fragmentUsseAllocCallback = NULL;
	patcherParams.fragmentUsseFreeCallback = NULL;
	patcherParams.fragmentUsseMem = patcherFragmentUsse;
	patcherParams.fragmentUsseMemSize = patcherFragmentUsseSize;
	patcherParams.fragmentUsseOffset = patcherFragmentUsseOffset;

	SceGxmShaderPatcher* shaderPatcher = NULL;
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

	// register programs with the patcher
	SceGxmShaderPatcherId clearVertexProgramId;
	SceGxmShaderPatcherId clearFragmentProgramId;
	SceGxmShaderPatcherId basicVertexProgramId;
	SceGxmShaderPatcherId basicFragmentProgramId;
	
	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, clearShaderGxp_vs, &clearVertexProgramId);
	AES_ASSERT(err == SCE_OK);
	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, clearShaderGxp_fs, &clearFragmentProgramId);
	AES_ASSERT(err == SCE_OK);
	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, basicShaderGxp_vs, &basicVertexProgramId);
	AES_ASSERT(err == SCE_OK);
	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, basicShaderGxp_fs, &basicFragmentProgramId);
	AES_ASSERT(err == SCE_OK);


	/* ---------------------------------------------------------------------
		8. Create the programs and data for the clear

		On SGX hardware, vertex programs must perform the unpack operations
		on vertex data, so we must define our vertex formats in order to
		create the vertex program.  Similarly, fragment programs must be
		specialized based on how they output their pixels and MSAA mode.

		We define the clear geometry vertex format here and create the vertex
		and fragment program.

		The clear vertex and index data is static, we allocate and write the
		data here.
	   --------------------------------------------------------------------- */

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
	SceGxmVertexProgram* clearVertexProgram = NULL;
	SceGxmFragmentProgram* clearFragmentProgram = NULL;
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
	SceUID clearVerticesUid;
	SceUID clearIndicesUid;
	glm::vec2* const clearVertices = (glm::vec2*)graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		3 * sizeof(glm::vec2),
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&clearVerticesUid);

	uint16_t* const clearIndices = (uint16_t*)graphicsAlloc(
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

	/* ---------------------------------------------------------------------
	9. Create the programs and data for the spinning triangle

	We define the spinning triangle vertex format here and create the
	vertex and fragment program.

	The vertex and index data is static, we allocate and write the data
	here too.
   --------------------------------------------------------------------- */

   // get attributes by name to create vertex format bindings
   // first retrieve the underlying program to extract binding information
	const SceGxmProgramParameter* paramBasicPositionAttribute = sceGxmProgramFindParameterByName(basicShaderGxp_vs, "aPosition");
	AES_ASSERT(paramBasicPositionAttribute && (sceGxmProgramParameterGetCategory(paramBasicPositionAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE));
	const SceGxmProgramParameter* paramBasicColorAttribute = sceGxmProgramFindParameterByName(basicShaderGxp_vs, "aColor");
	AES_ASSERT(paramBasicColorAttribute && (sceGxmProgramParameterGetCategory(paramBasicColorAttribute) == SCE_GXM_PARAMETER_CATEGORY_ATTRIBUTE));

	typedef struct BasicVertex {
		float x;
		float y;
		float z;
		uint32_t color;
	} BasicVertex;
	
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

	// create shaded triangle shaders
	SceGxmVertexProgram* basicVertexProgram = NULL;
	SceGxmFragmentProgram* basicFragmentProgram = NULL;
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
	const SceGxmProgramParameter* wvpParam = sceGxmProgramFindParameterByName(basicShaderGxp_vs, "wvp");
	AES_ASSERT(wvpParam && (sceGxmProgramParameterGetCategory(wvpParam) == SCE_GXM_PARAMETER_CATEGORY_UNIFORM));

	// create shaded triangle vertex/index data
	SceUID basicVerticesUid;
	SceUID basicIndiceUid;
	BasicVertex* const basicVertices = (BasicVertex*)graphicsAlloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		3 * sizeof(BasicVertex),
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&basicVerticesUid);
	uint16_t* const basicIndices = (uint16_t*)graphicsAlloc(
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

	/* ---------------------------------------------------------------------
	10. Start the main loop

	Now that everything has been initialized, we can start the main
	rendering loop of the sample.
   	--------------------------------------------------------------------- */

    // initialize controller data
	SceCtrlData ctrlData = {};

	// message for SDK sample auto test
	AES_LOG("## api_libgxm/basic: INIT SUCCEEDED ##\n");

	// loop until exit
	uint32_t backBufferIndex = 0;
	uint32_t frontBufferIndex = 0;
	float rotationAngle = 0.0f;
	bool quit = false;
	while (!quit) 
	{
		/* -----------------------------------------------------------------
		11. Update step

		Firstly, we check the control data for quit.

		Next, we perform the update step for this sample.  We advance the
		triangle angle by a fixed amount and update its matrix data.
	   ----------------------------------------------------------------- */

		// check control data
		sceCtrlPeekBufferPositive(0, &ctrlData, 1);
		quit = ctrlData.buttons & SCE_CTRL_CIRCLE;
		
		// update triangle angle
		rotationAngle += SCE_MATH_TWOPI / 60.0f;
		if (rotationAngle > SCE_MATH_TWOPI)
			rotationAngle -= SCE_MATH_TWOPI;

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

		/* -----------------------------------------------------------------
		12. Rendering step

		This sample renders a single scene containing the two triangles,
		the clear triangle followed by the spinning triangle.  Before
		any drawing can take place, a scene must be started.  We render
		to the back buffer, so it is also important to use a sync object
		to ensure that these rendering operations are synchronized with
		display operations.

		The clear triangle shaders do not declare any uniform variables,
		so this may be rendered immediately after setting the vertex and
		fragment program.

		The spinning triangle vertex program declares a matrix parameter,
		so this forms part of the vertex default uniform buffer and must
		be written before the triangle can be drawn.

		Once both triangles have been drawn the scene can be ended, which
		submits it for rendering on the GPU.
	   ----------------------------------------------------------------- */

	   // start rendering to the main render target
		err = sceGxmBeginScene(
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
		err = sceGxmDraw(context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, clearIndices, 3);
		// render the rotating triangle
		sceGxmSetVertexProgram(context, basicVertexProgram);
		sceGxmSetFragmentProgram(context, basicFragmentProgram);
		
		// set the vertex program constants
		void* vertexDefaultBuffer;
		sceGxmReserveVertexDefaultUniformBuffer(context, &vertexDefaultBuffer);
		sceGxmSetUniformDataF(vertexDefaultBuffer, wvpParam, 0, 16, wvpData);

		// draw the spinning triangle
		sceGxmSetVertexStream(context, 0, basicVertices);
		err = sceGxmDraw(context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, basicIndices, 3);
		
		// end the scene on the main render target, submitting rendering work to the GPU
		err = sceGxmEndScene(context, NULL, NULL);
		// PA heartbeat to notify end of frame
		err = sceGxmPadHeartbeat(&displaySurface[backBufferIndex], displayBufferSync[backBufferIndex]);

		/* -----------------------------------------------------------------
			13. Flip operation

			Now we have finished submitting rendering work for this frame it
			is time to submit a flip operation.  As part of specifying this
			flip operation we must provide the sync objects for both the old
			buffer and the new buffer.  This is to allow synchronization both
			ways: to not flip until rendering is complete, but also to ensure
			that future rendering to these buffers does not start until the
			flip operation is complete.

			The callback function will be called from an internal thread once
			queued GPU operations involving the sync objects is complete.
			Assuming we have not reached our maximum number of queued frames,
			this function returns immediately.

			Once we have queued our flip, we manually cycle through our back
			buffers before starting the next frame.
		   ----------------------------------------------------------------- */
		// queue the display swap for this frame
		DisplayData displayData;
		displayData.address = displayBufferData[backBufferIndex];
		err = sceGxmDisplayQueueAddEntry(
			displayBufferSync[frontBufferIndex],	// front buffer is OLD buffer
			displayBufferSync[backBufferIndex],		// back buffer is NEW buffer
			&displayData);

		// update buffer indices
		frontBufferIndex = backBufferIndex;
		backBufferIndex = (backBufferIndex + 1) % vita_display_buffer_count;
	}

	/* ---------------------------------------------------------------------
		14. Wait for rendering to complete and shut down

		Since there could be many queued operations not yet completed it is
		important to wait until the GPU has finished before destroying
		resources.  We do this by calling sceGxmFinish for our rendering
		context.

		Once the GPU is finished, we release all our programs, deallocate
		all our memory, destroy all object and finally terminate libgxm.
	   --------------------------------------------------------------------- */

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
	err = sceGxmDisplayQueueFinish();
	AES_ASSERT(err == SCE_OK);

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
	free(contextParams.hostMem);

	// terminate libgxm
	sceGxmTerminate();

	AES_LOG("## api_libgxm/basic: FINISHED ##\n");
}

#endif

int main()
{
#ifdef __vita__
	std::ofstream logFile("ux0:log/AES_log.txt");
	//aes::Logger::instance().addSink(std::make_unique<aes::PsvDebugScreenSink>());
#else
	std::ofstream logFile("AES_log.txt");
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(std::cout));
#endif
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(logFile));

#ifdef __vita__
	initGxm();
	return 0;
#endif
	
	Game game({
		.appName = "aes cubes"
	});

	game.init();
	game.run();

	return 0;
}
