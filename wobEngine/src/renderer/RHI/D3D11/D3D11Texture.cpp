#include "renderer/RHI/D3D11/D3D11Texture.hpp"

#include "renderer/RHI/D3D11/D3D11renderer.hpp"
#include "renderer/RHI/D3D11/D3D11Elements.hpp"

using namespace aes;

D3D11Texture::D3D11Texture(D3D11Texture&& rhs) noexcept :
	texture(rhs.texture), textureView(rhs.textureView)
{
	rhs.texture = nullptr;
	rhs.textureView = nullptr;
}

D3D11Texture& D3D11Texture::operator=(D3D11Texture&& rhs) noexcept
{
	destroy();
	texture = rhs.texture;
	textureView = rhs.textureView;
	rhs.texture = nullptr;
	rhs.textureView = nullptr;
	return *this;
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
