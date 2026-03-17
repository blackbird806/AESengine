#ifndef WOB_RHISAMPLER_HPP
#define WOB_RHISAMPLER_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef WOB_GRAPHIC_API_D3D11
#include "renderer/RHI/D3D11/D3D11Sampler.hpp"
#elif defined(WOB_GRAPHIC_API_GXM)
#include "renderer/RHI/SceGxm/gxmSampler.hpp"
#endif

#endif