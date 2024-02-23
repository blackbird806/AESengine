#ifndef AES_VERTEX_HPP
#define AES_VERTEX_HPP

#include <glm/glm.hpp>
#include "core/vec3.hpp"
#include "core/vec4.hpp"

namespace aes {

	struct ModelBuffer
	{
		glm::mat4 world;
	};

	struct CameraBuffer
	{
		glm::mat4 view;
		glm::mat4 proj;
	};

	struct Vertex
	{
		vec3 pos;
		vec4 color;
	};
}

#endif