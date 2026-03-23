#include "RHIDevice.hpp"

using namespace wob;

Result<void> RHIDevice::setBufferData(RHIBuffer const& buffer, void* data, size_t size)
{
	if (auto rdata = mapBuffer(buffer))
	{
		memcpy(rdata, data, size);
	}
	else
	{
		WOB_LOG_ERROR("failed to map unform buffer");
		return { AESError{ AESError::GPUBufferMappingFailed } };
	}
	return unmapBuffer(buffer);
}

Result<void> RHIDevice::reallocBuffer(RHIBuffer& buffer, BufferDescription const& reallocDesc)
{
	WOB_PROFILE_FUNCTION();
	
	auto err = createBuffer(reallocDesc);
	if (!err)
		return { err.error() };

	RHIBuffer newBuffer  = wob::move(err.value());
	if (buffer.isValid())
	{
		auto copyErr = copyBuffer(buffer, newBuffer);
		if (!copyErr)
			return copyErr;
	}
	buffer = wob::move(newBuffer);
	
	return {};
}

Result<void> RHIDevice::ensureBufferCapacity(RHIBuffer& buffer, BufferDescription const& reallocDesc)
{
	WOB_PROFILE_FUNCTION();

	if (buffer.isValid() && buffer.getSize() >= reallocDesc.sizeInBytes)
		return {};

	return reallocBuffer(buffer, reallocDesc);
}
