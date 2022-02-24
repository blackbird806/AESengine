#ifndef GXM_COMPATIBILITY_H
#define GXM_COMPATIBILITY_H

#define SCE_UID_INVALID_UID (static_cast<SceUID>(0xFFFFFFFF))
#define SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE
#define SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW

#define SCE_DISPLAY_UPDATETIMING_NEXTHSYNC	0
#define SCE_DISPLAY_UPDATETIMING_NEXTVSYNC	1

#define SCE_CTRL_L (1 << 8)
#define SCE_CTRL_R (1 << 9)

#endif