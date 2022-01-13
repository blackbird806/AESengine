#ifndef OCTREE_HPP
#define OCTREE_HPP
#include <vector>

namespace aes
{
	class Octree
	{
		using LocCode_t = uint32_t;

		struct Object
		{
			void* userData;
		};
		
		struct Node
		{
			LocCode_t locCode;
			uint8_t childExist;
			Object* userData;
		};

	};
}

#endif