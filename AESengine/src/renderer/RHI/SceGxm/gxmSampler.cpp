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

	sceGxmTextureSetLodBias(tex, description.lodBias);
	sceGxmTextureSetLodMin(tex, description.lodMin);

	sceGxmTextureSetUAddrMode(tex, description.addressU);
	sceGxmTextureSetVAddrMode(tex, description.addressV);

	sceGxmTextureSetMipFilter(tex, description.filter);
	sceGxmTextureSetMagFilter(tex, description.filter);
	sceGxmTextureSetMinFilter(tex, description.filter);
}

