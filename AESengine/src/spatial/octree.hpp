#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <list>
#include <unordered_map>
#include <glm/glm.hpp>
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
			uint8_t childExist = 0xFF;
			std::list<Object> objects;
		};

		// root locCode is one to mark end of locCode sequence https://geidav.wordpress.com/2014/08/18/advanced-octrees-2-node-representations/
		Node* build(glm::vec3 const& center, float halfSize, int stopDepth, LocCode_t locCode = 1);
		void insertObject(Node& tree, Object const& obj);

		static uint32_t getNodeTreeDepth(Octree::Node const& node);

		Node* root();
		
	//private:
		// @TODO use cache friendly representation
		std::unordered_map<LocCode_t, Node> nodes;
	};
}

#endif