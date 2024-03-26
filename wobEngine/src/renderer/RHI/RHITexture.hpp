#ifndef AES_RHITEXTURE_HPP
#define AES_RHITEXTURE_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef AES_GRAPHIC_API_D3D11
#include "renderer/RHI/D3D11/D3D11Texture.hpp"
#elif defined(AES_GRAPHIC_API_GXM)
#include "renderer/RHI/SceGxm/gxmTexture.hpp"
#endif

#endif