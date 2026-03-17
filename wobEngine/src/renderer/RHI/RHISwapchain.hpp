#ifndef WOB_RHISWAPCHAIN_HPP
#define WOB_RHISWAPCHAIN_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef WOB_GRAPHIC_API_D3D11
#include "renderer/RHI/D3D11/D3D11Swapchain.hpp"
#elif defined(WOB_GRAPHIC_API_GXM)
#include "renderer/RHI/SceGxm/gxmSwapchain.hpp"
#endif


#endif