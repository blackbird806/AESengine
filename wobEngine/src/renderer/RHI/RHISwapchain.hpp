#ifndef AES_RHISWAPCHAIN_HPP
#define AES_RHISWAPCHAIN_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef AES_GRAPHIC_API_D3D11
#include "renderer/RHI/D3D11/D3D11Swapchain.hpp"
#elif defined(AES_GRAPHIC_API_GXM)
#include "renderer/RHI/SceGxm/gxmSwapchain.hpp"
#endif


#endif