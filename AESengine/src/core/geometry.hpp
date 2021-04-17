#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <glm/glm.hpp>

namespace aes {

	struct Line2D
	{
		glm::vec2 from, to;
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
		
		RectBounds getBounds() const noexcept
		{
			return {
				.topL = { min.x, max.y },
				.topR = max,
				.minL = min,
				.minR = { max.x, min.y }
			};
		}
	};
}

#endif // !GEOMETRY_HPP
