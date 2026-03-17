#ifndef WOB_RHIRENDERTARGET_HPP
#define WOB_RHIRENDERTARGET_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef WOB_GRAPHIC_API_D3D11
#include "renderer/RHI/D3D11/D3D11RenderTarget.hpp"
#elif defined(__vita__)
#include "renderer/RHI/SceGxm/gxmRenderTarget.hpp"
#endif

#endif