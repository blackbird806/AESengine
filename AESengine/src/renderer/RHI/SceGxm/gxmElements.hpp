#ifndef GXMELEMENTS_HPP
#define GXMELEMENTS_HPP

#include "renderer/RHI/RHIElements.hpp"

#include <psp2/gxm.h>

namespace aes
{
	SceGxmPrimitiveType rhiPrimitiveTypeToApi(DrawPrimitiveType type);
	uint8_t rhiGPUAccessFlagsToApi(uint8_t flags);
	SceGxmIndexFormat rhiIndexFormatToApi(TypeFormat format);
}


#endif
