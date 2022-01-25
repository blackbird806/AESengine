#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <list>
#include <span>
#include <unordered_map>
#include <glm/glm.hpp>

#include "core/aes.hpp"
#include "core/geometry.hpp"

namespace aes
{
	class Octree
	{
	public:
		// 32bits => 10 max depth
		// 64bits => 21 max depth
		using LocCode_t = uint32_t;

		struct Object
		{
			AABB bounds;
			void* userData;
		};

		struct Node
		{
			glm::vec3 center;
			float halfSize;
			LocCode_t locCode;
			std::list<Object> objects;
			bool isLeaf;
		};

		// root locCode is one to mark end of locCode sequence https://geidav.wordpress.com/2014/08/18/advanced-octrees-2-node-representations/
		Node* build(glm::vec3 const& center, float halfSize, int stopDepth, LocCode_t locCode = 1);
		void insertObject(Node& tree, Object const& obj);

		static uint getNodeTreeDepth(Octree::Node const& node);

		// call callback on userdata carried by the colliding nodes
		void testAllCollisions(Node const& node, void(*callback)(void*)) const;
		void testAllCollisionsRec(Node const& node, void(*callback)(void*), uint& depth, std::span<Node const*> ancestorStack) const;

		// return null if tree wasn't built
		Node* root();
		Node const* root() const;

		// range interface
		
		auto begin() const
		{
			return nodes.begin();
		}

		auto end() const
		{
			return nodes.end();
		}
		
	private:
		
		static LocCode_t getChildCode(LocCode_t parentCode, uint childNumber)
		{
			return (parentCode << 3) + childNumber;
		}
		
		// @Review try other node representations
		std::unordered_map<LocCode_t, Node> nodes;
	};
}

#endif