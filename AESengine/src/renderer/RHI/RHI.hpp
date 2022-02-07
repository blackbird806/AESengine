#ifndef RHI_HPP
#define RHI_HPP

#include "core/aes.hpp"

#ifdef _WIN32
	#define AES_GRAPHIC_API_D3D11
#elif defined(__vita__)
	#define AES_GRAPHIC_API_GXM
#endif

#endif
