#include "gxmBuffer.hpp"
#include "core/error.hpp"

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
	return {};
}

Result<void*> GxmBuffer::map()
{
	return { AESError::GPUBufferMappingFailed };
}

Result<void> GxmBuffer::unmap()
{
	return { AESError::GPUBufferMappingFailed };
}

GxmBuffer::~GxmBuffer()
{

}
