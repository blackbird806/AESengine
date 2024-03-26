#ifndef AES_GXMMEMORY_HPP
#define AES_GXMMEMORY_HPP

#include <psp2/kernel/sysmem.h>  // for SceKernelMemBlockType
#include <psp2/gxm.h>

namespace aes
{
	void* graphicsAlloc(SceKernelMemBlockType type, uint32_t size, uint32_t alignement, uint32_t attribs, SceUID* uid, const char* name = "unnamed");
	void graphicsFree(SceUID uid);
}

#endif