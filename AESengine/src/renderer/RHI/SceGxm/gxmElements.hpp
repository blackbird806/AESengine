#ifndef AES_GXMELEMENTS_HPP
#define AES_GXMELEMENTS_HPP

#include "renderer/RHI/RHIElements.hpp"

#include <psp2/gxm.h>

namespace aes
{
	SceGxmPrimitiveType rhiPrimitiveTypeToApi(DrawPrimitiveType type);
	SceGxmIndexFormat rhiIndexFormatToApi(IndexTypeFormat format);
	SceGxmParameterSemantic rhiSemanticTypeToApi(SemanticType type);
	uint8_t rhiMemoryUsageToApi(MemoryUsage usage);
}

#endif
