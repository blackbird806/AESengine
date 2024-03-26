#include "RHIBuffer.hpp"

#include "core/debug.hpp"

using namespace aes;
//
//Result<void> RHIBuffer::setData(void* data, size_t size)
//{
//	if (auto rdata = map())
//	{
//		memcpy(rdata.value(), data, size);
//	}
//	else
//	{
//		AES_LOG_ERROR("failed to map model buffer data : {}", rdata.error());
//		return { AESError{ rdata.error() } };
//	}
//	unmap();
//	return {};
//}
//
//Result<void> aes::reallocRHIBuffer(RHIBuffer& buffer, BufferDescription const& reallocDesc)
//{
//	AES_PROFILE_FUNCTION();
//
//	RHIBuffer newBuffer;
//	//auto err = newBuffer.init(reallocDesc);
//	//if (!err)
//	//	return err;
//
//	if (buffer.isValid())
//	{
//		//err = buffer.copyTo(newBuffer);
//		//if (!err)
//		//	return err;
//	}
//	buffer = std::move(newBuffer);
//	
//	return {};
//}
//
//Result<void> aes::ensureRHIBufferCapacity(RHIBuffer& buffer, BufferDescription const& reallocDesc)
//{
//	AES_PROFILE_FUNCTION();
//
//	if (buffer.isValid() && buffer.getSize() >= reallocDesc.sizeInBytes)
//		return {};
//
//	reallocRHIBuffer(buffer, reallocDesc);
//
//	return {};
//}