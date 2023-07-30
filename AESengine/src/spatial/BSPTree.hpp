#ifndef BSPTREE_HPP
#define BSPTREE_HPP

#include <vector>
#include "core/aes.hpp"
#include "core/geometry.hpp"
#include "core/arrayView.hpp"
#include "core/uniquePtr.hpp"
#include "core/array.hpp"
#include "core/jobSystem.hpp"

namespace aes
{
	// References:
	// Realtime collision detection ch8 p356
	// Game physics engine developement 12.4.1 p251
	// Naive implementation, @Review we may want to try a more advanced implementation 
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
			virtual void testAllCollisions(void(*)(void* userData)) const = 0;
			virtual void* raycast(Ray const& r) const = 0;
			virtual ~BSPElement() = default;
		};
		
		struct Leaf final : BSPElement
		{
			Leaf(Array<Object>&& obj) : objects(std::move(obj))
			{
				
			}

			~Leaf() override = default;

			void testAllCollisions(void(*)(void* userData)) const override;
			void* raycast(Ray const& r) const override;
			Array<Object> objects;
		};

		struct Node final : BSPElement
		{
			Node(Plane const& p, UniquePtr<BSPElement> f, UniquePtr<BSPElement> b)
				: plane(p), front(std::move(f)), back(std::move(b))
			{
				
			}
			~Node() override = default;

			void testAllCollisions(void(*)(void* userData)) const override;
			void* raycast(Ray const& r) const override;

			Plane plane;
			UniquePtr<BSPElement> front;
			UniquePtr<BSPElement> back;
		};

		static constexpr uint maxDepth = 16;
		static constexpr uint minLeafSize = 2;
		
		static UniquePtr<BSPElement> build(IAllocator& allocator, ArrayView<Object> objects, uint depth = 0);
	};
}

#endif
