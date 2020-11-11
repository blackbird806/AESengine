#ifndef D3D11SHADER_HPP
#define D3D11SHADER_HPP

#include <d3d11.h>
#include <glm/glm.hpp>
#include <string_view>

namespace aes {

	class D3D11Shader
	{

	public:

		void init();
		void init(std::string_view vs, std::string_view ps);

		void destroy();

		void render(glm::mat4 const& view, glm::mat4 const& proj);

	private:
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* layout;
		ID3D11Buffer* cameraBuffer;
	};

}

#endif
