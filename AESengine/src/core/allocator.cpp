#include "allocator.hpp"
#include "profiler.hpp"
#include "core/utility.hpp"
#include <new>
#include <memory>
#include <cstdlib>

#ifdef AES_ENABLE_PROFILING

#define AES_PROFILE_MEMORY_ALLOC(ptr, size) ::aes::MemoryProfiler::instance().profileAlloc(ptr, size)
#define AES_PROFILE_MEMORY_DEALLOC(ptr) ::aes::MemoryProfiler::instance().profileDealloc(ptr)

#else

#define AES_PROFILE_MEMORY_ALLOC(ptr, size) 
#define AES_PROFILE_MEMORY_DEALLOC(ptr)

#endif

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

void* aes::IAllocator::do_allocate(size_t size, size_t align)
{
	return allocate(size, align);
}

void aes::IAllocator::do_deallocate(void* ptr, size_t size, size_t align)
{
	deallocate(ptr);
}

bool aes::IAllocator::do_is_equal(const memory_resource& rhs) const noexcept
{
	return *this == rhs;
}

void* aes::Mallocator::allocate(size_t size, size_t align)
{
	return alignedAlloc(size, align);
}

void aes::Mallocator::deallocate(void* ptr)
{
	alignedFree(ptr);
}

aes::StackAllocator::StackAllocator(IAllocator& base, size_t size) : baseAllocator(&base),
	start(static_cast<uint8_t*>(base.allocate(size))), totalSize(size), offset(0)
{
}

aes::StackAllocator::~StackAllocator()
{
	baseAllocator->deallocate(start);
}

void* aes::StackAllocator::allocate(size_t size, size_t alignement)
{
	AES_ASSERT(isPowerOf2(alignement));
	auto const alignedOffset = aes::align(offset, alignement);
	
	if (alignedOffset + size > totalSize)
		return nullptr;

	offset = alignedOffset + size;

	return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(start) + alignedOffset);
}

void aes::StackAllocator::deallocateFromMarker(size_t marker)
{
	AES_ASSERT(marker < offset);
	offset = marker;
}

size_t aes::StackAllocator::getMarker() const
{
	return offset;
}

