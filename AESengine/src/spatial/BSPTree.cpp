#include "BSPTree.hpp"

using namespace aes;

namespace 
{
	enum class ObjectPlanePlacement
	{
		Front,
		Back,
		Straddling,
		Coplanar
	};

	ObjectPlanePlacement classifyObjectToPlane(BSPTree::Object const& obj, Plane const& plane)
	{
		AES_PROFILE_FUNCTION();
		// Loop over all polygon vertices and count how many vertices
		// lie in front of and how many lie behind of the thickened plane
		int numInFront = 0, numBehind = 0;
		auto const vertices = obj.bounds.getVertices();
		for (int i = 0; i < vertices.size(); i++) 
		{
			glm::vec3 const p = vertices[i];
			switch (classifyPointToPlane(plane, p))
			{
			case PointPlanePlacement::Front:
				numInFront++;
				break;
			case PointPlanePlacement::Back:
				numBehind++;
				break;
			case OnPlane:
			default: ;
			}
		}
		// If vertices on both sides of the plane, the polygon is straddling
		if (numBehind != 0 && numInFront != 0)
			return ObjectPlanePlacement::Straddling;
		// If one or more vertices in front of the plane and no vertices behind
		// the plane, the polygon lies in front of the plane
		if (numInFront != 0)
			return ObjectPlanePlacement::Front;
		// Ditto, the polygon lies behind the plane if no vertices in front of
		// the plane, and one or more vertices behind the plane
		if (numBehind != 0)
			return ObjectPlanePlacement::Back;
		// All vertices lie on the plane so the polygon is coplanar with the plane
		return ObjectPlanePlacement::Coplanar;
	}

	// @Review how do we get a good plane from aabb ?
	Plane getPlaneFromAABB(AABB const& aabb)
	{
		AES_PROFILE_FUNCTION();

		return Plane{ glm::length(aabb.center()),
			glm::normalize(glm::vec3{aabb.max.x - aabb.min.x, aabb.min.y, aabb.min.z}) };
	}
	
	Plane pickSplittingPlane(ArrayView<BSPTree::Object> objects)
	{
		AES_PROFILE_FUNCTION();
		// Blend factor for optimizing for balance or splits (should be tweaked)
		const float K = 0.8f;
		// Variables for tracking best splitting plane seen so far
		Plane bestPlane;
		float bestScore = FLT_MAX;
		// Try the plane of each polygon as a dividing plane
		for (int i = 0; i < objects.size(); i++) 
		{
			int numInFront = 0, numBehind = 0, numStraddling = 0;
			Plane plane = getPlaneFromAABB(objects[i].bounds);
			// Test against all other polygons
			for (int j = 0; j < objects.size(); j++) 
			{
				// Ignore testing against self
				if (i == j) 
					continue;
				// Keep standing count of the various poly-plane relationships
				switch (classifyObjectToPlane(objects[j], plane))
				{
					/* Coplanar polygons treated as being in front of plane */
					case ObjectPlanePlacement::Coplanar:
					case ObjectPlanePlacement::Front:
						numInFront++;
						break;
					case ObjectPlanePlacement::Back:
						numBehind++;
						break;
					case ObjectPlanePlacement::Straddling:
						numStraddling++;
						break;
					default: AES_UNREACHABLE();
				}
			}
			// Compute score as a weighted combination (based on K, with K in range
			// 0..1) between balance and splits (lower score is better)
			float const score = K * numStraddling + (1.0f - K) * abs(numInFront - numBehind);
			if (score < bestScore) {
				bestScore = score;
				bestPlane = plane;
			}
		}
		return bestPlane;
	}
}

void BSPTree::Leaf::testAllCollisions(void(*callback)(void* userData)) const
{
	AES_PROFILE_FUNCTION();
	for (auto const& objA : objects)
	{
		for (auto const& objB : objects)
		{
			if (objA.userData == objB.userData)
				continue;
			
			if (AABB_AABBIntersect(objA.bounds, objB.bounds))
			{
				callback(objA.userData);
				callback(objB.userData);
			}
		}
	}
}

void* BSPTree::Leaf::raycast(Ray const& r) const
{
	AES_PROFILE_FUNCTION();

	for (auto const& obj : objects)
	{
		if (ray_AABBIntersect(r, obj.bounds))
		{
			return obj.userData;
		}
	}
	
	return nullptr;
}

void BSPTree::Node::testAllCollisions(void(*callback)(void* userData)) const
{
	AES_PROFILE_FUNCTION();
	AES_ASSERT(callback);

	if (front)
		front->testAllCollisions(callback);
	if (back)
		back->testAllCollisions(callback);
}

void* BSPTree::Node::raycast(Ray const& r) const
{
	AES_PROFILE_FUNCTION();

	BSPElement* relativeFront = front.get();
	BSPElement* relativeBack = back.get();

	// if camera is behind plane swap front and back spaces
	if (classifyPointToPlane(plane, r.start) == PointPlanePlacement::Back)
		std::swap(relativeFront, relativeBack);

	void* const frontResult = relativeFront ? relativeFront->raycast(r) : nullptr;

	if (frontResult == nullptr && ray_PlaneIntersect(r, plane))
	{
		return relativeBack->raycast(r);
	}
	
	return frontResult;
}

UniquePtr<BSPTree::BSPElement> BSPTree::build(IAllocator& allocator, ArrayView<Object> objects, uint depth)
{
	AES_PROFILE_FUNCTION();
	if (objects.empty())
		return nullptr;

	if (depth > maxDepth || objects.size() <= minLeafSize)
		return makeUnique<Leaf>(allocator, Array<Object>(allocator, objects));

	Plane const splitPlane = pickSplittingPlane(objects);

	Array<Object> backList(allocator), frontList(allocator);

	for (auto const& o : objects)
	{
		switch (classifyObjectToPlane(o, splitPlane))
		{
		case ObjectPlanePlacement::Front:
			frontList.push(o);
			break;
		case ObjectPlanePlacement::Back:
			backList.push(o);
			break;
		case ObjectPlanePlacement::Straddling:
			// split polygon ?
			frontList.push(o);
			backList.push(o);
			break;
		default: AES_UNREACHABLE();
		}
	}
	
	return makeUnique<Node>(allocator, splitPlane,
		build(allocator, ArrayView<Object>(frontList.data(), frontList.size()), depth + 1),
		build(allocator, ArrayView<Object>(backList.data(), backList.size()), depth + 1));
}
