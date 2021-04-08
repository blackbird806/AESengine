#ifndef GXMELEMENTS_HPP
#define GXMELEMENTS_HPP

#include "renderer/RHI/RHIElements.hpp"

#include <psp2/gxm.h>

namespace aes
{
	SceGxmPrimitiveType rhiPrimitiveTypeToApi(DrawPrimitiveType type);
	SceGxmMemoryAttribFlags rhiCPUAccessFlagsToApi(CPUAccessFlags flag);
	SceGxmIndexFormat rhiIndexFormatToApi(TypeFormat format);
}


#endif
