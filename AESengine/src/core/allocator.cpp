#include "allocator.hpp"
#include "profiler.hpp"
#include "core/utility.hpp"
#include <new>
#include <memory>
#include <cstdlib>

// @TODO
#ifndef __vita__

void* operator new(size_t size)
{
	auto* ptr = malloc(size);
	if (ptr == nullptr) throw std::bad_alloc();
	AES_PROFILE_MEMORY_ALLOC(ptr, size);
	return ptr;
}

void* operator new(size_t size, std::nothrow_t const&)
{
	auto* ptr = malloc(size);
	AES_PROFILE_MEMORY_ALLOC(ptr, size);
	return ptr;
}

void* operator new[](size_t size)
{
	auto* ptr = malloc(size);
	if (ptr == nullptr) throw std::bad_alloc();
	AES_PROFILE_MEMORY_ALLOC(ptr, size);
	return ptr;
}

void* operator new(size_t size, std::align_val_t al)
{
	auto* ptr = _aligned_malloc(size, (size_t)al);
	AES_PROFILE_MEMORY_ALLOC(ptr, size);
	return ptr;
}

void* operator new[](size_t size, std::align_val_t al)
{
	auto* ptr = _aligned_malloc(size, (size_t)al);
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

#endif

aes::StackAllocator::StackAllocator(size_t size) : totalSize(size), offset(0)
{
	start = malloc(totalSize);
}

aes::StackAllocator::~StackAllocator()
{
	free(start);
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

