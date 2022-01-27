#include "geometry.hpp"

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
