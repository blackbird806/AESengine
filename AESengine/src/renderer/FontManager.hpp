#ifndef FONT_MANAGER_HPP
#define FONT_MANAGER_HPP

#include <d3d11.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include "core/error.hpp"

// https://loulou.developpez.com/tutoriels/moteur3d/partie6/
namespace aes {

	struct GraphicString
	{
		std::string str;
		glm::vec2 pos;
		glm::vec4 color;
		float textSize;
	};

	struct TextureFont
	{
		ID3D11Texture2D* texture;
		ID3D11SamplerState* sampleState;
		ID3D11ShaderResourceView* textureView;

		glm::ivec2 charSize[256];
	};

	class FontManager
	{
		struct Vertex
		{
			glm::vec2 pos;
			glm::vec2 uv;
		};
		
	public:
		Result<void> init();
		void drawString(GraphicString const& gstring);
		void draw();
		void destroy();
		
	private:
		TextureFont defaultFont;

		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;
		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
		ID3D11InputLayout* layout = nullptr;
		
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};

}

#endif // !FONT_MANAGER_HPP
