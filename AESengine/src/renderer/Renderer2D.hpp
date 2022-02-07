#ifndef IMMEDIATE_RENDERER_HPP
#define IMMEDIATE_RENDERER_HPP

#include <glm/glm.hpp>
#include <vector>
#include "core/geometry.hpp"
// include d3d11 after geometry since geometry use near and far identifiers that are defined in the global namespace by the shitty windows API
#include <d3d11.h> 

namespace aes {

	class Renderer2D
	{

	public:

		struct Vertex
		{
			glm::vec2 pos;
			glm::vec4 col;
		};

		struct Command
		{
			Command() {};
			Command(Command const& other);

			Command& operator=(Command const& other);

			enum class Type
			{
				Line,
				Rect,
				FillRect,
				Text,
			};

			Type type;
			glm::vec4 col;
			// @TODO use variant here
			union
			{
				Line2D line;
				Rect rect;
			};
		};

		static uint32_t constexpr maxVertices = 1024;
		static uint32_t constexpr maxIndices = 4096;

		static Renderer2D& Instance();

		void init();

		void drawLine(glm::vec2 p1, glm::vec2 p2, glm::vec4 const& col);
		void drawFillRect(Rect const& rect, glm::vec4 const& col);

		void draw();

		void destroy();

	private:

		void updateBuffers();
		std::vector<Command> commands;
		
		ID3D11BlendState* blendState = nullptr;
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11PixelShader* pixelShader = nullptr;
		ID3D11Buffer* vertexBuffer = nullptr;
		ID3D11Buffer* indexBuffer = nullptr;
		ID3D11InputLayout* layout = nullptr;
	};

}

#endif