#ifndef GXM_DEBUG_HPP
#define GXM_DEBUG_HPP

#include "core/aes.hpp"
#include "core/debug.hpp"
#include "gxmCompatibilty.h"

#define AES_GXM_CHECK(err) if (err != SCE_OK) { AES_LOG_ERROR("AES_GXM_CHECK {}", err); AES_DEBUG_BREAK(); } 

#endif