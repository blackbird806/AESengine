#ifndef AES_D3D11TEXTURE_HPP
#define AES_D3D11TEXTURE_HPP

#include <d3d11.h>

#include "core/error.hpp"
#include "renderer/RHI/RHIElements.hpp"

namespace aes
{
	class D3D11Texture
	{
	public:
		D3D11Texture(D3D11Texture&& rhs) noexcept;
		D3D11Texture& operator=(D3D11Texture&& rhs) noexcept;
		Result<void> init(TextureDescription const& info);
		~D3D11Texture() noexcept;
	private:
		ID3D11Texture2D* texture = nullptr;
		ID3D11ShaderResourceView* textureView = nullptr;
	};
}

#endif // !D3D11TEXTURE_HPP
