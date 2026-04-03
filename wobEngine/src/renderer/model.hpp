#ifndef WOB_MODEL_HPP
#define WOB_MODEL_HPP

#include "core/vec4.hpp"
#include "core/matrix.hpp"
#include "core/array.hpp"
#include "core/arrayView.hpp"

#include "RHI/RHIBuffer.hpp"
#include "vertex.hpp"

/*
 * Simple and dumb model RHI compliant
 */
namespace wob
{
	[[deprecated("use simpleMeshes instead")]] Array<Vertex> getCubeVertices();
	[[deprecated("use simpleMeshes instead")]] Array<Vertex> getCubeVertices(vec4 const&);
	
	class Model
	{
		
	public:
		
		Model() = default;
		Model(Model&& rhs) noexcept = default;
		Model& operator=(Model&& rhs) noexcept = default;
		
		Result<void> init(ArrayView<Vertex const> vertices, ArrayView<uint32_t const> indices);
		
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
