#include "octree.hpp"

#include "core/aes.hpp"

using namespace aes;

// TODO: clean and optimize

// see: Christer_Ericson-Real-Time_Collision_Detection: part 7.3, page 311
// @Review lazy building ?
Octree::Node* Octree::build(glm::vec3 const& center, float halfSize, int stopDepth, LocCode_t locCode)
{
	AES_PROFILE_FUNCTION();
	
	if (stopDepth < 0)
		return nullptr;

	auto [it, inserted] = nodes.insert(std::make_pair(locCode, Node{ center, halfSize, locCode, 0xFF }));
	AES_ASSERT(inserted);

	if (stopDepth == 0) [[unlikely]]
		it->second.childExist = 0;
	
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

void Octree::insertObject(Node& tree, Object const& obj)
{
	AES_PROFILE_FUNCTION();

	uint8_t index = 0;
	bool straddle = false;
	// Compute the octant number [0..7] the object sphere center is in
	// If straddling any of the dividing x, y, or z planes, exit directly
	for (int i = 0; i < 3; i++) {
		float const delta = obj.bounds.center()[i] - tree.center[i];
		if (abs(delta) <= glm::length(obj.bounds.max - obj.bounds.min))
		{
			straddle = true;
			break;
		}
		if (delta > 0.0f) index |= (1 << i); // ZYX
	}
	
	if (!straddle && tree.childExist > 0) {
		// Fully contained in existing child node; insert in that subtree
		insertObject(nodes.at((tree.locCode << 3) + index), obj);
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
	AES_PROFILE_FUNCTION();

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

void Octree::testAllCollisions(Node const& node, void(* callback)(void*)) const
{
	AES_PROFILE_FUNCTION();
	AES_ASSERT(callback);
	
	//Keep track of all ancestor object lists in a stack
	constexpr int MAX_DEPTH = 40;
	static Node const* ancestorStack[MAX_DEPTH];
	static int depth = 0; // ’Depth == 0’ is invariant over calls
	// Check collision between all objects on this level and all
	// ancestor objects. The current level is included as its own
	// ancestor so all necessary pairwise tests are done
	ancestorStack[depth++] = &node;
	for (int n = 0; n < depth; n++) {
		for (auto const& pA : ancestorStack[n]->objects) {
			for (auto const& pB : node.objects) {
				// Avoid testing both A->B and B->A
				// @Review use userData for equality ?
				if (pA.userData == pB.userData) break;
				
				// Now perform the collision test between pA and pB in some manner
				if (aes::AABB_AABBIntersect(pA.bounds, pB.bounds))
				{
					callback(pA.userData);
					callback(pB.userData);
				}

			}
		}
	}
	// Recursively visit all existing children
	for (int i = 0; i < 8; i++)
	{
		auto const it = nodes.find((node.locCode << 3) + i);
		if (it != nodes.end())
		{
			testAllCollisions(it->second, callback);
		}
	}
	
	// Remove current node from ancestor stack before returning
	depth--;
}

Octree::Node* Octree::root()
{
	AES_PROFILE_FUNCTION();

	auto const it = nodes.find(1);
	if (it != nodes.end())
		return &it->second;
	return nullptr;
}
