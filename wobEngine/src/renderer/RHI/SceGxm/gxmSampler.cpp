#include "gxmSampler.hpp"
#include "gxmElements.hpp"

using namespace aes;

GxmSampler::GxmSampler(GxmSampler&& rhs) noexcept
{
	description = rhs.description;
}

Result<void> GxmSampler::init(SamplerDescription const& desc)
{
	AES_PROFILE_FUNCTION();
	description.filter = rhiTextureFilterToApi(desc.filter);
	description.addressU = rhiTextureAddressModeToApi(desc.addressU);
	description.addressV = rhiTextureAddressModeToApi(desc.addressV);
	description.lodBias = desc.lodBias;
	description.lodMin = desc.lodMin;
	return {};
}

GxmSampler& GxmSampler::operator=(GxmSampler&& rhs) noexcept
{
	description = rhs.description;
	return *this;
}

Result<void> GxmSampler::apply(SceGxmTexture* tex)
{
	AES_PROFILE_FUNCTION();
	AES_ASSERT(tex);

	auto err = sceGxmTextureSetLodBias(tex, description.lodBias);

	// unsuported by vita sdk for now
	// @TODO see if we can do a trick like vitagl to replace this call
	// see: https://github.com/Rinnegatamante/vitaGL/blob/master/source/utils/gxm_utils.c#L90
	// sceGxmTextureSetLodMin(tex, description.lodMin);

	err = sceGxmTextureSetUAddrMode(tex, description.addressU);
	err = sceGxmTextureSetVAddrMode(tex, description.addressV);

	// @Review what is a mipfilter ?
	// err = sceGxmTextureSetMipFilter(tex, filter);

	err = sceGxmTextureSetMagFilter(tex, description.filter);
	err = sceGxmTextureSetMinFilter(tex, description.filter);
	
	if (err != SCE_OK)
	{
		AES_LOG_ERROR("failed to apply GXM sampler err {}", err);
		return { AESError::SamplerApplicationFailed };
	}

	return {};
}

