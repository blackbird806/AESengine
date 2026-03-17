#ifndef WOB_SIMPLE_MESHES_HPP
#define WOB_SIMPLE_MESHES_HPP

#include "core/vec3.hpp"

// collection of hard coded vertices/indices for simple geometry

namespace wob::sm
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

	constexpr vec3 cubeVertices[] = {
		{ -1, -1,  1.0f },
		{ 1, -1,  1.0f },
		{ -1,  1,  1.0f },
		{ 1,  1,  1.0f },
		{ -1, -1, -1.0f },
		{ 1, -1, -1.0f },
		{ -1,  1, -1.0f },
		{ 1,  1, -1.0f }
	};
}

#endif