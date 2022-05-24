#include "renderer/RHI/D3D11/D3D11Texture.hpp"

#include "renderer/RHI/D3D11/D3D11renderer.hpp"
#include "renderer/RHI/D3D11/D3D11Elements.hpp"

using namespace aes;

D3D11Texture::D3D11Texture(D3D11Texture&& rhs) noexcept :
	texture(rhs.texture), textureView(rhs.textureView)
{
	rhs.texture = nullptr;
	// only texture is checked in dtor
	//rhs.textureView = nullptr;
}

D3D11Texture& D3D11Texture::operator=(D3D11Texture&& rhs) noexcept
{
	destroy();
	texture = rhs.texture;
	textureView = rhs.textureView;
	rhs.texture = nullptr;
	//rhs.textureView = nullptr;
	return *this;
}

Result<void> D3D11Texture::init(TextureDescription const& info)
{
	validateTextureDescription(info);

	ID3D11Device* device = D3D11Renderer::instance().getDevice();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Height = info.height;
	textureDesc.Width = info.width;
	textureDesc.MipLevels = 1; // info.mipsLevel;
	textureDesc.ArraySize = 1;
	textureDesc.Format = rhiFormatToApi(info.format);
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = rhiMemoryUsageToApi(info.usage);
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;  // @Review
	textureDesc.CPUAccessFlags = rhiCPUAccessFlagsToApi(info.cpuAccess);
	//textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

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
		AES_LOG_ERROR("D3D11 CreateTexture2D failed, err: {}", err);
		return { AESError::GPUTextureCreationFailed };
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1; // @Review

	// Create the shader resource view for the texture.
	err = device->CreateShaderResourceView(texture, &srvDesc, &textureView);
	if (FAILED(err))
	{
		AES_LOG_ERROR("D3D11 CreateShaderResourceView failed !");
		return { AESError::GPUTextureCreationFailed };
	}
	//D3D11Renderer::instance().getDeviceContext()->GenerateMips(textureView);

	return {};
}

void D3D11Texture::destroy() noexcept
{
	if (texture)
	{
		texture->Release();
		textureView->Release();

		texture = nullptr;
		textureView = nullptr;
	}
}

D3D11Texture::~D3D11Texture() noexcept
{
	destroy();
}

ID3D11ShaderResourceView* D3D11Texture::getResourceView() const
{
	return textureView;
}
