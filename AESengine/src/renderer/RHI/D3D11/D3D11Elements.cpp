#include "D3D11Elements.hpp"

#include "core/aes.hpp"

using namespace aes;

DXGI_FORMAT aes::rhiTypeFormatToApi(IndexTypeFormat format)
{
	switch (format)
	{
		case IndexTypeFormat::Uint32:
			return DXGI_FORMAT_R32_UINT;
		case IndexTypeFormat::Uint16:
			return DXGI_FORMAT_R16_UINT;
		default:;
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
		case RHIFormat::R8G8B8A8_Uint:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case RHIFormat::D24_S8_Uint:
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
			// TODO
		//case RHIFormat::R8G8B8A8_Uint:
		//		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		default: ;
	}
	AES_UNREACHABLE();
}

D3D11_USAGE aes::rhiMemoryUsageToApi(MemoryUsage u)
{
	switch (u)
	{
		case MemoryUsage::Default: return D3D11_USAGE_DEFAULT;
		case MemoryUsage::Immutable: return D3D11_USAGE_IMMUTABLE;
		case MemoryUsage::Dynamic: return D3D11_USAGE_DYNAMIC;
		case MemoryUsage::Staging: return D3D11_USAGE_STAGING;
	}
	AES_UNREACHABLE();
}

D3D11_BIND_FLAG aes::rhiBufferBindFlagsToApi(BindFlags flags)
{
	int32_t result = {};
	
	if (flags & BindFlagBits::VertexBuffer)
		result |= D3D11_BIND_VERTEX_BUFFER;
	
	if (flags & BindFlagBits::IndexBuffer)
		result |= D3D11_BIND_INDEX_BUFFER;
	
	if (flags & BindFlagBits::UniformBuffer)
		result |= D3D11_BIND_CONSTANT_BUFFER;
	
	return static_cast<D3D11_BIND_FLAG>(result);
}

D3D_PRIMITIVE_TOPOLOGY aes::rhiPrimitiveTypeToApi(DrawPrimitiveType primitiveMode)
{
	switch (primitiveMode)
	{
		case DrawPrimitiveType::Lines:
			return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		case DrawPrimitiveType::LineStrip:
			return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case DrawPrimitiveType::TrianglesFill:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case DrawPrimitiveType::TriangleStrip:
			return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case DrawPrimitiveType::Points:
			return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
	}
	AES_UNREACHABLE();
}

// @Review
UINT aes::rhiCPUAccessFlagsToApi(CPUAccessFlags flags)
{
	UINT result = 0;
	if (flags & CPUAccessFlagBits::Read)
		result |= D3D11_CPU_ACCESS_READ;
	if (flags & CPUAccessFlagBits::Write)
		result |= D3D11_CPU_ACCESS_WRITE;
	return result;
}

D3D11_BLEND_OP aes::rhiBlendOpToApi(BlendOp op)
{
	switch (op)
	{
		case BlendOp::Sub:
			return D3D11_BLEND_OP_SUBTRACT;
		case BlendOp::Add:
			return D3D11_BLEND_OP_ADD;
	}
	AES_UNREACHABLE();
}

D3D11_BLEND aes::rhiBlendFactorToApi(BlendFactor blend)
{
	switch (blend)
	{
		case BlendFactor::Zero: return D3D11_BLEND_ZERO;
		case BlendFactor::One: return D3D11_BLEND_ONE;
		case BlendFactor::SrcColor: return D3D11_BLEND_SRC_COLOR;
		case BlendFactor::SrcAlpha: return D3D11_BLEND_SRC_ALPHA;
		case BlendFactor::DstColor: return D3D11_BLEND_DEST_COLOR;
		case BlendFactor::DstAlpha: return D3D11_BLEND_DEST_ALPHA;
		case BlendFactor::OneMinusSrcColor: return D3D11_BLEND_INV_SRC_COLOR;
		case BlendFactor::OneMinusDstColor: return D3D11_BLEND_INV_DEST_COLOR;
		case BlendFactor::OneMinusSrcAlpha: return D3D11_BLEND_INV_SRC_ALPHA;
		case BlendFactor::OneMinusDstAlpha: return D3D11_BLEND_INV_DEST_ALPHA;
	}
	AES_UNREACHABLE();
}

D3D11_FILTER aes::rhiTextureFilterToApi(TextureFilter filter)
{
	switch(filter)
	{
		case TextureFilter::Point:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case TextureFilter::Linear:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	}
	AES_UNREACHABLE();
}

D3D11_TEXTURE_ADDRESS_MODE aes::rhiTextureAddressModeToApi(TextureAddressMode adressMode)
{
	switch(adressMode)
	{
		case TextureAddressMode::Repeat:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case TextureAddressMode::Mirror:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		case TextureAddressMode::Clamp:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
	}
	AES_UNREACHABLE();
}

D3D11_CULL_MODE aes::rhiCullModeToApi(CullMode cullMode)
{
	// I'm really not sure if GXM match 1/1 D3D on this case
	switch (cullMode)
	{
	case CullMode::Clockwise: return D3D11_CULL_FRONT;
	case CullMode::CounterClockwise: return D3D11_CULL_BACK;
	case CullMode::None: return D3D11_CULL_NONE;
	}
	AES_UNREACHABLE();
}

