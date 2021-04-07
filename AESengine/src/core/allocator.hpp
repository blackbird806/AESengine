#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include "core/aes.hpp"

namespace aes
{
	class StackAllocator
	{
	public:
		
		StackAllocator(size_t size);
		~StackAllocator();
		void* allocate(size_t size, size_t align = 0);
		void deallocateFromMarker(size_t marker);

		size_t getMarker() const;
		
	private:

		void *start;;
		size_t totalSize, offset;
	};
	
}

#endif