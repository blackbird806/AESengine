#include "RHIBuffer.hpp"

#include "core/debug.hpp"

using namespace aes;

Result<void> RHIBuffer::setData(void* data, size_t size)
{
	auto rdata = map();
	if (rdata)
	{
		memcpy(rdata.value(), data, size);
	}
	else
	{
		AES_LOG_ERROR("failed to map model buffer data : {}", rdata.error());
		return { rdata.error() };
	}
	unmap();
	return {};
}
