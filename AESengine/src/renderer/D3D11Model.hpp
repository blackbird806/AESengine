#ifndef D3D11MODEL_HPP
#define D3D11MODEL_HPP

#include <d3d11.h>
#include <span>
#include <vector>
#include <glm/gtx/quaternion.hpp>
#include "vertex.hpp"

namespace aes {

	constexpr uint32_t cubeIndices[] = {
		//Top
		2, 7, 6,
		3, 7, 2,

		////Bottom
		0, 4, 5,
		0, 5, 1,

		////Left
		0, 2, 6,
		0, 6, 4,

		////Right
		1, 7, 3,
		1, 5, 7,

		//Front
		2, 0, 1,
		2, 1, 3,

		////Back
		4, 6, 7,
		4, 7, 5
	};

	constexpr glm::vec3 cubeVerticesPos[] = {
		{ -1, -1,  1.0f },
		{ 1, -1,  1.0f },
		{ -1,  1,  1.0f },
		{ 1,  1,  1.0f },
		{ -1, -1, -1.0f },
		{ 1, -1, -1.0f },
		{ -1,  1, -1.0f },
		{ 1,  1, -1.0f }
	};

	std::vector<Vertex> getCubeVertices();

	class D3D11Model
	{
	public:

		void init(std::span<Vertex const> vertices, std::span<uint32_t const> indices);
		 
		void destroy();

		void setPos(glm::vec3 const& p);
		void setRot(glm::quat const& r);
		void setSize(glm::vec3 const& s);
		glm::mat4 getModel() const;

		void render(ID3D11DeviceContext*);

	private:

		glm::mat4 model;
		ID3D11Buffer* vertexBuffer = nullptr, *indexBuffer = nullptr;
		int vertexCount, indexCount;
	};

}

#endif