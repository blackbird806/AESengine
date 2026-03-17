#ifndef WOB_RHISHADER_HPP
#define WOB_RHISHADER_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef WOB_GRAPHIC_API_D3D11
#include "renderer/RHI/D3D11/D3D11Shader.hpp"
#elif defined(WOB_GRAPHIC_API_GXM)
#include "renderer/RHI/SceGxm/gxmShader.hpp"
#endif

#endif