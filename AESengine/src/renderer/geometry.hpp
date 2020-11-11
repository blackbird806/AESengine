#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <glm/glm.hpp>

namespace aes {

	struct Line2D
	{
		glm::vec2 from, to;
	};

	struct Rect
	{
		glm::vec2 min, max;
	};

}

#endif // !GEOMETRY_HPP
