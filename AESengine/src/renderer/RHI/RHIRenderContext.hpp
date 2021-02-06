#ifndef RHI_RENDER_CONTEXT_HPP
#define RHI_RENDER_CONTEXT_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef AES_GRAPHIC_API_D3D11
#include "renderer/RHI/D3D11/D3D11renderer.hpp"
#elif defined(AES_GRAPHIC_API_VITAGL)
#include "renderer/RHI/VitaGL/vglRenderer.hpp"
#endif


#endif