#ifndef WOB_RHITEXTURE_HPP
#define WOB_RHITEXTURE_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef WOB_GRAPHIC_API_D3D11
#include "renderer/RHI/D3D11/D3D11Texture.hpp"
#elif defined(WOB_GRAPHIC_API_GXM)
#include "renderer/RHI/SceGxm/gxmTexture.hpp"
#endif

#endif