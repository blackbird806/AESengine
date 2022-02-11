#include "geometry.hpp"

#include <algorithm>
#include "aes.hpp"
#include "core/simd.hpp"

using namespace aes;

std::array<glm::vec3, 8> aes::AABB::getVertices() const
{
	AES_PROFILE_FUNCTION();

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
	AES_PROFILE_FUNCTION();

	return	(a.min.x <= b.max.x && a.max.x >= b.min.x) &&
			(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
			(a.min.z <= b.max.z && a.max.z >= b.min.z);
}

// http://www.cs.uu.nl/docs/vakken/magr/2017-2018/slides/lecture%2005%20-%20SIMD%20recap.pdf
static bool ray_AABBIntersectSIMD(aes::Ray const& ray, aes::AABB const& box)
{
	AES_PROFILE_FUNCTION();

	glm::vec3 const invRayDir = 1.0f / ray.dir;

	r128_t bmin = load_r128(&box.min[0]);
	r128_t bmax = load_r128(&box.max[0]);
	r128_t rstart = load_r128(&ray.start[0]);
	r128_t idir = load_r128(&invRayDir[0]);

	r128_t t1 = mul_r128(sub_r128(bmin, rstart), idir);
	r128_t t2 = mul_r128(sub_r128(bmax, rstart), idir);

	glm::vec4 vmin;
	store_r128(&vmin[0], min_r128(t1, t2));
	glm::vec4 vmax;
	store_r128(&vmax[0], max_r128(t1, t2));

	float const tmax = std::min(vmax[0], std::min(vmax[1], vmax[2]));
	float const tmin = std::max(vmin[0], std::max(vmin[1], vmin[2]));

	return tmax >= std::max(0.0f, tmin);
}

// https://tavianator.com/2011/ray_box.html
bool aes::ray_AABBIntersect(Ray const& ray, AABB const& box)
{
	AES_PROFILE_FUNCTION();
	return ray_AABBIntersectSIMD(ray, box);
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
	AES_PROFILE_FUNCTION();

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
	AES_PROFILE_FUNCTION();

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

// http://www8.cs.umu.se/kurser/5DV051/HT12/lab/plane_extraction.pdf
// normal is inside the frustum
// @Review normalize dir ?
static aes::Frustum extractFrustumD3D(glm::mat4 const& m)
{
	aes::Frustum f;
	
	// Left clipping plane
	f.left.dir.x = m[1][4] + m[1][1];
	f.left.dir.y = m[2][4] + m[2][1];
	f.left.dir.z = m[3][4] + m[3][1];
	f.left.dist  = m[4][4] + m[4][1];
	
	// Right clipping plane
	f.right.dir.x = m[1][4] - m[1][1];
	f.right.dir.y = m[2][4] - m[2][1];
	f.right.dir.z = m[3][4] - m[3][1];
	f.right.dist  = m[4][4] - m[4][1];
	
	// Top clipping plane
	f.top.dir.x = m[1][4] - m[1][2];
	f.top.dir.y = m[2][4] - m[2][2];
	f.top.dir.z = m[3][4] - m[3][2];
	f.top.dist  = m[4][4] - m[4][2];
	
	// Bottom clipping plane
	f.bottom.dir.x = m[1][4] + m[1][2];
	f.bottom.dir.y = m[2][4] + m[2][2];
	f.bottom.dir.z = m[3][4] + m[3][2];
	f.bottom.dist  = m[4][4] + m[4][2];
	
	// Near clipping plane
	f.near.dir.x = m[1][3];
	f.near.dir.y = m[2][3];
	f.near.dir.z = m[3][3];
	f.near.dist = m[4][3];
	
	// Far clipping plane
	f.far.dir.x = m[1][4] - m[1][3];
	f.far.dir.y = m[2][4] - m[2][3];
	f.far.dir.z = m[3][4] - m[3][3];
	f.far.dist  = m[4][4] - m[4][3];

	return f;
}

aes::Frustum aes::Frustum::createFromPerspective(glm::mat4 const& m)
{
#ifdef AES_GRAPHIC_API_D3D11
	return extractFrustumD3D(m);
#elif defined(AES_GRAPHIC_API_GXM)
	AES_ASSERT(false);
#endif
}

// @Performance naive implementation
bool aes::frustum_AABBIntersect(Frustum const& f, AABB const& b)
{
	return false;
}

bool aes::frustum_PlaneIntersect(Frustum const& f, Plane const& b)
{
	return false;
}
