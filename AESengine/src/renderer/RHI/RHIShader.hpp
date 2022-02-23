#ifndef AES_RHISHADER_HPP
#define AES_RHISHADER_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef AES_GRAPHIC_API_D3D11
#include "renderer/RHI/D3D11/D3D11Shader.hpp"
#elif defined(AES_GRAPHIC_API_GXM)
#include "renderer/RHI/SceGxm/gxmShader.hpp"
#endif

#endif