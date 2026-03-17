#include "geometry.hpp"

#include <algorithm>
#include <cfloat>
#include "wob.hpp"
#include "renderer/RHI/RHI.hpp"

using namespace wob;

bool wob::pointInRect(vec2 p, Rect const& r)
{
	return p.x > r.min.x && p.x < r.max.x&&
		p.y > r.min.y && p.y < r.max.y;
}

std::array<vec3, 8> wob::AABB::getVertices() const
{
	WOB_PROFILE_FUNCTION();

	return std::array<vec3, 8>
		{ min, max,
		{ min.x, min.y, max.z },
		{ min.x, max.y, max.z },
		{ max.x, min.y, max.z },
		{ max.x, min.y, min.z },
		{ max.x, max.y, min.z },
		{ min.x, max.y, min.z},
		};
}

bool wob::AABB_AABBIntersect(AABB const& a, AABB const& b)
{
	WOB_PROFILE_FUNCTION();

	return	(a.min.x <= b.max.x && a.max.x >= b.min.x) &&
			(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
			(a.min.z <= b.max.z && a.max.z >= b.min.z);
}

// https://tavianator.com/2011/ray_box.html
bool wob::ray_AABBIntersect(Ray const& ray, AABB const& box)
{
	WOB_PROFILE_FUNCTION();

	vec3 const invRayDir = 1.0f / ray.dir;
	
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

bool wob::ray_PlaneIntersect(Ray const& r, Plane const& plane)
{
	WOB_PROFILE_FUNCTION();

	// assuming vectors are all normalized
	float const d = -plane.dir.dot(r.dir);
	if (d > FLT_EPSILON)
	{
		vec3 const p = (plane.dir * plane.dist) - r.start;
		float const t = p.dot(-plane.dir) / d;

		if (t < 0)
			return false;

		return true;
	}

	return false;
}

wob::PointPlanePlacement wob::classifyPointToPlane(Plane const& plane, vec3 const& pt)
{
	WOB_PROFILE_FUNCTION();

	float constexpr planeThicknessEpsilon = 0.01f;
	// Compute signed distance of point from plane
	float const dist = plane.dir.dot(pt) - plane.dist;
	// Classify p based on the signed distance
	if (dist > planeThicknessEpsilon)
		return PointPlanePlacement::Front;
	if (dist < -planeThicknessEpsilon)
		return PointPlanePlacement::Back;
	return PointPlanePlacement::OnPlane;
}

// http://www8.cs.umu.se/kurser/5DV051/HT12/lab/plane_extraction.pdf
// normal is inside the frustum
// @Review normalize dir ?
static wob::Frustum extractFrustumD3D(mat4 const& m)
{
	wob::Frustum f;

	WOB_NOT_IMPLEMENTED();

	// Left clipping plane
	//f.left.dir.x = m[1][4] + m[1][1];
	//f.left.dir.y = m[2][4] + m[2][1];
	//f.left.dir.z = m[3][4] + m[3][1];
	//f.left.dist  = m[4][4] + m[4][1];
	//
	//// Right clipping plane
	//f.right.dir.x = m[1][4] - m[1][1];
	//f.right.dir.y = m[2][4] - m[2][1];
	//f.right.dir.z = m[3][4] - m[3][1];
	//f.right.dist  = m[4][4] - m[4][1];
	//
	//// Top clipping plane
	//f.top.dir.x = m[1][4] - m[1][2];
	//f.top.dir.y = m[2][4] - m[2][2];
	//f.top.dir.z = m[3][4] - m[3][2];
	//f.top.dist  = m[4][4] - m[4][2];
	//
	//// Bottom clipping plane
	//f.bottom.dir.x = m[1][4] + m[1][2];
	//f.bottom.dir.y = m[2][4] + m[2][2];
	//f.bottom.dir.z = m[3][4] + m[3][2];
	//f.bottom.dist  = m[4][4] + m[4][2];
	//
	//// Near clipping plane
	//f.near.dir.x = m[1][3];
	//f.near.dir.y = m[2][3];
	//f.near.dir.z = m[3][3];
	//f.near.dist = m[4][3];
	//
	//// Far clipping plane
	//f.far.dir.x = m[1][4] - m[1][3];
	//f.far.dir.y = m[2][4] - m[2][3];
	//f.far.dir.z = m[3][4] - m[3][3];
	//f.far.dist  = m[4][4] - m[4][3];

	return f;
}

wob::Frustum wob::Frustum::createFromPerspective(mat4 const& m)
{
#ifdef WOB_GRAPHIC_API_D3D11
	return extractFrustumD3D(m);
#elif defined(WOB_GRAPHIC_API_GXM)
	WOB_ASSERT(false);
#endif
}

// @Performance naive implementation
bool wob::frustum_AABBIntersect(Frustum const& f, AABB const& b)
{
	return false;
}

bool wob::frustum_PlaneIntersect(Frustum const& f, Plane const& b)
{
	return false;
}
