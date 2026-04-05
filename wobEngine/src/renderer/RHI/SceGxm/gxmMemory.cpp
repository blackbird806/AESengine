#include "gxmMemory.hpp"
#include "gxmCompatibilty.h"
#include "core/wob.hpp"
#include "core/utility.hpp"

using namespace aes;

void* aes::graphicsAlloc(SceKernelMemBlockType type, uint32_t size, uint32_t alignement, uint32_t attribs, SceUID* uid, const char* name)
{
	WOB_ASSERT(uid != nullptr);
	
	// Page align the size
	if (type == SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW)
	{
		// 	CDRAM memblock must be 256Kib aligned
		WOB_ASSERT(alignement <= 256 * 1024);
		size = aes::align(size, 256 * 1024);
	} 
	else
	{
		// LPDDR memblocks must be 4KiB aligned
		WOB_ASSERT(alignement <= 4 * 1024);
		size = aes::align(size, 4 * 1024);
	}
	
	uint32_t err = SCE_OK;
	*uid = sceKernelAllocMemBlock(name, type, size, nullptr);
	WOB_ASSERT(*uid > SCE_OK);

	void* mem = nullptr;
	err = (SceGxmErrorCode) sceKernelGetMemBlockBase(*uid, &mem);
	WOB_ASSERT(err == SCE_OK);

	// map for the GPU
	err = (SceGxmErrorCode) sceGxmMapMemory(mem, size, (SceGxmMemoryAttribFlags)attribs);
	WOB_ASSERT(err == SCE_OK);
	
	return mem;
}

void aes::graphicsFree(SceUID uid)
{
	// grab the base address
	void* mem = nullptr;
	uint32_t err = sceKernelGetMemBlockBase(uid, &mem);
	WOB_ASSERT(err == SCE_OK);

	// unmap memory
	err = sceGxmUnmapMemory(mem);
	WOB_ASSERT(err == SCE_OK);

	// free the memory block
	err = sceKernelFreeMemBlock(uid);
	WOB_ASSERT(err == SCE_OK);
}
