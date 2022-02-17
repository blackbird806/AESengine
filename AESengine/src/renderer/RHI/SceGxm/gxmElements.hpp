#ifndef GXMELEMENTS_HPP
#define GXMELEMENTS_HPP

#include "renderer/RHI/RHIElements.hpp"

#include <psp2/gxm.h>

namespace aes
{
	SceGxmPrimitiveType rhiPrimitiveTypeToApi(DrawPrimitiveType type);
	SceGxmIndexFormat rhiIndexFormatToApi(IndexTypeFormat format);
	SceGxmParameterSemantic rhiSemanticTypeToApi(SemanticType type);
	uint8_t rhiBufferUsageToApi(BufferUsage usage);
}


#endif
