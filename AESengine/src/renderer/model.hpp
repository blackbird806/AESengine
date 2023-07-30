#ifndef AES_MODEL_HPP
#define AES_MODEL_HPP

#include <vector>
#include <glm/glm.hpp>

#include "core/arrayView.hpp"
#include "RHI/RHIBuffer.hpp"
#include "material.hpp"
#include "vertex.hpp"

/*
 * Simple and dumb model RHI compliant
 */
namespace aes
{
	class Model
	{
		
	public:
		
		//Model() = default;
		//Model(Model&& rhs) noexcept = default;
		//Model& operator=(Model&& rhs) noexcept = default;
		
		Result<void> init(ArrayView<Vertex const> vertices, ArrayView<uint32_t const> indices);
		
		void draw();
		
		glm::mat4 toWorld;
		RHIBuffer modelBuffer;
		
	private:
		
		RHIBuffer vertexBuffer, indexBuffer;
		size_t vertexCount, indexCount;
	};

	Result<Model> createCube();
	Result<Model> createCube(glm::vec4 const&);
}

#endif
