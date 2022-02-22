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
