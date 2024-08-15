#ifndef AES_RHIBUFFER_HPP
#define AES_RHIBUFFER_HPP

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
	using RHIBuffer = RHIBufferBase;
}

#endif
