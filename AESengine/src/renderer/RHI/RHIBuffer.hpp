#ifndef RHIBUFFER_HPP
#define RHIBUFFER_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef AES_GRAPHIC_API_D3D11
	#include "renderer/RHI/D3D11/D3D11Buffer.hpp"
#elif defined(AES_GRAPHIC_API_VITAGL)
	#include "renderer/RHI/VitaGL/vglBuffer.hpp"
#endif

namespace aes
{
	class RHIBuffer : public RHIBufferBase
	{
	public:
		
		Result<void> setData(void* data, size_t size);

		template<typename T>
		Result<void> setData(T const& t)
		{
			static_assert(std::is_standard_layout<T>::value);
			return setData((void*)&t, sizeof(t));
		}
	};
}

#endif
