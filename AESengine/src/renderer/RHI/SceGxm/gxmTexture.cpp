#include "gxmTexture.hpp"

using namespace aes;

Result<void> GxmTexture::init(TextureDescription const& info)
{
	AES_ASSERT(info.width > 0 && info.width < vita_texture_max_resolution);
	AES_ASSERT(info.height > 0 && info.height < vita_texture_max_resolution);
	AES_ASSERT(info.mipCount < vita_texture_max_mip_count);
	
	size_t const textureSize = info.width * info.height * getFormatSize(info.format);
	AES_ASSERT(textureSize > 0);

	void* textureData = aes::graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW /* @Review */,
		textureSize,
		SCE_GXM_TEXTURE_ALIGNMENT,
		rhiMemoryUsageToApi(desc.usage),
		&memID);

	AES_ASSERT(textureData);

	sceGxmTextureInitLinear(&texture, textureData, rhiFormatToApi(info.format), info.width, info.height, info.mipCount);
}
