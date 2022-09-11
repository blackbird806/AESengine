#include "gxmDevice.hpp"
#include "core/utility.hpp"
#include "core/debug.hpp"
#include "core/allocator.hpp"
#include "gxmElements.hpp"
#include "gxmDebug.hpp"

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
		AES_ASSERT(alignement <= 4 * 1024);
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

Result<void> GxmDevice::init()
{
	AES_PROFILE_FUNCTION();

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

	err = sceGxmCreateContext(&contextParams, &context);
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

	// wait until rendering is done
	sceGxmFinish(context);

	sceGxmDestroyContext(context);
	fragmentUsseFree(fragmentUsseRingBufferUid);
	graphicsFree(fragmentRingBufferUid);
	graphicsFree(vertexRingBufferUid);
	graphicsFree(vdmRingBufferUid);
	globalAllocator.deallocate(hostMem);
}

void GxmDevice::drawIndexed(uint indexCount, uint indexOffset)
{
	AES_PROFILE_FUNCTION();

	if (currentState.indexBufferInfo.typeFormat == IndexTypeFormat::Uint16)
	{
			int err = sceGxmDraw(context, rhiPrimitiveTypeToApi(currentState.primitiveType),
				rhiIndexFormatToApi(currentState.indexBufferInfo.typeFormat),
				((uint16_t*)currentState.indexBufferInfo.buffer) + indexOffset,
				(uint32_t)indexCount);
		AES_ASSERT(err == SCE_OK);
	}
	else {
		int err = sceGxmDraw(context, rhiPrimitiveTypeToApi(currentState.primitiveType),
					rhiIndexFormatToApi(currentState.indexBufferInfo.typeFormat),
					((uint32_t*)currentState.indexBufferInfo.buffer) + indexOffset,
					(uint32_t)indexCount);
		AES_ASSERT(err == SCE_OK);
	}
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

Result<void> GxmDevice::setVertexBuffer(RHIBuffer& buffer, uint stride, uint offset = 0)
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
	currentState.indexBufferInfo.typeFormat = format;
	currentState.indexBufferInfo.buffer = buffer.getHandle();
	return {};
}

Result<void> GxmDevice::bindFragmentUniformBuffer(RHIBUffer& buffer, uint slot)
{
	AES_PROFILE_FUNCTION();
	AES_ASSERT(slot < SCE_GXM_MAX_UNIFORM_BUFFERS);
	auto const err = sceGxmSetFragmentUniformBuffer(context, index, tex.getHandle());
	if (err != SCE_OK)
	{
		AES_LOG_ERROR("failed to set gxm fragment uniform buffer error : {}", err);
		// @Review error code
		return { AESError::Undefined };
	}
	return {};
}

Result<void> GxmDevice::bindVertexUniformBuffer(RHIBUffer& buffer, uint slot);
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

Result<void> GxmDevice::bindFragmentTexture(RHITexture& buffer, uint slot)
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

Result<void> GxmDevice::bindVertexTexture(RHITexture& buffer, uint slot)
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