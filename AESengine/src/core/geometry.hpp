#ifndef AES_GEOMETRY_HPP
#define AES_GEOMETRY_HPP

#include <glm/glm.hpp>
#include <array>

// undef windows shit
#undef near
#undef far

/*
 * Geometry structs and collisions funcs
 */

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
			RectBounds bounds{};
			bounds.topL = { min.x, max.y };
			bounds.topR = max;
			bounds.minL = min;
			bounds.minR = { max.x, min.y };
			return bounds;
		}
	};

	bool pointInRect(glm::vec2 p, Rect const& r);

	struct Triangle2D
	{
		glm::vec2 a, b, c;
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

		std::array<glm::vec3, 8> getVertices() const;
	};

	bool AABB_AABBIntersect(AABB const& a, AABB const& b);
	
	struct Sphere
	{
		glm::vec3 pos;
		float size;
	};

	struct Plane
	{
		float dist;
		glm::vec3 dir;
	};

	struct Ray
	{
		glm::vec3 start, dir;
	};

	bool ray_AABBIntersect(Ray const& r, AABB const& aabb);
	bool ray_PlaneIntersect(Ray const& r, Plane const& p);
	
	enum PointPlanePlacement
	{
		Back = -1,
		OnPlane = 0,
		Front = 1,
	};

	PointPlanePlacement classifyPointToPlane(Plane const& plane, glm::vec3 const& pt);

	struct Frustum
	{
		static Frustum createFromPerspective(glm::mat4 const& m);
		Plane left, right, top, bottom, near, far;
	};

	bool frustum_AABBIntersect(Frustum const& f, AABB const& b);
	bool frustum_PlaneIntersect(Frustum const& f, Plane const& b);
	
}

#endif // !GEOMETRY_HPP
