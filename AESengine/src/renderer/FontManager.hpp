#ifndef FONT_MANAGER_HPP
#define FONT_MANAGER_HPP

#include <d3d11.h>
#include <glm/glm.hpp>
#include <string>
#include "core/error.hpp"

// https://loulou.developpez.com/tutoriels/moteur3d/partie6/
namespace aes {

	struct GraphicString
	{
		std::string str;
		glm::vec2 pos;
		glm::vec4 color;
		int size;
	};

	struct TextureFont
	{
		ID3D11Texture2D* texture;
		glm::ivec2 charSize[256];
	};

	class FontManager
	{

	public:
		Result<void> init();
		void drawString(GraphicString const& gstring);

		TextureFont defaultFont;
	};

}

#endif // !FONT_MANAGER_HPP
