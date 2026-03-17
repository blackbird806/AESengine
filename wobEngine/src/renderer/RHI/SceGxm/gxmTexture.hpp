#ifndef WOB_GXMTEXTURE_HPP
#define WOB_GXMTEXTURE_HPP

#include "renderer/RHI/RHIElements.hpp"
#include "core/error.hpp"
#include <psp2/gxm.h>

namespace wob
{
	class GxmTexture
	{
		public:
		Result<void> init(TextureDescription const& info);

		~GxmTexture() noexcept;
		SceGxmTexture const* getHandle() const;
		
		private:
		SceUID memID;
		SceGxmTexture texture;
	};

	using RHITexture = GxmTexture;
} // namespace aes

#endif