#include "allocator.hpp"
#include "profiler.hpp"

#include <new>
#include <cstdlib>

// @TODO
#ifndef __vita__

void* operator new(size_t count)
{
	auto ptr = malloc(count);
	if (ptr == nullptr) throw std::bad_alloc();
	AES_PROFILE_MEMORY_NEW(ptr, count);
	return ptr;
}

void* operator new[](size_t count)
{
	auto* ptr = malloc(count);
	if (ptr == nullptr) throw std::bad_alloc();
	AES_PROFILE_MEMORY_NEW(ptr, count);
	return ptr;
}

void* operator new(size_t count, std::align_val_t al)
{
	auto* ptr = _aligned_malloc(count, (size_t)al);
	AES_PROFILE_MEMORY_NEW(ptr, count);
	return ptr;
}

void* operator new[](size_t count, std::align_val_t al)
{
	auto* ptr = _aligned_malloc(count, (size_t)al);
	AES_PROFILE_MEMORY_NEW(ptr, count);
	return ptr;
}

void operator delete(void* ptr) noexcept
{
	AES_PROFILE_MEMORY_DELETE(ptr);
	free(ptr);
}

#endif