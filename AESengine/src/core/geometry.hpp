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
		static AABB createHalfCenter(glm::vec3 const& center, glm::vec3 const& halfSize)
		{
			return AABB{ center - halfSize, center + halfSize };
		}

		glm::vec3 min;
		glm::vec3 max;

		glm::vec3 center() const
		{
			return min + glm::length(max - min) * 0.5f;
		}

		glm::vec3 halfSize() const
		{
			return max - min;
		}
	};

	bool AABB_AABBIntersect(AABB const& a, AABB const& b);
	
	struct Sphere
	{
		glm::vec3 pos;
		float size;
	};
}

#endif // !GEOMETRY_HPP
