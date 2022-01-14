#include "octree.hpp"

#include "core/aes.hpp"

using namespace aes;

// see: Christer_Ericson-Real-Time_Collision_Detection: part 7.3, page 311
Octree::Node* Octree::build(glm::vec3 const& center, float halfSize, int stopDepth, LocCode_t locCode)
{
	if (stopDepth < 0)
		return nullptr;
	
	auto [it, inserted] = nodes.insert(std::make_pair(locCode, Node{center, halfSize, locCode, 0XFF}));
	AES_ASSERT(inserted);
	
	// Recursively construct the eight children of the subtree
	float const step = halfSize * 0.5f;
	for (int i = 0; i < 8; i++) 
	{
		glm::vec3 offset;
		offset.x = ((i & 1) ? step : -step);
		offset.y = ((i & 2) ? step : -step);
		offset.z = ((i & 4) ? step : -step);
		LocCode_t const childCode = (locCode << 3) + i;
		build(center + offset, step, stopDepth - 1, childCode);
	}
	
	return &it->second;
}

void Octree::insertObject(Node& tree, Object& obj)
{
	uint8_t index = 0;
	bool straddle = false;
	// Compute the octant number [0..7] the object sphere center is in
	// If straddling any of the dividing x, y, or z planes, exit directly
	for (int i = 0; i < 3; i++) {
		float const delta = obj.bounds.center()[i] - tree.center[i];
		if (AABB_AABBIntersect(obj.bounds, AABB::createHalfCenter(tree.center, glm::vec3(tree.halfSize)))) 
		{
			straddle = true;
			break;
		}
		if (delta > 0.0f) index |= (1 << i); // ZYX
	}
	if (!straddle && ((tree.childExist & index) == index)) {
		// Fully contained in existing child node; insert in that subtree
		insertObject(nodes[0 << index], obj);
	}
	else {
		// Straddling, or no child node to descend into, so
		// link object into linked list at this node
		tree.objects.push_back(obj);
	}
}

// https://geidav.wordpress.com/2014/08/18/advanced-octrees-2-node-representations/
uint32_t Octree::getNodeTreeDepth(Octree::Node const& node)
{
	AES_ASSERT(node.locCode); // at least flag bit must be set

#if defined(__GNUC__)
	return (31 - __builtin_clz(node.locCode)) / 3;
#elif defined(_MSC_VER)
	unsigned long msb;
	_BitScanReverse(&msb, node.locCode);
	return msb / 3;
#else
	for (uint32_t lc = node.locCode, depth=0; lc!=1; lc>>=3, depth++);
	return depth;
#endif
}