#ifndef AES_D3D11ELEMENTS_HPP
#define AES_D3D11ELEMENTS_HPP

#include <d3d11.h>

#include "renderer/RHI/RHIElements.hpp"

namespace aes
{
	DXGI_FORMAT rhiTypeFormatToApi(IndexTypeFormat format);
	DXGI_FORMAT rhiFormatToApi(RHIFormat format);
	D3D11_USAGE rhiMemoryUsageToApi(MemoryUsage u);
	D3D11_BIND_FLAG rhiBufferBindFlagsToApi(BindFlags flags);
	D3D_PRIMITIVE_TOPOLOGY rhiPrimitiveTypeToApi(DrawPrimitiveType primitiveMode);
	UINT rhiCPUAccessFlagsToApi(CPUAccessFlags flags);
	D3D11_BLEND_OP rhiBlendOpToApi(BlendOp op);
	D3D11_BLEND rhiBlendFactorToApi(BlendFactor blend);
}

#endif