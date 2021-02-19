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
