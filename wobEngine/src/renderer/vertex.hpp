#ifndef AES_VERTEX_HPP
#define AES_VERTEX_HPP

#include "core/vec3.hpp"
#include "core/vec4.hpp"
#include "core/matrix.hpp"

namespace aes {

	struct ModelBuffer
	{
		mat4 world;
	};

	struct CameraBuffer
	{
		mat4 view;
		mat4 proj;
	};

	struct Vertex
	{
		vec3 pos;
		vec4 color;
	};
}

#endif