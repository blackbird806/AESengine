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
			uint8_t childExist;
			std::list<Object> objects;
		};

		Node* build(glm::vec3 const& center, float halfSize, int stopDepth, LocCode_t locCode = 0);
		void insertObject(Node& tree, Object& obj);

		static uint32_t getNodeTreeDepth(Octree::Node const& node);

	private:
		std::unordered_map<LocCode_t, Node> nodes;
	};
}

#endif