#ifndef IMMEDIATE_RENDERER_HPP
#define IMMEDIATE_RENDERER_HPP

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <glm/glm.hpp>
#include "color.hpp"
#include <vector>

namespace aes {

	enum class PrimitiveType
	{
		Line,
	};

	struct ImVertex
	{
		glm::vec3 pos;
		Color col;
	};

	struct DrawList
	{
		PrimitiveType type;
		std::vector<ImVertex> vertices;
	};

	class ImmediateRenderer
	{

	public:
		static uint32_t constexpr maxVertices = 1024;

		static ImmediateRenderer& Instance();

		void init();

		void drawLine(glm::vec3 const& p1, glm::vec3 const& p2, Color col);
		void submitDrawList(DrawList const& list);

	private:

		static ImmediateRenderer* instance;
		DrawList lineList;
		ID3D11Buffer* vertexBuffer;
	};

}

#endif