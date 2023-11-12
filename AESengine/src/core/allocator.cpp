#include <new>
#include <memory>
#include <cstdlib>

#include "aes.hpp"
#include "allocator.hpp"
#include "profiler.hpp"
#include "utility.hpp"
#include "context.hpp"

#ifdef AES_ENABLE_PROFILING

#define AES_PROFILE_MEMORY_ALLOC(ptr, size) ::aes::MemoryProfiler::instance().profileAlloc(ptr, size)
#define AES_PROFILE_MEMORY_DEALLOC(ptr) ::aes::MemoryProfiler::instance().profileDealloc(ptr)

#else

#define AES_PROFILE_MEMORY_ALLOC(ptr, size) 
#define AES_PROFILE_MEMORY_DEALLOC(ptr)

#endif

using namespace aes;

aes::Mallocator aes::mallocator{};

// https://stackoverflow.com/questions/53922209/how-to-invoke-aligned-new-delete-properly
static void* alignedAlloc(size_t size, size_t al) noexcept
{
	AES_ASSERT(size % al == 0);
#if _MSC_VER
	// msvc doesn't support c11 aligned_alloc
	return _aligned_malloc(size, al);
#else
	// vita toolchains don't seems to support aligned alloc
	// TODO write a custom version
	return malloc(size);
#endif
}

static void alignedFree(void* ptr) noexcept
{
#if _MSC_VER
	_aligned_free(ptr);
#else
	free(ptr);
#endif
}


// @TODO
#if 0 

void* operator new(size_t size) noexcept
{
	void* ptr = malloc(size);
	AES_PROFILE_MEMORY_ALLOC(ptr, size);
	return ptr;
}

void* operator new[](size_t size) noexcept
{
	auto* ptr = malloc(size);
	AES_PROFILE_MEMORY_ALLOC(ptr, size);
	return ptr;
}

void* operator new(size_t size, std::align_val_t al)
{
	auto* ptr = alignedAlloc(size, al);
	AES_PROFILE_MEMORY_ALLOC(ptr, size);
	return ptr;
}

void* operator new[](size_t size, std::align_val_t al)
{
	auto* ptr = alignedAlloc(size, al);
	AES_PROFILE_MEMORY_ALLOC(ptr, size);
	return ptr;
}

void operator delete(void* ptr) noexcept
{
	AES_PROFILE_MEMORY_DEALLOC(ptr);
	free(ptr);
}

void operator delete[](void* ptr) noexcept
{
	AES_PROFILE_MEMORY_DEALLOC(ptr);
	free(ptr);
}

void operator delete(void* ptr, std::align_val_t al) noexcept
{
	AES_PROFILE_MEMORY_DEALLOC(ptr);
	alignedFree(ptr);
}

void operator delete[](void* ptr, std::align_val_t al) noexcept
{
	AES_PROFILE_MEMORY_DEALLOC(ptr);
	alignedFree(ptr);
}


#endif

void* Mallocator::allocate(size_t size, size_t align)
{
	return alignedAlloc(size, align);
}

void Mallocator::deallocate(void* ptr)
{
	alignedFree(ptr);
}

StackAllocator::StackAllocator(IAllocator& base, size_t size) : baseAllocator(&base),
	start(static_cast<uint8_t*>(base.allocate(size))), totalSize(size), offset(0)
{
}

StackAllocator::~StackAllocator()
{
	baseAllocator->deallocate(start);
}

void* StackAllocator::allocate(size_t size, size_t alignement)
{
	AES_ASSERT(isPowerOf2(alignement));
	auto const alignedOffset = aes::align(offset, alignement);
	
	if (alignedOffset + size > totalSize)
		return nullptr;

	offset = alignedOffset + size;

	return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(start) + alignedOffset);
}

void aes::StackAllocator::deallocate(void* ptr)
{
	AES_UNUSED(ptr);
}

void StackAllocator::deallocateFromMarker(size_t marker)
{
	AES_ASSERT(marker < offset);
	offset = marker;
}

size_t StackAllocator::getMarker() const
{
	return offset;
}

AllocatorProfiler::AllocatorProfiler(IAllocator& base) : baseAllocator(&base)
{
}

void* AllocatorProfiler::allocate(size_t size, size_t align)
{
	return baseAllocator->allocate(size, align);
}

void AllocatorProfiler::deallocate(void* ptr)
{
	baseAllocator->deallocate(ptr);
}

IAllocator* aes::getContextAllocator() noexcept
{
    return context.allocator;
}
