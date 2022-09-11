#ifndef AES_RHIDEVICE_HPP
#define AES_RHIDEVICE_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef AES_GRAPHIC_API_D3D11
#elif defined(__vita__)
	#include "renderer/RHI/SceGxm/gxmDevice.hpp"
#endif

#endif