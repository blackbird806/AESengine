#include "renderer/RHI/D3D11/D3D11Texture.hpp"

#include "renderer/RHI/D3D11/D3D11renderer.hpp"
#include "renderer/RHI/D3D11/D3D11Elements.hpp"

using namespace aes;

Result<void> D3D11Texture::init(TextureDescription const& info)
{
	ID3D11Device* device = D3D11Renderer::instance().getDevice();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Height = info.height;
	textureDesc.Width = info.width;
	textureDesc.MipLevels = info.mipsLevel;
	textureDesc.ArraySize = 1;
	textureDesc.Format = rhiFormatToApi(info.format);
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = rhiMemoryUsageToApi(info.usage);
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = rhiCPUAccessFlagsToApi(info.cpuAccess);
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	HRESULT err;
	if (info.initialData)
	{
		D3D11_SUBRESOURCE_DATA subresource;
		subresource.pSysMem = info.initialData;
		subresource.SysMemPitch = getFormatSize(info.format) * info.width;
		err = device->CreateTexture2D(&textureDesc, &subresource, &texture);
	}
	else
	{
		err = device->CreateTexture2D(&textureDesc, nullptr, &texture);
	}

	if (FAILED(err))
	{
		AES_LOG_ERROR("D3D11 CreateTexture2D failed !");
		return { AESError::GPUTextureCreationFailed };
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view for the texture.
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

}
