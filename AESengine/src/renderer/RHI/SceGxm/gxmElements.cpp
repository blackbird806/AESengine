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
			AES_ASSERT(false);
	}

	AES_UNREACHABLE();
}

// @Review
uint8_t aes::rhiGPUAccessFlagsToApi(uint8_t flags)
{
	uint8_t apiFlags;
	if ((flags & (uint8_t)GPUAccessFlags::Read) == (uint8_t)GPUAccessFlags::Read)
		apiFlags |= SCE_GXM_MEMORY_ATTRIB_READ;
	if ((flags & (uint8_t)GPUAccessFlags::Write) == (uint8_t)GPUAccessFlags::Write)
		apiFlags |= SCE_GXM_MEMORY_ATTRIB_WRITE;
	return apiFlags;
}

SceGxmIndexFormat aes::rhiIndexFormatToApi(TypeFormat format)
{
	if (format == TypeFormat::Uint32)
		return SCE_GXM_INDEX_FORMAT_U32;
	if (format == TypeFormat::Uint16)
		return SCE_GXM_INDEX_FORMAT_U16;
	AES_ASSERT(false); // unsupported format type
}

