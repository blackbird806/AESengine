#ifndef RHIBUFFER_HPP
#define RHIBUFFER_HPP

#include "renderer/RHI/RHI.hpp"
#include <cstddef>
#include <span>

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
		Result<void> setData(std::span<T> data)
		{
			return setData(data.data(), data.size());
		}

		template<typename T>
		Result<void> setDataFromPOD(T const& t)
		{
			static_assert(std::is_standard_layout<T>::value);
			return setData((void*)&t, sizeof(t));
		}
	};
}

#endif
