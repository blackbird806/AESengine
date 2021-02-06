#ifndef RHI_HPP
#define RHI_HPP

#include "core/aes.hpp"

#ifdef _WIN32
#define AES_GRAPHIC_API_D3D11
#elif defined(AES_PLATFORM_VITA)
#define AES_GRAPHIC_API_VITAGL
#endif

#endif
