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

D3D11_USAGE aes::rhiBufferUsageToApi(Usage u)
{
	switch (u)
	{
	case Usage::Default: return D3D11_USAGE_DEFAULT;
	case Usage::Immutable: return D3D11_USAGE_IMMUTABLE;
	case Usage::Dynamic: return D3D11_USAGE_DYNAMIC;
	case Usage::Staging: return D3D11_USAGE_STAGING;
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
D3D11_CPU_ACCESS_FLAG aes::rhiCPUAccessFlagToApi(CPUAccessFlags flag)
{
	switch(flag)
	{
	case CPUAccessFlags::None:
		return (D3D11_CPU_ACCESS_FLAG)0;
	case CPUAccessFlags::Read:
		return D3D11_CPU_ACCESS_READ;
	case CPUAccessFlags::Write:
		return D3D11_CPU_ACCESS_WRITE;
	}
	AES_UNREACHABLE();
}


