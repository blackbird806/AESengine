#include <glm/gtx/transform.hpp>
#include <iostream>
#include <fstream>
#include "core/debugMath.hpp"
#include "engine.hpp"
#include "renderer/RHI/model.hpp"
#include "core/os.hpp"

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
		return;
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
	}

	void draw() override
	{
		AES_PROFILE_FUNCTION();
	}
};

#ifdef __vita__
#define DISPLAY_WIDTH			960
#define DISPLAY_HEIGHT			544
#define DISPLAY_STRIDE_IN_PIXELS	960
#define DISPLAY_COLOR_FORMAT		SCE_GXM_COLOR_FORMAT_A8B8G8R8
#define DISPLAY_PIXEL_FORMAT		SCE_DISPLAY_PIXELFORMAT_A8B8G8R8
#define DISPLAY_BUFFER_COUNT		3
#define DISPLAY_MAX_PENDING_SWAPS	2
#define DEFAULT_TEMP_POOL_SIZE		(1 * 1024 * 1024)

void* gpu_alloc(SceKernelMemBlockType type, unsigned int size, unsigned int alignment, unsigned int attribs, SceUID* uid)
{
	void* mem;

	if (type == SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW) {
		size = ALIGN(size, 256 * 1024);
	}
	else {
		size = ALIGN(size, 4 * 1024);
	}

	*uid = sceKernelAllocMemBlock("gpu_mem", type, size, NULL);

	if (*uid < 0)
		return NULL;

	if (sceKernelGetMemBlockBase(*uid, &mem) < 0)
		return NULL;

	if (sceGxmMapMemory(mem, size, attribs) < 0)
		return NULL;

	return mem;
}

void gpu_free(SceUID uid)
{
	void* mem = NULL;
	if (sceKernelGetMemBlockBase(uid, &mem) < 0)
		return;
	sceGxmUnmapMemory(mem);
	sceKernelFreeMemBlock(uid);
}

typedef struct vita2d_display_data {
	void* address;
} vita2d_display_data;


static void display_callback(const void* callback_data)
{
	SceDisplayFrameBuf framebuf;
	const vita2d_display_data* display_data = (const vita2d_display_data*)callback_data;

	memset(&framebuf, 0x00, sizeof(SceDisplayFrameBuf));
	framebuf.size = sizeof(SceDisplayFrameBuf);
	framebuf.base = display_data->address;
	framebuf.pitch = DISPLAY_STRIDE_IN_PIXELS;
	framebuf.pixelformat = DISPLAY_PIXEL_FORMAT;
	framebuf.width = DISPLAY_WIDTH;
	framebuf.height = DISPLAY_HEIGHT;
	sceDisplaySetFrameBuf(&framebuf, SCE_DISPLAY_SETBUF_NEXTFRAME);

	if (vblank_wait) {
		sceDisplayWaitVblankStart();
	}
}

void* fragment_usse_alloc(unsigned int size, SceUID* uid, unsigned int* usse_offset)
{
	void* mem = NULL;

	size = ALIGN(size, 4096);
	*uid = sceKernelAllocMemBlock("fragment_usse", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE, size, NULL);

	if (sceKernelGetMemBlockBase(*uid, &mem) < 0)
		return NULL;
	if (sceGxmMapFragmentUsseMemory(mem, size, usse_offset) < 0)
		return NULL;

	return mem;
}

void fragment_usse_free(SceUID uid)
{
	void* mem = NULL;
	if (sceKernelGetMemBlockBase(uid, &mem) < 0)
		return;
	sceGxmUnmapFragmentUsseMemory(mem);
	sceKernelFreeMemBlock(uid);
}

#endif

int main()
{
#ifdef __vita__
	std::ofstream logFile("ux0:log/AES_log.txt");
	aes::Logger::instance().addSink(std::make_unique<aes::PsvDebugScreenSink>());
#else
	std::ofstream logFile("AES_log.txt");
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(std::clog));
#endif
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(logFile));

#ifdef __vita__
	SceUID vdmRingBufferUid;
	SceUID vertexRingBufferUid;
	SceUID fragmentRingBufferUid;
	SceUID fragmentUsseRingBufferUid;
	
	SceAppMgrBudgetInfo info;
	info.size = sizeof(SceAppMgrBudgetInfo);
	if (!sceAppMgrGetBudgetInfo(&info)) 
		system_app_mode = 1;

	SceGxmInitializeParams initializeParams;
	memset(&initializeParams, 0, sizeof(SceGxmInitializeParams));
	initializeParams.flags = system_app_mode ? 0x0A : 0;
	initializeParams.displayQueueMaxPendingCount = DISPLAY_MAX_PENDING_SWAPS;
	initializeParams.displayQueueCallback = display_callback;
	initializeParams.displayQueueCallbackDataSize = sizeof(vita2d_display_data);
	initializeParams.parameterBufferSize = system_app_mode ? 2 * 1024 * 1024 : SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;

	auto err = system_app_mode ? sceGxmVshInitialize(&initializeParams) : sceGxmInitialize(&initializeParams);
	AES_LOG("sceGxmInitialize err : {}", err);
	
	// allocate ring buffer memory using default sizes
	void* vdmRingBuffer = gpu_alloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&vdmRingBufferUid);

	void* vertexRingBuffer = gpu_alloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&vertexRingBufferUid);

	void* fragmentRingBuffer = gpu_alloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&fragmentRingBufferUid);

	unsigned int fragmentUsseRingBufferOffset;
	void* fragmentUsseRingBuffer = fragment_usse_alloc(
		SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE,
		&fragmentUsseRingBufferUid,
		&fragmentUsseRingBufferOffset);

	memset(&contextParams, 0, sizeof(SceGxmContextParams));
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

	err = sceGxmCreateContext(&contextParams, &_vita2d_context);
	AES_LOG("sceGxmCreateContext(): {}\n", err);

	// set up parameters
	SceGxmRenderTargetParams renderTargetParams;
	memset(&renderTargetParams, 0, sizeof(SceGxmRenderTargetParams));
	renderTargetParams.flags = 0;
	renderTargetParams.width = DISPLAY_WIDTH;
	renderTargetParams.height = DISPLAY_HEIGHT;
	renderTargetParams.scenesPerFrame = 1;
	renderTargetParams.multisampleMode = msaa;
	renderTargetParams.multisampleLocations = 0;
	renderTargetParams.driverMemBlock = -1; // Invalid UID
	err = sceGxmCreateRenderTarget(&renderTargetParams, &renderTarget);
	AES_LOG("sceGxmCreateRenderTarget(): {}\n", err);

	for (i = 0; i < DISPLAY_BUFFER_COUNT; i++) {

		// allocate memory for display
		if (!system_app_mode) {
			displayBufferData[i] = gpu_alloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
				4 * DISPLAY_STRIDE_IN_PIXELS * DISPLAY_HEIGHT,
				SCE_GXM_COLOR_SURFACE_ALIGNMENT,
				SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
				&displayBufferUid[i]);

			// memset the buffer to black
			for (y = 0; y < DISPLAY_HEIGHT; y++) {
				unsigned int* row = (unsigned int*)displayBufferData[i] + y * DISPLAY_STRIDE_IN_PIXELS;
				for (x = 0; x < DISPLAY_WIDTH; x++) {
					row[x] = 0xff000000;
				}
			}
		}

		// initialize a color surface for this display buffer
		err = sceGxmColorSurfaceInit(
			&displaySurface[i],
			DISPLAY_COLOR_FORMAT,
			SCE_GXM_COLOR_SURFACE_LINEAR,
			(msaa == SCE_GXM_MULTISAMPLE_NONE) ? SCE_GXM_COLOR_SURFACE_SCALE_NONE : SCE_GXM_COLOR_SURFACE_SCALE_MSAA_DOWNSCALE,
			SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
			DISPLAY_WIDTH,
			DISPLAY_HEIGHT,
			DISPLAY_STRIDE_IN_PIXELS,
			displayBufferData[i]);

		// create a sync object that we will associate with this buffer
		err = sceGxmSyncObjectCreate(&displayBufferSync[i]);
	}

	// compute the memory footprint of the depth buffer
	const unsigned int alignedWidth = ALIGN(DISPLAY_WIDTH, SCE_GXM_TILE_SIZEX);
	const unsigned int alignedHeight = ALIGN(DISPLAY_HEIGHT, SCE_GXM_TILE_SIZEY);
	unsigned int sampleCount = alignedWidth * alignedHeight;
	unsigned int depthStrideInSamples = alignedWidth;
	if (msaa == SCE_GXM_MULTISAMPLE_4X) {
		// samples increase in X and Y
		sampleCount *= 4;
		depthStrideInSamples *= 2;
	}
	else if (msaa == SCE_GXM_MULTISAMPLE_2X) {
		// samples increase in Y only
		sampleCount *= 2;
	}

	// allocate the depth buffer
	depthBufferData = gpu_alloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		4 * sampleCount,
		SCE_GXM_DEPTHSTENCIL_SURFACE_ALIGNMENT,
		SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
		&depthBufferUid);

	// allocate the stencil buffer
	stencilBufferData = gpu_alloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		4 * sampleCount,
		SCE_GXM_DEPTHSTENCIL_SURFACE_ALIGNMENT,
		SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
		&stencilBufferUid);

	// create the SceGxmDepthStencilSurface structure
	err = sceGxmDepthStencilSurfaceInit(
		&depthSurface,
		SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24,
		SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
		depthStrideInSamples,
		depthBufferData,
		stencilBufferData);

	// set the stencil test reference (this is currently assumed to always remain 1 after here for region clipping)
	sceGxmSetFrontStencilRef(_vita2d_context, 1);
	// set the stencil function (this wouldn't actually be needed, as the set clip rectangle function has to call this at the begginning of every scene)
	sceGxmSetFrontStencilFunc(
		_vita2d_context,
		SCE_GXM_STENCIL_FUNC_ALWAYS,
		SCE_GXM_STENCIL_OP_KEEP,
		SCE_GXM_STENCIL_OP_KEEP,
		SCE_GXM_STENCIL_OP_KEEP,
		0xFF,
		0xFF);

	// set buffer sizes for this sample
	const unsigned int patcherBufferSize = 64 * 1024;
	const unsigned int patcherVertexUsseSize = 64 * 1024;
	const unsigned int patcherFragmentUsseSize = 64 * 1024;

	// allocate memory for buffers and USSE code
	void* patcherBuffer = gpu_alloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		patcherBufferSize,
		4,
		SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
		&patcherBufferUid);

	unsigned int patcherVertexUsseOffset;
	void* patcherVertexUsse = vertex_usse_alloc(
		patcherVertexUsseSize,
		&patcherVertexUsseUid,
		&patcherVertexUsseOffset);

	unsigned int patcherFragmentUsseOffset;
	void* patcherFragmentUsse = fragment_usse_alloc(
		patcherFragmentUsseSize,
		&patcherFragmentUsseUid,
		&patcherFragmentUsseOffset);

	// create a shader patcher
	SceGxmShaderPatcherParams patcherParams;
	memset(&patcherParams, 0, sizeof(SceGxmShaderPatcherParams));
	patcherParams.userData = NULL;
	patcherParams.hostAllocCallback = &patcher_host_alloc;
	patcherParams.hostFreeCallback = &patcher_host_free;
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

	err = sceGxmShaderPatcherCreate(&patcherParams, &shaderPatcher);
	DEBUG("sceGxmShaderPatcherCreate(): 0x%08X\n", err);

	// check the shaders
	err = sceGxmProgramCheck(clearVertexProgramGxp);
	DEBUG("clear_v sceGxmProgramCheck(): 0x%08X\n", err);
	err = sceGxmProgramCheck(clearFragmentProgramGxp);
	DEBUG("clear_f sceGxmProgramCheck(): 0x%08X\n", err);
	err = sceGxmProgramCheck(colorVertexProgramGxp);
	DEBUG("color_v sceGxmProgramCheck(): 0x%08X\n", err);
	err = sceGxmProgramCheck(colorFragmentProgramGxp);
	DEBUG("color_f sceGxmProgramCheck(): 0x%08X\n", err);
	err = sceGxmProgramCheck(textureVertexProgramGxp);
	DEBUG("texture_v sceGxmProgramCheck(): 0x%08X\n", err);
	err = sceGxmProgramCheck(textureFragmentProgramGxp);
	DEBUG("texture_f sceGxmProgramCheck(): 0x%08X\n", err);
	err = sceGxmProgramCheck(textureTintFragmentProgramGxp);
	DEBUG("texture_tint_f sceGxmProgramCheck(): 0x%08X\n", err);

	// register programs with the patcher
	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, clearVertexProgramGxp, &clearVertexProgramId);
	DEBUG("clear_v sceGxmShaderPatcherRegisterProgram(): 0x%08X\n", err);

	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, clearFragmentProgramGxp, &clearFragmentProgramId);
	DEBUG("clear_f sceGxmShaderPatcherRegisterProgram(): 0x%08X\n", err);

	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, colorVertexProgramGxp, &colorVertexProgramId);
	DEBUG("color_v sceGxmShaderPatcherRegisterProgram(): 0x%08X\n", err);

	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, colorFragmentProgramGxp, &colorFragmentProgramId);
	DEBUG("color_f sceGxmShaderPatcherRegisterProgram(): 0x%08X\n", err);

	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, textureVertexProgramGxp, &textureVertexProgramId);
	DEBUG("texture_v sceGxmShaderPatcherRegisterProgram(): 0x%08X\n", err);

	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, textureFragmentProgramGxp, &textureFragmentProgramId);
	DEBUG("texture_f sceGxmShaderPatcherRegisterProgram(): 0x%08X\n", err);

	err = sceGxmShaderPatcherRegisterProgram(shaderPatcher, textureTintFragmentProgramGxp, &textureTintFragmentProgramId);
	DEBUG("texture_tint_f sceGxmShaderPatcherRegisterProgram(): 0x%08X\n", err);

	// Fill SceGxmBlendInfo
	static const SceGxmBlendInfo blend_info = {
		.colorFunc = SCE_GXM_BLEND_FUNC_ADD,
		.alphaFunc = SCE_GXM_BLEND_FUNC_ADD,
		.colorSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA,
		.colorDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.alphaSrc = SCE_GXM_BLEND_FACTOR_SRC_ALPHA,
		.alphaDst = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorMask = SCE_GXM_COLOR_MASK_ALL
	};

	static const SceGxmBlendInfo blend_info_add = {
		.colorFunc = SCE_GXM_BLEND_FUNC_ADD,
		.alphaFunc = SCE_GXM_BLEND_FUNC_ADD,
		.colorSrc = SCE_GXM_BLEND_FACTOR_ONE,
		.colorDst = SCE_GXM_BLEND_FACTOR_ONE,
		.alphaSrc = SCE_GXM_BLEND_FACTOR_ONE,
		.alphaDst = SCE_GXM_BLEND_FACTOR_ONE,
		.colorMask = SCE_GXM_COLOR_MASK_ALL
	};

	// get attributes by name to create vertex format bindings
	const SceGxmProgramParameter* paramClearPositionAttribute = sceGxmProgramFindParameterByName(clearVertexProgramGxp, "aPosition");

	// create clear vertex format
	SceGxmVertexAttribute clearVertexAttributes[1];
	SceGxmVertexStream clearVertexStreams[1];
	clearVertexAttributes[0].streamIndex = 0;
	clearVertexAttributes[0].offset = 0;
	clearVertexAttributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	clearVertexAttributes[0].componentCount = 2;
	clearVertexAttributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(paramClearPositionAttribute);
	clearVertexStreams[0].stride = sizeof(vita2d_clear_vertex);
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

	DEBUG("clear sceGxmShaderPatcherCreateVertexProgram(): 0x%08X\n", err);

	err = sceGxmShaderPatcherCreateFragmentProgram(
		shaderPatcher,
		clearFragmentProgramId,
		SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
		msaa,
		NULL,
		clearVertexProgramGxp,
		&clearFragmentProgram);

	DEBUG("clear sceGxmShaderPatcherCreateFragmentProgram(): 0x%08X\n", err);

	// create the clear triangle vertex/index data
	clearVertices = (vita2d_clear_vertex*)gpu_alloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		3 * sizeof(vita2d_clear_vertex),
		4,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&clearVerticesUid);

	// Allocate a 64k * 2 bytes = 128 KiB buffer and store all possible
	// 16-bit indices in linear ascending order, so we can use this for
	// all drawing operations where we don't want to use indexing.
	linearIndices = (uint16_t*)gpu_alloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
		UINT16_MAX * sizeof(uint16_t),
		sizeof(uint16_t),
		SCE_GXM_MEMORY_ATTRIB_READ,
		&linearIndicesUid);

	// Range of i must be greater than uint16_t, this doesn't endless-loop
	for (uint32_t i = 0; i <= UINT16_MAX; ++i) {
		linearIndices[i] = i;
	}

	clearVertices[0].x = -1.0f;
	clearVertices[0].y = -1.0f;
	clearVertices[1].x = 3.0f;
	clearVertices[1].y = -1.0f;
	clearVertices[2].x = -1.0f;
	clearVertices[2].y = 3.0f;

	const SceGxmProgramParameter* paramColorPositionAttribute = sceGxmProgramFindParameterByName(colorVertexProgramGxp, "aPosition");
	DEBUG("aPosition sceGxmProgramFindParameterByName(): %p\n", paramColorPositionAttribute);

	const SceGxmProgramParameter* paramColorColorAttribute = sceGxmProgramFindParameterByName(colorVertexProgramGxp, "aColor");
	DEBUG("aColor sceGxmProgramFindParameterByName(): %p\n", paramColorColorAttribute);

	// create color vertex format
	SceGxmVertexAttribute colorVertexAttributes[2];
	SceGxmVertexStream colorVertexStreams[1];
	/* x,y,z: 3 float 32 bits */
	colorVertexAttributes[0].streamIndex = 0;
	colorVertexAttributes[0].offset = 0;
	colorVertexAttributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	colorVertexAttributes[0].componentCount = 3; // (x, y, z)
	colorVertexAttributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(paramColorPositionAttribute);
	/* color: 4 unsigned char  = 32 bits */
	colorVertexAttributes[1].streamIndex = 0;
	colorVertexAttributes[1].offset = 12; // (x, y, z) * 4 = 12 bytes
	colorVertexAttributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_U8N;
	colorVertexAttributes[1].componentCount = 4; // (color)
	colorVertexAttributes[1].regIndex = sceGxmProgramParameterGetResourceIndex(paramColorColorAttribute);
	// 16 bit (short) indices
	colorVertexStreams[0].stride = sizeof(vita2d_color_vertex);
	colorVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

	// create color shaders
	err = sceGxmShaderPatcherCreateVertexProgram(
		shaderPatcher,
		colorVertexProgramId,
		colorVertexAttributes,
		2,
		colorVertexStreams,
		1,
		&_vita2d_colorVertexProgram);

	DEBUG("color sceGxmShaderPatcherCreateVertexProgram(): 0x%08X\n", err);


	const SceGxmProgramParameter* paramTexturePositionAttribute = sceGxmProgramFindParameterByName(textureVertexProgramGxp, "aPosition");
	DEBUG("aPosition sceGxmProgramFindParameterByName(): %p\n", paramTexturePositionAttribute);

	const SceGxmProgramParameter* paramTextureTexcoordAttribute = sceGxmProgramFindParameterByName(textureVertexProgramGxp, "aTexcoord");
	DEBUG("aTexcoord sceGxmProgramFindParameterByName(): %p\n", paramTextureTexcoordAttribute);

	// create texture vertex format
	SceGxmVertexAttribute textureVertexAttributes[2];
	SceGxmVertexStream textureVertexStreams[1];
	/* x,y,z: 3 float 32 bits */
	textureVertexAttributes[0].streamIndex = 0;
	textureVertexAttributes[0].offset = 0;
	textureVertexAttributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	textureVertexAttributes[0].componentCount = 3; // (x, y, z)
	textureVertexAttributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(paramTexturePositionAttribute);
	/* u,v: 2 floats 32 bits */
	textureVertexAttributes[1].streamIndex = 0;
	textureVertexAttributes[1].offset = 12; // (x, y, z) * 4 = 12 bytes
	textureVertexAttributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
	textureVertexAttributes[1].componentCount = 2; // (u, v)
	textureVertexAttributes[1].regIndex = sceGxmProgramParameterGetResourceIndex(paramTextureTexcoordAttribute);
	// 16 bit (short) indices
	textureVertexStreams[0].stride = sizeof(vita2d_texture_vertex);
	textureVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

	// create texture shaders
	err = sceGxmShaderPatcherCreateVertexProgram(
		shaderPatcher,
		textureVertexProgramId,
		textureVertexAttributes,
		2,
		textureVertexStreams,
		1,
		&_vita2d_textureVertexProgram);

	DEBUG("texture sceGxmShaderPatcherCreateVertexProgram(): 0x%08X\n", err);

	// Create variations of the fragment program based on blending mode
	_vita2d_make_fragment_programs(&_vita2d_fragmentPrograms.blend_mode_normal, &blend_info, msaa);
	_vita2d_make_fragment_programs(&_vita2d_fragmentPrograms.blend_mode_add, &blend_info_add, msaa);

	// Default to "normal" blending mode (non-additive)
	vita2d_set_blend_mode_add(0);

	// find vertex uniforms by name and cache parameter information
	_vita2d_clearClearColorParam = sceGxmProgramFindParameterByName(clearFragmentProgramGxp, "uClearColor");
	DEBUG("_vita2d_clearClearColorParam sceGxmProgramFindParameterByName(): %p\n", _vita2d_clearClearColorParam);

	_vita2d_colorWvpParam = sceGxmProgramFindParameterByName(colorVertexProgramGxp, "wvp");
	DEBUG("color wvp sceGxmProgramFindParameterByName(): %p\n", _vita2d_colorWvpParam);

	_vita2d_textureWvpParam = sceGxmProgramFindParameterByName(textureVertexProgramGxp, "wvp");
	DEBUG("texture wvp sceGxmProgramFindParameterByName(): %p\n", _vita2d_textureWvpParam);

	_vita2d_textureTintColorParam = sceGxmProgramFindParameterByName(textureTintFragmentProgramGxp, "uTintColor");
	DEBUG("texture wvp sceGxmProgramFindParameterByName(): %p\n", _vita2d_textureWvpParam);

	// Allocate memory for the memory pool
	pool_size = temp_pool_size;
	pool_addr = gpu_alloc(
		SCE_KERNEL_MEMBLOCK_TYPE_USER_RW,
		pool_size,
		sizeof(void*),
		SCE_GXM_MEMORY_ATTRIB_READ,
		&poolUid);


	matrix_init_orthographic(_vita2d_ortho_matrix, 0.0f, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0.0f, 0.0f, 1.0f);

	backBufferIndex = 0;
	frontBufferIndex = 0;

	pgf_module_was_loaded = sceSysmoduleIsLoaded(SCE_SYSMODULE_PGF);

	if (pgf_module_was_loaded != SCE_SYSMODULE_LOADED)
		sceSysmoduleLoadModule(SCE_SYSMODULE_PGF);
	
	return;
#endif
	
	Game game({
		.appName = "aes cubes"
	});
	
	game.init();
	game.run();
}
