#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <glm/glm.hpp>

namespace aes {

	struct Line2D
	{
		glm::vec2 p1, p2;
	};

	struct Line3D
	{
		glm::vec3 p1, p2;
	};
	
	struct RectBounds
	{
		glm::vec2 topL, topR, minL, minR;
	};
	
	struct Rect
	{
		glm::vec2 min, max;

		static Rect createHalfCenter(glm::vec2 center, glm::vec2 halfSize)
		{
			return Rect{ center - halfSize, center + halfSize };
		}
		
		RectBounds getBounds() const
		{
			return {
				.topL = { min.x, max.y },
				.topR = max,
				.minL = min,
				.minR = { max.x, min.y }
			};
		}
	};

	struct Circle
	{
		glm::vec2 pos;
		float size;
	};
	
	struct AABB
	{
		glm::vec3 min;
		glm::vec3 max;
	};

	struct Sphere
	{
		glm::vec3 pos;
		float size;
	};
}

#endif // !GEOMETRY_HPP
