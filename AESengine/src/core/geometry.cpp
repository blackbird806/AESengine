#include "geometry.hpp"
#define SIMD_OPTI

bool aes::AABB_AABBIntersect(AABB const& a, AABB const& b) noexcept
{
	return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z);
}
