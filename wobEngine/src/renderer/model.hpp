#ifndef AES_MODEL_HPP
#define AES_MODEL_HPP

#include <span>
#include "core/vec3.hpp"
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

	inline vec3 cubeVerticesPos[] = {
		{ -1, -1,  1.0f },
		{ 1, -1,  1.0f },
		{ -1,  1,  1.0f },
		{ 1,  1,  1.0f },
		{ -1, -1, -1.0f },
		{ 1, -1, -1.0f },
		{ -1,  1, -1.0f },
		{ 1,  1, -1.0f }
	};

	Array<Vertex> getCubeVertices();
	Array<Vertex> getCubeVertices(vec4 const&);
	
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
