#ifndef AES_ALLOCATOR_HPP
#define AES_ALLOCATOR_HPP

#include "core/aes.hpp"

namespace aes
{
	// malloc based allocator that is not tracked by memory profiler
	struct UntrackedAllocator
	{
	};
	
	class MemoryProfiler
	{
		
	public:
		static MemoryProfiler& instance()
		{
			static MemoryProfiler prof;
			return prof;
		}

		void profileAlloc(void* ptr, size_t size);
		void profileDealloc(void* ptr){};
		
	private:
		size_t memoryAllocated = 0;
		size_t nbAllocations = 0;
	};

	class StackAllocator
	{
	public:
		
		StackAllocator(size_t size);
		~StackAllocator();
		[[nodiscard]] void* allocate(size_t size, size_t align = 0);
		void deallocateFromMarker(size_t marker);
		size_t getMarker() const;
		
	private:

		uint8_t* start;;
		size_t totalSize, offset;
	};

}

#endif