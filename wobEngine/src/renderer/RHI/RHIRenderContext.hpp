#ifndef WOB_RHIRENDERCONTEXT_HPP
#define WOB_RHIRENDERCONTEXT_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef WOB_GRAPHIC_API_D3D11
#elif defined(__vita__)
#include "renderer/RHI/SceGxm/gxmRenderer.hpp"
#endif

#endif