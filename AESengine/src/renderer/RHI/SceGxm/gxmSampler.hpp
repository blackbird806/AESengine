#ifndef AES_GXMSAMPLER_HPP
#define AES_GXMSAMPLER_HPP

#include "core/error.hpp"
#include "renderer/RHI/RHIElements.hpp"
#include "core/platformVita/vitagxm.h"

namespace aes
{
	class GxmSampler
	{
		public:
		GxmSampler() = default;
		GxmSampler(GxmSampler&&) noexcept;
		Result<void> init(SamplerDescription const& desc);

		GxmSampler& operator=(GxmSampler&&) noexcept;
		~GxmSampler() = default;

		Result<void> apply(SceGxmTexture* tex); 
		
		private:
		struct GxmSamplerDescription
		{
			SceGxmTextureFilter filter;
			SceGxmTextureAddrMode addressU;
			SceGxmTextureAddrMode addressV;
			uint32_t lodMin;
			uint32_t lodBias;
		} description;
	};

	using RHISampler = GxmSampler;
} // namespace aes

#endif