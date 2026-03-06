#ifndef AES_MODEL_HPP
#define AES_MODEL_HPP

#include <span>
#include "core/vec4.hpp"
#include "core/matrix.hpp"
#include "core/array.hpp"

#include "RHI/RHIBuffer.hpp"
#include "vertex.hpp"

/*
 * Simple and dumb model RHI compliant
 */
namespace aes
{
	[[deprecated("use simpleMeshes instead")]] Array<Vertex> getCubeVertices();
	[[deprecated("use simpleMeshes instead")]] Array<Vertex> getCubeVertices(vec4 const&);
	
	class Model
	{
		
	public:
		
		Model() = default;
		Model(Model&& rhs) noexcept = default;
		Model& operator=(Model&& rhs) noexcept = default;
		
		Result<void> init(std::span<Vertex const> vertices, std::span<uint32_t const> indices);
		
		void draw();
		
		mat4 toWorld;
		RHIBuffer modelBuffer;
		
	private:
		
		RHIBuffer vertexBuffer, indexBuffer;
		size_t vertexCount, indexCount;
	};

	Result<Model> createCube();
	Result<Model> createCube(vec4 const&);
}

#endif
