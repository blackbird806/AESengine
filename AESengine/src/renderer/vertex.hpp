#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>

namespace aes {

	struct UBO
	{
		glm::mat4 world;
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