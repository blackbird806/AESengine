#include "geometry.hpp"

#include <cmath>
#include <algorithm>

std::array<glm::vec3, 8> aes::AABB::getVertices() const
{
	return std::array<glm::vec3, 8>
		{ min, max,
		{ min.x, min.y, max.z },
		{ min.x, max.y, max.z },
		{ max.x, min.y, max.z },
		{ max.x, min.y, min.z },
		{ max.x, max.y, min.z },
		{ min.x, max.y, min.z},
		};
}

bool aes::AABB_AABBIntersect(AABB const& a, AABB const& b)
{
	return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z);
}

// https://tavianator.com/2011/ray_box.html
bool aes::ray_AABBIntersect(Ray const& ray, AABB const& box)
{
	glm::vec3 const invRayDir = 1.0f / ray.dir;
	
	float const tx1 = (box.min.x - ray.start.x) * invRayDir.x;
	float const tx2 = (box.max.x - ray.start.x) * invRayDir.x;

	float tmin = std::min(tx1, tx2);
	float tmax = std::max(tx1, tx2);

	float const ty1 = (box.min.y - ray.start.y) * invRayDir.y;
	float const ty2 = (box.max.y - ray.start.y) * invRayDir.y;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));

	float const tz1 = (box.min.z - ray.start.z) * invRayDir.z;
	float const tz2 = (box.max.z - ray.start.z) * invRayDir.z;

	tmin = std::max(tmin, std::min(tz1, tz2));
	tmax = std::min(tmax, std::max(tz1, tz2));

	return tmax >= std::max(0.0f, tmin);
}

bool aes::ray_PlaneIntersect(Ray const& r, Plane const& plane)
{
	// assuming vectors are all normalized
	float const d = glm::dot(-plane.dir, r.dir);
	if (d > FLT_EPSILON)
	{
		glm::vec3 const p = (plane.dir * plane.dist) - r.start;
		float const t = glm::dot(p, -plane.dir) / d;

		if (t < 0)
			return false;

		return true;
	}

	return false;
}

aes::PointPlanePlacement aes::classifyPointToPlane(Plane const& plane, glm::vec3 const& pt)
{
	float constexpr planeThicknessEpsilon = 0.01f;
	// Compute signed distance of point from plane
	float const dist = glm::dot(plane.dir, pt) - plane.dist;
	// Classify p based on the signed distance
	if (dist > planeThicknessEpsilon)
		return PointPlanePlacement::Front;
	if (dist < -planeThicknessEpsilon)
		return PointPlanePlacement::Back;
	return PointPlanePlacement::OnPlane;
}
