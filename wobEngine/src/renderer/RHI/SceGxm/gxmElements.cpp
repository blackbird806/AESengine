#include "gxmElements.hpp"

#include "core/wob.hpp"

using namespace wob;

SceGxmPrimitiveType wob::rhiPrimitiveTypeToApi(DrawPrimitiveType type)
{
	switch (type)
	{
		case DrawPrimitiveType::TrianglesFill: return SCE_GXM_PRIMITIVE_TRIANGLES; 
		case DrawPrimitiveType::Points: return SCE_GXM_PRIMITIVE_POINTS; 
		case DrawPrimitiveType::TriangleStrip: return SCE_GXM_PRIMITIVE_TRIANGLE_STRIP; 
		case DrawPrimitiveType::Lines: return SCE_GXM_PRIMITIVE_LINES; 
		case DrawPrimitiveType::LineStrip: // Linestrip not supported by vita
		default:
			WOB_ASSERTF(false, "DrawPrimitiveType not supported {}", type);
	}
	WOB_UNREACHABLE();
}

SceGxmIndexFormat wob::rhiIndexFormatToApi(IndexTypeFormat format)
{
	if (format == IndexTypeFormat::Uint32)
		return SCE_GXM_INDEX_FORMAT_U32;
	if (format == IndexTypeFormat::Uint16)
		return SCE_GXM_INDEX_FORMAT_U16;
	AES_ASSERT(false); // unsupported format type
}

uint8_t wob::rhiMemoryUsageToApi(MemoryUsage usage)
{
	switch(usage)
	{
		case MemoryUsage::Default:
		case MemoryUsage::Staging:
		case MemoryUsage::Dynamic:
			return SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE;
		case MemoryUsage::Immutable:
			return SCE_GXM_MEMORY_ATTRIB_READ;
	}
	WOB_UNREACHABLE();
}

SceGxmParameterSemantic wob::rhiSemanticTypeToApi(SemanticType type)
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
	WOB_UNREACHABLE();
}

SceGxmTextureFormat wob::rhiFormatToApiTextureFormat(RHIFormat format)
{
	switch(format)
	{
		case RHIFormat::R8G8B8A8_Uint: return SCE_GXM_TEXTURE_FORMAT_U8U8U8U8_RGBA;
		case RHIFormat::R16G16_Float: return SCE_GXM_TEXTURE_FORMAT_F16F16_RGGG; // @review
		case RHIFormat::R16G16B16A16_Float: return SCE_GXM_TEXTURE_FORMAT_F16F16F16F16_RGBA;
	}
	WOB_ASSERT(false); // unsupported format type
}

SceGxmColorFormat wob::rhiFormatToApiColorFormat(RHIFormat format)
{
	switch(format)
	{
		case RHIFormat::R8G8B8A8_Uint: return SCE_GXM_COLOR_FORMAT_U8U8U8U8_RGBA;
		case RHIFormat::R16G16_Float: return SCE_GXM_COLOR_FORMAT_F16F16_RG; // @review
		case RHIFormat::R16G16B16A16_Float: return SCE_GXM_COLOR_FORMAT_F16F16F16F16_RGBA;
	}
	WOB_ASSERT(false); // unsupported format type
}

SceGxmTextureFilter wob::rhiTextureFilterToApi(TextureFilter filter)
{
	switch(filter)
	{
		case TextureFilter::Point:
			return SCE_GXM_TEXTURE_FILTER_POINT;
		case TextureFilter::Linear:
			return SCE_GXM_TEXTURE_FILTER_LINEAR;
	}
	WOB_UNREACHABLE();
}

SceGxmTextureAddrMode wob::rhiTextureAddressModeToApi(TextureAddressMode mode)
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
	WOB_UNREACHABLE();
}

uint8_t wob::rhiBlendOpToApi(BlendOp blendop)
{
	switch(blendop)
	{
		case BlendOp::Add:
			return SCE_GXM_BLEND_FUNC_ADD;
		case BlendOp::Sub:
			return SCE_GXM_BLEND_FUNC_SUBTRACT;
	}

	// TODO
	// SCE_GXM_BLEND_FUNC_NONE,
	// SCE_GXM_BLEND_FUNC_REVERSE_SUBTRACT,
    // SCE_GXM_BLEND_FUNC_MIN,
    // SCE_GXM_BLEND_FUNC_MAX

	WOB_UNREACHABLE();
}

uint8_t wob::rhiBlendFactorToApi(BlendFactor blendfactor)
{
	switch (blendfactor)
	{
		case BlendFactor::Zero: return SCE_GXM_BLEND_FACTOR_ZERO;
		case BlendFactor::One: return SCE_GXM_BLEND_FACTOR_ONE;
		case BlendFactor::SrcColor: return SCE_GXM_BLEND_FACTOR_SRC_COLOR;
		case BlendFactor::SrcAlpha: return SCE_GXM_BLEND_FACTOR_SRC_ALPHA;
		case BlendFactor::DstColor: return SCE_GXM_BLEND_FACTOR_DST_COLOR;
		case BlendFactor::DstAlpha: return SCE_GXM_BLEND_FACTOR_DST_ALPHA;
		case BlendFactor::OneMinusSrcColor: return SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case BlendFactor::OneMinusDstColor: return SCE_GXM_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case BlendFactor::OneMinusSrcAlpha: return SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::OneMinusDstAlpha: return SCE_GXM_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	}
	// SCE_GXM_BLEND_FACTOR_SRC_ALPHA_SATURATE,
    // SCE_GXM_BLEND_FACTOR_DST_ALPHA_SATURATE

	WOB_UNREACHABLE();
}

uint8_t wob::rhiColorMaskToApi(ColorMaskFlags colorMask)
{
	// rhi maps directly to gxm
	return (uint8_t)colorMask;
}

SceGxmCullMode wob::rhiCullModeToApi(CullMode mode)
{
	switch (mode)
	{
		case CullMode::None: 				return SCE_GXM_CULL_NONE;
		case CullMode::Clockwise:			return SCE_GXM_CULL_CW;
		case CullMode::CounterClockwise:	return SCE_GXM_CULL_CCW;
	}
	WOB_UNREACHABLE();
}

SceGxmMultisampleMode wob::rhiMultisampleModeToApi(MultisampleMode mode)
{
	switch (mode)
	{
		case MultisampleMode::None: return SCE_GXM_MULTISAMPLE_NONE;
		case MultisampleMode::X2: return SCE_GXM_MULTISAMPLE_2X;
		case MultisampleMode::X4: return SCE_GXM_MULTISAMPLE_4X;
	}
	WOB_UNREACHABLE();
}

SceGxmPolygonMode wob::rhiPolygonModeToApi(DrawPrimitiveType type)
{
	switch(type)
	{
		case DrawPrimitiveType::Lines:
			return SCE_GXM_POLYGON_MODE_LINE;
		case DrawPrimitiveType::TrianglesFill:
		case DrawPrimitiveType::TriangleStrip:
			return SCE_GXM_POLYGON_MODE_TRIANGLE_FILL;
		case DrawPrimitiveType::Points:
			return SCE_GXM_POLYGON_MODE_POINT;
		case DrawPrimitiveType::TrianglesLine:
			return SCE_GXM_POLYGON_MODE_TRIANGLE_LINE;
		default:
			WOB_ASSERT(false); // unsupported polygon type
	}

	WOB_UNREACHABLE();
}

