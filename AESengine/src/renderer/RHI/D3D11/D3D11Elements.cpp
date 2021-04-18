#include "D3D11Elements.hpp"

#include "core/aes.hpp"


DXGI_FORMAT aes::rhiTypeFormatToApi(TypeFormat format)
{
	switch (format)
	{
	case TypeFormat::Uint32:
		return DXGI_FORMAT_R32_UINT;
	case TypeFormat::Uint16:
		return DXGI_FORMAT_R16_UINT;
	case TypeFormat::Uint8:
		return DXGI_FORMAT_R8_UINT;
	default: ;
	}
	AES_UNREACHABLE();
}

DXGI_FORMAT aes::rhiFormatToApi(RHIFormat format)
{
	switch (format)
	{
	case RHIFormat::R16G16_Float:
		return DXGI_FORMAT_R16G16_FLOAT;
	case RHIFormat::R32G32_Float:
		return DXGI_FORMAT_R32G32_FLOAT;
	case RHIFormat::R32G32B32_Float:
		return DXGI_FORMAT_R32G32B32_FLOAT;
	case RHIFormat::R16G16B16A16_Float:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	case RHIFormat::R32G32B32A32_Float:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	AES_UNREACHABLE();
}

D3D11_USAGE aes::rhiBufferUsageToApi(BufferUsage u)
{
	switch (u)
	{
	case BufferUsage::Default: return D3D11_USAGE_DEFAULT;
	case BufferUsage::Immutable: return D3D11_USAGE_IMMUTABLE;
	case BufferUsage::Dynamic: return D3D11_USAGE_DYNAMIC;
	case BufferUsage::Staging: return D3D11_USAGE_STAGING;
	}
	AES_UNREACHABLE();
}

D3D11_BIND_FLAG aes::rhiBufferBindFlagsToApi(BindFlags flags)
{
	switch (flags)
	{
	case BindFlags::VertexBuffer: return D3D11_BIND_VERTEX_BUFFER;
	case BindFlags::IndexBuffer: return D3D11_BIND_INDEX_BUFFER;
	case BindFlags::UniformBuffer: return D3D11_BIND_CONSTANT_BUFFER;
	}
	AES_UNREACHABLE();
}

D3D_PRIMITIVE_TOPOLOGY aes::rhiPrimitiveTypeToApi(DrawPrimitiveType primitiveMode)
{
	switch (primitiveMode)
	{
	case DrawPrimitiveType::Lines: 
		return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
	case DrawPrimitiveType::LineStrip:
		return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	case DrawPrimitiveType::Triangles:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	case DrawPrimitiveType::TriangleStrip:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	case DrawPrimitiveType::Points:
		return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	}
	AES_UNREACHABLE();
}

// @Review
UINT aes::rhiCPUAccessFlagsToApi(uint8_t flags)
{
	UINT result = 0;
	if((flags & (uint8_t)CPUAccessFlags::Read) == (uint8_t)CPUAccessFlags::Read)
		result |= D3D11_CPU_ACCESS_READ;
	if ((flags & (uint8_t)CPUAccessFlags::Write) == (uint8_t)CPUAccessFlags::Write)
		result |= D3D11_CPU_ACCESS_WRITE;
	return result;
}


