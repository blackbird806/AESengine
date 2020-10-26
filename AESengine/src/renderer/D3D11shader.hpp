#ifndef D3D11SHADER_HPP
#define D3D11SHADER_HPP

#include <d3d11.h>
#include <glm/glm.hpp>

namespace aes {

	class D3D11Shader
	{

	public:

		void init(ID3D11Device* device);
		void destroy();

		void render(ID3D11DeviceContext* deviceContext, glm::mat4 world, glm::mat4 view, glm::mat4 proj);

	private:
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* layout;
		ID3D11Buffer* UBOBuffer;
	};

}

#endif
