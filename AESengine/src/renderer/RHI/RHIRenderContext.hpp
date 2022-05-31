#ifndef AES_RHIRENDERCONTEXT_HPP
#define AES_RHIRENDERCONTEXT_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef AES_GRAPHIC_API_D3D11
#include "renderer/RHI/D3D11/D3D11renderer.hpp"
#elif defined(__vita__)
#include "renderer/RHI/SceGxm/gxmRenderer.hpp"
#endif


#endif