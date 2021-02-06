#ifndef D3D11MODEL_HPP
#define D3D11MODEL_HPP

#include <d3d11.h>
#include <span>
#include <glm/glm.hpp>
#include "core/error.hpp"
#include "renderer/vertex.hpp"

namespace aes {

	class D3D11Model
	{
	public:

		Result<void> init(std::span<Vertex const> vertices, std::span<uint32_t const> indices);
		void destroy();

		void render();

		glm::mat4 toWorld;
	private:

		ID3D11Buffer* vertexBuffer = nullptr, *indexBuffer = nullptr;
		ID3D11Buffer* modelBuffer;
		size_t vertexCount, indexCount;
	};

}

#endif