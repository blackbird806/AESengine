#include "gxmTexture.hpp"
#include "gxmRenderer.hpp"
#include "gxmElements.hpp"

using namespace aes;

Result<void> GxmTexture::init(TextureDescription const& info)
{
	AES_ASSERT(info.width > 0 && info.width < vita_texture_max_resolution);
	AES_ASSERT(info.height > 0 && info.height < vita_texture_max_resolution);
	AES_ASSERT(info.mipsLevel < vita_texture_max_mip_count);
	
	size_t const textureSize = info.width * info.height * getFormatSize(info.format);
	AES_ASSERT(textureSize > 0);

	void* textureData = aes::graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW /* @Review */,
		textureSize,
		SCE_GXM_TEXTURE_ALIGNMENT,
		rhiMemoryUsageToApi(info.usage),
		&memID);
	
	if (textureData == nullptr)
	{
		AES_LOG_ERROR("GxmTexture allocation failed");
		return { AESError::GPUTextureCreationFailed };
	}

	sceGxmTextureInitLinear(&texture, textureData, rhiFormatToApiTextureFormat(info.format), info.width, info.height, info.mipsLevel);

	if (info.initialData != nullptr)
	{
		memcpy(textureData, info.initialData, textureSize);
	}
	return {};
}

SceGxmTexture const* GxmTexture::getHandle() const
{
	return &texture;
}

GxmTexture::~GxmTexture() noexcept
{
	if (memID != SCE_UID_INVALID_UID)
	{
		aes::graphicsFree(memID);
		memID = SCE_UID_INVALID_UID;
	}
}
