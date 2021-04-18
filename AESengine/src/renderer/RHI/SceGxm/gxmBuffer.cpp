#include "gxmBuffer.hpp"
#include "core/error.hpp"

#include "gxmRenderer.hpp"
#include "gxmElements.hpp"

using namespace aes;

GxmBuffer::GxmBuffer(GxmBuffer&& rhs) noexcept :
	bindFlags(rhs.bindFlags),
	size(rhs.size),
	memID(rhs.memID),
	buffer(rhs.buffer)
{
	rhs.buffer = nullptr;
	rhs.memID = SCE_UID_INVALID_UID;
}

GxmBuffer& GxmBuffer::operator=(GxmBuffer&& rhs) noexcept
{
	bindFlags = rhs.bindFlags;
	size = rhs.size;
	buffer = rhs.buffer;
	memID = rhs.memID;
	rhs.buffer = nullptr;
	rhs.memID = SCE_UID_INVALID_UID;
	return *this;
}

Result<void> GxmBuffer::create(BufferDescription const& desc)
{
	// @TODO vram memory manager
	buffer = aes::graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE /* @Review */,
		desc.sizeInBytes,
		0 /* alignement */,
		rhiBufferUsageToApi(desc.bufferUsage),
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
	if (memID != SCE_UID_INVALID_UID)
	{
		aes::graphicsFree(memID);
		memID = SCE_UID_INVALID_UID;
	}
}

void* GxmBuffer::getHandle()
{
	return buffer;
}

bool GxmBuffer::isValid() const
{
	return buffer != nullptr;
}

size_t GxmBuffer::getSize() const
{
	return size;
}

Result<void> GxmBuffer::copyTo(GxmBuffer& dest)
{
	AES_ASSERT(dest.isValid());
	AES_ASSERT(dest.size >= size);

	memcpy(dest.buffer, buffer, size);

	return {};
}