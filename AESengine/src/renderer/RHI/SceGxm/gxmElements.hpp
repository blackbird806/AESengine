#ifndef AES_GXMELEMENTS_HPP
#define AES_GXMELEMENTS_HPP

#include "core/platformVita/vitagxm.h"
#include "renderer/RHI/RHIElements.hpp"

namespace aes
{
	SceGxmPrimitiveType rhiPrimitiveTypeToApi(DrawPrimitiveType type);
	SceGxmIndexFormat rhiIndexFormatToApi(IndexTypeFormat format);
	SceGxmParameterSemantic rhiSemanticTypeToApi(SemanticType type);
	uint8_t rhiMemoryUsageToApi(MemoryUsage usage);
	SceGxmTextureFormat rhiFormatToApiTextureFormat(RHIFormat format);
	SceGxmColorFormat rhiFormatToApiColorFormat(RHIFormat format);
	SceGxmTextureFilter rhiTextureFilterToApi(TextureFilter filter);
	SceGxmTextureAddrMode rhiTextureAddressModeToApi(TextureAddressMode mode);
	uint8_t  rhiBlendOpToApi(BlendOp blendop);
	uint8_t  rhiBlendFactorToApi(BlendFactor blendfactor);
	uint8_t  rhiColorMaskToApi(ColorMaskFlags colorMask);
	SceGxmCullMode rhiCullModeToApi(CullMode mode);
	SceGxmMultisampleMode rhiMultisampleModeToApi(MultisampleMode mode);
	SceGxmPolygonMode rhiPolygonModeToApi(DrawPrimitiveType type);
}

#endif
