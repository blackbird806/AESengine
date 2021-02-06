#include "vglBuffer.hpp"

using namespace aes;

VglBuffer::VglBuffer(VglBuffer&& rhs) :
	bindFlags(rhs.bindFlags),
	apiBuffer(rhs.apiBuffer),
	size(rhs.size)
{
	rhs.apiBuffer = 0;
}

VglBuffer& VglBuffer::operator=(VglBuffer&& rhs)
{
	bindFlags = rhs.bindFlags;
	apiBuffer = rhs.apiBuffer;
	size = rhs.size;
	rhs.apiBuffer = 0;
}

Result<void> VglBuffer::create(BufferDescription const& desc)
{
	glGenBuffers(1, &apiBuffer);
	// TODO
	return {};
}

Result<void*> VglBuffer::map()
{
	return { Error::GPUBufferMappingFailed };
}


Result<void*> VglBuffer::unmap()
{
	return { Error::GPUBufferMappingFailed };
}

VglBuffer::~VglBuffer()
{
	// if api buffer is invalid nothing will happen
	glDeleteBuffers(1, &apiBuffer);
}

GLuint VglBuffer::getHandle()
{
	return apiBuffer;
}
