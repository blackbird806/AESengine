#ifndef D3D11SHADER_HPP
#define D3D11SHADER_HPP

#include <d3d11.h>

namespace aes {

	class D3D11Shader
	{

	public:

		void init(ID3D11Device* device);
		void destroy();

		void render(ID3D11DeviceContext* deviceContext);

	private:
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* layout;
	};

}

#endif
