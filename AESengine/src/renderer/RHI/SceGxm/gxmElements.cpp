#include "gxmElements.hpp"

#include "core/aes.hpp"

using namespace aes;

SceGxmPrimitiveType aes::rhiPrimitiveTypeToApi(DrawPrimitiveType type)
{
	switch (type)
	{
		case DrawPrimitiveType::Triangles: return SCE_GXM_PRIMITIVE_TRIANGLES; 
		case DrawPrimitiveType::Points: return SCE_GXM_PRIMITIVE_POINTS; 
		case DrawPrimitiveType::TriangleStrip: return SCE_GXM_PRIMITIVE_TRIANGLE_STRIP; 
		case DrawPrimitiveType::Lines: return SCE_GXM_PRIMITIVE_LINES; 
		case DrawPrimitiveType::LineStrip: // Linestrip not supported by vita
		default:
			AES_ASSERTF(false, "DrawPrimitiveType not supported {}", type);
	}
}

SceGxmIndexFormat aes::rhiIndexFormatToApi(IndexTypeFormat format)
{
	if (format == IndexTypeFormat::Uint32)
		return SCE_GXM_INDEX_FORMAT_U32;
	if (format == IndexTypeFormat::Uint16)
		return SCE_GXM_INDEX_FORMAT_U16;
	AES_ASSERT(false); // unsupported format type
}

uint8_t aes::rhiMemoryUsageToApi(MemoryUsage usage)
{
	switch(usage)
	{
		case MemoryUsage::Default:
		case MemoryUsage::Staging:
			return SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE;
		case MemoryUsage::Dynamic:
		case MemoryUsage::Immutable:
			return SCE_GXM_MEMORY_ATTRIB_READ;
	}
	AES_UNREACHABLE();
}

SceGxmParameterSemantic aes::rhiSemanticTypeToApi(SemanticType type)
{
	switch (type)
	{
	case SemanticType::Position:
		return SCE_GXM_PARAMETER_SEMANTIC_POSITION;
	case SemanticType::Color:
		return SCE_GXM_PARAMETER_SEMANTIC_COLOR;
	case SemanticType::Normal:
		return SCE_GXM_PARAMETER_SEMANTIC_NORMAL;
	case SemanticType::TexCoord:
		return SCE_GXM_PARAMETER_SEMANTIC_TEXCOORD;
	}
	AES_UNREACHABLE();
}

SceGxmTextureFormat aes::rhiFormatToApiTextureFormat(RHIFormat format)
{
	switch(format)
	{
		case RHIFormat::R8G8B8A8_Uint: return SCE_GXM_TEXTURE_FORMAT_U8U8U8U8_RGBA;
		case RHIFormat::R16G16_Float: return SCE_GXM_TEXTURE_FORMAT_F16F16_RGGG; // @review
		case RHIFormat::R16G16B16A16_Float: return SCE_GXM_TEXTURE_FORMAT_F16F16F16F16_RGBA;
	}
	AES_ASSERT(false); // unsupported format type
}


SceGxmTextureFilter aes::rhiTextureFilterToApi(TextureFilter filter)
{
	switch(filter)
	{
		case TextureFilter::Point:
			return SCE_GXM_TEXTURE_FILTER_POINT;
		case TextureFilter::Linear:
			return SCE_GXM_TEXTURE_FILTER_LINEAR;
	}
	AES_UNREACHABLE();
}

SceGxmTextureAddrMode aes::rhiTextureAddressModeToApi(TextureAddressMode mode)
{
	switch(mode)
	{
		case TextureAddressMode::Repeat:
			return SCE_GXM_TEXTURE_ADDR_REPEAT;
		case TextureAddressMode::Mirror:
			return SCE_GXM_TEXTURE_ADDR_MIRROR;
		case TextureAddressMode::Clamp:
			return SCE_GXM_TEXTURE_ADDR_CLAMP;
	}
	AES_UNREACHABLE();
}