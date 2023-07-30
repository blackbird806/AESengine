#ifndef AES_RHIBUFFER_HPP
#define AES_RHIBUFFER_HPP

#include <cstddef>
#include <type_traits>
#include "renderer/RHI/RHI.hpp"
#include "core/arrayView.hpp"

#ifdef AES_GRAPHIC_API_D3D11
	#include "renderer/RHI/D3D11/D3D11Buffer.hpp"
#elif defined(AES_GRAPHIC_API_GXM)
	#include "renderer/RHI/SceGxm/gxmBuffer.hpp"
#endif

namespace aes
{
	class RHIBuffer : public RHIBufferBase
	{
	public:
		
		Result<void> setData(void* data, size_t size);

		template<typename T>
		Result<void> setData(ArrayView<T> data)
		{
			return setData(data.data(), data.size_bytes());
		}

		template<typename T>
		Result<void> setDataFromPOD(T const& t)
		{
			//static_assert(std::is_standard_layout<T>::value);
			return setData((void*)&t, sizeof(t));
		}
	};

	// realloc a RHIBuffer conserving its content
	Result<void> reallocRHIBuffer(RHIBuffer& buffer, BufferDescription const& reallocDesc);

	// realloc buffer only if reallocDesc.sizeInBytes > buffer.size
	Result<void> ensureRHIBufferCapacity(RHIBuffer& buffer, BufferDescription const& reallocDesc);
}

#endif
