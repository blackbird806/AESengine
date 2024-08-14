#ifndef AES_GEOMETRY_HPP
#define AES_GEOMETRY_HPP

#include <array>
#include "core/vec2.hpp"
#include "core/vec3.hpp"
#include "core/matrix.hpp"

// undef windows shit
#undef near
#undef far

/*
 * Geometry structs and collisions funcs
 */

namespace aes {

	struct Line2D
	{
		vec2 p1, p2;
	};

	struct Line3D
	{
		vec3 p1, p2;
	};
	
	struct RectBounds
	{
		vec2 topL, topR, minL, minR;
	};
	
	struct Rect
	{
		vec2 min, max;

		static Rect createHalfCenter(vec2 center, vec2 halfSize)
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

	bool pointInRect(vec2 p, Rect const& r);

	struct Triangle2D
	{
		vec2 a, b, c;
	};

	struct Circle
	{
		vec2 pos;
		float size;
	};
	
	struct AABB
	{
		static AABB createHalfCenter(vec3 const& center, vec3 const& halfSize)
		{
			return AABB{ center - halfSize, center + halfSize };
		}

		vec3 min;
		vec3 max;

		vec3 center() const
		{
			return min + (max - min).length() * 0.5f;
		}

		vec3 halfSize() const
		{
			return max - min;
		}

		std::array<vec3, 8> getVertices() const;
	};

	bool AABB_AABBIntersect(AABB const& a, AABB const& b);
	
	struct Sphere
	{
		vec3 pos;
		float size;
	};

	struct Plane
	{
		float dist;
		vec3 dir;
	};

	struct Ray
	{
		vec3 start, dir;
	};

	bool ray_AABBIntersect(Ray const& r, AABB const& aabb);
	bool ray_PlaneIntersect(Ray const& r, Plane const& p);
	
	enum PointPlanePlacement
	{
		Back = -1,
		OnPlane = 0,
		Front = 1,
	};

	PointPlanePlacement classifyPointToPlane(Plane const& plane, vec3 const& pt);

	struct Frustum
	{
		static Frustum createFromPerspective(mat4 const& m);
		Plane left, right, top, bottom, near, far;
	};

	bool frustum_AABBIntersect(Frustum const& f, AABB const& b);
	bool frustum_PlaneIntersect(Frustum const& f, Plane const& b);
	
}

#endif // !GEOMETRY_HPP
