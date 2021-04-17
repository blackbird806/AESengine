#ifndef D3D11ELEMENTS_HPP
#define D3D11ELEMENTS_HPP

#include <d3d11.h>

#include "renderer/RHI/RHIElements.hpp"

namespace aes
{
	DXGI_FORMAT rhiTypeFormatToApi(TypeFormat format);
	D3D11_USAGE rhiBufferUsageToApi(BufferUsage u);
	D3D11_BIND_FLAG rhiBufferBindFlagsToApi(BindFlags flags);
	D3D_PRIMITIVE_TOPOLOGY rhiPrimitiveTypeToApi(DrawPrimitiveType primitiveMode);
	UINT rhiCPUAccessFlagsToApi(uint8_t flags);
}

#endif