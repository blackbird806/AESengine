#ifndef AES_RHIRENDERTARGET_HPP
#define AES_RHIRENDERTARGET_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef AES_GRAPHIC_API_D3D11
#include "renderer/RHI/D3D11/D3D11RenderTarget.hpp"
#elif defined(__vita__)
#include "renderer/RHI/SceGxm/gxmRenderTarget.hpp"
#endif

#endif