#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>

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
		glm::vec3 pos;
		glm::vec4 color;
	};
}

#endif