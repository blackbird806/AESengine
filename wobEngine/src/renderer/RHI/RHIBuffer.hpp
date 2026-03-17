#ifndef WOB_RHIBUFFER_HPP
#define WOB_RHIBUFFER_HPP

#include "renderer/RHI/RHI.hpp"
#include <cstddef>
#include <span>

#ifdef WOB_GRAPHIC_API_D3D11
	#include "renderer/RHI/D3D11/D3D11Buffer.hpp"
#elif defined(WOB_GRAPHIC_API_GXM)
	#include "renderer/RHI/SceGxm/gxmBuffer.hpp"
#endif

namespace wob
{
	using RHIBuffer = RHIBufferBase;
}

#endif
