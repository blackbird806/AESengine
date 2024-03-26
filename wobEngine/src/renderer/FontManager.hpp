#ifndef FONT_MANAGER_HPP
#define FONT_MANAGER_HPP

#include <d3d11.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include "core/error.hpp"

// DEPRECATED file, only D3D implementation
// I will keep this until the new impl is finished

// https://loulou.developpez.com/tutoriels/moteur3d/partie6/
namespace aes {

	struct GraphicString
	{
		std::string str;
		glm::vec2 pos;
		glm::vec4 color;
		float textSize;
	};

	struct GlyphOld
	{
		glm::vec2 u;
		glm::vec2 v;

		float xoff, yoff, xadvance;
		float xoff2, yoff2;
		
		glm::vec2 getSize() const;
	};
	
	struct TextureFont
	{
		ID3D11Texture2D* texture;
		ID3D11SamplerState* sampleState;
		ID3D11ShaderResourceView* textureView;

		int width, height;
		std::vector<GlyphOld> backedChars;
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
		void drawFontTexture(glm::vec2 pos = { -0.5f, -0.5f }, float size = 1.0f);
		void render();
		void destroy();
		
	private:
		void addQuadIndices();

		TextureFont defaultFont;

		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;
		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
		ID3D11InputLayout* layout = nullptr;
		
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		uint32_t lastIndex = 0;
	};

}

#endif // !FONT_MANAGER_HPP
