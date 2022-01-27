#ifndef BSPTREE_HPP
#define BSPTREE_HPP

#include <span>
#include <memory>
#include <vector>
#include "core/aes.hpp"
#include "core/geometry.hpp"

namespace aes
{
	// References:
	// Realtime collision detection ch8 p356
	// Game physics engine developement 12.4.1 p251
	
	class BSPTree
	{
	public:
		struct Object
		{
			void* userData;
			AABB bounds;
		};

		struct BSPElement
		{
			
		};
		
		struct Leaf final : BSPElement
		{
			Leaf(std::vector<Object> obj) : objects(std::move(obj))
			{
				
			}
			std::vector<Object> objects;
		};

		struct Node final : BSPElement
		{
			Node(Plane const& p, std::unique_ptr<BSPElement> f, std::unique_ptr<BSPElement> b)
				: plane(p), front(std::move(f)), back(std::move(b))
			{
				
			}
			
			Plane plane;
			std::unique_ptr<BSPElement> front;
			std::unique_ptr<BSPElement> back;
		};

		static constexpr uint maxDepth = 32;
		static constexpr uint minLeafSize = 2;

		static std::unique_ptr<BSPElement> build(std::span<Object> objects, uint depth = 0);
	};
}

#endif
