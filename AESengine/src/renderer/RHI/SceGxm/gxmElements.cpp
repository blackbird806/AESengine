#include "gxmElements.hpp"

#include "core/aes.hpp"

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

SceGxmMemoryAttribFlags aes::rhiCPUAccessFlagsToApi(CPUAcessFlags flags)
{
	SceGxmMemoryAttribFlags apiFlags;
	if (flags & CPUAcessFlags::Read)
		apiFlags |= SCE_GXM_MEMORY_ATTRIB_READ;
	if (flags & CPUAcessFlags::write)
		apiFlags |= SCE_GXM_MEMORY_ATTRIB_Write;
	return apiFlags;
}

SceGxmIndexFormat rhiIndexFormatToApi(TypeFormat format)
{
	if (format == TypeFormat::Uint32)
		return SCE_GXM_INDEX_FORMAT_U32;
	if (format == TypeFormat::Uint16)
		return SCE_GXM_INDEX_FORMAT_U16;
	AES_ASSERT(false); // unsupported format type
}

