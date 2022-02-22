#ifndef AES_GXMTEXTURE_HPP
#define AES_GXMTEXTURE_HPP

#include "renderer/RHI/RHIElements.hpp"
#include "core/error.hpp"
#include <psp2/gxm.h>

namespace aes
{
	class GxmTexture
	{
		public:
		Result<void> init(TextureDescription const& info);


		private:
			SceUID memID;
			SceGxmTexture texture;
	};
} // namespace aes


#endif