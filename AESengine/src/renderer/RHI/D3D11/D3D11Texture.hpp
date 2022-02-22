#ifndef D3D11TEXTURE_HPP
#define D3D11TEXTURE_HPP

#include <d3d11.h>

#include "core/error.hpp"
#include "renderer/RHI/RHIElements.hpp"

namespace aes
{

	class D3D11Texture
	{
	public:
		Result<void> init(TextureDescription const& info);

	private:
		ID3D11Texture2D* texture = nullptr;
		ID3D11ShaderResourceView* textureView = nullptr;
	};

}

#endif // !D3D11TEXTURE_HPP
