#include "gxmBuffer.hpp"
#include "core/error.hpp"

#include "gxmRenderer.hpp"
#include "gxmElements.hpp"

using namespace aes;

GxmBuffer::GxmBuffer(GxmBuffer&& rhs) noexcept :
	bindFlags(rhs.bindFlags),
	size(rhs.size)
{
}

GxmBuffer& GxmBuffer::operator=(GxmBuffer&& rhs) noexcept
{
	bindFlags = rhs.bindFlags;
	size = rhs.size;
	return *this;
}

Result<void> GxmBuffer::create(BufferDescription const& desc)
{
	buffer = aes::graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE /*@Review*/,
		desc.sizeInBytes,
		0 /* @Review*/,
		rhiCPUAccessFlagsToApi(desc.cpuAccessFlags),
		&memID);
	return {};
}

// On ps vita CPU can access the whole memory
Result<void*> GxmBuffer::map()
{
	return { buffer };
}

Result<void> GxmBuffer::unmap()
{
	return { };
}

GxmBuffer::~GxmBuffer()
{
	// @Review 
	// no need for sceKernelGetMemBlockBase here ?
	aes::graphicsFree(memID);
}

void* GxmBuffer::getHandle()
{
	return buffer;
}
